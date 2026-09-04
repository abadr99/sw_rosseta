#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <utility>

#include "frontend/BinaryLoader.hpp"

namespace fs = std::filesystem;
namespace loader = rosetta::frontend::loader;

class BinaryLoaderTest : public ::testing::Test {
 protected:
    fs::path valid_x86_64_elf = "tests/fibonacci_x86_64.elf";
    fs::path invalid_file = "tests/invalid_data.txt";

    // Additional test fixtures
    fs::path non_x86_64_elf = "tests/non_x86_64.elf";
    fs::path no_text_x86_64_elf = "tests/no_text_x86_64.elf";

    void SetUp() override {
        // Generate a corrupt/non-ELF file on the fly for error testing
        std::ofstream ofs(invalid_file);
        ofs << "01010101 Not an ELF file, just garbage data.";
        ofs.close();
    }

    void TearDown() override {
        if (fs::exists(invalid_file)) {
            fs::remove(invalid_file);
        }
    }
};

// ==========================================
// 1. File I/O and Format Validation
// ==========================================

TEST_F(BinaryLoaderTest, FailsGracefullyOnMissingFile) {
    auto loader_opt = loader::BinaryLoader::create("does_not_exist.elf");
    EXPECT_FALSE(loader_opt.has_value());
}

TEST_F(BinaryLoaderTest, FailsGracefullyOnEmptyPath) {
    auto loader_opt = loader::BinaryLoader::create("");
    EXPECT_FALSE(loader_opt.has_value());
}

TEST_F(BinaryLoaderTest, FailsGracefullyOnCorruptedFileFormat) {
    auto loader_opt = loader::BinaryLoader::create(invalid_file.string());
    EXPECT_FALSE(loader_opt.has_value());
}

// ==========================================
// 2. Architecture Validation
// ==========================================

TEST_F(BinaryLoaderTest, RejectsNonX86_64Binary) {
    auto loader_opt = loader::BinaryLoader::create(non_x86_64_elf.string());
    EXPECT_FALSE(loader_opt.has_value());
}

// ==========================================
// 3. Required Section Validation
// ==========================================

TEST_F(BinaryLoaderTest, RejectsX86_64BinaryWithoutTextSection) {
    auto loader_opt = loader::BinaryLoader::create(no_text_x86_64_elf.string());
    EXPECT_FALSE(loader_opt.has_value());
}

// ==========================================
// 4. Ownership & Memory Management
// ==========================================

TEST_F(BinaryLoaderTest, MoveConstructorTransfersOwnership) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader_opt.has_value());

    loader::BinaryLoader original = std::move(loader_opt.value());
    uint64_t expected_entry = original.get_entry_point();

    // Trigger move constructor
    loader::BinaryLoader moved(std::move(original));

    // Destination keeps the original data
    EXPECT_EQ(moved.get_entry_point(), expected_entry);
    EXPECT_TRUE(moved.get_section(".text").has_value());

    // Our implementation explicitly resets the moved-from object's entry point.
    EXPECT_EQ(original.get_entry_point(), 0);
}

TEST_F(BinaryLoaderTest, MoveAssignmentTransfersOwnership) {
    auto loader1_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader1_opt.has_value());

    auto loader2_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader2_opt.has_value());

    loader::BinaryLoader loader1 = std::move(loader1_opt.value());
    loader::BinaryLoader loader2 = std::move(loader2_opt.value());
    uint64_t expected_entry = loader1.get_entry_point();

    // loader2 already contains data here.
    // This tests replacing its existing state.
    loader2 = std::move(loader1);

    EXPECT_EQ(loader2.get_entry_point(), expected_entry);
    EXPECT_TRUE(loader2.get_section(".text").has_value());

    // Our implementation resets the moved-from object's entry point.
    EXPECT_EQ(loader1.get_entry_point(), 0);
}

TEST_F(BinaryLoaderTest, SelfMoveAssignmentDoesNotCorruptObject) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader_opt.has_value());

    loader::BinaryLoader loader = std::move(loader_opt.value());
    uint64_t expected_entry = loader.get_entry_point();

    // Test: loader = std::move(loader);
    // The implementation contains: if (this != &other)
    loader = std::move(loader);

    EXPECT_EQ(loader.get_entry_point(), expected_entry);
    EXPECT_TRUE(loader.get_section(".text").has_value());
}

// ==========================================
// 5. Section Lookup Mechanisms
// ==========================================

TEST_F(BinaryLoaderTest, GetSectionReturnsValidDataForText) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader_opt.has_value());

    const auto& loader = loader_opt.value();
    auto section_opt = loader.get_section(".text");
    ASSERT_TRUE(section_opt.has_value());

    const auto& section = section_opt.value().get();
    EXPECT_EQ(section.name, ".text");
    EXPECT_GT(section.size, 0);
    EXPECT_EQ(section.data.size(), section.size);
}

TEST_F(BinaryLoaderTest, GetSectionReturnsNulloptForMissingSection) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader_opt.has_value());

    const auto& loader = loader_opt.value();
    EXPECT_FALSE(loader.get_section(".this_does_not_exist").has_value());
}

TEST_F(BinaryLoaderTest, GetSectionIsCaseSensitive) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader_opt.has_value());

    const auto& loader = loader_opt.value();

    // ".TEXT" should fail because the actual section name is ".text".
    EXPECT_FALSE(loader.get_section(".TEXT").has_value());
}

// ==========================================
// 6. Other Section Data
// ==========================================

TEST_F(BinaryLoaderTest, DataSectionContainsValidData) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader_opt.has_value());

    const auto& loader = loader_opt.value();
    auto section_opt = loader.get_section(".data");

    // Some perfectly valid ELF files may not contain a .data section,
    // so don't fail just because it doesn't exist.
    if (!section_opt.has_value()) {
        GTEST_SKIP() << "ELF file does not contain a .data section";
    }

    const auto& section = section_opt.value().get();
    EXPECT_EQ(section.name, ".data");
    EXPECT_EQ(section.data.size(), section.size);
}

// ==========================================
// 7. Mathematical Bounds Checking
//    VMA -> Offset
// ==========================================

TEST_F(BinaryLoaderTest, VmaToOffsetMathematicalBoundaries) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader_opt.has_value());

    const auto& loader = loader_opt.value();
    auto section_opt = loader.get_section(".text");
    ASSERT_TRUE(section_opt.has_value());

    const auto& text = section_opt.value().get();

    // --------------------------------------
    // Exact Start Boundary
    // --------------------------------------
    auto start_opt = loader.vma_to_offset(text.vma);
    ASSERT_TRUE(start_opt.has_value());
    EXPECT_EQ(start_opt.value(), 0);

    // --------------------------------------
    // Exact Middle
    // --------------------------------------
    uint64_t middle_vma = text.vma + (text.size / 2);
    auto middle_opt = loader.vma_to_offset(middle_vma);
    ASSERT_TRUE(middle_opt.has_value());
    EXPECT_EQ(middle_opt.value(), text.size / 2);

    // --------------------------------------
    // Exact End Boundary (Last valid byte)
    // --------------------------------------
    auto end_opt = loader.vma_to_offset(text.vma + text.size - 1);
    ASSERT_TRUE(end_opt.has_value());
    EXPECT_EQ(end_opt.value(), text.size - 1);

    // --------------------------------------
    // Out of Bounds: 1 byte below start
    // --------------------------------------
    EXPECT_FALSE(loader.vma_to_offset(text.vma - 1).has_value());

    // --------------------------------------
    // Out of Bounds: 1 byte past end
    // --------------------------------------
    EXPECT_FALSE(loader.vma_to_offset(text.vma + text.size).has_value());

    // --------------------------------------
    // Extreme Values
    // --------------------------------------
    EXPECT_FALSE(loader.vma_to_offset(0x1000).has_value());
    EXPECT_FALSE(loader.vma_to_offset(0xFFFFFFFFFFFFFFFF).has_value());
}

// ==========================================
// 8. Entry Point Validation
// ==========================================

TEST_F(BinaryLoaderTest, EntryPointMapsToValidVirtualMemory) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader_opt.has_value());

    const auto& loader = loader_opt.value();
    uint64_t entry_vma = loader.get_entry_point();
    auto offset_opt = loader.vma_to_offset(entry_vma);

    // The entry point must correspond to an address inside one of the
    // loaded sections.
    EXPECT_TRUE(offset_opt.has_value());
}

TEST_F(BinaryLoaderTest, EntryPointIsNotZero) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader_opt.has_value());

    const auto& loader = loader_opt.value();
    EXPECT_NE(loader.get_entry_point(), 0);
}

// ==========================================
// 9. General ELF Loader Integrity
// ==========================================

TEST_F(BinaryLoaderTest, TextSectionHasConsistentMetadata) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader_opt.has_value());

    const auto& loader = loader_opt.value();
    auto section_opt = loader.get_section(".text");
    ASSERT_TRUE(section_opt.has_value());

    const auto& text = section_opt.value().get();

    // Section name is correct
    EXPECT_EQ(text.name, ".text");
    // Section has actual contents
    EXPECT_GT(text.size, 0);
    // Number of bytes loaded equals section size
    EXPECT_EQ(text.data.size(), text.size);
    // VMA should be a valid address
    EXPECT_NE(text.vma, 0);
}
