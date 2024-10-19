#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H

#include <type_traits>

namespace cpp_cli {

// check if the given type defines a Setting
template <typename T>
struct is_setting : std::false_type {};
template <template <auto, typename> typename Setting, auto SettingName, typename T>
struct is_setting<Setting<SettingName, T>> : std::true_type {};

// check if all elements in a parameter pack are settings
template <typename... Args>
struct is_setting_pack : std::false_type {};
template <typename T>
struct is_setting_pack<T> : std::integral_constant<bool, is_setting<T>::value> {};
template <typename T, typename... Rest>
struct is_setting_pack<T, Rest...>
    : std::conditional<is_setting<T>::value, is_setting_pack<Rest...>, std::false_type>::type {};

template <typename T>
struct get_setting_type;

template <template <auto, typename, typename...> class t, auto Name, typename T, typename... Settings>
struct get_setting_type<t<Name, T, Settings...>> {
  typedef T type_t;
};
template <typename T>
using get_setting_type_t = typename get_setting_type<T>::type_t;

/** find the index of a the setting with a specific name */
template <int index, auto TargetName, typename...>
struct setting_index_by_name_impl : std::integral_constant<int, -1> {};
template <int index, auto TargetName, typename T, typename... Rest>
struct setting_index_by_name_impl<index, TargetName, T, Rest...>
    : setting_index_by_name_impl<index + 1, TargetName, Rest...> {};
template <
    int index,
    auto TargetName,
    template <auto, typename> typename Setting,
    typename SettingType,
    typename... Rest>
struct setting_index_by_name_impl<index, TargetName, Setting<TargetName, SettingType>, Rest...>
    : std::integral_constant<int, index>::type {};

template <auto SettingName, typename... Settings>
struct setting_index_by_name : setting_index_by_name_impl<0, SettingName, Settings...> {};

template <template <auto, typename> typename Setting, auto SettingName, typename SettingValueType>
struct SettingTypeWrapper {
  using type = Setting<SettingName, SettingValueType>;
};

/** get a setting from the list by its name */
template <auto TargetName, typename...>
struct setting_by_name_impl {};
template <auto TargetName, typename T, typename... Rest>
struct setting_by_name_impl<TargetName, T, Rest...> : setting_by_name_impl<TargetName, Rest...> {};
template <auto TargetName, template <auto, typename> typename Setting, typename SettingType, typename... Rest>
struct setting_by_name_impl<TargetName, Setting<TargetName, SettingType>, Rest...> {
  using type = Setting<TargetName, SettingType>;
};

template <auto SettingName, typename... Settings>
struct setting_by_name : setting_by_name_impl<SettingName, Settings...> {};

// get the information of a given Setting (the enum value used as its name, the enum type of the name, the type of the
// value stored for the setting)
template <typename T>
struct setting_info;
template <template <auto, typename> typename Setting, auto SettingName, typename SettingType>
struct setting_info<Setting<SettingName, SettingType>> {
  using setting_name_type = decltype(SettingName);
  static const decltype(SettingName) setting_name{SettingName};
  using setting_value_type = SettingType;
};

// check if every setting in the given pack has a different name
template <typename... Settings>
struct settings_have_unique_names : std::true_type {};
template <typename T, typename... Settings>
struct settings_have_unique_names<T, Settings...>
    : std::conditional<
          setting_index_by_name<setting_info<T>::setting_name, Settings...>::value == -1,
          settings_have_unique_names<Settings...>,
          std::false_type>::type {};

};  // namespace cpp_cli

#endif
