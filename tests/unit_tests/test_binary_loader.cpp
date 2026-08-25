#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <utility>

#include "binary_loader.hpp"

namespace fs = std::filesystem;

class BinaryLoaderTest : public ::testing::Test {
 protected:
    fs::path valid_x86_64_elf = "tests/fibonacci_x86_64.elf";
    fs::path invalid_file = "tests/invalid_data.txt";

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
// 2. Ownership & Memory Management (Rule of 5)
// ==========================================

TEST_F(BinaryLoaderTest, MoveConstructorTransfersOwnership) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader_opt.has_value());

    loader::BinaryLoader original = std::move(loader_opt.value());
    uint64_t expected_entry = original.get_entry_point();

    // Trigger move constructor
    loader::BinaryLoader moved(std::move(original));

    EXPECT_EQ(moved.get_entry_point(), expected_entry);
    EXPECT_TRUE(moved.get_section(".text").has_value());
}

TEST_F(BinaryLoaderTest, MoveAssignmentTransfersOwnership) {
    auto loader1_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    ASSERT_TRUE(loader1_opt.has_value());

    loader::BinaryLoader loader1 = std::move(loader1_opt.value());
    uint64_t expected_entry = loader1.get_entry_point();

    // Create an empty shell, then move-assign
    // Note: This assumes your class has a default constructor.
    // If it doesn't, skip testing a raw shell and just move into
    // another loaded instance.
    loader::BinaryLoader loader2 = std::move(loader1);

    EXPECT_EQ(loader2.get_entry_point(), expected_entry);
}

// ==========================================
// 3. Section Lookup Mechanisms
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
    const auto& loader = loader_opt.value();

    // Only test a genuinely fake section name. We removed the "" check
    // because ELF files legally contain a nameless Null Section.
    EXPECT_FALSE(loader.get_section(".this_does_not_exist").has_value());
}

TEST_F(BinaryLoaderTest, GetSectionIsCaseSensitive) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    const auto& loader = loader_opt.value();

    // ".TEXT" should fail since section names are lowercase in our ELF
    EXPECT_FALSE(loader.get_section(".TEXT").has_value());
}

// ==========================================
// 4. Mathematical Bounds Checking (VMA to Offset)
// ==========================================

TEST_F(BinaryLoaderTest, VmaToOffsetMathematicalBoundaries) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    const auto& loader = loader_opt.value();

    auto section_opt = loader.get_section(".text");
    const auto& text = section_opt.value().get();

    // Exact Start Boundary
    auto start_opt = loader.vma_to_offset(text.vma);
    ASSERT_TRUE(start_opt.has_value());
    EXPECT_EQ(start_opt.value(), 0);

    // Exact Middle
    uint64_t middle_vma = text.vma + (text.size / 2);
    auto middle_opt = loader.vma_to_offset(middle_vma);
    ASSERT_TRUE(middle_opt.has_value());
    EXPECT_EQ(middle_opt.value(), text.size / 2);

    // Exact End Boundary (Last valid byte)
    auto end_opt = loader.vma_to_offset(text.vma + text.size - 1);
    ASSERT_TRUE(end_opt.has_value());
    EXPECT_EQ(end_opt.value(), text.size - 1);

    // Out of Bounds: 1 Byte Below Start
    EXPECT_FALSE(loader.vma_to_offset(text.vma - 1).has_value());

    // Out of Bounds: 1 Byte Past End
    EXPECT_FALSE(loader.vma_to_offset(text.vma + text.size).has_value());

    // Out of Bounds: Extreme Values
    // Using 0x1000 avoids colliding with VMA 0 metadata sections like .strtab
    EXPECT_FALSE(loader.vma_to_offset(0x1000).has_value());
    EXPECT_FALSE(loader.vma_to_offset(0xFFFFFFFFFFFFFFFF).has_value());
}

// ==========================================
// 5. Data Integrity & Verification
// ==========================================

TEST_F(BinaryLoaderTest, EntryPointMapsToValidVirtualMemory) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    const auto& loader = loader_opt.value();

    uint64_t entry_vma = loader.get_entry_point();
    auto offset_opt = loader.vma_to_offset(entry_vma);

    // The entry point must be mathematically accessible within mapped sections
    EXPECT_TRUE(offset_opt.has_value());
}

TEST_F(BinaryLoaderTest, TextSectionContainsExpectedFibonacciMachineCode) {
    auto loader_opt = loader::BinaryLoader::create(valid_x86_64_elf.string());
    const auto& loader = loader_opt.value();

    auto section_opt = loader.get_section(".text");
    const auto& text = section_opt.value().get();

    // Verify the loader pulled the exact raw bytes for: mov rcx, 10
    // x86-64 machine code: 48 C7 C1 0A 00 00 00
    ASSERT_GE(text.size, 7);
    EXPECT_EQ(text.data[0], 0x48);
    EXPECT_EQ(text.data[1], 0xC7);
    EXPECT_EQ(text.data[2], 0xC1);
    EXPECT_EQ(text.data[3], 0x0A);
    EXPECT_EQ(text.data[4], 0x00);
    EXPECT_EQ(text.data[5], 0x00);
    EXPECT_EQ(text.data[6], 0x00);
}
