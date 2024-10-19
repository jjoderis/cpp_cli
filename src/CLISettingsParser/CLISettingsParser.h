#ifndef CLI_SETTING_PARSER_H
#define CLI_SETTING_PARSER_H

#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>

#include "CLArgument/CLArgument.h"
#include "CLIException/CLIException.h"
#include "CLISetting/CLISetting.h"
#include "ProgramSettings/ProgramSettings.h"
#include "ValueParser/ValueParser.h"

namespace cpp_cli {

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
    if (!std::string{argv[i]}.compare("--help"))
      handledFlags[i] = true;
    else
      handledFlags[i] = false;
  }

  ProgramSettings<Settings...> progSettings{};

  if (argc == 2 && !std::string{argv[1]}.compare("--help")) {
    std::string helpString{};

    size_t maxFlagStringLength = getMaxFlagsStringLength(settings...);
    addToHelpString(helpString, maxFlagStringLength, settings...);

    std::cout << helpString;
    exit(EXIT_SUCCESS);
  }

  parseProgramSettingsFromCLImpl(argc, argv, handledFlags, progSettings, settings...);

  return progSettings;
}

template <auto SettingName, typename SettingValueType>
std::string getSettingFlagsString(const CLISetting<SettingName, SettingValueType> &setting) {
  auto arg = setting.getArg();
  std::string flagString{};
  if (arg.hasShort()) {
    flagString.append("-").append(1, arg.getShort());
    if (arg.hasLong()) flagString.append(", ");
  }
  if (arg.hasLong()) flagString.append("--").append(arg.getLong());

  return flagString;
}

template <auto SettingName, typename SettingValueType, typename...>
size_t getMaxFlagsStringLength(const CLISetting<SettingName, SettingValueType> &setting) {
  return getSettingFlagsString(setting).length();
}
template <auto SettingName, typename SettingValueType, typename... Rest>
size_t getMaxFlagsStringLength(const CLISetting<SettingName, SettingValueType> &setting, const Rest &...rest) {
  return std::max(getSettingFlagsString(setting).length(), getMaxFlagsStringLength(rest...));
}

template <auto SettingName, typename SettingValueType, typename...>
void addToHelpString(
    std::string &helpString, size_t maxFlagsStringLength, const CLISetting<SettingName, SettingValueType> &setting
) {
  CLArgument arg{setting.getArg()};
  helpString.append("  ");
  std::string flagString{getSettingFlagsString(setting)};
  if (flagString.length() < maxFlagsStringLength) flagString.append(maxFlagsStringLength - flagString.length(), ' ');
  helpString.append(flagString);
  if (arg.hasDescription()) helpString.append("     ").append(arg.getDescription());
  helpString.append(1, '\n');
}
template <auto SettingName, typename SettingValueType, typename... Rest>
void addToHelpString(
    std::string &helpString,
    size_t maxFlagsStringLenght,
    const CLISetting<SettingName, SettingValueType> &setting,
    const Rest &...rest
) {
  addToHelpString(helpString, maxFlagsStringLenght, setting);
  addToHelpString(helpString, maxFlagsStringLenght, rest...);
}

}  // namespace cpp_cli

#endif
