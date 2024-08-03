#include <CLArgument/CLArgument.h>
#include <ProgramArguments.h>

#include <iostream>
#include <optional>

enum ArgNames {
  Width,
  Height,
  Name,
};

enum OtherArgNames { Active };

int main(int argc, const char **argv) {
  try {
    auto res = cpp_cli::parseProgramArgumentsFromCL(
        argc,
        argv,
        cpp_cli::NamedOptionalArgument<ArgNames, Height, int>{"height"},
        cpp_cli::NamedOptionalArgument<ArgNames, Width, int>{"width"},
        cpp_cli::NamedRequiredArgument<ArgNames, Name, std::string>{"name", 'n'},
        cpp_cli::NamedOptionalArgument<OtherArgNames, Active, bool>{nullptr, 'a'}
    );
    auto args = std::get<0>(res);
    std::cout << args.getArg<ArgNames, Height>().hasValidator() << '\n';
  } catch (cpp_cli::CLIException &err) {
    std::cout << err.getMessage() << '\n';
    exit(EXIT_FAILURE);
  }

  // auto settings = CLArgumentParser<ArgTypes>::parseOptions(
  //     argc, argv, CL_Argument<ArgTypes, Width, int>{"width", 'w'},
  //     CL_Argument<ArgTypes, Height, int>{"height", 'h', std::make_shared<int>(128)},
  //     CL_Argument<ArgTypes, Name, std::string>{"name"});

  // // settings.set<Width>(std::make_shared<int>(124));
  // std::cout << settings.get<Width>() << "\n";
  // std::cout << settings.get<Height>() << "\n";
  // std::cout << settings.get<Name>() << "\n";
  // // std::cout << settings.get<Active>() << "\n";

  // std::cout << "End Program\n";
}