#include <CLArgument/CLArgument.h>
#include <gtest/gtest.h>

TEST(Argument_TEST, class_test) {
  auto no_long_and_short = []() { cpp_cli::CL_Argument<int> arg{}; };
  EXPECT_THROW(no_long_and_short(), cpp_cli::CLIException);

  auto minusInLong = []() { cpp_cli::CL_Argument<int> arg{"--flag"}; };
  EXPECT_THROW(minusInLong(), cpp_cli::CLIException);

  cpp_cli::CL_Argument<int> a{"longopt"};
  EXPECT_TRUE(a.hasLong());
  EXPECT_FALSE(a.hasShort());
  EXPECT_FALSE(a.hasDescription());
  EXPECT_FALSE(a.hasValue());

  cpp_cli::CL_Argument<int> b{nullptr, 's'};
  EXPECT_FALSE(b.hasLong());
  EXPECT_TRUE(b.hasShort());
  EXPECT_FALSE(b.hasDescription());
  EXPECT_FALSE(b.hasValue());

  cpp_cli::CL_Argument<int> c{"longopt", 's'};
  EXPECT_TRUE(c.hasLong());
  EXPECT_TRUE(c.hasShort());
  EXPECT_FALSE(c.hasDescription());
  EXPECT_FALSE(c.hasValue());

  cpp_cli::CL_Argument<int> d{"longopt", 's', "Description"};
  EXPECT_TRUE(d.hasLong());
  EXPECT_TRUE(d.hasShort());
  EXPECT_TRUE(d.hasDescription());
  EXPECT_FALSE(d.hasValue());

  cpp_cli::CL_Argument<int> e{"longopt", 's', "Description", std::make_shared<int>(0)};
  EXPECT_TRUE(e.hasLong());
  EXPECT_TRUE(e.hasShort());
  EXPECT_TRUE(e.hasDescription());
  EXPECT_TRUE(e.hasValue());
}

TEST(Argument_TEST, parse_test) {
  const char *cl[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool"};
  constexpr int argc{sizeof(cl) / sizeof(*cl)};

  cpp_cli::CL_Argument<float> a{"float"};
  parseArgFromCL(argc, cl, a);
  EXPECT_TRUE(a.hasValue());
  EXPECT_EQ(*a.value.get(), 12.5);

  cpp_cli::CL_Argument<std::string> b{"arg1"};
  parseArgFromCL(argc, cl, b);
  EXPECT_TRUE(b.hasValue());
  EXPECT_EQ(*b.value.get(), std::string{"val1"});

  cpp_cli::CL_Argument<int> c{"int", 'i'};
  parseArgFromCL(argc, cl, c);
  EXPECT_TRUE(c.hasValue());
  EXPECT_EQ(*c.value.get(), 123);

  cpp_cli::CL_Argument<bool> d{"bool", 'b'};
  parseArgFromCL(argc, cl, d);
  EXPECT_TRUE(d.hasValue());
  EXPECT_EQ(*d.value.get(), true);

  // if a bool argument is not present it will be automatically set to false
  cpp_cli::CL_Argument<bool> e{"otherBool"};
  parseArgFromCL(argc, cl, e);
  EXPECT_TRUE(e.hasValue());
  EXPECT_EQ(*e.value.get(), false);

  // it throws when both the long and short version of an arg exist in the function call
  const char *cl2[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool", "--int", "345"};
  constexpr int argc2{sizeof(cl2) / sizeof(*cl2)};
  cpp_cli::CL_Argument<int> f{"int", 'i'};
  EXPECT_THROW(parseArgFromCL(argc2, cl2, f), cpp_cli::CLIException);

  const char *cl3[]{"./main", "--bool1", "-b", "--bool2", "wrong", "--bool3"};
  constexpr int argc3{sizeof(cl3) / sizeof(*cl3)};
  // it throws if a bool type argument is followed by some value
  cpp_cli::CL_Argument<bool> g{"bool2"};
  EXPECT_THROW(parseArgFromCL(argc3, cl3, g), cpp_cli::CLIException);
  // it does not throw if a bool flag is at the end
  cpp_cli::CL_Argument<bool> h{"bool3"};
  EXPECT_NO_THROW(parseArgFromCL(argc3, cl3, h));
  EXPECT_TRUE(h.hasValue());
  EXPECT_EQ(*h.value.get(), true);
  // it does not throw if a bool flag is followed directly by another flag
  cpp_cli::CL_Argument<bool> i{nullptr, 'b'};
  EXPECT_NO_THROW(parseArgFromCL(argc3, cl3, i));
  EXPECT_TRUE(i.hasValue());
  EXPECT_EQ(*i.value.get(), true);

  const char *cl4[]{"./main", "--int1", "-i", "--int2", "correct", "--int3"};
  constexpr int argc4{sizeof(cl4) / sizeof(*cl4)};
  // it throws if a non bool flag is followed by another flag
  cpp_cli::CL_Argument<int> j{"int1"};
  EXPECT_THROW(parseArgFromCL(argc4, cl4, j), cpp_cli::CLIException);
  // it throws if a non bool flag is at the end
  cpp_cli::CL_Argument<int> k{"int3"};
  EXPECT_THROW(parseArgFromCL(argc4, cl4, k), cpp_cli::CLIException);

  // it throws if a non boolean arg is required but not found in the program call
  cpp_cli::CL_Argument<int> l{"int4"};
  EXPECT_THROW(parseArgFromCL(argc4, cl4, l), cpp_cli::CLIException);

  // // it throws if the parse function for the type of the arg is unable to parse the value after the flag
  cpp_cli::CL_Argument<int> m{"float"};
  EXPECT_THROW(parseArgFromCL(argc2, cl2, m), cpp_cli::FlagException);
  // this tests _that_ the expected exception is thrown
  EXPECT_THROW(
      {
        try {
          parseArgFromCL(argc2, cl2, m);
        } catch (const cpp_cli::FlagException &e) {
          // and this tests that it has the correct message
          EXPECT_STREQ(
              "Ran into an error when parsing the value for flag --float. (Original Error: Invalid argument (12.5) for "
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
  cpp_cli::CL_Argument<float>
      n{"float", 0, "", nullptr, [](const float &val, const std::string &parsedLongFlag, char parsedShortFlag) {
          if (val < 0 || val > 1)
            throw cpp_cli::FlagException(
                "The value for %s should be in the range [0, 1]", "", parsedLongFlag, parsedShortFlag
            );
        }};
  // this tests _that_ the expected exception is thrown
  EXPECT_THROW(
      {
        try {
          parseArgFromCL(argc2, cl2, n);
        } catch (const cpp_cli::FlagException &e) {
          // and this tests that it has the correct message
          EXPECT_STREQ("The value for --float should be in the range [0, 1]", e.what());
          throw;
        }
      },
      cpp_cli::FlagException
  );
}