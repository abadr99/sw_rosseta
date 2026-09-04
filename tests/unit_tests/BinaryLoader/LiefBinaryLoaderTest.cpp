#include <gtest/gtest.h>

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "frontend/BinaryLoaderInterface.hpp"
#include "frontend/LiefBinaryLoader.hpp"

using namespace rosetta::frontend::loader;  // NOLINT

namespace {

namespace fs = std::filesystem;

fs::path MakeX86Elf() {
  const fs::path dir = fs::temp_directory_path() / "rosetta_loader_test";
  std::error_code ec;
  fs::create_directories(dir, ec);
  if (ec) {
    return {};
  }

  const fs::path src = dir / "main.c";
  const fs::path out = dir / "x86_64_elf";

  {
    std::ofstream f(src);
    f << "int main(void) { return 42; }\n";
  }

  const std::string cmd =
      "gcc -o " + out.string() + " " + src.string() + " 2>/dev/null";
  if (std::system(cmd.c_str()) != 0 || !fs::exists(out)) {
    return {};
  }
  return out;
}

fs::path MakeGarbageFile() {
  const fs::path dir = fs::temp_directory_path() / "rosetta_loader_test";
  std::error_code ec;
  fs::create_directories(dir, ec);
  if (ec) {
    return {};
  }
  const fs::path out = dir / "garbage.bin";
  std::ofstream f(out, std::ios::binary);
  f << "this is not an executable";
  return out;
}

// Golden values from `readelf -h -l tests/unit_tests/BinaryLoader/main.elf`
//
//   Machine:             Advanced Micro Devices X86-64
//   Entry point address: 0x1040
//
//   LOAD  VirtAddr=0x1000  FileSiz=0x159  MemSiz=0x159  Flags=R E
//         maps sections: .init .plt .plt.got .text .fini
//   .text Address=0x1040  Size=0x109
constexpr uint64_t kEntryPoint = 0x1040;
constexpr uint64_t kRxVirtAddr = 0x1000;
constexpr uint64_t kRxFileSize = 0x159;
constexpr uint8_t kRxFirstBytes[] = {0xF3, 0x0F, 0x1E, 0xFA};  // .init: endbr64
constexpr uint8_t kEntryBytes[] = {
    0xF3, 0x0F, 0x1E, 0xFA, 0x31, 0xED};  // _start: endbr64; xor ebp, ebp

fs::path MainElf() { 
  // Get this file's directory
  const fs::path this_file_dir = fs::path(__FILE__).parent_path();
  // Join this file's directory with "main.elf"
  const fs::path main_elf_path = this_file_dir / "main.elf"; 
  return main_elf_path; 
}

}  // namespace

TEST(LiefBinaryLoaderTest, MissingFileLeavesParserEmpty) {
  LiefBinaryParser parser("/no/such/binary");
  EXPECT_EQ(parser.GetArchitecture(), Architecture::kUnknown);
}

TEST(LiefBinaryLoaderTest, GarbageFileIsNotX86) {
  const fs::path garbage = MakeGarbageFile();
  ASSERT_FALSE(garbage.empty());
  LiefBinaryParser parser(garbage);
  EXPECT_EQ(parser.GetArchitecture(), Architecture::kUnknown);
}

TEST(LiefBinaryLoaderTest, DetectsX86_64) {
  const fs::path elf = MakeX86Elf();
  ASSERT_FALSE(elf.empty()) << "gcc not available; skipping";
  LiefBinaryParser parser(elf);
  EXPECT_EQ(parser.GetArchitecture(), Architecture::kX86_64);
}

TEST(LiefBinaryLoaderTest, EntryPointIsNonZero) {
  const fs::path elf = MakeX86Elf();
  ASSERT_FALSE(elf.empty()) << "gcc not available; skipping";
  LiefBinaryParser parser(elf);
  EXPECT_NE(parser.GetEntryPoint(), 0);
}

TEST(LiefBinaryLoaderTest, ExecutableCodeContainsEntryPoint) {
  const fs::path elf = MakeX86Elf();
  ASSERT_FALSE(elf.empty()) << "gcc not available; skipping";
  LiefBinaryParser parser(elf);

  const BinarySection code = parser.GetExecutableCode();
  const uint64_t entry = parser.GetEntryPoint();

  EXPECT_GE(entry, code.VirtualAddress);
  EXPECT_LT(entry, code.VirtualAddress + code.Data.size());
}

TEST(LiefBinaryLoaderTest, ExecutableCodeIsNonEmpty) {
  const fs::path elf = MakeX86Elf();
  ASSERT_FALSE(elf.empty()) << "gcc not available; skipping";
  LiefBinaryParser parser(elf);

  const BinarySection code = parser.GetExecutableCode();
  EXPECT_FALSE(code.Data.empty());
  EXPECT_NE(code.VirtualAddress, 0);
}

TEST(LiefBinaryLoaderTest, ExecutableCodeStartsWithEndbr64) {
  const fs::path elf = MakeX86Elf();
  ASSERT_FALSE(elf.empty()) << "gcc not available; skipping";
  LiefBinaryParser parser(elf);

  const BinarySection code = parser.GetExecutableCode();
  ASSERT_GE(code.Data.size(), 4);

  EXPECT_EQ(code.Data[0], 0xF3);
  EXPECT_EQ(code.Data[1], 0x0F);
  EXPECT_EQ(code.Data[2], 0x1E);
  EXPECT_EQ(code.Data[3], 0xFA);
}

TEST(LiefBinaryLoaderTest, LoadableSegmentsNotYetImplemented) {
  const fs::path elf = MakeX86Elf();
  ASSERT_FALSE(elf.empty()) << "gcc not available; skipping";
  LiefBinaryParser parser(elf);

  EXPECT_DEATH(parser.GetLoadableSegments(), "Unimplemented Function");
}

class MainElfTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(fs::exists(MainElf())) << "missing fixture " << MainElf();
    parser_ = std::make_unique<LiefBinaryParser>(MainElf());
  }

  std::unique_ptr<LiefBinaryParser> parser_;
};

TEST_F(MainElfTest, ArchitectureMatchesReadelfX86_64) {
  EXPECT_EQ(parser_->GetArchitecture(), Architecture::kX86_64);
}

TEST_F(MainElfTest, EntryPointMatchesReadelf) {
  EXPECT_EQ(parser_->GetEntryPoint(), kEntryPoint);
}

TEST_F(MainElfTest, ExecutableCodeIsTheRxLoadSegment) {
  const BinarySection code = parser_->GetExecutableCode();

  EXPECT_EQ(code.VirtualAddress, kRxVirtAddr);
  EXPECT_EQ(code.Data.size(), kRxFileSize);
}

TEST_F(MainElfTest, ExecutableSegmentContainsReadelfEntry) {
  const BinarySection code = parser_->GetExecutableCode();
  EXPECT_GE(kEntryPoint, code.VirtualAddress);
  EXPECT_LT(kEntryPoint, code.VirtualAddress + code.Data.size());
}

TEST_F(MainElfTest, RxSegmentStartsWithInitEndbr64) {
  const BinarySection code = parser_->GetExecutableCode();
  ASSERT_GE(code.Data.size(), sizeof(kRxFirstBytes));
  for (size_t i = 0; i < sizeof(kRxFirstBytes); ++i) {
    EXPECT_EQ(code.Data[i], kRxFirstBytes[i]) << "byte " << i;
  }
}

TEST_F(MainElfTest, BytesAtEntryMatchReadelfText) {
  const BinarySection code = parser_->GetExecutableCode();
  const uint64_t off = kEntryPoint - kRxVirtAddr;
  ASSERT_GE(code.Data.size(), off + sizeof(kEntryBytes));
  for (size_t i = 0; i < sizeof(kEntryBytes); ++i) {
    EXPECT_EQ(code.Data[off + i], kEntryBytes[i]) << "byte " << i;
  }
}
