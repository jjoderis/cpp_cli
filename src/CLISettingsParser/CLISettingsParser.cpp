#include "CLISettingsParser.h"

namespace cpp_cli {
template <>
std::optional<bool> parseCLISetting(
    int argc, const char **argv, const CLArgument &arg, std::vector<bool> &handledFlags
) {
  int argFlagIndex = getFlagIndex(arg, argc, argv);

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
        throw FlagException(message, "", isLongFlag ? arg.getLong() : "", !isLongFlag ? arg.getShort() : 0);
      }
    }

    handledFlags[argFlagIndex] = true;
    return std::optional<bool>{true};
  }

  return std::optional<bool>{false};
}
}  // namespace cpp_cli
