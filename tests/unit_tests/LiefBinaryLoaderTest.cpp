#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

#include "frontend/BinaryLoaderInterface.hpp"
#include "frontend/LiefBinaryLoader.hpp"

using namespace rosetta::frontend::loader;  // NOLINT

namespace {

namespace fs = std::filesystem;

// Compiles a tiny x86-64 ELF at test time so no binary is checked in.
// Returns the path, or an empty path if the compiler is unavailable.
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

// Writes a file that is not a valid ELF/PE.
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

}  // namespace

// ---------------------------------------------------------------------------
// Parse failure
// ---------------------------------------------------------------------------

TEST(LiefBinaryLoaderTest, MissingFileLeavesParserEmpty) {
  LiefBinaryParser parser("/no/such/binary");
  // The constructor does not throw; every query must handle a null binary.
  // GetArchitecture is the only method that is safe to call unconditionally.
  EXPECT_EQ(parser.GetArchitecture(), Architecture::kUnknown);
}

TEST(LiefBinaryLoaderTest, GarbageFileIsNotX86) {
  const fs::path garbage = MakeGarbageFile();
  ASSERT_FALSE(garbage.empty());
  LiefBinaryParser parser(garbage);
  EXPECT_EQ(parser.GetArchitecture(), Architecture::kUnknown);
}

// ---------------------------------------------------------------------------
// Architecture detection
// ---------------------------------------------------------------------------

TEST(LiefBinaryLoaderTest, DetectsX86_64) {
  const fs::path elf = MakeX86Elf();
  ASSERT_FALSE(elf.empty()) << "gcc not available; skipping";
  LiefBinaryParser parser(elf);
  EXPECT_EQ(parser.GetArchitecture(), Architecture::kX86_64);
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

TEST(LiefBinaryLoaderTest, EntryPointIsNonZero) {
  const fs::path elf = MakeX86Elf();
  ASSERT_FALSE(elf.empty()) << "gcc not available; skipping";
  LiefBinaryParser parser(elf);
  EXPECT_NE(parser.GetEntryPoint(), 0);
}

// ---------------------------------------------------------------------------
// Executable code
// ---------------------------------------------------------------------------

TEST(LiefBinaryLoaderTest, ExecutableCodeContainsEntryPoint) {
  const fs::path elf = MakeX86Elf();
  ASSERT_FALSE(elf.empty()) << "gcc not available; skipping";
  LiefBinaryParser parser(elf);

  const BinarySection code = parser.GetExecutableCode();
  const uint64_t entry = parser.GetEntryPoint();

  // The returned region must contain the entry point.
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

  // Modern GCC emits endbr64 (f3 0f 1e fa) at the start of the code segment.
  EXPECT_EQ(code.Data[0], 0xF3);
  EXPECT_EQ(code.Data[1], 0x0F);
  EXPECT_EQ(code.Data[2], 0x1E);
  EXPECT_EQ(code.Data[3], 0xFA);
}

// ---------------------------------------------------------------------------
// Loadable segments (not yet implemented — expect empty)
// ---------------------------------------------------------------------------

TEST(LiefBinaryLoaderTest, LoadableSegmentsNotYetImplemented) {
  const fs::path elf = MakeX86Elf();
  ASSERT_FALSE(elf.empty()) << "gcc not available; skipping";
  LiefBinaryParser parser(elf);

  // GetLoadableSegments is currently UNIMPLEMENTED() and aborts.
  // When it is implemented, replace this with a test that expects
  // the real segment list (4 PT_LOAD segments for a typical PIE binary).
  EXPECT_DEATH(parser.GetLoadableSegments(), "Unimplemented Function");
}
