#ifndef CLI_EXCEPTION_H
#define CLI_EXCEPTION_H

#include <exception>
#include <string>

namespace cpp_cli {

class CLIException : std::exception {
 public:
  CLIException() = delete;
  CLIException(const std::string &message);

  std::string getMessage() const;

 private:
  std::string m_message;
};

class ParseException : public CLIException {};
class ValidationException : public CLIException {};

};  // namespace cpp_cli

#endif