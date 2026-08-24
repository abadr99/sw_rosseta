/**
 * @file test_Decoder.cpp
 * @brief Unit tests for Decoder.
 *
 * Exercises Decoder::decode() against real x86-64 byte sequences covering
 * register, immediate, and SIB-memory operands, zero-operand instructions,
 * VMA propagation, and the null/zero-length/invalid-opcode/truncated-buffer
 * rejection paths. Also covers every instruction listed in Intel SDM Vol. 1
 * secs. 5.1.1-5.1.5 (data transfer, binary arithmetic, decimal arithmetic,
 * logical, and shift/rotate instructions), verifying mnemonic, disassembly
 * text, length, category, operand count, and CPU flags for each. All byte
 * encodings were generated and cross-checked with NASM in 64-bit mode.
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
TEST_F(DecoderTest, RejectsLoneRexPrefixWithNoFollowingOpcode) {
  const std::vector<uint8_t> lone_rex = {0x40};
  auto result = decoder.decode(0x400000, lone_rex.data(), lone_rex.size());
  EXPECT_FALSE(result.has_value());
}
TEST_F(DecoderTest, ResolvesRipRelativeOperandAgainstProvidedVma) {
  const std::vector<uint8_t> rip_relative_mov = {
      0x8B, 0x05, 0x03, 0x00, 0x00, 0x00};
  auto low = decoder.decode(
      0x400000, rip_relative_mov.data(), rip_relative_mov.size());
  auto high = decoder.decode(
      0x500000, rip_relative_mov.data(), rip_relative_mov.size());
  ASSERT_TRUE(low.has_value());
  ASSERT_TRUE(high.has_value());
  EXPECT_EQ(low->text, "mov eax, [0x0000000000400009]");
  EXPECT_EQ(high->text, "mov eax, [0x0000000000500009]");
}

namespace {
struct InstructionCase {
  std::string description;
  std::vector<uint8_t> bytes;
  ZydisMnemonic mnemonic;
  std::string text;
  uint8_t length;
  ZydisInstructionCategory category;
  uint8_t operand_count;
  uint32_t flags_read;
  uint32_t flags_written;
};

std::string InstructionCaseName(
    const ::testing::TestParamInfo<InstructionCase>& info) {
  return info.param.description;
}
}  // namespace

class InstructionDecodeTest
    : public ::testing::TestWithParam<InstructionCase> {
 protected:
  Decoder decoder;
};

TEST_P(InstructionDecodeTest, DecodesAsExpected) {
  const auto& tc = GetParam();
  auto result = decoder.decode(0x400000, tc.bytes.data(), tc.bytes.size());
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->mnemonic, tc.mnemonic);
  EXPECT_EQ(result->text, tc.text);
  EXPECT_EQ(result->length, tc.length);
  EXPECT_EQ(result->category, tc.category);
  EXPECT_EQ(result->operand_count, tc.operand_count);
  EXPECT_EQ(result->flags_read, tc.flags_read);
  EXPECT_EQ(result->flags_written, tc.flags_written);
}
INSTANTIATE_TEST_SUITE_P(
    DataTransfer, InstructionDecodeTest,
    ::testing::Values(
        InstructionCase{"mov_rax_rcx", {0x48, 0x89, 0xC8},
                         ZYDIS_MNEMONIC_MOV, "mov rax, rcx", 3,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"mov_rbx_0x12345678", {0xBB, 0x78, 0x56, 0x34, 0x12},
                         ZYDIS_MNEMONIC_MOV, "mov ebx, 0x12345678", 5,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"mov_eax_rbx_rcx_4_0x10", {0x8B, 0x44, 0x8B, 0x10},
                         ZYDIS_MNEMONIC_MOV, "mov eax, [rbx+rcx*4+0x10]", 4,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"cmovz_cmove_eax_ebx", {0x0F, 0x44, 0xC3},
                         ZYDIS_MNEMONIC_CMOVZ, "cmovz eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_ZF,
                         0},
        InstructionCase{"cmovnz_cmovne_eax_ebx", {0x0F, 0x45, 0xC3},
                         ZYDIS_MNEMONIC_CMOVNZ, "cmovnz eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_ZF,
                         0},
        InstructionCase{"cmovnbe_cmova_eax_ebx", {0x0F, 0x47, 0xC3},
                         ZYDIS_MNEMONIC_CMOVNBE, "cmovnbe eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_ZF,
                         0},
        InstructionCase{"cmovnb_cmovae_eax_ebx", {0x0F, 0x43, 0xC3},
                         ZYDIS_MNEMONIC_CMOVNB, "cmovnb eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_CF,
                         0},
        InstructionCase{"cmovb_eax_ebx", {0x0F, 0x42, 0xC3},
                         ZYDIS_MNEMONIC_CMOVB, "cmovb eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_CF,
                         0},
        InstructionCase{"cmovbe_eax_ebx", {0x0F, 0x46, 0xC3},
                         ZYDIS_MNEMONIC_CMOVBE, "cmovbe eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_ZF,
                         0},
        InstructionCase{"cmovnle_cmovg_eax_ebx", {0x0F, 0x4F, 0xC3},
                         ZYDIS_MNEMONIC_CMOVNLE, "cmovnle eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF |
                         ZYDIS_CPUFLAG_OF,
                         0},
        InstructionCase{"cmovnl_cmovge_eax_ebx", {0x0F, 0x4D, 0xC3},
                         ZYDIS_MNEMONIC_CMOVNL, "cmovnl eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF,
                         0},
        InstructionCase{"cmovl_eax_ebx", {0x0F, 0x4C, 0xC3},
                         ZYDIS_MNEMONIC_CMOVL, "cmovl eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF,
                         0},
        InstructionCase{"cmovle_eax_ebx", {0x0F, 0x4E, 0xC3},
                         ZYDIS_MNEMONIC_CMOVLE, "cmovle eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF |
                         ZYDIS_CPUFLAG_OF,
                         0},
        InstructionCase{"cmovo_eax_ebx", {0x0F, 0x40, 0xC3},
                         ZYDIS_MNEMONIC_CMOVO, "cmovo eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_OF,
                         0},
        InstructionCase{"cmovno_eax_ebx", {0x0F, 0x41, 0xC3},
                         ZYDIS_MNEMONIC_CMOVNO, "cmovno eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_OF,
                         0},
        InstructionCase{"cmovs_eax_ebx", {0x0F, 0x48, 0xC3},
                         ZYDIS_MNEMONIC_CMOVS, "cmovs eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_SF,
                         0},
        InstructionCase{"cmovns_eax_ebx", {0x0F, 0x49, 0xC3},
                         ZYDIS_MNEMONIC_CMOVNS, "cmovns eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_SF,
                         0},
        InstructionCase{"cmovp_eax_ebx", {0x0F, 0x4A, 0xC3},
                         ZYDIS_MNEMONIC_CMOVP, "cmovp eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_PF,
                         0},
        InstructionCase{"cmovnp_eax_ebx", {0x0F, 0x4B, 0xC3},
                         ZYDIS_MNEMONIC_CMOVNP, "cmovnp eax, ebx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_PF,
                         0},
        InstructionCase{"xchg_rax_rcx", {0x48, 0x91},
                         ZYDIS_MNEMONIC_XCHG, "xchg rcx, rax", 2,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"bswap_eax", {0x0F, 0xC8},
                         ZYDIS_MNEMONIC_BSWAP, "bswap eax", 2,
                         ZYDIS_CATEGORY_DATAXFER, 1,
                         0,
                         0},
        InstructionCase{"xadd_eax_ebx", {0x0F, 0xC1, 0xD8},
                         ZYDIS_MNEMONIC_XADD, "xadd eax, ebx", 3,
                         ZYDIS_CATEGORY_SEMAPHORE, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_AF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF},
        InstructionCase{"cmpxchg_eax_ebx", {0x0F, 0xB1, 0xD8},
                         ZYDIS_MNEMONIC_CMPXCHG, "cmpxchg eax, ebx", 3,
                         ZYDIS_CATEGORY_SEMAPHORE, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_AF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF},
        InstructionCase{"lock_cmpxchg8b_rax", {0xF0, 0x0F, 0xC7, 0x08},
                         ZYDIS_MNEMONIC_CMPXCHG8B, "lock cmpxchg8b [rax]", 4,
                         ZYDIS_CATEGORY_SEMAPHORE, 1,
                         0,
                         ZYDIS_CPUFLAG_ZF},
        InstructionCase{"push_rax", {0x50},
                         ZYDIS_MNEMONIC_PUSH, "push rax", 1,
                         ZYDIS_CATEGORY_PUSH, 1,
                         0,
                         0},
        InstructionCase{"pop_rax", {0x58},
                         ZYDIS_MNEMONIC_POP, "pop rax", 1,
                         ZYDIS_CATEGORY_POP, 1,
                         0,
                         0},
        InstructionCase{"cwd", {0x66, 0x99},
                         ZYDIS_MNEMONIC_CWD, "cwd", 2,
                         ZYDIS_CATEGORY_CONVERT, 0,
                         0,
                         0},
        InstructionCase{"cdq", {0x99},
                         ZYDIS_MNEMONIC_CDQ, "cdq", 1,
                         ZYDIS_CATEGORY_CONVERT, 0,
                         0,
                         0},
        InstructionCase{"cbw", {0x66, 0x98},
                         ZYDIS_MNEMONIC_CBW, "cbw", 2,
                         ZYDIS_CATEGORY_CONVERT, 0,
                         0,
                         0},
        InstructionCase{"cwde", {0x98},
                         ZYDIS_MNEMONIC_CWDE, "cwde", 1,
                         ZYDIS_CATEGORY_CONVERT, 0,
                         0,
                         0},
        InstructionCase{"movsx_rax_bl", {0x48, 0x0F, 0xBE, 0xC3},
                         ZYDIS_MNEMONIC_MOVSX, "movsx rax, bl", 4,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"movzx_rax_bl", {0x48, 0x0F, 0xB6, 0xC3},
                         ZYDIS_MNEMONIC_MOVZX, "movzx rax, bl", 4,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0}),
    InstructionCaseName);
INSTANTIATE_TEST_SUITE_P(
    BinaryArithmetic, InstructionDecodeTest,
    ::testing::Values(
        InstructionCase{"adcx_eax_ebx", {0x66, 0x0F, 0x38, 0xF6, 0xC3},
                         ZYDIS_MNEMONIC_ADCX, "adcx eax, ebx", 5,
                         ZYDIS_CATEGORY_ADOX_ADCX, 2,
                         ZYDIS_CPUFLAG_CF,
                         ZYDIS_CPUFLAG_CF},
        InstructionCase{"adox_eax_ebx", {0xF3, 0x0F, 0x38, 0xF6, 0xC3},
                         ZYDIS_MNEMONIC_ADOX, "adox eax, ebx", 5,
                         ZYDIS_CATEGORY_ADOX_ADCX, 2,
                         ZYDIS_CPUFLAG_OF,
                         ZYDIS_CPUFLAG_OF},
        InstructionCase{"add_eax_ebx", {0x01, 0xD8},
                         ZYDIS_MNEMONIC_ADD, "add eax, ebx", 2,
                         ZYDIS_CATEGORY_BINARY, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_AF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF},
        InstructionCase{"adc_eax_ebx", {0x11, 0xD8},
                         ZYDIS_MNEMONIC_ADC, "adc eax, ebx", 2,
                         ZYDIS_CATEGORY_BINARY, 2,
                         ZYDIS_CPUFLAG_CF,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_AF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF},
        InstructionCase{"sub_eax_ebx", {0x29, 0xD8},
                         ZYDIS_MNEMONIC_SUB, "sub eax, ebx", 2,
                         ZYDIS_CATEGORY_BINARY, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_AF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF},
        InstructionCase{"sbb_eax_ebx", {0x19, 0xD8},
                         ZYDIS_MNEMONIC_SBB, "sbb eax, ebx", 2,
                         ZYDIS_CATEGORY_BINARY, 2,
                         ZYDIS_CPUFLAG_CF,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF |
                         ZYDIS_CPUFLAG_OF},
        InstructionCase{"imul_eax_ebx", {0x0F, 0xAF, 0xC3},
                         ZYDIS_MNEMONIC_IMUL, "imul eax, ebx", 3,
                         ZYDIS_CATEGORY_BINARY, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_OF},
        InstructionCase{"mul_ebx", {0xF7, 0xE3},
                         ZYDIS_MNEMONIC_MUL, "mul ebx", 2,
                         ZYDIS_CATEGORY_BINARY, 1,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_OF},
        InstructionCase{"idiv_ebx", {0xF7, 0xFB},
                         ZYDIS_MNEMONIC_IDIV, "idiv ebx", 2,
                         ZYDIS_CATEGORY_BINARY, 1,
                         0,
                         0},
        InstructionCase{"div_ebx", {0xF7, 0xF3},
                         ZYDIS_MNEMONIC_DIV, "div ebx", 2,
                         ZYDIS_CATEGORY_BINARY, 1,
                         0,
                         0},
        InstructionCase{"inc_eax", {0xFF, 0xC0},
                         ZYDIS_MNEMONIC_INC, "inc eax", 2,
                         ZYDIS_CATEGORY_BINARY, 1,
                         0,
                         ZYDIS_CPUFLAG_PF | ZYDIS_CPUFLAG_AF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF |
                         ZYDIS_CPUFLAG_OF},
        InstructionCase{"dec_eax", {0xFF, 0xC8},
                         ZYDIS_MNEMONIC_DEC, "dec eax", 2,
                         ZYDIS_CATEGORY_BINARY, 1,
                         0,
                         ZYDIS_CPUFLAG_PF | ZYDIS_CPUFLAG_AF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF |
                         ZYDIS_CPUFLAG_OF},
        InstructionCase{"neg_eax", {0xF7, 0xD8},
                         ZYDIS_MNEMONIC_NEG, "neg eax", 2,
                         ZYDIS_CATEGORY_BINARY, 1,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_AF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF},
        InstructionCase{"cmp_eax_ebx", {0x39, 0xD8},
                         ZYDIS_MNEMONIC_CMP, "cmp eax, ebx", 2,
                         ZYDIS_CATEGORY_BINARY, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_AF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF}),
    InstructionCaseName);
INSTANTIATE_TEST_SUITE_P(
    Logical, InstructionDecodeTest,
    ::testing::Values(
        InstructionCase{"and_eax_ebx", {0x21, 0xD8},
                         ZYDIS_MNEMONIC_AND, "and eax, ebx", 2,
                         ZYDIS_CATEGORY_LOGICAL, 2,
                         0,
                         ZYDIS_CPUFLAG_PF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF},
        InstructionCase{"or_eax_ebx", {0x09, 0xD8},
                         ZYDIS_MNEMONIC_OR, "or eax, ebx", 2,
                         ZYDIS_CATEGORY_LOGICAL, 2,
                         0,
                         ZYDIS_CPUFLAG_PF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF},
        InstructionCase{"xor_eax_ebx", {0x31, 0xD8},
                         ZYDIS_MNEMONIC_XOR, "xor eax, ebx", 2,
                         ZYDIS_CATEGORY_LOGICAL, 2,
                         0,
                         ZYDIS_CPUFLAG_PF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF},
        InstructionCase{"not_eax", {0xF7, 0xD0},
                         ZYDIS_MNEMONIC_NOT, "not eax", 2,
                         ZYDIS_CATEGORY_LOGICAL, 1,
                         0,
                         0}),
    InstructionCaseName);
INSTANTIATE_TEST_SUITE_P(
    ShiftRotate, InstructionDecodeTest,
    ::testing::Values(
        InstructionCase{"sar_eax_1", {0xD1, 0xF8},
                         ZYDIS_MNEMONIC_SAR, "sar eax, 0x01", 2,
                         ZYDIS_CATEGORY_SHIFT, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF},
        InstructionCase{"shr_eax_1", {0xD1, 0xE8},
                         ZYDIS_MNEMONIC_SHR, "shr eax, 0x01", 2,
                         ZYDIS_CATEGORY_SHIFT, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF},
        InstructionCase{"shl_sal_eax_1", {0xD1, 0xE0},
                         ZYDIS_MNEMONIC_SHL, "shl eax, 0x01", 2,
                         ZYDIS_CATEGORY_SHIFT, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF},
        InstructionCase{"shrd_eax_ebx_4", {0x0F, 0xAC, 0xD8, 0x04},
                         ZYDIS_MNEMONIC_SHRD, "shrd eax, ebx, 0x04", 4,
                         ZYDIS_CATEGORY_SHIFT, 3,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF},
        InstructionCase{"shld_eax_ebx_4", {0x0F, 0xA4, 0xD8, 0x04},
                         ZYDIS_MNEMONIC_SHLD, "shld eax, ebx, 0x04", 4,
                         ZYDIS_CATEGORY_SHIFT, 3,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF},
        InstructionCase{"ror_eax_1", {0xD1, 0xC8},
                         ZYDIS_MNEMONIC_ROR, "ror eax, 0x01", 2,
                         ZYDIS_CATEGORY_ROTATE, 2,
                         0,
                         ZYDIS_CPUFLAG_CF},
        InstructionCase{"rol_eax_1", {0xD1, 0xC0},
                         ZYDIS_MNEMONIC_ROL, "rol eax, 0x01", 2,
                         ZYDIS_CATEGORY_ROTATE, 2,
                         0,
                         ZYDIS_CPUFLAG_CF},
        InstructionCase{"rcr_eax_1", {0xD1, 0xD8},
                         ZYDIS_MNEMONIC_RCR, "rcr eax, 0x01", 2,
                         ZYDIS_CATEGORY_ROTATE, 2,
                         ZYDIS_CPUFLAG_CF,
                         ZYDIS_CPUFLAG_CF},
        InstructionCase{"rcl_eax_1", {0xD1, 0xD0},
                         ZYDIS_MNEMONIC_RCL, "rcl eax, 0x01", 2,
                         ZYDIS_CATEGORY_ROTATE, 2,
                         ZYDIS_CPUFLAG_CF,
                         ZYDIS_CPUFLAG_CF}),
    InstructionCaseName);
INSTANTIATE_TEST_SUITE_P(
    WidthAndRexVariants, InstructionDecodeTest,
    ::testing::Values(
        InstructionCase{"mov_al_cl", {0x88, 0xC8},
                         ZYDIS_MNEMONIC_MOV, "mov al, cl", 2,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"mov_ax_cx", {0x66, 0x89, 0xC8},
                         ZYDIS_MNEMONIC_MOV, "mov ax, cx", 3,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"mov_eax_ecx", {0x89, 0xC8},
                         ZYDIS_MNEMONIC_MOV, "mov eax, ecx", 2,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"mov_rax_rcx", {0x48, 0x89, 0xC8},
                         ZYDIS_MNEMONIC_MOV, "mov rax, rcx", 3,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"add_al_cl", {0x00, 0xC8},
                         ZYDIS_MNEMONIC_ADD, "add al, cl", 2,
                         ZYDIS_CATEGORY_BINARY, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_AF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF},
        InstructionCase{"add_ax_cx", {0x66, 0x01, 0xC8},
                         ZYDIS_MNEMONIC_ADD, "add ax, cx", 3,
                         ZYDIS_CATEGORY_BINARY, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_AF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF},
        InstructionCase{"add_eax_ecx", {0x01, 0xC8},
                         ZYDIS_MNEMONIC_ADD, "add eax, ecx", 2,
                         ZYDIS_CATEGORY_BINARY, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_AF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF},
        InstructionCase{"add_rax_rcx", {0x48, 0x01, 0xC8},
                         ZYDIS_MNEMONIC_ADD, "add rax, rcx", 3,
                         ZYDIS_CATEGORY_BINARY, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_AF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF},
        InstructionCase{"shl_al_1", {0xD0, 0xE0},
                         ZYDIS_MNEMONIC_SHL, "shl al, 0x01", 2,
                         ZYDIS_CATEGORY_SHIFT, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF},
        InstructionCase{"shl_ax_1", {0x66, 0xD1, 0xE0},
                         ZYDIS_MNEMONIC_SHL, "shl ax, 0x01", 3,
                         ZYDIS_CATEGORY_SHIFT, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF},
        InstructionCase{"shl_eax_1", {0xD1, 0xE0},
                         ZYDIS_MNEMONIC_SHL, "shl eax, 0x01", 2,
                         ZYDIS_CATEGORY_SHIFT, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF},
        InstructionCase{"shl_rax_1", {0x48, 0xD1, 0xE0},
                         ZYDIS_MNEMONIC_SHL, "shl rax, 0x01", 3,
                         ZYDIS_CATEGORY_SHIFT, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF},
        InstructionCase{"inc_al", {0xFE, 0xC0},
                         ZYDIS_MNEMONIC_INC, "inc al", 2,
                         ZYDIS_CATEGORY_BINARY, 1,
                         0,
                         ZYDIS_CPUFLAG_PF | ZYDIS_CPUFLAG_AF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF |
                         ZYDIS_CPUFLAG_OF},
        InstructionCase{"inc_ax", {0x66, 0xFF, 0xC0},
                         ZYDIS_MNEMONIC_INC, "inc ax", 3,
                         ZYDIS_CATEGORY_BINARY, 1,
                         0,
                         ZYDIS_CPUFLAG_PF | ZYDIS_CPUFLAG_AF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF |
                         ZYDIS_CPUFLAG_OF},
        InstructionCase{"inc_eax", {0xFF, 0xC0},
                         ZYDIS_MNEMONIC_INC, "inc eax", 2,
                         ZYDIS_CATEGORY_BINARY, 1,
                         0,
                         ZYDIS_CPUFLAG_PF | ZYDIS_CPUFLAG_AF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF |
                         ZYDIS_CPUFLAG_OF},
        InstructionCase{"inc_rax", {0x48, 0xFF, 0xC0},
                         ZYDIS_MNEMONIC_INC, "inc rax", 3,
                         ZYDIS_CATEGORY_BINARY, 1,
                         0,
                         ZYDIS_CPUFLAG_PF | ZYDIS_CPUFLAG_AF |
                         ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_SF |
                         ZYDIS_CPUFLAG_OF},
        InstructionCase{"cmovz_ax_cx", {0x66, 0x0F, 0x44, 0xC1},
                         ZYDIS_MNEMONIC_CMOVZ, "cmovz ax, cx", 4,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_ZF,
                         0},
        InstructionCase{"cmovz_eax_ecx", {0x0F, 0x44, 0xC1},
                         ZYDIS_MNEMONIC_CMOVZ, "cmovz eax, ecx", 3,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_ZF,
                         0},
        InstructionCase{"cmovz_rax_rcx", {0x48, 0x0F, 0x44, 0xC1},
                         ZYDIS_MNEMONIC_CMOVZ, "cmovz rax, rcx", 4,
                         ZYDIS_CATEGORY_CMOV, 2,
                         ZYDIS_CPUFLAG_ZF,
                         0},
        InstructionCase{"mov_r8_r9", {0x4D, 0x89, 0xC8},
                         ZYDIS_MNEMONIC_MOV, "mov r8, r9", 3,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"mov_r8d_r9d", {0x45, 0x89, 0xC8},
                         ZYDIS_MNEMONIC_MOV, "mov r8d, r9d", 3,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"mov_r8w_r9w", {0x66, 0x45, 0x89, 0xC8},
                         ZYDIS_MNEMONIC_MOV, "mov r8w, r9w", 4,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"mov_r8b_r9b", {0x45, 0x88, 0xC8},
                         ZYDIS_MNEMONIC_MOV, "mov r8b, r9b", 3,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"add_ah_bh", {0x00, 0xFC},
                         ZYDIS_MNEMONIC_ADD, "add ah, bh", 2,
                         ZYDIS_CATEGORY_BINARY, 2,
                         0,
                         ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_PF |
                         ZYDIS_CPUFLAG_AF | ZYDIS_CPUFLAG_ZF |
                         ZYDIS_CPUFLAG_SF | ZYDIS_CPUFLAG_OF},
        InstructionCase{"mov_eax_r8_r9_4", {0x43, 0x8B, 0x04, 0x88},
                         ZYDIS_MNEMONIC_MOV, "mov eax, [r8+r9*4]", 4,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"mov_rip_relative",
                         {0x8B, 0x05, 0x03, 0x00, 0x00, 0x00},
                         ZYDIS_MNEMONIC_MOV,
                         "mov eax, [0x0000000000400009]", 6,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0},
        InstructionCase{"mov_word_rax_cx", {0x66, 0x89, 0x08},
                         ZYDIS_MNEMONIC_MOV, "mov [rax], cx", 3,
                         ZYDIS_CATEGORY_DATAXFER, 2,
                         0,
                         0}),
    InstructionCaseName);

namespace {
struct RejectedInstructionCase {
  std::string description;
  std::vector<uint8_t> bytes;
};

std::string RejectedInstructionCaseName(
    const ::testing::TestParamInfo<RejectedInstructionCase>& info) {
  return info.param.description;
}
}  // namespace

class RejectedInstructionDecodeTest
    : public ::testing::TestWithParam<RejectedInstructionCase> {
 protected:
  Decoder decoder;
};

TEST_P(RejectedInstructionDecodeTest, IsNotValidInLongMode) {
  const auto& tc = GetParam();
  auto result = decoder.decode(0x400000, tc.bytes.data(), tc.bytes.size());
  EXPECT_FALSE(result.has_value());
}
INSTANTIATE_TEST_SUITE_P(
    RemovedInLongMode, RejectedInstructionDecodeTest,
    ::testing::Values(
        RejectedInstructionCase{"pusha", {0x60}},
        RejectedInstructionCase{"popa", {0x61}},
        RejectedInstructionCase{"daa", {0x27}},
        RejectedInstructionCase{"das", {0x2F}},
        RejectedInstructionCase{"aaa", {0x37}},
        RejectedInstructionCase{"aas", {0x3F}},
        RejectedInstructionCase{"aam", {0xD4, 0x0A}},
        RejectedInstructionCase{"aad", {0xD5, 0x0A}}),
    RejectedInstructionCaseName);
