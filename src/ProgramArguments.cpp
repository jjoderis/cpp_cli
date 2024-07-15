#include "ProgramArguments.h"

namespace cpp_cli {

template <>
void markAsHandled(int argc, const char **argv, std::vector<bool> &handledFlags, CL_Argument<bool> &arg) {
  int flagIndex, longIndex, shortIndex;

  std::tie(flagIndex, longIndex, shortIndex) = arg.getFlagIndex(argc, argv);
  if (flagIndex < argc) {
    handledFlags[flagIndex] = true;
  }
}

};  // namespace cpp_cli