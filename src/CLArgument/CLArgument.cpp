#include "CLArgument.h"

namespace cpp_cli {

// TODO: provide default parsers for typical types: e.g. double etc
// TODO: maybe do some error handling and validation of when parsing the types
template <>
std::shared_ptr<int> parse<int>(const char *val) {
  std::string toParse{val};
  return std::make_shared<int>(std::stoi(toParse));
}
template <>
std::shared_ptr<float> parse<float>(const char *val) {
  return std::make_shared<float>(atof(val));
}
template <>
std::shared_ptr<std::string> parse<std::string>(const char *val) {
  return std::make_shared<std::string>(val);
}

template <>
void parseArgFromCL(int argc, const char **argv, CL_Argument<bool> &arg) {
  std::tuple<int, int, int> indices = arg.getFlagIndex(argc, argv);
  int flagIndex = std::get<0>(indices);
  int longIndex = std::get<1>(indices);

  if (flagIndex < argc) {
    if (flagIndex + 1 < argc && std::string{argv[flagIndex + 1]}.substr(0, 1).compare("-") != 0) {
      std::string errMessage =
          std::string{"Flag "} +
          (longIndex < argc ? std::string{"--"}.append(arg.longOpt) : std::string{"-"}.append(1, arg.shortOpt)) +
          " which is of a bool type should not be followed by a value!";

      throw CLIException{errMessage};
    }

    arg.value = std::make_shared<bool>(true);
  } else {
    arg.value = std::make_shared<bool>(false);
  }
}

}  // namespace cpp_cli