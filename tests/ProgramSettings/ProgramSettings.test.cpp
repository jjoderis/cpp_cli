#include <ProgramSettings/ProgramSettings.h>
#include <gtest/gtest.h>
#include <util/type_traits.h>

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

template <auto SettingName, typename SettingValueType>
class Setting {};

TEST(ProgramArguments_TEST, class_test) {
  // create a container for settings that takes settings of generic types and allows access to a setting by its name
  cpp_cli::ProgramSettings<Setting<Arg3, int>, Setting<Arg1, int>, Setting<s1::X, std::string>, Setting<s2::X, bool>>
      settings{};

  settings.set<Arg1>(125);
  settings.set<s2::X>(true);
  settings.set<Arg3>(37);
  settings.set<s1::X>("Test123");

  int arg3{settings.get<Arg3>()};
  EXPECT_EQ(arg3, 37);

  int arg1{settings.get<Arg1>()};
  EXPECT_EQ(arg1, 125);

  std::string s1X{settings.get<s1::X>()};
  EXPECT_STREQ(s1X.c_str(), "Test123");

  bool s2X{settings.get<s2::X>()};
  EXPECT_TRUE(s2X);
}
