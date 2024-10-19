#include "CLISetting/CLISetting.h"
#include "CLISettingsParser/CLISettingsParser.h"

enum ArgNames { Width, Height, Float, Name, Output };

enum OtherArgNames { Active };
enum TestType { Arg1, Arg2, Arg3, Arg4, Arg5 };

namespace s1 {

enum OtherType {
  X,
};

};  // namespace s1

namespace s2 {

enum OtherType2 {
  X,
};

};

int main(int argc, const char **argv) {
  using cpp_cli::CLISettingBuilder;
  cpp_cli::parseProgramSettingsFromCL(
      argc,
      argv,
      CLISettingBuilder<Arg3, int>{}.addShort('i').build(),
      CLISettingBuilder<Arg1, std::string>{}.addLong("arg1").build(),
      CLISettingBuilder<s2::X, bool>{}.addLong("bool").build(),
      CLISettingBuilder<Arg2, float>{}.addLong("unknown").addShort('u').build()
  );
  cpp_cli::parseProgramSettingsFromCL(
      argc,
      argv,
      CLISettingBuilder<Arg3, int>{}.addShort('i').build(),
      CLISettingBuilder<Arg1, std::string>{}.addLong("arg1").build(),
      CLISettingBuilder<s2::X, bool>{}.addLong("bool").build()
  );
  cpp_cli::parseProgramSettingsFromCL(
      argc,
      argv,
      CLISettingBuilder<Arg1, bool>{}.addLong("bool1").build(),
      CLISettingBuilder<Arg3, bool>{}.addShort('b').build()
  );
}
