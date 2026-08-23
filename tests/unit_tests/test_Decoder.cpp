/**
 * @file test_Decoder.cpp
 * @brief Unit tests for Decoder.
 *
 * Exercises Decoder::decode() against real x86-64 byte sequences covering
 * register, immediate, and SIB-memory operands, zero-operand instructions,
 * VMA propagation, and the null/zero-length/invalid-opcode/truncated-buffer
 * rejection paths.
 *
 * @author a.atta
 * @date 2026-08-23
 */
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "Decoder.hpp"
namespace {
const std::vector<uint8_t> kAddRaxRcx = {0x48, 0x01, 0xC8};
const std::vector<uint8_t> kMovImm = {0x48, 0xC7, 0xC3, 0x78, 0x56, 0x34, 0x12};
const std::vector<uint8_t> kMovEaxMem = {0x8B, 0x44, 0x8B, 0x10};
const std::vector<uint8_t> kNop = {0x90};
const std::vector<uint8_t> kInvalidOpcode = {0x0F, 0xFF};
}  // namespace
class DecoderTest : public ::testing::Test {
 protected:
  Decoder decoder;
};
TEST_F(DecoderTest, DecodesSimpleRegisterInstruction) {
  auto result = decoder.decode(0x400000, kAddRaxRcx.data(), kAddRaxRcx.size());
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->address, 0x400000u);
  EXPECT_EQ(result->length, 3);
  EXPECT_EQ(result->mnemonic, ZYDIS_MNEMONIC_ADD);
  EXPECT_EQ(result->name, "add");
  EXPECT_EQ(result->text, "add rax, rcx");
}
TEST_F(DecoderTest, DecodesImmediateOperandInstruction) {
  auto result = decoder.decode(0x401000, kMovImm.data(), kMovImm.size());
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->length, 7);
  EXPECT_EQ(result->mnemonic, ZYDIS_MNEMONIC_MOV);
  EXPECT_EQ(result->operand_count, 2);
  EXPECT_TRUE(result->is_operand_register(0));
  EXPECT_FALSE(result->is_operand_register(1));
}
TEST_F(DecoderTest, DecodesMemoryOperandWithSib) {
  auto result = decoder.decode(0x402000, kMovEaxMem.data(), kMovEaxMem.size());
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->length, 4);
  EXPECT_EQ(result->mnemonic, ZYDIS_MNEMONIC_MOV);
  EXPECT_EQ(result->operand_count, 2);
  EXPECT_TRUE(result->is_operand_register(0));
  EXPECT_FALSE(result->is_operand_register(1));
}
TEST_F(DecoderTest, DecodesInstructionWithNoOperands) {
  auto result = decoder.decode(0x403000, kNop.data(), kNop.size());
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->mnemonic, ZYDIS_MNEMONIC_NOP);
  EXPECT_EQ(result->operand_count, 0);
}
TEST_F(DecoderTest, StoresProvidedVmaAsAddress) {
  auto low = decoder.decode(0x1000, kAddRaxRcx.data(), kAddRaxRcx.size());
  auto high = decoder.decode(
      0x7FFFFFFF0000ULL, kAddRaxRcx.data(), kAddRaxRcx.size());
  ASSERT_TRUE(low.has_value());
  ASSERT_TRUE(high.has_value());
  EXPECT_EQ(low->address, 0x1000u);
  EXPECT_EQ(high->address, 0x7FFFFFFF0000ULL);
}
TEST_F(DecoderTest, RejectsNullBuffer) {
  auto result = decoder.decode(0x400000, nullptr, 10);
  EXPECT_FALSE(result.has_value());
}
TEST_F(DecoderTest, RejectsZeroLength) {
  auto result = decoder.decode(0x400000, kAddRaxRcx.data(), 0);
  EXPECT_FALSE(result.has_value());
}
TEST_F(DecoderTest, RejectsNullBufferWithZeroLength) {
  auto result = decoder.decode(0x400000, nullptr, 0);
  EXPECT_FALSE(result.has_value());
}
TEST_F(DecoderTest, RejectsInvalidOpcode) {
  auto result = decoder.decode(
      0x400000, kInvalidOpcode.data(), kInvalidOpcode.size());
  EXPECT_FALSE(result.has_value());
}
TEST_F(DecoderTest, RejectsTruncatedInstruction) {
  std::vector<uint8_t> truncated(kMovImm.begin(), kMovImm.begin() + 4);
  auto result = decoder.decode(0x400000, truncated.data(), truncated.size());
  EXPECT_FALSE(result.has_value());
}
TEST_F(DecoderTest, DecodesConsecutiveInstructionsFromSameBuffer) {
  std::vector<uint8_t> combined = kAddRaxRcx;
  combined.insert(combined.end(), kMovImm.begin(), kMovImm.end());
  uint64_t vma = 0x400000;
  size_t pc = 0;
  auto first = decoder.decode(vma, &combined[pc], combined.size() - pc);
  ASSERT_TRUE(first.has_value());
  EXPECT_EQ(first->mnemonic, ZYDIS_MNEMONIC_ADD);
  pc += first->length;
  vma += first->length;
  auto second = decoder.decode(vma, &combined[pc], combined.size() - pc);
  ASSERT_TRUE(second.has_value());
  EXPECT_EQ(second->mnemonic, ZYDIS_MNEMONIC_MOV);
  EXPECT_EQ(second->address, 0x400000u + 3u);
}
TEST_F(DecoderTest, SameDecoderInstanceIsReusableAcrossCalls) {
  auto a = decoder.decode(0x1000, kAddRaxRcx.data(), kAddRaxRcx.size());
  auto b = decoder.decode(0x2000, kInvalidOpcode.data(), kInvalidOpcode.size());
  auto c = decoder.decode(0x3000, kNop.data(), kNop.size());
  ASSERT_TRUE(a.has_value());
  EXPECT_FALSE(b.has_value());
  ASSERT_TRUE(c.has_value());
  EXPECT_EQ(c->mnemonic, ZYDIS_MNEMONIC_NOP);
}
