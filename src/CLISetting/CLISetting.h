#ifndef CLI_SETTING_H
#define CLI_SETTING_H

#include <functional>
#include <optional>
#include <string>

#include "CLIException/CLIException.h"

namespace cpp_cli {

template <auto SettingName, typename SettingValueType>
class CLISettingBuilder;

template <auto SettingName, typename SettingValueType>
class CLISetting {
 public:
  bool hasLong() const { return m_long != nullptr; }
  bool hasShort() const { return m_short != '\0'; }
  bool hasDescription() const { return !m_description.empty(); }

  const char *getLong() const { return m_long; }
  char getShort() const { return m_short; }
  const std::string &getDescription() const { return m_description; }

  bool hasDefault() const { return m_defaultValue.has_value(); }
  const SettingValueType &getDefault() const { return m_defaultValue.value(); }

  void validate(const SettingValueType &value, const std::string &longFlag, char shortFlag) const {
    if (m_validator != nullptr) {
      m_validator(value, longFlag, shortFlag);
    }
  }

 private:
  CLISetting() {}

  const char *m_long{nullptr};
  char m_short{0};
  std::string m_description{""};

  std::optional<SettingValueType> m_defaultValue{};
  std::function<void(const SettingValueType &, const std::string &, char)> m_validator{nullptr};

  friend class CLISettingBuilder<SettingName, SettingValueType>;
};

template <auto SettingName, typename SettingValueType>
class CLISettingBuilder {
 public:
  CLISettingBuilder &addLong(const char *longOpt) {
    if (longOpt != nullptr && longOpt[0] == '-') {
      throw CLIException{
          "Please omit the - sign at the start of the given longOpt. If you want to handle the flag --arg just "
          "provide the longOpt \"arg\"!"
      };
    }
    if (!std::string{"help"}.compare(longOpt)) {
      throw CLIException{"The --help flag is automatically handled. Please use a different flag name."};
    }

    m_setting.m_long = longOpt;
    return *this;
  }

  CLISettingBuilder &addShort(char shortOpt) {
    m_setting.m_short = shortOpt;
    return *this;
  }

  CLISettingBuilder &addDescription(const std::string &d) {
    m_setting.m_description = d;
    return *this;
  }

  CLISettingBuilder &addDefault(const SettingValueType &d) {
    m_setting.m_defaultValue = d;
    return *this;
  }

  CLISettingBuilder &addValidator(std::function<void(const SettingValueType &, const std::string &, char)> validator) {
    m_setting.m_validator = validator;
    return *this;
  }

  CLISetting<SettingName, SettingValueType> build() {
    if (!m_setting.m_long && !m_setting.m_short) {
      throw CLIException{
          "CLISetting expects either a longOpt for flags of the form --flag or a shortOpt for flags of the "
          "form "
          "-f."
      };
    }

    return m_setting;
  };

 private:
  CLISetting<SettingName, SettingValueType> m_setting{};
};

namespace cpp_cli_internal {
template <auto SettingName, typename SettingValueType>
int getFlagIndex(const CLISetting<SettingName, SettingValueType> &setting, int argc, const char **argv) {
  int longFlagIndex = setting.hasLong() ? 1 : argc;
  for (; longFlagIndex < argc; ++longFlagIndex) {
    std::string a = std::string{"--"}.append(setting.getLong());
    std::string b{argv[longFlagIndex]};
    if (a.compare(b) == 0) break;
  }
  int shortFlagIndex = setting.hasShort() ? 1 : argc;
  for (; shortFlagIndex < argc; ++shortFlagIndex) {
    std::string a = std::string{"-"}.append(1, setting.getShort());
    std::string b{argv[shortFlagIndex]};
    if (a.compare(b) == 0) break;
  }

  if (longFlagIndex < argc && shortFlagIndex < argc) {
    std::string errMessage{"Provided values for both the short (%s) and long (%s) version of a program parameter!"};
    throw FlagException(errMessage, "", setting.getLong(), setting.getShort());
  }

  return longFlagIndex < argc ? longFlagIndex : shortFlagIndex;
}
template <auto SettingName, typename SettingValueType>
std::optional<std::string> getFlagValue(
    const CLISetting<SettingName, SettingValueType> &setting, int argc, const char **argv
) {
  int flagIndex = getFlagIndex(setting, argc, argv);

  if (flagIndex >= argc) return {};

  if (flagIndex + 1 >= argc || std::string{argv[flagIndex + 1]}.substr(0, 2).compare("--") == 0) {
    return {};
  }

  return {argv[flagIndex + 1]};
}
}  // namespace cpp_cli_internal

}  // namespace cpp_cli

#endif
