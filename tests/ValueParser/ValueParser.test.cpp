#include <CLIException/CLIException.h>
#include <ValueParser/ValueParser.h>
#include <gtest/gtest.h>

#include <filesystem>

TEST(ValueParser_Test, parse_test) {
  using cpp_cli::parse;

  EXPECT_EQ(parse<int>("123"), 123);
  EXPECT_EQ(parse<int>("-123"), -123);
  EXPECT_THROW(
      {
        try {
          EXPECT_EQ(parse<int>("-12.5"), -123);
        } catch (const cpp_cli::CLIException &e) {
          EXPECT_STREQ("Invalid argument (-12.5) for an argument of type int.", e.what());
          throw;
        }
      },
      cpp_cli::CLIException
  );
  EXPECT_THROW(
      {
        try {
          EXPECT_EQ(parse<int>("99999999999999999999999999999999999"), -123);
        } catch (const cpp_cli::CLIException &e) {
          EXPECT_STREQ(
              "Value (99999999999999999999999999999999999) does not fit into an argument of type int.", e.what()
          );
          throw;
        }
      },
      cpp_cli::CLIException
  );

  EXPECT_EQ(parse<unsigned int>("123"), 123u);
  EXPECT_THROW(
      {
        try {
          EXPECT_EQ(parse<unsigned int>("12.5"), -123);
        } catch (const cpp_cli::CLIException &e) {
          EXPECT_STREQ("Invalid argument (12.5) for an argument of type unsigned int.", e.what());
          throw;
        }
      },
      cpp_cli::CLIException
  );
  EXPECT_THROW(
      {
        try {
          EXPECT_EQ(parse<unsigned int>("99999999999999999999999999999999999"), -123);
        } catch (const cpp_cli::CLIException &e) {
          EXPECT_STREQ(
              "Value (99999999999999999999999999999999999) does not fit into an argument of type unsigned int.",
              e.what()
          );
          throw;
        }
      },
      cpp_cli::CLIException
  );
  EXPECT_THROW(
      {
        try {
          EXPECT_EQ(parse<unsigned int>("-12"), -12);
        } catch (const cpp_cli::CLIException &e) {
          EXPECT_STREQ("Negative number not allowed as value of an unsigned int.", e.what());
          throw;
        }
      },
      cpp_cli::CLIException
  );

  EXPECT_FLOAT_EQ(parse<float>("123"), 123);
  EXPECT_FLOAT_EQ(parse<float>("-123"), -123);
  EXPECT_FLOAT_EQ(parse<float>("123.5"), 123.5);

  EXPECT_EQ(parse<std::string>("some string value"), std::string("some string value"));

  EXPECT_EQ(parse<std::filesystem::path>(std::filesystem::current_path()), std::filesystem::current_path());
  EXPECT_EQ(parse<std::filesystem::path>("."), std::filesystem::current_path());
  EXPECT_EQ(parse<std::filesystem::path>(".."), std::filesystem::current_path().parent_path());
}