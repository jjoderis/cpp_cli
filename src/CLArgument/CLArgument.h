#ifndef CL_ARGUMENT_H
#define CL_ARGUMENT_H

#include <functional>
#include <memory>
#include <sstream>
#include <string>

#include "util/CLIException.h"

namespace cpp_cli {
template <typename T>
std::shared_ptr<T> parse(const char *);
// {
//   static_assert(
//       false,
//       "Please provide a parse implementation that returns a shared ptr of type () with signature "
//       "std::shared_ptr<your-type>(const char*)\n"
//   );

//   return std::shared_ptr<T>{};
// }

template <typename ValueType>
class CL_Argument {
 public:
  const char *longOpt;
  char shortOpt;
  std::string description;
  std::function<void(const ValueType &)> validator;
  std::shared_ptr<ValueType> value{};

  CL_Argument() = delete;

  bool hasLong() const { return longOpt != nullptr; }

  bool hasShort() const { return shortOpt != '\0'; }

  bool hasValue() const { return value.get() != nullptr; }

  bool hasDescription() const { return !description.empty(); }

  bool isRequired() const { return m_required; }

  bool hasValidator() const { return validator != nullptr; }

  std::string getManualString(int margin = 2) const {
    std::string marg{};
    marg.append(margin, ' ');
    std::stringstream s;
    s << marg;
    if (hasShort()) s << '-' << shortOpt;
    if (hasShort() && hasLong()) s << ", ";
    if (hasLong()) s << "--" << longOpt;
    if (hasDescription()) s << marg << description;
    return s.str();
  }

  std::tuple<int, int, int> getFlagIndex(int argc, const char **argv) {
    int longFlagIndex = hasLong() ? 1 : argc;
    for (; longFlagIndex < argc; ++longFlagIndex) {
      std::string a = std::string{"--"}.append(longOpt);
      std::string b{argv[longFlagIndex]};
      if (a.compare(b) == 0) break;
    }
    int shortFlagIndex = hasShort() ? 1 : argc;
    for (; shortFlagIndex < argc; ++shortFlagIndex) {
      std::string a = std::string{"-"}.append(1, shortOpt);
      std::string b{argv[shortFlagIndex]};
      if (a.compare(b) == 0) break;
    }

    if (longFlagIndex < argc && shortFlagIndex < argc) {
      std::string errMessage{"Provided values for both the short (%s) and long (%s) version of a program parameter!"};
      throw FlagException(errMessage, "", longOpt, shortOpt);
    }

    return std::tuple<int, int, int>{
        longFlagIndex < argc ? longFlagIndex : shortFlagIndex, longFlagIndex, shortFlagIndex
    };
  }

 protected:
  CL_Argument(const char *longOpt, char shortOpt, const std::string &d, bool required)
      : longOpt{longOpt}, shortOpt{shortOpt}, description{d}, m_required{required} {
    if (!longOpt && !shortOpt) {
      throw CLIException{
          "CL_Argument expects either a longOpt for flags of the form --flag or a shortOpt for flags of the "
          "form "
          "-f."
      };
    }

    if (longOpt != nullptr && longOpt[0] == '-') {
      throw CLIException{
          "Please omit the - sign at the start of the given longOpt. If you want to handle the flag --arg just "
          "provide the longOpt \"arg\"!"
      };
    }
  }

 private:
  bool m_required;
};

template <typename ValueType>
class OptionalArgument : public CL_Argument<ValueType> {
 public:
  OptionalArgument(
      const char *longOpt = nullptr,
      char shortOpt = '\0',
      const std::string &d = std::string{},
      std::shared_ptr<ValueType> defaultVal = std::shared_ptr<ValueType>{}
  )
      : CL_Argument<ValueType>(longOpt, shortOpt, d, false) {
    this->value = defaultVal;
  }
};

template <typename ValueType>
class RequiredArgument : public CL_Argument<ValueType> {
  static_assert(
      std::integral_constant<bool, !std::is_same<ValueType, bool>::value>::value,
      "An argument of a boolean type cannot be required. If it does not exist the value is assumed "
      "to be false!"
  );

 public:
  RequiredArgument(const char *longOpt = nullptr, char shortOpt = '\0', const std::string &d = std::string{})
      : CL_Argument<ValueType>(longOpt, shortOpt, d, true) {}
};

template <typename ArgNames, ArgNames Name, typename ValueType>
class NamedOptionalArgument : public OptionalArgument<ValueType> {
 public:
  NamedOptionalArgument(
      const char *longOpt = nullptr,
      char shortOpt = '\0',
      const std::string &d = std::string{},
      std::shared_ptr<ValueType> defaultVal = std::shared_ptr<ValueType>{}
  )
      : OptionalArgument<ValueType>(longOpt, shortOpt, d, defaultVal) {}
};

template <typename ArgNames, ArgNames Name, typename ValueType>
class NamedRequiredArgument : public RequiredArgument<ValueType> {
  static_assert(
      std::integral_constant<bool, !std::is_same<ValueType, bool>::value>::value,
      "An argument of a boolean type cannot be required. If it does not exist the value is assumed "
      "to be false!"
  );

 public:
  NamedRequiredArgument(const char *longOpt = nullptr, char shortOpt = '\0', const std::string &d = std::string{})
      : RequiredArgument<ValueType>(longOpt, shortOpt, d) {}
};

template <typename ValueType>
void parseArgFromCL(int argc, const char **argv, CL_Argument<ValueType> &arg) {
  std::tuple<int, int, int> indices = arg.getFlagIndex(argc, argv);
  int flagIndex = std::get<0>(indices);
  int longIndex = std::get<1>(indices);

  if (flagIndex < argc) {
    if (flagIndex + 1 >= argc || std::string{argv[flagIndex + 1]}.substr(0, 1).compare("-") == 0) {
      std::string message{"Flag %s is not followed by a value!"};
      throw FlagException(message, "", longIndex < argc ? arg.longOpt : "", longIndex >= argc ? arg.shortOpt : '\0');
    }

    try {
      arg.value = parse<ValueType>(argv[flagIndex + 1]);
    } catch (std::exception const &err) {
      std::string message{"Ran into an error when parsing the value for flag %s. "};
      throw FlagException(
          message, err.what(), longIndex < argc ? arg.longOpt : "", longIndex >= argc ? arg.shortOpt : '\0'
      );
    }

  } else if (arg.isRequired()) {
    if (arg.hasLong() && arg.hasShort()) {
      std::string errMessage{"Missing a required function argument (%s, %s)!"};
      throw FlagException(errMessage, "", arg.longOpt, arg.shortOpt);
    } else if (arg.hasLong()) {
      std::string errMessage{"Missing a required function argument (%s)!"};
      throw FlagException(errMessage, "", arg.longOpt);
    } else {
      std::string errMessage{"Missing a required function argument (%s)!"};
      throw FlagException(errMessage, "", "", arg.shortOpt);
    }
  }
}

template <>
void parseArgFromCL(int argc, const char **argv, CL_Argument<bool> &arg);
};  // namespace cpp_cli

#endif