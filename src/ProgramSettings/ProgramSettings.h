#ifndef PROGRAM_SETTINGS_H
#define PROGRAM_SETTINGS_H

#include <tuple>

#include "../util/type_traits.h"

namespace cpp_cli {

template <typename... Settings>
class ProgramSettings {
  static_assert(
      cpp_cli_internal::is_setting_pack<Settings...>::value,
      "The program settings only accept template types of the form SettingType<NameEnum, ValueType>!"
  );
  static_assert(
      cpp_cli_internal::settings_have_unique_names<Settings...>::value, "Please use unique names for every setting!"
  );

 public:
  template <
      auto SettingName,
      int SettingIndex = cpp_cli_internal::setting_index_by_name<SettingName, Settings...>::value,
      typename SettingValueType = typename cpp_cli_internal::setting_info<
          typename cpp_cli_internal::setting_by_name<SettingName, Settings...>::type>::setting_value_type>
  void set(const SettingValueType& val) {
    std::get<SettingIndex>(m_settings) = val;
  }

  template <
      auto SettingName,
      int SettingIndex = cpp_cli_internal::setting_index_by_name<SettingName, Settings...>::value,
      typename SettingValueType = typename cpp_cli_internal::setting_info<
          typename cpp_cli_internal::setting_by_name<SettingName, Settings...>::type>::setting_value_type>
  SettingValueType& get() {
    return std::get<SettingIndex>(m_settings);
  }

  template <
      auto SettingName,
      int SettingIndex = cpp_cli_internal::setting_index_by_name<SettingName, Settings...>::value,
      typename SettingValueType = typename cpp_cli_internal::setting_info<
          typename cpp_cli_internal::setting_by_name<SettingName, Settings...>::type>::setting_value_type>
  const SettingValueType& get() const {
    return std::get<SettingIndex>(m_settings);
  }

 private:
  std::tuple<cpp_cli_internal::get_setting_type_t<Settings>...> m_settings;
};

}  // namespace cpp_cli

#endif
