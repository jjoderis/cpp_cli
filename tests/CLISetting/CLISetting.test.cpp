#include <CLIException/CLIException.h>
#include <CLISetting/CLISetting.h>
#include <gtest/gtest.h>

enum TestType { Arg1, Arg2, Arg3, Arg4, Arg5 };

TEST(CLISetting_TEST, class_test) {
  using cpp_cli::CLISettingBuilder;
  auto no_long_and_short = []() { CLISettingBuilder<Arg1, int>{}.build(); };
  EXPECT_THROW(no_long_and_short(), cpp_cli::CLIException);

  auto minusInLong = []() { CLISettingBuilder<Arg1, int>{}.addLong("--flag"); };
  EXPECT_THROW(minusInLong(), cpp_cli::CLIException);

  auto redefiningHelp = []() { CLISettingBuilder<Arg1, int>{}.addLong("help"); };
  EXPECT_THROW(redefiningHelp(), cpp_cli::CLIException);

  auto a = CLISettingBuilder<Arg1, int>{}.addLong("longopt").build();
  EXPECT_TRUE(a.hasLong());
  EXPECT_FALSE(a.hasShort());
  EXPECT_FALSE(a.hasDescription());

  auto b = CLISettingBuilder<Arg1, int>{}.addShort('s').build();
  EXPECT_FALSE(b.hasLong());
  EXPECT_TRUE(b.hasShort());
  EXPECT_FALSE(b.hasDescription());

  auto c = CLISettingBuilder<Arg1, int>{}.addLong("longopt").addShort('s').build();
  EXPECT_TRUE(c.hasLong());
  EXPECT_TRUE(c.hasShort());
  EXPECT_FALSE(c.hasDescription());

  auto d = CLISettingBuilder<Arg1, int>{}.addLong("longopt").addShort('s').addDescription("Description").build();
  EXPECT_TRUE(d.hasLong());
  EXPECT_TRUE(d.hasShort());
  EXPECT_TRUE(d.hasDescription());
}

TEST(CLISetting_TEST, parse_test) {
  using cpp_cli::CLISettingBuilder;
  using cpp_cli::cpp_cli_internal::getFlagIndex;
  using cpp_cli::cpp_cli_internal::getFlagValue;

  const char *cl[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool"};
  constexpr int argc{sizeof(cl) / sizeof(*cl)};

  auto a = CLISettingBuilder<Arg1, int>{}.addLong("float").build();
  EXPECT_EQ(getFlagIndex(a, argc, cl), 5);
  auto aVal{getFlagValue(a, argc, cl)};
  EXPECT_TRUE(aVal.has_value());
  EXPECT_EQ(aVal.value(), std::string{"12.5"});

  auto b = CLISettingBuilder<Arg1, int>{}.addLong("arg1").build();
  EXPECT_EQ(getFlagIndex(b, argc, cl), 1);
  auto bVal{getFlagValue(b, argc, cl)};
  EXPECT_TRUE(bVal.has_value());
  EXPECT_EQ(bVal.value(), std::string{"val1"});

  auto c = CLISettingBuilder<Arg1, int>{}.addLong("int").addShort('i').build();
  EXPECT_EQ(getFlagIndex(c, argc, cl), 3);
  auto cVal{getFlagValue(c, argc, cl)};
  EXPECT_TRUE(cVal.has_value());
  EXPECT_EQ(cVal.value(), std::string{"123"});

  auto d = CLISettingBuilder<Arg1, int>{}.addLong("bool").addShort('b').build();
  EXPECT_EQ(getFlagIndex(d, argc, cl), 7);
  auto dVal{getFlagValue(d, argc, cl)};
  EXPECT_FALSE(dVal.has_value());

  // it throws when both the long and short version of an arg exist in the function call
  const char *cl2[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool", "--int", "345"};
  constexpr int argc2{sizeof(cl2) / sizeof(*cl2)};
  auto f = CLISettingBuilder<Arg1, int>{}.addLong("int").addShort('i').build();
  EXPECT_THROW(getFlagValue(f, argc2, cl2), cpp_cli::CLIException);
}
