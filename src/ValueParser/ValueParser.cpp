#include "ValueParser.h"

#include <filesystem>
#include <limits>

#include "../CLIException/CLIException.h"

namespace cpp_cli {
namespace cpp_cli_internal {

// defining a few default parsers for frequent types

// INT
template <>
int parse<int>(const std::string &toParse) {
  std::size_t end;
  try {
    int parsed = std::stoi(toParse, &end);
    if (end != toParse.length()) throw std::invalid_argument("The given argument value contains non numeric elements");
    return parsed;
  } catch (std::invalid_argument const &err) {
    std::string msg{"Invalid argument ("};
    msg.append(toParse).append(") for an argument of type int.");
    throw CLIException(msg);
  } catch (std::out_of_range const &err) {
    std::string msg{"Value ("};
    msg.append(toParse).append(") does not fit into an argument of type int.");
    throw CLIException(msg);
  }
}

// UNSIGNED INT
template <>
unsigned int parse<unsigned int>(const std::string &toParse) {
  std::size_t end;
  unsigned long parsed;
  try {
    parsed = std::stoul(toParse, &end);
    if (end != toParse.length()) throw std::invalid_argument("The given argument value contains non numeric elements");
  } catch (std::invalid_argument const &err) {
    std::string msg{"Invalid argument ("};
    msg.append(toParse).append(") for an argument of type unsigned int.");
    throw CLIException(msg);
  } catch (std::out_of_range const &err) {
    std::string msg{"Value ("};
    msg.append(toParse).append(") does not fit into an argument of type unsigned int.");
    throw CLIException(msg);
  }

  if (toParse.find('-') != std::string::npos)
    throw CLIException("Negative number not allowed as value of an unsigned int.");

  if (parsed > std::numeric_limits<unsigned int>::max())
    throw CLIException("The given value exceeds the limits of an unsigned int!");

  return parsed;
}

// FLOAT
template <>
float parse<float>(const std::string &toParse) {
  std::size_t end;
  try {
    float parsed = std::stof(toParse, &end);
    if (end != toParse.length()) throw std::invalid_argument("The given argument value contains non numeric elements");
    return parsed;
  } catch (std::invalid_argument const &err) {
    std::string msg{"Invalid argument ("};
    msg.append(toParse).append(") for an argument of type float.");
    throw CLIException(msg);
  } catch (std::out_of_range const &err) {
    std::string msg{"Value ("};
    msg.append(toParse).append(") does not fit into an argument of type float.");
    throw CLIException(msg);
  }
}

// STD::STRING
template <>
std::string parse<std::string>(const std::string &toParse) {
  return toParse;
}

// STD::FILESYSTEM::PATH
template <>
std::filesystem::path parse<std::filesystem::path>(const std::string &toParse) {
  std::filesystem::path path{toParse};
  path = std::filesystem::canonical(path);
  return path;
}
}  // namespace cpp_cli_internal
}  // namespace cpp_cli
