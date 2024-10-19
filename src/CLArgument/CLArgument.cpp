#include "CLArgument.h"

#include "../CLIException/CLIException.h"

namespace cpp_cli {

CLArgument::CLArgument() {}

bool CLArgument::hasLong() const { return m_long != nullptr; }
bool CLArgument::hasShort() const { return m_short != '\0'; }
bool CLArgument::hasDescription() const { return !m_description.empty(); }

const char* CLArgument::getLong() const { return m_long; }
char CLArgument::getShort() const { return m_short; }
const std::string& CLArgument::getDescription() const { return m_description; }

CLArgumentBuilder& CLArgumentBuilder::addLong(const char* longOpt) {
  if (longOpt != nullptr && longOpt[0] == '-') {
    throw CLIException{
        "Please omit the - sign at the start of the given longOpt. If you want to handle the flag --arg just "
        "provide the longOpt \"arg\"!"
    };
  }
  if (!std::string{"help"}.compare(longOpt)) {
    throw CLIException{"The --help flag is automatically handled. Please use a different flag name."};
  }

  m_argument.m_long = longOpt;
  return *this;
}

CLArgumentBuilder& CLArgumentBuilder::addShort(char shortOpt) {
  m_argument.m_short = shortOpt;
  return *this;
}

CLArgumentBuilder& CLArgumentBuilder::addDescription(const std::string& d) {
  m_argument.m_description = d;
  return *this;
}

CLArgument CLArgumentBuilder::build() {
  if (!m_argument.m_long && !m_argument.m_short) {
    throw CLIException{
        "CLArgument expects either a longOpt for flags of the form --flag or a shortOpt for flags of the "
        "form "
        "-f."
    };
  };

  return m_argument;
}

int getFlagIndex(const CLArgument& clArg, int argc, const char** argv) {
  int longFlagIndex = clArg.hasLong() ? 1 : argc;
  for (; longFlagIndex < argc; ++longFlagIndex) {
    std::string a = std::string{"--"}.append(clArg.getLong());
    std::string b{argv[longFlagIndex]};
    if (a.compare(b) == 0) break;
  }
  int shortFlagIndex = clArg.hasShort() ? 1 : argc;
  for (; shortFlagIndex < argc; ++shortFlagIndex) {
    std::string a = std::string{"-"}.append(1, clArg.getShort());
    std::string b{argv[shortFlagIndex]};
    if (a.compare(b) == 0) break;
  }

  if (longFlagIndex < argc && shortFlagIndex < argc) {
    std::string errMessage{"Provided values for both the short (%s) and long (%s) version of a program parameter!"};
    throw FlagException(errMessage, "", clArg.getLong(), clArg.getShort());
  }

  return longFlagIndex < argc ? longFlagIndex : shortFlagIndex;
}

std::optional<std::string> getFlagValue(const CLArgument& clArg, int argc, const char** argv) {
  int flagIndex = getFlagIndex(clArg, argc, argv);

  if (flagIndex >= argc) return {};

  if (flagIndex + 1 >= argc || std::string{argv[flagIndex + 1]}.substr(0, 2).compare("--") == 0) {
    return {};
  }

  return {argv[flagIndex + 1]};
}

}  // namespace cpp_cli
