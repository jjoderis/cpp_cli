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

};  // namespace s2

template <auto SettingName, typename ValueType>
class Setting {};

TEST(Type_Traits_Test, type_trait_test) {
  // check if a type is a Setting containing an enum and a type
  bool aa = cpp_cli::cpp_cli_internal::is_setting<int>::value;
  EXPECT_FALSE(aa);

  bool ac = cpp_cli::cpp_cli_internal::is_setting<Setting<Arg1, int>>::value;
  EXPECT_TRUE(ac);

  // check if a parameter pack contains only Settings
  bool ba = cpp_cli::cpp_cli_internal::is_setting_pack<>::value;
  EXPECT_FALSE(ba);

  bool bb = cpp_cli::cpp_cli_internal::is_setting_pack<int>::value;
  EXPECT_FALSE(bb);

  bool bd = cpp_cli::cpp_cli_internal::is_setting_pack<Setting<Arg1, int>>::value;
  EXPECT_TRUE(bd);

  bool be = cpp_cli::cpp_cli_internal::is_setting_pack<Setting<Arg1, int>, Setting<s1::X, float>>::value;
  EXPECT_TRUE(be);

  bool bf = cpp_cli::cpp_cli_internal::is_setting_pack<Setting<Arg1, int>, int>::value;
  EXPECT_FALSE(bf);

  bool bg = cpp_cli::cpp_cli_internal::is_setting_pack<int, Setting<Arg1, int>>::value;
  EXPECT_FALSE(bg);

  bool bh = cpp_cli::cpp_cli_internal::is_setting_pack<Setting<Arg1, int>, int, Setting<Arg1, float>>::value;
  EXPECT_FALSE(bh);

  // extract information about a Setting from its template
  TestType ca = cpp_cli::cpp_cli_internal::setting_info<Setting<Arg1, int>>::setting_name;
  EXPECT_EQ(ca, Arg1);

  TestType cb = cpp_cli::cpp_cli_internal::setting_info<Setting<Arg2, int>>::setting_name;
  EXPECT_NE(cb, Arg1);
  EXPECT_EQ(cb, Arg2);

  bool cc =
      std::is_same<cpp_cli::cpp_cli_internal::setting_info<Setting<s1::X, int>>::setting_value_type, float>::value;
  EXPECT_FALSE(cc);

  bool cd = std::is_same<
      cpp_cli::cpp_cli_internal::setting_info<Setting<s1::X, std::string>>::setting_value_type,
      std::string>::value;
  EXPECT_TRUE(cd);

  // find first index of an element with specific name
  int da = cpp_cli::cpp_cli_internal::setting_index_by_name<Arg1, Setting<Arg2, int>>::value;
  EXPECT_EQ(-1, da);

  int db = cpp_cli::cpp_cli_internal::setting_index_by_name<Arg1, Setting<Arg1, int>>::value;
  EXPECT_EQ(0, db);

  int dc = cpp_cli::cpp_cli_internal::setting_index_by_name<Arg1, Setting<Arg1, int>, Setting<Arg1, float>>::value;
  EXPECT_EQ(0, dc);

  int dd = cpp_cli::cpp_cli_internal::setting_index_by_name<Arg1, Setting<Arg2, int>, Setting<Arg1, float>>::value;
  EXPECT_EQ(1, dd);

  int de = cpp_cli::cpp_cli_internal::
      setting_index_by_name<Arg1, Setting<Arg2, int>, Setting<Arg1, float>, Setting<Arg3, int>>::value;
  EXPECT_EQ(1, de);

  // find an arg in a pack by indexing it with the args name
  bool ea = std::
      is_same<cpp_cli::cpp_cli_internal::setting_by_name<Arg1, Setting<Arg1, int>>::type, Setting<Arg1, int>>::value;
  EXPECT_TRUE(ea);

  bool ec = std::is_same<
      cpp_cli::cpp_cli_internal::setting_by_name<Arg1, Setting<Arg1, int>, Setting<Arg1, float>>::type,
      Setting<Arg1, int>>::value;
  EXPECT_TRUE(ec);

  bool ed = std::is_same<
      cpp_cli::cpp_cli_internal::setting_by_name<Arg1, Setting<Arg2, int>, Setting<Arg1, float>>::type,
      Setting<Arg1, float>>::value;
  EXPECT_TRUE(ed);

  bool ee = std::is_same<
      cpp_cli::cpp_cli_internal::setting_by_name<Arg1, Setting<Arg2, int>, Setting<Arg1, float>, Setting<Arg3, int>>::
          type,
      Setting<Arg1, float>>::value;
  EXPECT_TRUE(ee);

  // check if the pack only contains names args using different names
  bool fa = cpp_cli::cpp_cli_internal::settings_have_unique_names<Setting<Arg2, int>>::value;
  EXPECT_TRUE(fa);

  bool fb = cpp_cli::cpp_cli_internal::settings_have_unique_names<Setting<Arg2, int>, Setting<Arg1, int>>::value;
  EXPECT_TRUE(fb);

  bool fc = cpp_cli::cpp_cli_internal::settings_have_unique_names<Setting<Arg2, int>, Setting<Arg2, int>>::value;
  EXPECT_FALSE(fc);

  bool fd = cpp_cli::cpp_cli_internal::settings_have_unique_names<Setting<s1::X, int>, Setting<s2::X, int>>::value;
  EXPECT_TRUE(fd);

  bool fe = cpp_cli::cpp_cli_internal::settings_have_unique_names<Setting<Arg1, float>, Setting<Arg1, int>>::value;
  EXPECT_FALSE(fe);

  bool ff = cpp_cli::cpp_cli_internal::
      settings_have_unique_names<Setting<Arg1, float>, Setting<Arg2, int>, Setting<Arg1, float>>::value;
  EXPECT_FALSE(ff);
}
