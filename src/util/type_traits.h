#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H

#include <tuple>
#include <typeinfo>
#include <utility>

#include "../CLArgument/CLArgument.h"

namespace cpp_cli {

// check for a combination of an enum with a argument type
template <typename T>
struct is_named_arg : std::false_type {};
template <typename ArgNames, ArgNames ArgName, typename T>
struct is_named_arg<NamedRequiredArgument<ArgNames, ArgName, T>> : std::true_type {};
template <typename ArgNames, ArgNames ArgName, typename T>
struct is_named_arg<NamedOptionalArgument<ArgNames, ArgName, T>> : std::true_type {};

// check if all elements in a parameter pack are named args
template <typename... Args>
struct is_named_arg_pack : std::false_type {};
template <typename T>
struct is_named_arg_pack<T> : std::integral_constant<bool, is_named_arg<T>::value> {};
template <typename T, typename... Rest>
struct is_named_arg_pack<T, Rest...>
    : std::conditional<is_named_arg<T>::value, is_named_arg_pack<Rest...>, std::false_type>::type {};

// get the information of a given named arg (the enum value used as its name, the enum type of the name, the type of the
// value stored in the arg)
template <typename T>
struct named_arg_info;
template <typename ArgNames, ArgNames Name, typename T>
struct named_arg_info<NamedRequiredArgument<ArgNames, Name, T>> {
  using arg_name_type = ArgNames;
  static const ArgNames arg_name{Name};
  using arg_value_type = T;
};
template <typename ArgNames, ArgNames Name, typename T>
struct named_arg_info<NamedOptionalArgument<ArgNames, Name, T>> {
  using arg_name_type = ArgNames;
  static const ArgNames arg_name{Name};
  using arg_value_type = T;
};

/** find the index of a the arg with a specific name */
template <int index, typename ArgNames, ArgNames Name, typename...>
struct arg_index_by_name_impl : std::integral_constant<int, -1> {};
template <int index, typename ArgNames, ArgNames Name, typename T, typename... Args>
struct arg_index_by_name_impl<index, ArgNames, Name, T, Args...>
    : arg_index_by_name_impl<index + 1, ArgNames, Name, Args...> {};
template <int index, typename ArgNames, ArgNames Name, typename T, typename... Args>
struct arg_index_by_name_impl<index, ArgNames, Name, NamedRequiredArgument<ArgNames, Name, T>, Args...>
    : std::integral_constant<int, index> {};
template <int index, typename ArgNames, ArgNames Name, typename T, typename... Args>
struct arg_index_by_name_impl<index, ArgNames, Name, NamedOptionalArgument<ArgNames, Name, T>, Args...>
    : std::integral_constant<int, index> {};

template <typename ArgNames, ArgNames Name, typename... Args>
struct arg_index_by_name : arg_index_by_name_impl<0, ArgNames, Name, Args...> {};

/** get an arg from the list by its name */
template <typename ArgNames, ArgNames Name, typename...>
struct arg_by_name_impl {};
template <typename ArgNames, ArgNames Name, typename T, typename... Args>
struct arg_by_name_impl<ArgNames, Name, T, Args...> : arg_by_name_impl<ArgNames, Name, Args...> {};
template <typename ArgNames, ArgNames Name, typename T, typename... Args>
struct arg_by_name_impl<ArgNames, Name, NamedRequiredArgument<ArgNames, Name, T>, Args...> {
  using type = NamedRequiredArgument<ArgNames, Name, T>;
};
template <typename ArgNames, ArgNames Name, typename T, typename... Args>
struct arg_by_name_impl<ArgNames, Name, NamedOptionalArgument<ArgNames, Name, T>, Args...> {
  using type = NamedOptionalArgument<ArgNames, Name, T>;
};

template <typename ArgNames, ArgNames Name, typename... Args>
struct arg_by_name : arg_by_name_impl<ArgNames, Name, Args...> {};

// check if every arg in the given pack has a different name
template <typename... Args>
struct args_have_unique_names : std::true_type {};
template <typename T, typename... Args>
struct args_have_unique_names<T, Args...>
    : std::conditional<
          arg_index_by_name<typename named_arg_info<T>::arg_name_type, named_arg_info<T>::arg_name, Args...>::value ==
              -1,
          args_have_unique_names<Args...>,
          std::false_type>::type {};

// template <typename T> using prevent_deduction = typename std::common_type<T>::type;

};  // namespace cpp_cli

#endif