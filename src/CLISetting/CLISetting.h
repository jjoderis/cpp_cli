#ifndef CLI_SETTING_H
#define CLI_SETTING_H

#include <functional>

#include "CLArgument/CLArgument.h"

namespace cpp_cli {
template <auto SettingName, typename SettingValueType>
class CLISetting {
 public:
  CLISetting() = delete;
  CLISetting(const CLArgument &arg) : m_arg{arg} {}
  CLISetting(const CLArgument &arg, const SettingValueType &defaultValue) : m_arg{arg}, m_defaultValue{defaultValue} {}
  CLISetting(const CLArgument &arg, std::function<void(const SettingValueType &, const std::string &, char)> validator)
      : m_arg{arg}, m_validator{validator} {}
  CLISetting(
      const CLArgument &arg,
      const SettingValueType &defaultValue,
      std::function<void(const SettingValueType &, const std::string &, char)> validator
  )
      : m_arg{arg}, m_defaultValue{defaultValue}, m_validator{validator} {}

  const CLArgument &getArg() const { return m_arg; }
  bool hasDefault() const { return m_defaultValue.has_value(); }
  const SettingValueType &getDefault() const { return m_defaultValue.value(); }

  void validate(const SettingValueType &value, const std::string &longFlag, char shortFlag) const {
    if (m_validator != nullptr) {
      m_validator(value, longFlag, shortFlag);
    }
  }

 private:
  CLArgument m_arg;
  std::optional<SettingValueType> m_defaultValue;
  std::function<void(const SettingValueType &, const std::string &, char)> m_validator;
};
}  // namespace cpp_cli

#endif
