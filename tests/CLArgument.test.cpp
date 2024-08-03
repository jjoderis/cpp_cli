#include <CLArgument/CLArgument.h>
#include <gtest/gtest.h>

TEST(Argument_TEST, class_test) {
  auto no_long_and_short = []() { cpp_cli::OptionalArgument<int> arg{}; };
  EXPECT_THROW(no_long_and_short(), cpp_cli::CLIException);

  auto minusInLong = []() { cpp_cli::OptionalArgument<int> arg{"--flag"}; };
  EXPECT_THROW(minusInLong(), cpp_cli::CLIException);

  cpp_cli::OptionalArgument<int> a{"longopt"};
  EXPECT_TRUE(a.hasLong());
  EXPECT_FALSE(a.hasShort());
  EXPECT_FALSE(a.hasDescription());
  EXPECT_FALSE(a.hasValue());

  cpp_cli::OptionalArgument<int> b{nullptr, 's'};
  EXPECT_FALSE(b.hasLong());
  EXPECT_TRUE(b.hasShort());
  EXPECT_FALSE(b.hasDescription());
  EXPECT_FALSE(b.hasValue());

  cpp_cli::OptionalArgument<int> c{"longopt", 's'};
  EXPECT_TRUE(c.hasLong());
  EXPECT_TRUE(c.hasShort());
  EXPECT_FALSE(c.hasDescription());
  EXPECT_FALSE(c.hasValue());

  cpp_cli::OptionalArgument<int> d{"longopt", 's', "Description"};
  EXPECT_TRUE(d.hasLong());
  EXPECT_TRUE(d.hasShort());
  EXPECT_TRUE(d.hasDescription());
  EXPECT_FALSE(d.hasValue());

  cpp_cli::OptionalArgument<int> e{"longopt", 's', "Description", std::make_shared<int>(0)};
  EXPECT_TRUE(e.hasLong());
  EXPECT_TRUE(e.hasShort());
  EXPECT_TRUE(e.hasDescription());
  EXPECT_TRUE(e.hasValue());
}

TEST(Argument_TEST, parse_test) {
  const char *cl[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool"};
  constexpr int argc{sizeof(cl) / sizeof(*cl)};

  cpp_cli::OptionalArgument<float> a{"float"};
  parseArgFromCL(argc, cl, a);
  EXPECT_TRUE(a.hasValue());
  EXPECT_EQ(*a.value.get(), 12.5);

  cpp_cli::OptionalArgument<std::string> b{"arg1"};
  parseArgFromCL(argc, cl, b);
  EXPECT_TRUE(b.hasValue());
  EXPECT_EQ(*b.value.get(), std::string{"val1"});

  cpp_cli::OptionalArgument<int> c{"int", 'i'};
  parseArgFromCL(argc, cl, c);
  EXPECT_TRUE(c.hasValue());
  EXPECT_EQ(*c.value.get(), 123);

  cpp_cli::OptionalArgument<bool> d{"bool", 'b'};
  parseArgFromCL(argc, cl, d);
  EXPECT_TRUE(d.hasValue());
  EXPECT_EQ(*d.value.get(), true);

  // if a bool argument is not present it will be automatically set to false
  cpp_cli::OptionalArgument<bool> e{"otherBool"};
  parseArgFromCL(argc, cl, e);
  EXPECT_TRUE(e.hasValue());
  EXPECT_EQ(*e.value.get(), false);

  // it throws when both the long and short version of an arg exist in the function call
  const char *cl2[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool", "--int", "345"};
  constexpr int argc2{sizeof(cl2) / sizeof(*cl2)};
  cpp_cli::OptionalArgument<int> f{"int", 'i'};
  EXPECT_THROW(parseArgFromCL(argc2, cl2, f), cpp_cli::CLIException);

  const char *cl3[]{"./main", "--bool1", "-b", "--bool2", "wrong", "--bool3"};
  constexpr int argc3{sizeof(cl3) / sizeof(*cl3)};
  // it throws if a bool type argument is followed by some value
  cpp_cli::OptionalArgument<bool> g{"bool2"};
  EXPECT_THROW(parseArgFromCL(argc3, cl3, g), cpp_cli::CLIException);
  // it does not throw if a bool flag is at the end
  cpp_cli::OptionalArgument<bool> h{"bool3"};
  EXPECT_NO_THROW(parseArgFromCL(argc3, cl3, h));
  EXPECT_TRUE(h.hasValue());
  EXPECT_EQ(*h.value.get(), true);
  // it does not throw if a bool flag is followed directly by another flag
  cpp_cli::OptionalArgument<bool> i{nullptr, 'b'};
  EXPECT_NO_THROW(parseArgFromCL(argc3, cl3, i));
  EXPECT_TRUE(i.hasValue());
  EXPECT_EQ(*i.value.get(), true);

  const char *cl4[]{"./main", "--int1", "-i", "--int2", "correct", "--int3"};
  constexpr int argc4{sizeof(cl4) / sizeof(*cl4)};
  // it throws if a non bool flag is followed by another flag
  cpp_cli::OptionalArgument<int> j{"int1"};
  EXPECT_THROW(parseArgFromCL(argc4, cl4, j), cpp_cli::CLIException);
  // it throws if a non bool flag is at the end
  cpp_cli::OptionalArgument<int> k{"int3"};
  EXPECT_THROW(parseArgFromCL(argc4, cl4, k), cpp_cli::CLIException);

  // it throws if a non boolean arg is required but not found in the program call
  cpp_cli::RequiredArgument<int> l{"int4"};
  EXPECT_THROW(parseArgFromCL(argc4, cl4, l), cpp_cli::CLIException);

  // it throws if the parse function for the type of the arg is unable to parse the value after the flag
  cpp_cli::OptionalArgument<int> m{"float"};
  EXPECT_THROW(parseArgFromCL(argc2, cl2, m), cpp_cli::ParseException);
}