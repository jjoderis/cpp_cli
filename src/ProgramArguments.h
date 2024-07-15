#ifndef PROGRAM_ARGUMENTS_H
#define PROGRAM_ARGUMENTS_H

#include <iostream>
#include <memory>
#include <tuple>
#include <vector>

#include "util/type_traits.h"

namespace cpp_cli {

template <typename... Args>
class ProgramArguments {
  static_assert(
      is_named_arg_pack<Args...>::value,
      "Please provide only named arguments for the creation of a program arguments object!"
  );
  static_assert(args_have_unique_names<Args...>::value, "Please use an unique enum value for each named argument!");

 public:
  ProgramArguments() = delete;
  ProgramArguments(const Args &...args) : m_arguments{args...} {}

  template <
      typename ArgNames,
      ArgNames Name,
      int argIndex = arg_index_by_name<ArgNames, Name, Args...>::value,
      typename ArgType = typename arg_by_name<ArgNames, Name, Args...>::type>
  ArgType &getArg() {
    return std::get<argIndex>(m_arguments);
  }

  template <
      typename ArgNames,
      ArgNames Name,
      int argIndex = arg_index_by_name<ArgNames, Name, Args...>::value,
      typename ArgValueType =
          typename named_arg_info<typename arg_by_name<ArgNames, Name, Args...>::type>::arg_value_type>
  bool hasValue() {
    return (static_cast<CL_Argument<ArgValueType>>(std::get<argIndex>(m_arguments))).hasValue();
  }

  template <
      typename ArgNames,
      ArgNames Name,
      int argIndex = arg_index_by_name<ArgNames, Name, Args...>::value,
      typename ArgValueType =
          typename named_arg_info<typename arg_by_name<ArgNames, Name, Args...>::type>::arg_value_type>
  ArgValueType &getValue() {
    return *(static_cast<CL_Argument<ArgValueType>>(std::get<argIndex>(m_arguments))).value.get();
  }

 private:
  std::tuple<Args...> m_arguments;
};

template <typename ArgValueType>
void markAsHandled(int argc, const char **argv, std::vector<bool> &handledFlags, CL_Argument<ArgValueType> &arg) {
  int flagIndex, longIndex, shortIndex;

  std::tie(flagIndex, longIndex, shortIndex) = arg.getFlagIndex(argc, argv);
  if (flagIndex < argc) {
    handledFlags[flagIndex] = true;
    handledFlags[flagIndex + 1] = true;
  }
}

template <typename T, typename... AllArgs, typename...>
void parseProgramArgumentFromCL(int argc, const char **argv, std::vector<bool> &handledFlags, ProgramArguments<AllArgs...> &progArgs, const T &) {
  using ArgNameType = typename named_arg_info<T>::arg_name_type;
  static const ArgNameType argName = named_arg_info<T>::arg_name;
  using ArgValueType = typename named_arg_info<T>::arg_value_type;

  CL_Argument<ArgValueType> &storedArg = progArgs.template getArg<ArgNameType, argName>();
  parseArgFromCL(argc, argv, storedArg);
  markAsHandled(argc, argv, handledFlags, storedArg);
}
template <typename T, typename... AllArgs, typename... CurrArgs>
void parseProgramArgumentFromCL(
    int argc,
    const char **argv,
    std::vector<bool> &handledFlags,
    ProgramArguments<AllArgs...> &progArgs,
    const T &arg,
    const CurrArgs &...args
) {
  parseProgramArgumentFromCL(argc, argv, handledFlags, progArgs, arg);
  parseProgramArgumentFromCL(argc, argv, handledFlags, progArgs, args...);
}

template <typename... Args>
std::tuple<ProgramArguments<Args...>, int> parseProgramArgumentsFromCL(
    int argc, const char **argv, const Args &...args
) {
  std::vector<bool> handledFlags(argc);

  for (int i = 1; i < argc; ++i) {
    handledFlags[i] = false;
  }

  ProgramArguments<Args...> progArgs{args...};

  parseProgramArgumentFromCL(argc, argv, handledFlags, progArgs, args...);

  for (int i = 1; i < argc; ++i) {
    if (!handledFlags[i]) {
      return {progArgs, i};
    }
  }

  return {progArgs, -1};
}

}  // namespace cpp_cli

#endif