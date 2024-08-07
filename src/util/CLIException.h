#ifndef CLI_EXCEPTION_H
#define CLI_EXCEPTION_H

#include <exception>
#include <string>

namespace cpp_cli {

class CLIException : public std::exception {
 public:
  CLIException(const std::string &message);

  const char *what() const noexcept override;

 protected:
  CLIException();
  std::string m_message{};
};

class FlagException : public CLIException {
 public:
  FlagException() = delete;

  /**
   * Creates a CLIException with the given message. You can use %s in the message which will be automatically replaced
   * with the given flag (either the long flag or the short flag name must be given; if both are given and the string
   * contains two %s the first is replaced with the long name and the second with the long name)
   */
  FlagException(
      const std::string &msg,
      const std::string &originalErrorMessage,
      const std::string &longFlag = "",
      char shortFlag = '\0'
  );
};

};  // namespace cpp_cli

#endif