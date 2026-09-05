#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "frontend/Cli11OptionParser.hpp"

namespace {

using rosetta::frontend::Cli11OptionParser;

class Argv {
 public:
  explicit Argv(std::initializer_list<std::string> args) : storage_(args) {
    pointers_.reserve(storage_.size());
    for (std::string& arg : storage_) {
      pointers_.push_back(arg.data());
    }
  }

  int Count() const { return static_cast<int>(pointers_.size()); }
  char** Values() { return pointers_.data(); }

 private:
  std::vector<std::string> storage_;
  std::vector<char*> pointers_;
};

TEST(Cli11OptionParserTest, ParsesLongOptionsIntoBoundValues) {
  Cli11OptionParser parser;
  std::string input;
  std::string output;
  std::string stage;

  parser.AddOption("-i,--input", input, true, "Input file");
  parser.AddOption("-o,--output", output, true, "Output file");
  parser.AddOption("-s,--stop-after", stage, false, "Pipeline stage");

  Argv args({"rosetta", "--input", "in.bin", "--output", "out.ll", "--stop-after", "loader"});  // NOLINT
  ASSERT_NO_THROW(parser.Parse(args.Count(), args.Values()));

  EXPECT_EQ(input, "in.bin");
  EXPECT_EQ(output, "out.ll");
  EXPECT_EQ(stage, "loader");
}

TEST(Cli11OptionParserTest, ParsesShortOptionsIntoBoundValues) {
  Cli11OptionParser parser;
  std::string input;
  std::string output;

  parser.AddOption("-i,--input", input, true, "Input file");
  parser.AddOption("-o,--output", output, true, "Output file");

  Argv args({"rosetta", "-i", "in.bin", "-o", "out.ll"});
  ASSERT_NO_THROW(parser.Parse(args.Count(), args.Values()));

  EXPECT_EQ(input, "in.bin");
  EXPECT_EQ(output, "out.ll");
}

TEST(Cli11OptionParserTest, ParsesEqualsForm) {
  Cli11OptionParser parser;
  std::string input;
  std::string output;

  parser.AddOption("-i,--input", input, true, "Input file");
  parser.AddOption("-o,--output", output, true, "Output file");

  Argv args({"rosetta", "--input=in.bin", "--output=out.ll"});
  ASSERT_NO_THROW(parser.Parse(args.Count(), args.Values()));

  EXPECT_EQ(input, "in.bin");
  EXPECT_EQ(output, "out.ll");
}

TEST(Cli11OptionParserTest, OptionalOptionKeepsDefaultWhenOmitted) {
  Cli11OptionParser parser;
  std::string input;
  std::string output;
  std::string stage = "all";

  parser.AddOption("-i,--input", input, true, "Input file");
  parser.AddOption("-o,--output", output, true, "Output file");
  parser.AddOption("-s,--stop-after", stage, false, "Pipeline stage");

  Argv args({"rosetta", "--input", "in.bin", "--output", "out.ll"});
  ASSERT_NO_THROW(parser.Parse(args.Count(), args.Values()));

  EXPECT_EQ(stage, "all");
}

TEST(Cli11OptionParserTest, ParsesIntAndBoolOptions) {
  Cli11OptionParser parser;
  int count = 0;
  bool enabled = false;

  parser.AddOption("--count", count, true, "Count");
  parser.AddOption("--enabled", enabled, true, "Enabled");

  Argv args({"rosetta", "--count", "7", "--enabled", "true"});
  ASSERT_NO_THROW(parser.Parse(args.Count(), args.Values()));

  EXPECT_EQ(count, 7);
  EXPECT_TRUE(enabled);
}

TEST(Cli11OptionParserTest, MissingRequiredOptionThrows) {
  Cli11OptionParser parser;
  std::string input;
  std::string output;

  parser.AddOption("-i,--input", input, true, "Input file");
  parser.AddOption("-o,--output", output, true, "Output file");

  Argv args({"rosetta", "--input", "in.bin"});
  EXPECT_THROW(parser.Parse(args.Count(), args.Values()), CLI::RequiredError);
  EXPECT_TRUE(output.empty());
}

TEST(Cli11OptionParserTest, UnknownOptionThrows) {
  Cli11OptionParser parser;
  std::string input;
  std::string output;

  parser.AddOption("-i,--input", input, true, "Input file");
  parser.AddOption("-o,--output", output, true, "Output file");

  Argv args({"rosetta", "--input", "in.bin", "--output", "out.ll", "--stop-after", "loader"});  // NOLINT
  EXPECT_THROW(parser.Parse(args.Count(), args.Values()), CLI::ParseError);
}

TEST(Cli11OptionParserTest, TypoForRequiredOptionThrows) {
  Cli11OptionParser parser;
  std::string input;
  std::string output;

  parser.AddOption("-i,--input", input, true, "Input file");
  parser.AddOption("-o,--output", output, true, "Output file");

  Argv args({"rosetta", "--input", "file", "--outptut", "ff"});
  EXPECT_THROW(parser.Parse(args.Count(), args.Values()), CLI::ParseError);
}

TEST(Cli11OptionParserTest, HelpListsRegisteredOptions) {
  Cli11OptionParser parser;
  std::string input;
  std::string output;

  parser.AddOption("-i,--input", input, true, "Input file");
  parser.AddOption("-o,--output", output, true, "Output file");

  testing::internal::CaptureStdout();
  parser.PrintHelp();
  const std::string help = testing::internal::GetCapturedStdout();

  EXPECT_NE(help.find("--input"), std::string::npos);
  EXPECT_NE(help.find("--output"), std::string::npos);
  EXPECT_NE(help.find("Input file"), std::string::npos);
}

}  // namespace
