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
using nArg = cpp_cli::NamedArgument<ArgNames, Name, ValueType>;

TEST(ProgramArguments_TEST, type_trait_test) {
  // check if a type is a pair containing an enum and an Argument
  bool aa = cpp_cli::is_named_arg<int>::value;
  EXPECT_FALSE(aa);

  bool ab = cpp_cli::is_named_arg<cpp_cli::CL_Argument<int>>::value;
  EXPECT_FALSE(ab);

  bool ac = cpp_cli::is_named_arg<nArg<TestType, Arg1, int>>::value;
  EXPECT_TRUE(ac);

  bool ad = cpp_cli::is_named_arg<nArg<TestType, Arg1, int>>::value;
  EXPECT_TRUE(ad);

  // check if a parameter pack contains only named arguments
  bool ba = cpp_cli::is_named_arg_pack<>::value;
  EXPECT_FALSE(ba);

  bool bb = cpp_cli::is_named_arg_pack<int>::value;
  EXPECT_FALSE(bb);

  bool bc = cpp_cli::is_named_arg_pack<cpp_cli::CL_Argument<int>>::value;
  EXPECT_FALSE(bc);

  bool bd = cpp_cli::is_named_arg_pack<nArg<TestType, Arg1, int>>::value;
  EXPECT_TRUE(bd);

  bool be = cpp_cli::is_named_arg_pack<nArg<TestType, Arg1, int>, nArg<s1::OtherType, s1::X, float>>::value;
  EXPECT_TRUE(be);

  bool bf = cpp_cli::is_named_arg_pack<nArg<TestType, Arg1, int>, int>::value;
  EXPECT_FALSE(bf);

  bool bg = cpp_cli::is_named_arg_pack<int, nArg<TestType, Arg1, int>>::value;
  EXPECT_FALSE(bg);

  bool bh = cpp_cli::is_named_arg_pack<nArg<TestType, Arg1, int>, int, nArg<TestType, Arg1, float>>::value;
  EXPECT_FALSE(bh);

  // extract information about a named arg from its template
  TestType ca = cpp_cli::named_arg_info<nArg<TestType, Arg1, int>>::arg_name;
  EXPECT_EQ(ca, Arg1);

  TestType cb = cpp_cli::named_arg_info<nArg<TestType, Arg2, int>>::arg_name;
  EXPECT_NE(cb, Arg1);
  EXPECT_EQ(cb, Arg2);

  bool cc = std::is_same<cpp_cli::named_arg_info<nArg<s1::OtherType, s1::X, int>>::arg_name_type, s1::OtherType>::value;
  EXPECT_TRUE(cc);

  bool cd = std::
      is_same<cpp_cli::named_arg_info<nArg<s1::OtherType, s1::X, std::string>>::arg_value_type, std::string>::value;
  EXPECT_TRUE(cd);

  // find first index of an element with specific name
  int da = cpp_cli::arg_index_by_name<TestType, Arg1, nArg<TestType, Arg2, int>>::value;
  EXPECT_EQ(-1, da);

  int db = cpp_cli::arg_index_by_name<TestType, Arg1, nArg<TestType, Arg1, int>>::value;
  EXPECT_EQ(0, db);

  int dc = cpp_cli::arg_index_by_name<TestType, Arg1, nArg<TestType, Arg1, int>, nArg<TestType, Arg1, float>>::value;
  EXPECT_EQ(0, dc);

  int dd = cpp_cli::arg_index_by_name<TestType, Arg1, nArg<TestType, Arg2, int>, nArg<TestType, Arg1, float>>::value;
  EXPECT_EQ(1, dd);

  int de = cpp_cli::arg_index_by_name<
      TestType,
      Arg1,
      nArg<TestType, Arg2, int>,
      nArg<TestType, Arg1, float>,
      nArg<TestType, Arg3, int>>::value;
  EXPECT_EQ(1, de);

  // find an arg in a pack by indexing it with the args name
  bool ea = std::
      is_same<cpp_cli::arg_by_name<TestType, Arg1, nArg<TestType, Arg1, int>>::type, nArg<TestType, Arg1, int>>::value;
  EXPECT_TRUE(ea);

  bool ec = std::is_same<
      cpp_cli::arg_by_name<TestType, Arg1, nArg<TestType, Arg1, int>, nArg<TestType, Arg1, float>>::type,
      nArg<TestType, Arg1, int>>::value;
  EXPECT_TRUE(ec);

  bool ed = std::is_same<
      cpp_cli::arg_by_name<TestType, Arg1, nArg<TestType, Arg2, int>, nArg<TestType, Arg1, float>>::type,
      nArg<TestType, Arg1, float>>::value;
  EXPECT_TRUE(ed);

  bool ee = std::is_same<
      cpp_cli::arg_by_name<
          TestType,
          Arg1,
          nArg<TestType, Arg2, int>,
          nArg<TestType, Arg1, float>,
          nArg<TestType, Arg3, int>>::type,
      nArg<TestType, Arg1, float>>::value;
  EXPECT_TRUE(ee);

  // check if the pack only contains names args using different names
  bool fa = cpp_cli::args_have_unique_names<nArg<TestType, Arg2, int>>::value;
  EXPECT_TRUE(fa);

  bool fb = cpp_cli::args_have_unique_names<nArg<TestType, Arg2, int>, nArg<TestType, Arg1, int>>::value;
  EXPECT_TRUE(fb);

  bool fc = cpp_cli::args_have_unique_names<nArg<TestType, Arg2, int>, nArg<TestType, Arg2, int>>::value;
  EXPECT_FALSE(fc);

  bool fd = cpp_cli::args_have_unique_names<nArg<s1::OtherType, s1::X, int>, nArg<s2::OtherType2, s2::X, int>>::value;
  EXPECT_TRUE(fd);

  bool fe = cpp_cli::args_have_unique_names<nArg<TestType, Arg1, float>, nArg<TestType, Arg1, int>>::value;
  EXPECT_FALSE(fe);

  bool ff = cpp_cli::args_have_unique_names<
      nArg<TestType, Arg1, float>,
      nArg<TestType, Arg2, int>,
      nArg<TestType, Arg1, float>>::value;
  EXPECT_FALSE(ff);
}

TEST(ProgramArguments_TEST, class_test) {
  // create a container for named args that takes args of generic types and allows access to an arg by its name
  cpp_cli::ProgramArguments<
      nArg<TestType, Arg3, int>,
      nArg<TestType, Arg1, int>,
      nArg<s1::OtherType, s1::X, std::string>,
      nArg<s2::OtherType2, s2::X, bool>>
      args{
          {"arg3"},
          {"arg2", '\0', "Second Argument", std::make_shared<int>(122)},
          {nullptr, 'x'},
          {nullptr, 'X', "Last Argument"}
      };

  auto arg1 = args.getArg<TestType, Arg1>();
  EXPECT_STREQ(arg1.longOpt, "arg2");
  EXPECT_FALSE(arg1.hasShort());
  EXPECT_STREQ(arg1.description.c_str(), "Second Argument");
  bool arg1HasValue = args.hasValue<TestType, Arg1>();
  EXPECT_TRUE(arg1HasValue);
  int arg1Value = args.getValue<TestType, Arg1>();
  EXPECT_EQ(*arg1.value.get(), 122);
  EXPECT_EQ(arg1Value, 122);
  EXPECT_FALSE(arg1.isRequired());

  auto otherX = args.getArg<s1::OtherType, s1::X>();
  EXPECT_FALSE(otherX.hasLong());
  EXPECT_EQ(otherX.shortOpt, 'x');
  EXPECT_FALSE(otherX.hasDescription());
  bool otherXHasValue = args.hasValue<s1::OtherType, s1::X>();
  EXPECT_FALSE(otherXHasValue);
  EXPECT_FALSE(otherX.hasValue());
  EXPECT_TRUE(otherX.isRequired());

  auto arg3 = args.getArg<TestType, Arg3>();
  EXPECT_STREQ(arg3.longOpt, "arg3");
  EXPECT_FALSE(arg3.hasShort());
  EXPECT_FALSE(arg3.hasDescription());
  bool arg3HasValue = args.hasValue<TestType, Arg3>();
  EXPECT_FALSE(arg3HasValue);
  EXPECT_FALSE(arg3.hasValue());
  EXPECT_TRUE(arg3.isRequired());

  auto other2X = args.getArg<s2::OtherType2, s2::X>();
  EXPECT_FALSE(other2X.hasLong());
  EXPECT_EQ(other2X.shortOpt, 'X');
  EXPECT_STREQ(other2X.description.c_str(), "Last Argument");
  bool other2XHasValue = args.hasValue<s2::OtherType2, s2::X>();
  EXPECT_FALSE(other2XHasValue);
  EXPECT_FALSE(other2X.hasValue());
  EXPECT_FALSE(other2X.isRequired());
}

TEST(ProgramArguments_TEST, parsing_test) {
  // test the function that takes a number of named args and the program call information and returns a named arg
  // container with the parsed values
  const char *cl[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool"};
  constexpr int argc{sizeof(cl) / sizeof(*cl)};
  auto parseRes = cpp_cli::parseProgramArgumentsFromCL(
      argc,
      cl,
      nArg<TestType, Arg3, int>{nullptr, 'i'},
      nArg<TestType, Arg1, std::string>{"arg1"},
      nArg<s2::OtherType2, s2::X, bool>{"bool"},
      nArg<s1::OtherType, s1::X, float>{"float", '\0', "", std::make_shared<float>(42.5)},
      nArg<TestType, Arg5, float>{"unknown1", '\0', "", std::make_shared<float>(2)},
      nArg<TestType, Arg2, bool>{"unknown2"},
      nArg<TestType, Arg4, float>{nullptr, 'u', "", std::make_shared<float>(23.8)}
  );

  auto parsedArgs = std::get<0>(parseRes);

  // check that the values were parsed correctly
  bool arg3HasValue = parsedArgs.hasValue<TestType, Arg3>();
  EXPECT_TRUE(arg3HasValue);
  int arg3Value = parsedArgs.getValue<TestType, Arg3>();
  EXPECT_EQ(arg3Value, 123);

  bool arg1HasValue = parsedArgs.hasValue<TestType, Arg1>();
  EXPECT_TRUE(arg1HasValue);
  std::string arg1Value = parsedArgs.getValue<TestType, Arg1>();
  EXPECT_STREQ(arg1Value.c_str(), "val1");

  bool other2XHasValue = parsedArgs.hasValue<s2::OtherType2, s2::X>();
  EXPECT_TRUE(other2XHasValue);
  bool other2XValue = parsedArgs.getValue<s2::OtherType2, s2::X>();
  EXPECT_TRUE(other2XValue);

  bool otherXHasValue = parsedArgs.hasValue<s1::OtherType, s1::X>();
  EXPECT_TRUE(otherXHasValue);
  float otherXValue = parsedArgs.getValue<s1::OtherType, s1::X>();
  EXPECT_FLOAT_EQ(otherXValue, 12.5);

  // check that a bool valued arg that is not found in the program call is set to false
  bool arg2HasValue = parsedArgs.hasValue<TestType, Arg2>();
  EXPECT_TRUE(arg2HasValue);
  bool arg2Value = parsedArgs.getValue<TestType, Arg2>();
  EXPECT_FALSE(arg2Value);

  // check that a non bool typed value with a default value still has that default value
  bool arg4HasValue = parsedArgs.hasValue<TestType, Arg4>();
  EXPECT_TRUE(arg4HasValue);
  float arg4Value = parsedArgs.getValue<TestType, Arg4>();
  EXPECT_FLOAT_EQ(arg4Value, 23.8);

  // if all flags in the program call are known the second value returned by the parse function should be -1
  int unknownFlagIndex = std::get<1>(parseRes);
  EXPECT_EQ(unknownFlagIndex, -1);

  // test that the second return value targets the correct index when there is an unknown value in the program call
  auto parse2Res = cpp_cli::parseProgramArgumentsFromCL(
      argc,
      cl,
      nArg<TestType, Arg3, int>{nullptr, 'i'},
      nArg<TestType, Arg1, std::string>{"arg1"},
      nArg<s2::OtherType2, s2::X, bool>{"bool"},
      nArg<TestType, Arg2, bool>{"unknown"}
  );

  int unknownFlagIndex2 = std::get<1>(parse2Res);
  EXPECT_EQ(unknownFlagIndex2, 5);

  auto parsedArgs2 = std::get<0>(parse2Res);

  // the rest should have been parsed as expected
  arg3HasValue = parsedArgs2.hasValue<TestType, Arg3>();
  EXPECT_TRUE(arg3HasValue);
  arg3Value = parsedArgs2.getValue<TestType, Arg3>();
  EXPECT_EQ(arg3Value, 123);

  arg1HasValue = parsedArgs2.hasValue<TestType, Arg1>();
  EXPECT_TRUE(arg1HasValue);
  arg1Value = parsedArgs2.getValue<TestType, Arg1>();
  EXPECT_STREQ(arg1Value.c_str(), "val1");

  other2XHasValue = parsedArgs2.hasValue<s2::OtherType2, s2::X>();
  EXPECT_TRUE(other2XHasValue);
  other2XValue = parsedArgs2.getValue<s2::OtherType2, s2::X>();
  EXPECT_TRUE(other2XValue);

  arg2HasValue = parsedArgs.hasValue<TestType, Arg2>();
  EXPECT_TRUE(arg2HasValue);
  arg2Value = parsedArgs.getValue<TestType, Arg2>();
  EXPECT_FALSE(arg2Value);

  // the function should throw if a required argument is not provided
  auto throwOnMissingReq = []() {
    const char *argv[]{"./main", "--arg1", "val1", "-i", "123", "--float", "12.5", "--bool"};
    constexpr int argc{sizeof(argv) / sizeof(*argv)};
    cpp_cli::parseProgramArgumentsFromCL(
        argc,
        argv,
        nArg<TestType, Arg3, int>{nullptr, 'i'},
        nArg<TestType, Arg1, std::string>{"arg1"},
        nArg<s2::OtherType2, s2::X, bool>{"bool"},
        nArg<TestType, Arg2, float>{"unknown"}
    );
  };

  EXPECT_THROW(throwOnMissingReq(), cpp_cli::CLIException);
}