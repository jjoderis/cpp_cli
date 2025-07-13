#include "CLIException.h"

#include <string.h>

#include <memory>

namespace cpp_cli {

CLIException::CLIException() {}
CLIException::CLIException(const std::string &message) : m_message{message} {}
CLIException::~CLIException() {}

const char *CLIException::what() const noexcept { return m_message.c_str(); }

FlagException::FlagException(
    const std::string &msg, const std::string &originalErrorMessage, const std::string &longFlag, char shortFlag
) {
  if (!longFlag.length() && shortFlag == '\0') {
    throw CLIException{
        "FlagException expects either a longOpt for flags of the form --flag or a shortOpt for flags of the "
        "form "
        "-f."
    };
  }

  std::string longFlagString{};
  std::string shortFlagString{};
  if (longFlag.length()) {
    // we replace the %s with the longFlag
    // found here: https://stackoverflow.com/a/26221725
    longFlagString.append("--");
    longFlagString.append(longFlag);
  }
  if (shortFlag != '\0') {
    shortFlagString.append("-");
    shortFlagString.append(1, shortFlag);
  }

  int size_s;
  if (longFlagString.length() && shortFlagString.length()) {
    size_s = std::snprintf(nullptr, 0, msg.c_str(), longFlagString.c_str(), shortFlagString.c_str()) + 1;
  } else if (longFlagString.length()) {
    size_s = std::snprintf(nullptr, 0, msg.c_str(), longFlagString.c_str()) + 1;
  } else {
    size_s = std::snprintf(nullptr, 0, msg.c_str(), shortFlagString.c_str()) + 1;
  }

  if (size_s <= 0) {
    throw CLIException("Error when trying to create the message for a FlagException");
  }
  auto size = static_cast<size_t>(size_s);
  std::unique_ptr<char[]> buf(new char[size]);
  if (longFlagString.length() && shortFlagString.length()) {
    std::snprintf(buf.get(), size, msg.c_str(), longFlagString.c_str(), shortFlagString.c_str());
  } else if (longFlagString.length()) {
    std::snprintf(buf.get(), size, msg.c_str(), longFlagString.c_str());
  } else {
    std::snprintf(buf.get(), size, msg.c_str(), shortFlagString.c_str());
  }
  m_message = {buf.get(), buf.get() + size - 1};

  if (originalErrorMessage.length()) {
    m_message.append("(Original Error: ");
    m_message.append(originalErrorMessage);
    m_message.append(")");
  }
}

};  // namespace cpp_cli
