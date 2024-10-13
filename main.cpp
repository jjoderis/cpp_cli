#include <CLArgument/CLArgument.h>

#include <filesystem>
#include <iostream>
#include <optional>
#include <ostream>

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
  using cpp_cli::CLArgumentBuilder;
  using cpp_cli::CLISetting;
  cpp_cli::parseProgramSettingsFromCL(
      argc,
      argv,
      CLISetting<Arg3, int>{CLArgumentBuilder{}.addShort('i').build()},
      CLISetting<Arg1, std::string>{CLArgumentBuilder{}.addLong("arg1").build()},
      CLISetting<s2::X, bool>{CLArgumentBuilder{}.addLong("bool").build()},
      CLISetting<Arg2, float>{CLArgumentBuilder{}.addLong("unknown").addShort('u').build()}
  );
  cpp_cli::parseProgramSettingsFromCL(
      argc,
      argv,
      CLISetting<Arg3, int>{CLArgumentBuilder{}.addShort('i').build()},
      CLISetting<Arg1, std::string>{CLArgumentBuilder{}.addLong("arg1").build()},
      CLISetting<s2::X, bool>{CLArgumentBuilder{}.addLong("bool").build()}
  );
  cpp_cli::parseProgramSettingsFromCL(
      argc,
      argv,
      CLISetting<Arg1, bool>{CLArgumentBuilder{}.addLong("bool1").build()},
      CLISetting<Arg3, bool>{CLArgumentBuilder{}.addShort('b').build()}
  );
  // try {
  //   auto res = cpp_cli::parseProgramArgumentsFromCL(
  //       argc,
  //       argv,
  //       cpp_cli::NamedArgument<ArgNames, Height, unsigned int>{"height"},
  //       cpp_cli::NamedArgument<ArgNames, Width, unsigned int>{"width"},
  //       cpp_cli::NamedArgument<ArgNames, Float, float>{"float", 0, "", std::make_shared<float>(0.0f)},
  //       cpp_cli::NamedArgument<ArgNames, Name, std::string>{"name", 'n'},
  //       cpp_cli::NamedArgument<OtherArgNames, Active, bool>{"active", 'a'},
  //       cpp_cli::NamedArgument<ArgNames, Output, std::filesystem::path>{"output", 'o'}
  //   );

  //   auto args = std::get<0>(res);
  //   if (args.hasValue<ArgNames, Output>()) {
  //     std::cout << args.getValue<ArgNames, Output>() << '\n';
  //   }
  // } catch (cpp_cli::CLIException &err) {
  //   std::cout << err.what() << '\n';
  //   exit(EXIT_FAILURE);
  // }

  // auto settings = CLArgumentParser<ArgTypes>::parseOptions(
  //     argc, argv, CLArgument<ArgTypes, Width, int>{"width", 'w'},
  //     CLArgument<ArgTypes, Height, int>{"height", 'h', std::make_shared<int>(128)},
  //     CLArgument<ArgTypes, Name, std::string>{"name"});

  // // settings.set<Width>(std::make_shared<int>(124));
  // std::cout << settings.get<Width>() << "\n";
  // std::cout << settings.get<Height>() << "\n";
  // std::cout << settings.get<Name>() << "\n";
  // // std::cout << settings.get<Active>() << "\n";

  // std::cout << "End Program\n";
}
