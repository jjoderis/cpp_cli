#ifndef CLI_SETTING_PARSER_H
#define CLI_SETTING_PARSER_H

#include <functional>
#include <optional>
#include <type_traits>
#include <vector>

#include "CLArgument/CLArgument.h"
#include "CLIException/CLIException.h"
#include "ProgramSettings/ProgramSettings.h"
#include "ValueParser/ValueParser.h"

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

template <typename SettingValueType>
std::optional<SettingValueType> parseCLISetting(
    int argc, const char **argv, const CLArgument &arg, std::vector<bool> &handledFlags
) {
  int argFlagIndex = getFlagIndex(arg, argc, argv);

  if (argFlagIndex < argc) {
    std::optional<std::string> valueString = getFlagValue(arg, argc, argv);

    std::string flag = argv[argFlagIndex];
    bool isLongFlag = flag.rfind("--", 0) == 0;

    if (valueString.has_value()) {
      handledFlags[argFlagIndex] = true;
      handledFlags[argFlagIndex + 1] = true;
      try {
        return {parse<SettingValueType>(valueString.value())};
      } catch (const cpp_cli::CLIException &e) {
        std::string message{"Ran into an error when parsing the value for flag %s. (Original Error: "};
        message.append(e.what());
        message.append(")");
        throw FlagException(message, "", isLongFlag ? arg.getLong() : "", !isLongFlag ? arg.getShort() : 0);
      }
    } else {
      std::string message{"Flag %s is missing a value!"};
      throw FlagException(message, "", isLongFlag ? arg.getLong() : "", !isLongFlag ? arg.getShort() : 0);
    }
  }

  return {};
}

// boolean typed settings are handled differently
template <>
std::optional<bool> parseCLISetting(
    int argc, const char **argv, const CLArgument &arg, std::vector<bool> &handledFlags
);

template <auto SettingName, typename SettingValueType, typename... AllSettings, typename...>
void parseProgramSettingsFromCLImpl(
    int argc,
    const char **argv,
    std::vector<bool> &handledFlags,
    ProgramSettings<AllSettings...> &progSettings,
    const CLISetting<SettingName, SettingValueType> &setting

) {
  const CLArgument &arg = setting.getArg();

  std::optional<SettingValueType> parsedValue = parseCLISetting<SettingValueType>(argc, argv, arg, handledFlags);

  if (parsedValue.has_value()) {
    int argFlagIndex = getFlagIndex(arg, argc, argv);
    if (argFlagIndex < argc) {
      std::string flag = argv[argFlagIndex];
      bool isLongFlag = flag.rfind("--", 0) == 0;
      setting.validate(parsedValue.value(), isLongFlag ? arg.getLong() : "", !isLongFlag ? arg.getShort() : 0);
    }
    progSettings.template set<SettingName>(parsedValue.value());
  } else if (setting.hasDefault()) {
    progSettings.template set<SettingName>(setting.getDefault());
  } else {
    // if there is no default value we consider the setting to be required from the user calling the program and throw
    // when it is missing
    if (arg.hasLong() && arg.hasShort()) {
      std::string errMessage{"Missing a required program argument (%s, %s)!"};
      throw FlagException(errMessage, "", arg.getLong(), arg.getShort());
    } else if (arg.hasLong()) {
      std::string errMessage{"Missing a required program argument (%s)!"};
      throw FlagException(errMessage, "", arg.getLong());
    } else {
      std::string errMessage{"Missing a required program argument (%s)!"};
      throw FlagException(errMessage, "", "", arg.getShort());
    }
  }
}

template <auto SettingName, typename SettingValueType, typename... AllArgs, typename... CurrArgs>
void parseProgramSettingsFromCLImpl(
    int argc,
    const char **argv,
    std::vector<bool> &handledFlags,
    ProgramSettings<AllArgs...> &progArgs,
    const CLISetting<SettingName, SettingValueType> &arg,
    const CurrArgs &...args
) {
  parseProgramSettingsFromCLImpl(argc, argv, handledFlags, progArgs, arg);
  parseProgramSettingsFromCLImpl(argc, argv, handledFlags, progArgs, args...);
}

template <typename... Settings>
ProgramSettings<Settings...> parseProgramSettingsFromCL(int argc, const char **argv, const Settings &...settings) {
  std::vector<bool> handledFlags(argc);

  for (int i = 1; i < argc; ++i) {
    handledFlags[i] = false;
  }

  ProgramSettings<Settings...> progSettings{};

  parseProgramSettingsFromCLImpl(argc, argv, handledFlags, progSettings, settings...);

  return progSettings;
}

}  // namespace cpp_cli

#endif
