#include <CLISettingsParser/CLISettingsParser.h>
#include <gtest/gtest.h>
#include <util/type_traits.h>

#include <cstdlib>

#include "CLArgument/CLArgument.h"
#include "CLIException/CLIException.h"
#include "gtest/gtest.h"

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

TEST(CLISettingParser_TEST, parsing_test) {
  using cpp_cli::CLArgumentBuilder;
  using cpp_cli::CLISetting;
  // test the function that takes a number of settings and the program call information and returns a settings
  // container with the parsed values
  const char *cl[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool"};
  constexpr int argc{sizeof(cl) / sizeof(*cl)};
  auto settings1 = cpp_cli::parseProgramSettingsFromCL(
      argc,
      cl,
      CLISetting<Arg3, int>{CLArgumentBuilder{}.addShort('i').build()},
      CLISetting<Arg1, std::string>{CLArgumentBuilder{}.addLong("arg1").build()},
      CLISetting<s2::X, bool>{CLArgumentBuilder{}.addLong("bool").build()},
      CLISetting<s1::X, float>{CLArgumentBuilder{}.addLong("float").build(), 42.5},
      CLISetting<Arg5, float>{CLArgumentBuilder{}.addLong("unknown1").build(), 2},
      CLISetting<Arg2, bool>{CLArgumentBuilder{}.addLong("unkown2").build()},
      CLISetting<Arg4, float>{CLArgumentBuilder{}.addShort('u').build(), 23.8}
  );

  // check that the values were parsed correctly
  int arg3Value = settings1.get<Arg3>();
  EXPECT_EQ(arg3Value, 123);

  std::string arg1Value = settings1.get<Arg1>();
  EXPECT_STREQ(arg1Value.c_str(), "val1");

  bool other2XValue = settings1.get<s2::X>();
  EXPECT_TRUE(other2XValue);

  float otherXValue = settings1.get<s1::X>();
  EXPECT_FLOAT_EQ(otherXValue, 12.5);

  // check that a bool valued arg that is not found in the program call is set to false
  bool arg2Value = settings1.get<Arg2>();
  EXPECT_FALSE(arg2Value);

  // check that a non bool typed value with a default value still has that default value
  float arg4Value = settings1.get<Arg4>();
  EXPECT_FLOAT_EQ(arg4Value, 23.8);

  //   TODO: throw when there are undefined flags in the program call
  //   TODO: allow definition of CLISettingInterface that is used to call parseSettingsFromCL and when the user calls
  //   with --help automatically prints the information about the available flags

  //   // test that the second return value targets the correct index when there is an unknown value in the program call
  //   auto parse2Res = cpp_cli::parseProgramArgumentsFromCL(
  //       argc,
  //       cl,
  //       nArg<TestType, Arg3, int>{nullptr, 'i'},
  //       nArg<TestType, Arg1, std::string>{"arg1"},
  //       nArg<s2::OtherType2, s2::X, bool>{"bool"},
  //       nArg<TestType, Arg2, bool>{"unknown"}
  //   );

  //   int unknownFlagIndex2 = std::get<1>(parse2Res);
  //   EXPECT_EQ(unknownFlagIndex2, 5);

  //   auto parsedArgs2 = std::get<0>(parse2Res);

  //   // the rest should have been parsed as expected
  //   arg3HasValue = parsedArgs2.hasValue<TestType, Arg3>();
  //   EXPECT_TRUE(arg3HasValue);
  //   arg3Value = parsedArgs2.getValue<TestType, Arg3>();
  //   EXPECT_EQ(arg3Value, 123);

  //   arg1HasValue = parsedArgs2.hasValue<TestType, Arg1>();
  //   EXPECT_TRUE(arg1HasValue);
  //   arg1Value = parsedArgs2.getValue<TestType, Arg1>();
  //   EXPECT_STREQ(arg1Value.c_str(), "val1");

  //   other2XHasValue = parsedArgs2.hasValue<s2::OtherType2, s2::X>();
  //   EXPECT_TRUE(other2XHasValue);
  //   other2XValue = parsedArgs2.getValue<s2::OtherType2, s2::X>();
  //   EXPECT_TRUE(other2XValue);

  //   arg2HasValue = parsedArgs.hasValue<TestType, Arg2>();
  //   EXPECT_TRUE(arg2HasValue);
  //   arg2Value = parsedArgs.getValue<TestType, Arg2>();
  //   EXPECT_FALSE(arg2Value);

  // the function should throw if a required argument is not provided
  auto throwOnMissingReqLong = []() {
    const char *argv[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool"};
    constexpr int argc{sizeof(argv) / sizeof(*argv)};
    cpp_cli::parseProgramSettingsFromCL(
        argc,
        argv,
        CLISetting<Arg3, int>{CLArgumentBuilder{}.addShort('i').build()},
        CLISetting<Arg1, std::string>{CLArgumentBuilder{}.addLong("arg1").build()},
        CLISetting<s2::X, bool>{CLArgumentBuilder{}.addLong("bool").build()},
        CLISetting<Arg2, float>{CLArgumentBuilder{}.addLong("unknown").build()}
    );
  };

  EXPECT_THROW(
      {
        try {
          throwOnMissingReqLong();
        } catch (const cpp_cli::CLIException &e) {
          EXPECT_STREQ("Missing a required program argument (--unknown)!", e.what());
          throw;
        }
      },
      cpp_cli::CLIException
  );

  auto throwOnMissingReqShort = []() {
    const char *argv[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool"};
    constexpr int argc{sizeof(argv) / sizeof(*argv)};
    cpp_cli::parseProgramSettingsFromCL(
        argc,
        argv,
        CLISetting<Arg3, int>{CLArgumentBuilder{}.addShort('i').build()},
        CLISetting<Arg1, std::string>{CLArgumentBuilder{}.addLong("arg1").build()},
        CLISetting<s2::X, bool>{CLArgumentBuilder{}.addLong("bool").build()},
        CLISetting<Arg2, float>{CLArgumentBuilder{}.addShort('u').build()}
    );
  };

  EXPECT_THROW(
      {
        try {
          throwOnMissingReqShort();
        } catch (const cpp_cli::CLIException &e) {
          EXPECT_STREQ("Missing a required program argument (-u)!", e.what());
          throw;
        }
      },
      cpp_cli::CLIException
  );

  auto throwOnMissingReqBoth = []() {
    const char *argv[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool"};
    constexpr int argc{sizeof(argv) / sizeof(*argv)};
    cpp_cli::parseProgramSettingsFromCL(
        argc,
        argv,
        CLISetting<Arg3, int>{CLArgumentBuilder{}.addShort('i').build()},
        CLISetting<Arg1, std::string>{CLArgumentBuilder{}.addLong("arg1").build()},
        CLISetting<s2::X, bool>{CLArgumentBuilder{}.addLong("bool").build()},
        CLISetting<Arg2, float>{CLArgumentBuilder{}.addLong("unknown").addShort('u').build()}
    );
  };

  EXPECT_THROW(
      {
        try {
          throwOnMissingReqBoth();
        } catch (const cpp_cli::CLIException &e) {
          EXPECT_STREQ("Missing a required program argument (--unknown, -u)!", e.what());
          throw;
        }
      },
      cpp_cli::CLIException
  );

  auto throwOnBoolWithValue = []() {
    const char *argv[]{"./main", "--arg1", "val1", "-i", "123", "--bool", "illegalValue"};
    constexpr int argc{sizeof(argv) / sizeof(*argv)};
    cpp_cli::parseProgramSettingsFromCL(
        argc,
        argv,
        CLISetting<Arg3, int>{CLArgumentBuilder{}.addShort('i').build()},
        CLISetting<Arg1, std::string>{CLArgumentBuilder{}.addLong("arg1").build()},
        CLISetting<s2::X, bool>{CLArgumentBuilder{}.addLong("bool").build()}
    );
  };

  EXPECT_THROW(
      {
        try {
          throwOnBoolWithValue();
        } catch (const cpp_cli::CLIException &e) {
          EXPECT_STREQ("Flag --bool which is of a bool type should not be followed by a value!", e.what());
          throw;
        }
      },
      cpp_cli::CLIException
  );

  // it does not throw if a bool flag is followed directly by another flag
  const char *cl3[]{"./main", "--bool1", "-b", "--bool2", "wrong", "--bool3"};
  int argc3{sizeof(cl3) / sizeof(*cl3)};
  auto settingsBool = cpp_cli::parseProgramSettingsFromCL(
      argc3,
      cl3,
      CLISetting<Arg1, bool>{CLArgumentBuilder{}.addLong("bool1").build()},
      CLISetting<Arg3, bool>{CLArgumentBuilder{}.addShort('b').build()}
  );
  bool boolArg1 = settingsBool.get<Arg1>();
  EXPECT_TRUE(boolArg1);
  bool boolArg3 = settingsBool.get<Arg3>();
  EXPECT_TRUE(boolArg3);

  // it throws if a non bool flag is at the end
  auto throwOnNonBoolAtEnd = []() {
    const char *argv[]{"./main", "--arg1", "val1", "-i"};
    constexpr int argc{sizeof(argv) / sizeof(*argv)};
    cpp_cli::parseProgramSettingsFromCL(
        argc,
        argv,
        CLISetting<Arg3, int>{CLArgumentBuilder{}.addShort('i').build()},
        CLISetting<Arg1, std::string>{CLArgumentBuilder{}.addLong("arg1").build()}
    );
  };

  EXPECT_THROW(
      {
        try {
          throwOnNonBoolAtEnd();
        } catch (const cpp_cli::CLIException &e) {
          EXPECT_STREQ("Flag -i is missing a value!", e.what());
          throw;
        }
      },
      cpp_cli::CLIException
  );

  // it throws if the parse function for the type of the arg is unable to parse the value after the flag
  auto throwOnFailedParsing = []() {
    const char *argv[]{"./main", "--int", "12.5"};
    constexpr int argc{sizeof(argv) / sizeof(*argv)};
    cpp_cli::parseProgramSettingsFromCL(argc, argv, CLISetting<Arg3, int>{CLArgumentBuilder{}.addLong("int").build()});
  };
  // this tests _that_ the expected exception is thrown
  EXPECT_THROW(
      {
        try {
          throwOnFailedParsing();
        } catch (const cpp_cli::FlagException &e) {
          // and this tests that it has the correct message
          EXPECT_STREQ(
              "Ran into an error when parsing the value for flag --int. (Original Error: Invalid argument (12.5) for "
              "an argument of type int.)",
              e.what()
          );
          throw;
        }
      },
      cpp_cli::FlagException
  );

  // it accepts a validator function that gets the parsed value and the flag that was parsed and can be used to throw an
  // error when the value is not as expected
  auto throwOnFailedValidation = []() {
    const char *argv[]{"./main", "--float", "12.5"};
    constexpr int argc{sizeof(argv) / sizeof(*argv)};
    cpp_cli::parseProgramSettingsFromCL(
        argc,
        argv,
        CLISetting<Arg3, float>{
            CLArgumentBuilder{}.addLong("float").build(),
            [](const float &val, const std::string &parsedLongFlag, char parsedShortFlag) {
              if (val < 0 || val > 1)
                throw cpp_cli::FlagException(
                    "The value for %s should be in the range [0, 1]", "", parsedLongFlag, parsedShortFlag
                );
            }
        }
    );
  };

  EXPECT_THROW(
      {
        try {
          throwOnFailedValidation();
        } catch (const cpp_cli::FlagException &e) {
          EXPECT_STREQ("The value for --float should be in the range [0, 1]", e.what());
          throw;
        }
      },
      cpp_cli::FlagException
  );

  auto testHelpString = []() {
    const char *argvHelp[]{"./main", "--help"};
    constexpr int argcHelp{sizeof(argvHelp) / sizeof(*argvHelp)};
    cpp_cli::parseProgramSettingsFromCL(
        argcHelp,
        argvHelp,
        CLISetting<Arg3, int>{CLArgumentBuilder{}.addLong("arg3").addDescription("This is the first argument.").build()
        },
        CLISetting<Arg1, float>{
            CLArgumentBuilder{}.addLong("arg1").addShort('1').addDescription("This is the second argument.").build()
        },
        CLISetting<Arg5, std::string>{
            CLArgumentBuilder{}.addShort('5').addDescription("This is the last argument.").build()
        }
    );
  };
  testing::internal::CaptureStdout();
  EXPECT_EXIT(testHelpString(), ::testing::ExitedWithCode(0), "");
  std::string helpString{
      "  --arg3         This is the first argument.\n"
      "  -1, --arg1     This is the second argument.\n"
      "  -5             This is the last argument.\n"
  };
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_STREQ(output.c_str(), helpString.c_str());
}
