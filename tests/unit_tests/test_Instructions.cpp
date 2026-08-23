/**
 * @file test_Instructions.cpp
 * @brief Unit tests for X86Instruction.
 *
 * Drives X86Instruction::populate() through the real Decoder rather than
 * hand-built Zydis structs, then checks core identity, category/branch
 * metadata, CPU flag extraction, operand extraction, is_operand_register(),
 * and the get_category_string()/get_flags_string() formatters.
 *
 * @author a.atta
 * @date 2026-08-23
 */
#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "Decoder.hpp"
#include "Instructions.hpp"
namespace {
std::optional<X86Instruction> Decode(Decoder& decoder, uint64_t vma,
                                      std::vector<uint8_t> bytes) {
  return decoder.decode(vma, bytes.data(), bytes.size());
}
}  // namespace
class InstructionsTest : public ::testing::Test {
 protected:
  Decoder decoder;
};
TEST_F(InstructionsTest, PopulateFillsCoreIdentity) {
  auto instr = Decode(decoder, 0x400000, {0x48, 0x01, 0xC8});
  ASSERT_TRUE(instr.has_value());
  EXPECT_EQ(instr->name, "add");
  EXPECT_EQ(instr->text, "add rax, rcx");
  EXPECT_EQ(instr->address, 0x400000u);
  EXPECT_EQ(instr->mnemonic, ZYDIS_MNEMONIC_ADD);
  EXPECT_EQ(instr->length, 3);
}
TEST_F(InstructionsTest, PopulateFillsMetadata) {
  auto instr = Decode(decoder, 0x400000, {0x48, 0x01, 0xC8});
  ASSERT_TRUE(instr.has_value());
  EXPECT_EQ(instr->category, ZYDIS_CATEGORY_BINARY);
  EXPECT_EQ(instr->branch_type, ZYDIS_BRANCH_TYPE_NONE);
}
TEST_F(InstructionsTest, PopulateFillsBranchMetadataForBranches) {
  auto instr = Decode(decoder, 0x400000, {0x74, 0x05});
  ASSERT_TRUE(instr.has_value());
  EXPECT_EQ(instr->category, ZYDIS_CATEGORY_COND_BR);
  EXPECT_NE(instr->branch_type, ZYDIS_BRANCH_TYPE_NONE);
}
TEST_F(InstructionsTest, PopulateExtractsWrittenFlagsForAdd) {
  auto instr = Decode(decoder, 0x400000, {0x48, 0x01, 0xC8});
  ASSERT_TRUE(instr.has_value());
  const uint32_t expected_written = ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
      ZYDIS_CPUFLAG_AF | ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF;
  EXPECT_EQ(instr->flags_written, expected_written);
  EXPECT_EQ(instr->flags_read, 0u);
}
TEST_F(InstructionsTest, PopulateExtractsReadFlagsForAdc) {
  auto instr = Decode(decoder, 0x400000, {0x48, 0x11, 0xC8});
  ASSERT_TRUE(instr.has_value());
  EXPECT_EQ(instr->flags_read, static_cast<uint32_t>(ZYDIS_CPUFLAG_CF));
}
TEST_F(InstructionsTest, PopulateFillsAllVisibleOperands) {
  auto instr = Decode(decoder, 0x400000, {0x8B, 0x44, 0x8B, 0x10});
  ASSERT_TRUE(instr.has_value());
  ASSERT_EQ(instr->operand_count, 2);
  EXPECT_EQ(instr->operands[0].type, ZYDIS_OPERAND_TYPE_REGISTER);
  EXPECT_EQ(instr->operands[1].type, ZYDIS_OPERAND_TYPE_MEMORY);
}
TEST_F(InstructionsTest, PopulateHandlesZeroOperandInstructions) {
  auto instr = Decode(decoder, 0x400000, {0x90});
  ASSERT_TRUE(instr.has_value());
  EXPECT_EQ(instr->operand_count, 0);
}
TEST_F(InstructionsTest, IsOperandRegisterTrueForRegisterOperand) {
  auto instr = Decode(decoder, 0x400000, {0x48, 0x01, 0xC8});
  ASSERT_TRUE(instr.has_value());
  EXPECT_TRUE(instr->is_operand_register(0));
  EXPECT_TRUE(instr->is_operand_register(1));
}
TEST_F(InstructionsTest, IsOperandRegisterFalseForNonRegisterOperand) {
  auto instr = Decode(decoder, 0x400000,
                       {0x48, 0xC7, 0xC3, 0x78, 0x56, 0x34, 0x12});
  ASSERT_TRUE(instr.has_value());
  EXPECT_TRUE(instr->is_operand_register(0));
  EXPECT_FALSE(instr->is_operand_register(1));
}
TEST_F(InstructionsTest, IsOperandRegisterFalseForOutOfRangeIndex) {
  auto instr = Decode(decoder, 0x400000, {0x90});
  ASSERT_TRUE(instr.has_value());
  EXPECT_FALSE(instr->is_operand_register(0));
  EXPECT_FALSE(instr->is_operand_register(255));
}
TEST_F(InstructionsTest, GetCategoryStringReturnsHumanReadableName) {
  auto instr = Decode(decoder, 0x400000, {0x48, 0x01, 0xC8});
  ASSERT_TRUE(instr.has_value());
  EXPECT_EQ(instr->get_category_string(), "BINARY");
}
TEST_F(InstructionsTest, GetFlagsStringReturnsNoneForZeroMask) {
  X86Instruction instr;
  EXPECT_EQ(instr.get_flags_string(0), "None");
}
TEST_F(InstructionsTest, GetFlagsStringSingleFlag) {
  X86Instruction instr;
  EXPECT_EQ(instr.get_flags_string(ZYDIS_CPUFLAG_ZF), "[ ZF ]");
}
TEST_F(InstructionsTest, GetFlagsStringOrdersMultipleFlagsCfToOf) {
  X86Instruction instr;
  const uint32_t mask = ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF | ZYDIS_CPUFLAG_AF |
      ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_TF |
      ZYDIS_CPUFLAG_IF | ZYDIS_CPUFLAG_DF | ZYDIS_CPUFLAG_OF;
  EXPECT_EQ(instr.get_flags_string(mask), "[ CF PF AF ZF SF TF IF DF OF ]");
}
TEST_F(InstructionsTest, GetFlagsStringIgnoresUnrelatedBits) {
  X86Instruction instr;
  const uint32_t mask = ZYDIS_CPUFLAG_CF | (1u << 31);
  EXPECT_EQ(instr.get_flags_string(mask), "[ CF ]");
}
TEST_F(InstructionsTest, DefaultConstructedInstructionIsEmpty) {
  X86Instruction instr;
  EXPECT_EQ(instr.address, 0u);
  EXPECT_EQ(instr.mnemonic, ZYDIS_MNEMONIC_INVALID);
  EXPECT_EQ(instr.length, 0);
  EXPECT_EQ(instr.operand_count, 0);
  EXPECT_TRUE(instr.name.empty());
  EXPECT_TRUE(instr.text.empty());
}
