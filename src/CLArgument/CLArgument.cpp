#include "CLArgument.h"

#include <iostream>
#include <limits>

#include "../util/CLIException.h"

namespace cpp_cli {

template <>
std::shared_ptr<int> parse<int>(const char *val) {
  std::string toParse{val};
  std::size_t end;
  try {
    int parsed = std::stoi(toParse, &end);
    if (end != toParse.length()) throw std::invalid_argument("The given argument value contains non numeric elements");
    return std::make_shared<int>(parsed);
  } catch (std::invalid_argument const &err) {
    std::string msg{"Invalid argument ("};
    msg.append(val).append(") for an argument of type int.");
    throw CLIException(msg);
  } catch (std::out_of_range const &err) {
    std::string msg{"Value ("};
    msg.append(val).append(") does not fit into an argument of type int.");
    throw CLIException(msg);
  }
}
template <>
std::shared_ptr<unsigned int> parse<unsigned int>(const char *val) {
  std::string toParse{val};
  std::size_t end;
  unsigned long parsed;
  try {
    parsed = std::stoul(toParse, &end);
    if (end != toParse.length()) throw std::invalid_argument("The given argument value contains non numeric elements");
  } catch (std::invalid_argument const &err) {
    std::string msg{"Invalid argument ("};
    msg.append(val).append(") for an argument of type unsigned int.");
    throw CLIException(msg);
  } catch (std::out_of_range const &err) {
    std::string msg{"Value ("};
    msg.append(val).append(") does not fit into an argument of type unsigned int.");
    throw CLIException(msg);
  }

  if (toParse.find('-') != std::string::npos)
    throw std::invalid_argument("Negative number not allowed as value of an unsigned int.");

  if (parsed > std::numeric_limits<unsigned int>::max())
    throw std::range_error("The given value exceeds the limits of an unsigned int!");

  return std::make_shared<unsigned int>(parsed);
}
template <>
std::shared_ptr<float> parse<float>(const char *val) {
  std::string toParse{val};
  std::size_t end;
  try {
    float parsed = std::stof(toParse, &end);
    if (end != toParse.length()) throw std::invalid_argument("The given argument value contains non numeric elements");
    return std::make_shared<float>(parsed);
  } catch (std::invalid_argument const &err) {
    std::string msg{"Invalid argument ("};
    msg.append(val).append(") for an argument of type float.");
    throw CLIException(msg);
  } catch (std::out_of_range const &err) {
    std::string msg{"Value ("};
    msg.append(val).append(") does not fit into an argument of type float.");
    throw CLIException(msg);
  }
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
      std::string message{"Flag %s which is of a bool type should not be followed by a value!"};
      throw FlagException(message, "", longIndex < argc ? arg.longOpt : "", longIndex >= argc ? arg.shortOpt : '\0');
    }

    arg.value = std::make_shared<bool>(true);
  } else {
    arg.value = std::make_shared<bool>(false);
  }
}

}  // namespace cpp_cli