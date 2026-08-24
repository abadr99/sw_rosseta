#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <string>

#include "binary_loader.hpp"

using namespace loader;
namespace fs = std::filesystem;

class BinaryLoaderTest : public ::testing::Test {
protected:
    // We create a temporary garbage file to test how the loader handles non-ELF/PE files.
    fs::path temp_garbage_file = "test_garbage.bin";
    
    // NOTE: For the valid tests to pass, you will need to place a real, 
    // minimal 64-bit x86 Linux ELF file at this path!
    fs::path valid_x86_64_elf = "test_data/dummy_x86_64.elf";

    void SetUp() override {
        // Generate a garbage file that isn't a valid binary
        std::ofstream ofs(temp_garbage_file, std::ios::binary);
        ofs << "This is not an ELF or PE file. It's just text.";
        ofs.close();
    }

    void TearDown() override {
        if (fs::exists(temp_garbage_file)) {
            fs::remove(temp_garbage_file);
        }
    }
};

// ==========================================
// 1. File I/O and Format Validation Tests
// ==========================================

TEST_F(BinaryLoaderTest, FailsGracefullyOnMissingFile) {
    auto loader = BinaryLoader::create("non_existent_file_path_12345.elf");
    EXPECT_FALSE(loader.has_value());
}

TEST_F(BinaryLoaderTest, FailsGracefullyOnInvalidFileFormat) {
    auto loader = BinaryLoader::create(temp_garbage_file);
    EXPECT_FALSE(loader.has_value());
}

// ==========================================
// 2. Memory Semantics Tests
// ==========================================

TEST_F(BinaryLoaderTest, MoveConstructorTransfersOwnership) {
    // Skip if the user hasn't provided a real test ELF yet
    if (!fs::exists(valid_x86_64_elf)) GTEST_SKIP() << "Missing real ELF for testing.";

    auto original = BinaryLoader::create(valid_x86_64_elf);
    ASSERT_TRUE(original.has_value());
    uint64_t entry = original->get_entry_point();

    // Move construct a new loader
    BinaryLoader moved_loader(std::move(*original));
    
    // The new loader should have the data
    EXPECT_EQ(moved_loader.get_entry_point(), entry);
    EXPECT_TRUE(moved_loader.get_section(".text").has_value());
}

TEST_F(BinaryLoaderTest, MoveAssignmentTransfersOwnership) {
    if (!fs::exists(valid_x86_64_elf)) GTEST_SKIP() << "Missing real ELF for testing.";

    auto original = BinaryLoader::create(valid_x86_64_elf);
    ASSERT_TRUE(original.has_value());
    uint64_t entry = original->get_entry_point();

    // Create an empty shell using a failed load to test assignment
    auto moved_loader = BinaryLoader::create("does_not_exist.elf");
    
    // Force move assignment
    BinaryLoader target_loader = std::move(*original);
    
    EXPECT_EQ(target_loader.get_entry_point(), entry);
    EXPECT_TRUE(target_loader.get_section(".text").has_value());
}

// ==========================================
// 3. Section Lookup and Math Tests
// ==========================================

TEST_F(BinaryLoaderTest, GetSectionReturnsCorrectData) {
    if (!fs::exists(valid_x86_64_elf)) GTEST_SKIP() << "Missing real ELF for testing.";

    auto loader = BinaryLoader::create(valid_x86_64_elf);
    ASSERT_TRUE(loader.has_value());

    auto section_opt = loader->get_section(".text");
    ASSERT_TRUE(section_opt.has_value());
    
    const Section& text_sec = section_opt->get();
    EXPECT_EQ(text_sec.name, ".text");
    EXPECT_GT(text_sec.size, 0); // Text section shouldn't be empty
    EXPECT_EQ(text_sec.data.size(), text_sec.size);
    
    // Requesting a non-existent section should return nullopt safely
    EXPECT_FALSE(loader->get_section(".this_does_not_exist").has_value());
}

TEST_F(BinaryLoaderTest, VmaToOffsetCalculatesBoundsCorrectly) {
    if (!fs::exists(valid_x86_64_elf)) GTEST_SKIP() << "Missing real ELF for testing.";

    auto loader = BinaryLoader::create(valid_x86_64_elf);
    ASSERT_TRUE(loader.has_value());

    auto section_opt = loader->get_section(".text");
    ASSERT_TRUE(section_opt.has_value());
    
    uint64_t base_vma = section_opt->get().vma;
    uint64_t size = section_opt->get().size;

    // Corner Case 1: Exactly at the base VMA (offset 0)
    auto offset_start = loader->vma_to_offset(base_vma);
    ASSERT_TRUE(offset_start.has_value());
    EXPECT_EQ(*offset_start, 0);

    // Corner Case 2: Deep inside the section
    auto offset_mid = loader->vma_to_offset(base_vma + (size / 2));
    ASSERT_TRUE(offset_mid.has_value());
    EXPECT_EQ(*offset_mid, size / 2);

    // Corner Case 3: Just one byte before the VMA
    EXPECT_FALSE(loader->vma_to_offset(base_vma - 1).has_value());

    // Corner Case 4: Exactly on the upper boundary (size is non-inclusive, should fail)
    EXPECT_FALSE(loader->vma_to_offset(base_vma + size).has_value());
}