#ifndef CLI_SETTING_PARSER_H
#define CLI_SETTING_PARSER_H

#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>

#include "CLIException/CLIException.h"
#include "CLISetting/CLISetting.h"
#include "ProgramSettings/ProgramSettings.h"
#include "ValueParser/ValueParser.h"

namespace cpp_cli {

template <auto SettingName, typename SettingValueType>
struct CLISettingParser {
  static std::optional<SettingValueType> parse(
      int argc,
      const char **argv,
      const CLISetting<SettingName, SettingValueType> &setting,
      std::vector<bool> &handledFlags
  ) {
    int argFlagIndex = getFlagIndex(setting, argc, argv);

    if (argFlagIndex < argc) {
      std::optional<std::string> valueString = getFlagValue(setting, argc, argv);

      std::string flag = argv[argFlagIndex];
      bool isLongFlag = flag.rfind("--", 0) == 0;

      if (valueString.has_value()) {
        handledFlags[argFlagIndex] = true;
        handledFlags[argFlagIndex + 1] = true;
        try {
          return {cpp_cli::parse<SettingValueType>(valueString.value())};
        } catch (const cpp_cli::CLIException &e) {
          std::string message{"Ran into an error when parsing the value for flag %s. (Original Error: "};
          message.append(e.what());
          message.append(")");
          throw FlagException(message, "", isLongFlag ? setting.getLong() : "", !isLongFlag ? setting.getShort() : 0);
        }
      } else {
        std::string message{"Flag %s is missing a value!"};
        throw FlagException(message, "", isLongFlag ? setting.getLong() : "", !isLongFlag ? setting.getShort() : 0);
      }
    }

    return {};
  }
};

// boolean typed settings are handled differently
template <auto SettingName>
struct CLISettingParser<SettingName, bool> {
  static std::optional<bool> parse(
      int argc, const char **argv, const CLISetting<SettingName, bool> &setting, std::vector<bool> &handledFlags
  ) {
    int argFlagIndex = getFlagIndex(setting, argc, argv);

    // for boolean typed settings the existance of the flag is enough to set it to true
    if (argFlagIndex < argc) {
      bool followedBySomething = argFlagIndex < argc - 1;
      if (followedBySomething) {
        std::string followingArg{argv[argFlagIndex + 1]};

        if (followingArg.rfind("-", 0) != 0) {
          // if the type of the setting is bool we consider it to be true if the flag is present and we don't expect a
          // following value
          std::string message{"Flag %s which is of a bool type should not be followed by a value!"};
          std::string flag = argv[argFlagIndex];
          bool isLongFlag = flag.rfind("--", 0) == 0;
          throw FlagException(message, "", isLongFlag ? setting.getLong() : "", !isLongFlag ? setting.getShort() : 0);
        }
      }

      handledFlags[argFlagIndex] = true;
      return std::optional<bool>{true};
    }

    return std::optional<bool>{false};
  }
};

template <auto SettingName, typename SettingValueType, typename... AllSettings, typename...>
void parseProgramSettingsFromCLImpl(
    int argc,
    const char **argv,
    std::vector<bool> &handledFlags,
    ProgramSettings<AllSettings...> &progSettings,
    const CLISetting<SettingName, SettingValueType> &setting
) {
  std::optional<SettingValueType> parsedValue =
      CLISettingParser<SettingName, SettingValueType>::parse(argc, argv, setting, handledFlags);

  if (parsedValue.has_value()) {
    int argFlagIndex = getFlagIndex(setting, argc, argv);
    if (argFlagIndex < argc) {
      std::string flag = argv[argFlagIndex];
      bool isLongFlag = flag.rfind("--", 0) == 0;
      setting.validate(parsedValue.value(), isLongFlag ? setting.getLong() : "", !isLongFlag ? setting.getShort() : 0);
    }
    progSettings.template set<SettingName>(parsedValue.value());
  } else if (setting.hasDefault()) {
    progSettings.template set<SettingName>(setting.getDefault());
  } else {
    // if there is no default value we consider the setting to be required from the user calling the program and throw
    // when it is missing
    if (setting.hasLong() && setting.hasShort()) {
      std::string errMessage{"Missing a required program argument (%s, %s)!"};
      throw FlagException(errMessage, "", setting.getLong(), setting.getShort());
    } else if (setting.hasLong()) {
      std::string errMessage{"Missing a required program argument (%s)!"};
      throw FlagException(errMessage, "", setting.getLong());
    } else {
      std::string errMessage{"Missing a required program argument (%s)!"};
      throw FlagException(errMessage, "", "", setting.getShort());
    }
  }
}

template <auto SettingName, typename SettingValueType, typename... AllSettings, typename... RemainingSettings>
void parseProgramSettingsFromCLImpl(
    int argc,
    const char **argv,
    std::vector<bool> &handledFlags,
    ProgramSettings<AllSettings...> &progArgs,
    const CLISetting<SettingName, SettingValueType> &setting,
    const RemainingSettings &...rest
) {
  parseProgramSettingsFromCLImpl(argc, argv, handledFlags, progArgs, setting);
  parseProgramSettingsFromCLImpl(argc, argv, handledFlags, progArgs, rest...);
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
  std::string flagString{};
  if (setting.hasShort()) {
    flagString.append("-").append(1, setting.getShort());
    if (setting.hasLong()) flagString.append(", ");
  }
  if (setting.hasLong()) flagString.append("--").append(setting.getLong());

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
  helpString.append("  ");
  std::string flagString{getSettingFlagsString(setting)};
  if (flagString.length() < maxFlagsStringLength) flagString.append(maxFlagsStringLength - flagString.length(), ' ');
  helpString.append(flagString);
  if (setting.hasDescription()) helpString.append("     ").append(setting.getDescription());
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
