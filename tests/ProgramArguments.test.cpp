#include <ProgramArguments.h>
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

template <typename ArgNames, ArgNames Name, typename ValueType>
using nOpt = cpp_cli::NamedOptionalArgument<ArgNames, Name, ValueType>;
template <typename ArgNames, ArgNames Name, typename ValueType>
using nReq = cpp_cli::NamedRequiredArgument<ArgNames, Name, ValueType>;

TEST(ProgramArguments_TEST, type_trait_test) {
  // check if a type is a pair containing an enum and an Argument
  bool aa = cpp_cli::is_named_arg<int>::value;
  EXPECT_FALSE(aa);

  bool ab = cpp_cli::is_named_arg<cpp_cli::OptionalArgument<int>>::value;
  EXPECT_FALSE(ab);

  bool ac = cpp_cli::is_named_arg<nOpt<TestType, Arg1, int>>::value;
  EXPECT_TRUE(ac);

  bool ad = cpp_cli::is_named_arg<nReq<TestType, Arg1, int>>::value;
  EXPECT_TRUE(ad);

  // check if a parameter pack contains only named arguments
  bool ba = cpp_cli::is_named_arg_pack<>::value;
  EXPECT_FALSE(ba);

  bool bb = cpp_cli::is_named_arg_pack<int>::value;
  EXPECT_FALSE(bb);

  bool bc = cpp_cli::is_named_arg_pack<cpp_cli::OptionalArgument<int>>::value;
  EXPECT_FALSE(bc);

  bool bd = cpp_cli::is_named_arg_pack<nOpt<TestType, Arg1, int>>::value;
  EXPECT_TRUE(bd);

  bool be = cpp_cli::is_named_arg_pack<nOpt<TestType, Arg1, int>, nReq<s1::OtherType, s1::X, float>>::value;
  EXPECT_TRUE(be);

  bool bf = cpp_cli::is_named_arg_pack<nOpt<TestType, Arg1, int>, int>::value;
  EXPECT_FALSE(bf);

  bool bg = cpp_cli::is_named_arg_pack<int, nReq<TestType, Arg1, int>>::value;
  EXPECT_FALSE(bg);

  bool bh = cpp_cli::is_named_arg_pack<nOpt<TestType, Arg1, int>, int, nOpt<TestType, Arg1, float>>::value;
  EXPECT_FALSE(bh);

  // extract information about a named arg from its template
  TestType ca = cpp_cli::named_arg_info<nOpt<TestType, Arg1, int>>::arg_name;
  EXPECT_EQ(ca, Arg1);

  TestType cb = cpp_cli::named_arg_info<nReq<TestType, Arg2, int>>::arg_name;
  EXPECT_NE(cb, Arg1);
  EXPECT_EQ(cb, Arg2);

  bool cc = std::is_same<cpp_cli::named_arg_info<nOpt<s1::OtherType, s1::X, int>>::arg_name_type, s1::OtherType>::value;
  EXPECT_TRUE(cc);

  bool cd = std::
      is_same<cpp_cli::named_arg_info<nOpt<s1::OtherType, s1::X, std::string>>::arg_value_type, std::string>::value;
  EXPECT_TRUE(cd);

  // find first index of an element with specific name
  int da = cpp_cli::arg_index_by_name<TestType, Arg1, nOpt<TestType, Arg2, int>>::value;
  EXPECT_EQ(-1, da);

  int db = cpp_cli::arg_index_by_name<TestType, Arg1, nOpt<TestType, Arg1, int>>::value;
  EXPECT_EQ(0, db);

  int dc = cpp_cli::arg_index_by_name<TestType, Arg1, nOpt<TestType, Arg1, int>, nOpt<TestType, Arg1, float>>::value;
  EXPECT_EQ(0, dc);

  int dd = cpp_cli::arg_index_by_name<TestType, Arg1, nOpt<TestType, Arg2, int>, nOpt<TestType, Arg1, float>>::value;
  EXPECT_EQ(1, dd);

  int de = cpp_cli::arg_index_by_name<
      TestType,
      Arg1,
      nOpt<TestType, Arg2, int>,
      nOpt<TestType, Arg1, float>,
      nOpt<TestType, Arg3, int>>::value;
  EXPECT_EQ(1, de);

  // find an arg in a pack by indexing it with the args name
  bool ea = std::
      is_same<cpp_cli::arg_by_name<TestType, Arg1, nOpt<TestType, Arg1, int>>::type, nOpt<TestType, Arg1, int>>::value;
  EXPECT_TRUE(ea);

  bool eb = std::
      is_same<cpp_cli::arg_by_name<TestType, Arg1, nOpt<TestType, Arg1, int>>::type, nReq<TestType, Arg1, int>>::value;
  EXPECT_FALSE(eb);

  bool ec = std::is_same<
      cpp_cli::arg_by_name<TestType, Arg1, nOpt<TestType, Arg1, int>, nOpt<TestType, Arg1, float>>::type,
      nOpt<TestType, Arg1, int>>::value;
  EXPECT_TRUE(ec);

  bool ed = std::is_same<
      cpp_cli::arg_by_name<TestType, Arg1, nOpt<TestType, Arg2, int>, nOpt<TestType, Arg1, float>>::type,
      nOpt<TestType, Arg1, float>>::value;
  EXPECT_TRUE(ed);

  bool ee = std::is_same<
      cpp_cli::arg_by_name<
          TestType,
          Arg1,
          nOpt<TestType, Arg2, int>,
          nOpt<TestType, Arg1, float>,
          nOpt<TestType, Arg3, int>>::type,
      nOpt<TestType, Arg1, float>>::value;
  EXPECT_TRUE(ee);

  // check if the pack only contains names args using different names
  bool fa = cpp_cli::args_have_unique_names<nOpt<TestType, Arg2, int>>::value;
  EXPECT_TRUE(fa);

  bool fb = cpp_cli::args_have_unique_names<nOpt<TestType, Arg2, int>, nOpt<TestType, Arg1, int>>::value;
  EXPECT_TRUE(fb);

  bool fc = cpp_cli::args_have_unique_names<nOpt<TestType, Arg2, int>, nReq<TestType, Arg2, int>>::value;
  EXPECT_FALSE(fc);

  bool fd = cpp_cli::args_have_unique_names<nOpt<s1::OtherType, s1::X, int>, nOpt<s2::OtherType2, s2::X, int>>::value;
  EXPECT_TRUE(fd);

  bool fe = cpp_cli::args_have_unique_names<nOpt<TestType, Arg1, float>, nReq<TestType, Arg1, int>>::value;
  EXPECT_FALSE(fe);

  bool ff = cpp_cli::args_have_unique_names<
      nOpt<TestType, Arg1, float>,
      nReq<TestType, Arg2, int>,
      nOpt<TestType, Arg1, float>>::value;
  EXPECT_FALSE(ff);
}

TEST(ProgramArguments_TEST, class_test) {
  // create a container for named args that takes args of generic types and allows access by to an arg by its name
  cpp_cli::ProgramArguments<
      nOpt<TestType, Arg3, int>,
      nOpt<TestType, Arg1, int>,
      nReq<s1::OtherType, s1::X, std::string>,
      nOpt<s2::OtherType2, s2::X, bool>>
      args{
          {"arg3"},
          {"arg2", '\0', "Second Argument", std::make_shared<int>(122)},
          {nullptr, 'x'},
          {nullptr, 'X', "Last Argument"}
      };

  auto aa = args.getArg<TestType, Arg1>();
  EXPECT_STREQ(aa.longOpt, "arg2");
  EXPECT_FALSE(aa.hasShort());
  EXPECT_STREQ(aa.description.c_str(), "Second Argument");
  bool aaHasValue = args.hasValue<TestType, Arg1>();
  EXPECT_TRUE(aaHasValue);
  int aaValue = args.getValue<TestType, Arg1>();
  EXPECT_EQ(*aa.value.get(), 122);
  EXPECT_EQ(aaValue, 122);
  EXPECT_FALSE(aa.isRequired());

  auto ab = args.getArg<s1::OtherType, s1::X>();
  EXPECT_FALSE(ab.hasLong());
  EXPECT_EQ(ab.shortOpt, 'x');
  EXPECT_FALSE(ab.hasDescription());
  bool abHasValue = args.hasValue<s1::OtherType, s1::X>();
  EXPECT_FALSE(abHasValue);
  EXPECT_FALSE(ab.hasValue());
  EXPECT_TRUE(ab.isRequired());

  auto ac = args.getArg<TestType, Arg3>();
  EXPECT_STREQ(ac.longOpt, "arg3");
  EXPECT_FALSE(ac.hasShort());
  EXPECT_FALSE(ac.hasDescription());
  bool acHasValue = args.hasValue<TestType, Arg3>();
  EXPECT_FALSE(acHasValue);
  EXPECT_FALSE(ac.hasValue());
  EXPECT_FALSE(ac.isRequired());

  auto ad = args.getArg<s2::OtherType2, s2::X>();
  EXPECT_FALSE(ad.hasLong());
  EXPECT_EQ(ad.shortOpt, 'X');
  EXPECT_STREQ(ad.description.c_str(), "Last Argument");
  bool adHasValue = args.hasValue<s2::OtherType2, s2::X>();
  EXPECT_FALSE(adHasValue);
  EXPECT_FALSE(ad.hasValue());
  EXPECT_FALSE(ad.isRequired());
}

TEST(ProgramArguments_TEST, parsing_test) {
  // test the function that takes a number of named args and the program call information and returns a named arg
  // container with the parsed values
  const char *cl[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool"};
  constexpr int argc{sizeof(cl) / sizeof(*cl)};
  auto parseRes = cpp_cli::parseProgramArgumentsFromCL(
      argc,
      cl,
      nReq<TestType, Arg3, int>{nullptr, 'i'},
      nOpt<TestType, Arg1, std::string>{"arg1"},
      nOpt<s2::OtherType2, s2::X, bool>{"bool"},
      nOpt<s1::OtherType, s1::X, float>{"float", '\0', "", std::make_shared<float>(42.5)},
      nOpt<TestType, Arg5, float>{"unknown1"},
      nOpt<TestType, Arg2, bool>{"unknown2"},
      nOpt<TestType, Arg4, float>{nullptr, 'u', "", std::make_shared<float>(23.8)}
  );

  auto parsedArgs = std::get<0>(parseRes);

  // check that the values were parsed correctly
  bool aaHasValue = parsedArgs.hasValue<TestType, Arg3>();
  EXPECT_TRUE(aaHasValue);
  int aaValue = parsedArgs.getValue<TestType, Arg3>();
  EXPECT_EQ(aaValue, 123);

  bool abHasValue = parsedArgs.hasValue<TestType, Arg1>();
  EXPECT_TRUE(abHasValue);
  std::string abValue = parsedArgs.getValue<TestType, Arg1>();
  EXPECT_STREQ(abValue.c_str(), "val1");

  bool acHasValue = parsedArgs.hasValue<s2::OtherType2, s2::X>();
  EXPECT_TRUE(acHasValue);
  bool acValue = parsedArgs.getValue<s2::OtherType2, s2::X>();
  EXPECT_TRUE(acValue);

  bool adHasValue = parsedArgs.hasValue<s1::OtherType, s1::X>();
  EXPECT_TRUE(adHasValue);
  float adValue = parsedArgs.getValue<s1::OtherType, s1::X>();
  EXPECT_FLOAT_EQ(adValue, 12.5);

  // check that a non bool valued arg that is not found still has no value
  bool aeHasValue = parsedArgs.hasValue<TestType, Arg5>();
  EXPECT_FALSE(aeHasValue);

  // check that a bool valued arg that is not found in the program call is set to false
  bool afHasValue = parsedArgs.hasValue<TestType, Arg2>();
  EXPECT_TRUE(afHasValue);
  bool afValue = parsedArgs.getValue<TestType, Arg2>();
  EXPECT_FALSE(afValue);

  // check that a non bool typed optional value with a default value still has that default value
  bool agHasValue = parsedArgs.hasValue<TestType, Arg4>();
  EXPECT_TRUE(agHasValue);
  float agValue = parsedArgs.getValue<TestType, Arg4>();
  EXPECT_FLOAT_EQ(agValue, 23.8);

  // if all flags in the program call are known the second value returned by the parse function should be -1
  int unknownFlagIndex = std::get<1>(parseRes);
  EXPECT_EQ(unknownFlagIndex, -1);

  // test that the second return value targets the correct index when there is an unknown value in the program call
  auto parse2Res = cpp_cli::parseProgramArgumentsFromCL(
      argc,
      cl,
      nReq<TestType, Arg3, int>{nullptr, 'i'},
      nOpt<TestType, Arg1, std::string>{"arg1"},
      nOpt<s2::OtherType2, s2::X, bool>{"bool"},
      nOpt<TestType, Arg2, bool>{"unknown"}
  );

  int unknownFlagIndex2 = std::get<1>(parse2Res);
  EXPECT_EQ(unknownFlagIndex2, 5);

  auto parsedArgs2 = std::get<0>(parse2Res);

  // the rest should have been parsed as expected
  bool baHasValue = parsedArgs2.hasValue<TestType, Arg3>();
  EXPECT_TRUE(baHasValue);
  int baValue = parsedArgs2.getValue<TestType, Arg3>();
  EXPECT_EQ(baValue, 123);

  bool bbHasValue = parsedArgs2.hasValue<TestType, Arg1>();
  EXPECT_TRUE(bbHasValue);
  std::string bbValue = parsedArgs2.getValue<TestType, Arg1>();
  EXPECT_STREQ(bbValue.c_str(), "val1");

  bool bcHasValue = parsedArgs2.hasValue<s2::OtherType2, s2::X>();
  EXPECT_TRUE(bcHasValue);
  bool bcValue = parsedArgs2.getValue<s2::OtherType2, s2::X>();
  EXPECT_TRUE(bcValue);

  bool bdHasValue = parsedArgs.hasValue<TestType, Arg2>();
  EXPECT_TRUE(bdHasValue);
  bool bdValue = parsedArgs.getValue<TestType, Arg2>();
  EXPECT_FALSE(bdValue);

  bool beHasValue = parsedArgs.hasValue<TestType, Arg5>();
  EXPECT_FALSE(beHasValue);

  bool bfHasValue = parsedArgs.hasValue<TestType, Arg4>();
  EXPECT_TRUE(bfHasValue);
  float bfValue = parsedArgs.getValue<TestType, Arg4>();
  EXPECT_FLOAT_EQ(bfValue, 23.8);

  // the function should throw if a required argument is not provided
  auto throwOnMissingReq = []() {
    const char *argv[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool"};
    constexpr int argc{sizeof(argv) / sizeof(*argv)};
    cpp_cli::parseProgramArgumentsFromCL(
        argc,
        argv,
        nReq<TestType, Arg3, int>{nullptr, 'i'},
        nOpt<TestType, Arg1, std::string>{"arg1"},
        nOpt<s2::OtherType2, s2::X, bool>{"bool"},
        nReq<TestType, Arg2, float>{"unknown"}
    );
  };

  EXPECT_THROW(throwOnMissingReq(), cpp_cli::CLIException);
}