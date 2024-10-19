#ifndef CL_ARGUMENT_H
#define CL_ARGUMENT_H

#include <optional>
#include <string>

namespace cpp_cli {

class CLArgumentBuilder;

class CLArgument {
 public:
  bool hasLong() const;
  bool hasShort() const;
  bool hasDescription() const;

  const char *getLong() const;
  char getShort() const;
  const std::string &getDescription() const;

 private:
  CLArgument();

  const char *m_long{nullptr};
  char m_short{0};
  std::string m_description{""};

  friend CLArgumentBuilder;
};

class CLArgumentBuilder {
 public:
  CLArgumentBuilder &addLong(const char *longOpt);

  CLArgumentBuilder &addShort(char shortOpt);

  CLArgumentBuilder &addDescription(const std::string &d);

  CLArgument build();

 private:
  CLArgument m_argument{};
};

int getFlagIndex(const CLArgument &clArg, int argc, const char **argv);
std::optional<std::string> getFlagValue(const CLArgument &clArg, int argc, const char **argv);

};  // namespace cpp_cli

#endif
