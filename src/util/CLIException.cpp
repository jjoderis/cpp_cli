#include "CLIException.h"

namespace cpp_cli {

CLIException::CLIException(const std::string &message) : m_message{message} {}

std::string CLIException::getMessage() const { return m_message; }

};  // namespace cpp_cli