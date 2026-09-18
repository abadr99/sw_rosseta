#include <gtest/gtest.h>

#include <cstdint>
#include <memory>

#include "frontend/DecoderInterface.hpp"
#include "frontend/Instruction.hpp"
#include "frontend/ZydisDecoder.hpp"

using rosetta::frontend::decoder::ZydisInstructionDecoder;
using rosetta::frontend::instruction::Instruction;
using rosetta::frontend::instruction::InstructionCategory;
using rosetta::frontend::instruction::InstructionOperand;
using rosetta::frontend::instruction::OperandType;

TEST(ZydisInstructionDecoderTest, DecodeArithmetic_Add) {
  ZydisInstructionDecoder decoder;
  // 64-bit: add rax, rbx -> AT&T: add %rbx, %rax
  const uint8_t binary[] = {0x48, 0x01, 0xD8};

  const auto decoded = decoder.Decode(0x00400000, binary, sizeof(binary));
  ASSERT_NE(decoded, nullptr);
  Instruction& ir = *decoded;

  EXPECT_EQ(ir.Mnemonic(), "add");
  EXPECT_EQ(ir.Category(), InstructionCategory::kArithmetic);
  ASSERT_EQ(ir.Operands().size(), 2u);

  // Zydis keeps decode order: (dest, src)
  const InstructionOperand& dst = ir.Operands()[0];
  EXPECT_EQ(dst.Type, OperandType::kRegister);
  EXPECT_EQ(dst.Reg, 0u);  // RAX = 0

  const InstructionOperand& src = ir.Operands()[1];
  EXPECT_EQ(src.Type, OperandType::kRegister);
  EXPECT_EQ(src.Reg, 3u);  // RBX = 3
}

TEST(ZydisInstructionDecoderTest, DecodeDataTransfer_MovImmediate) {
  ZydisInstructionDecoder decoder;
  // 64-bit: mov rcx, 4 -> AT&T: movq $0x04, %rcx
  const uint8_t binary[] = {0x48, 0xC7, 0xC1, 0x04, 0x00, 0x00, 0x00};

  const auto decoded = decoder.Decode(0x00400000, binary, sizeof(binary));
  ASSERT_NE(decoded, nullptr);
  Instruction& ir = *decoded;

  EXPECT_EQ(ir.Mnemonic(), "mov");
  EXPECT_EQ(ir.Category(), InstructionCategory::kDataTransfer);
  ASSERT_EQ(ir.Operands().size(), 2u);

  const InstructionOperand& dst = ir.Operands()[0];
  EXPECT_EQ(dst.Type, OperandType::kRegister);
  EXPECT_EQ(dst.Reg, 1u);  // RCX = 1

  const InstructionOperand& src = ir.Operands()[1];
  EXPECT_EQ(src.Type, OperandType::kImmediate);
  EXPECT_EQ(src.Imm, 4u);
}

TEST(ZydisInstructionDecoderTest, DecodeDataTransfer_MovMemory) {
  ZydisInstructionDecoder decoder;
  // 64-bit: mov rax, [rbx] -> AT&T: movq (%rbx), %rax
  const uint8_t binary[] = {0x48, 0x8B, 0x03};

  const auto decoded = decoder.Decode(0x00400000, binary, sizeof(binary));
  ASSERT_NE(decoded, nullptr);
  Instruction& ir = *decoded;

  EXPECT_EQ(ir.Mnemonic(), "mov");
  EXPECT_EQ(ir.Category(), InstructionCategory::kDataTransfer);
  ASSERT_EQ(ir.Operands().size(), 2u);

  const InstructionOperand& dst = ir.Operands()[0];
  EXPECT_EQ(dst.Type, OperandType::kRegister);
  EXPECT_EQ(dst.Reg, 0u);  // RAX = 0

  const InstructionOperand& src = ir.Operands()[1];
  EXPECT_EQ(src.Type, OperandType::kMemory);
  EXPECT_EQ(src.Mem.Base, 3u);  // RBX = 3
  EXPECT_EQ(src.Mem.Index, 16u);  // NONE defaults to 16
}

TEST(ZydisInstructionDecoderTest, DecodeCondControlFlow_Jle) {
  ZydisInstructionDecoder decoder;
  // 8-bit relative jump: jle +0x05 -> AT&T: jle 0x00400007
  const uint8_t binary[] = {0x7E, 0x05};

  const auto decoded = decoder.Decode(0x00400000, binary, sizeof(binary));
  ASSERT_NE(decoded, nullptr);
  Instruction& ir = *decoded;

  EXPECT_EQ(ir.Mnemonic(), "jle");
  EXPECT_EQ(ir.Category(), InstructionCategory::kCondControlFlow);
  ASSERT_EQ(ir.Operands().size(), 1u);
  EXPECT_EQ(ir.Operands()[0].Type, OperandType::kImmediate);
}

TEST(ZydisInstructionDecoderTest, DecodeUnCondControlFlow_Ret) {
  ZydisInstructionDecoder decoder;
  // ret
  const uint8_t binary[] = {0xC3};

  const auto decoded = decoder.Decode(0x00400000, binary, sizeof(binary));
  ASSERT_NE(decoded, nullptr);
  Instruction& ir = *decoded;

  EXPECT_EQ(ir.Mnemonic(), "ret");
  EXPECT_EQ(ir.Category(), InstructionCategory::kReturn);
  // `ret` implicitly uses the stack, but has 0 explicitly visible operands in Zydis formatting
}

TEST(ZydisInstructionDecoderTest, DecodeUnknownCategory_Nop) {
  ZydisInstructionDecoder decoder;
  // nop (Category: ZYDIS_CATEGORY_NOP)
  const uint8_t binary[] = {0x90};

  const auto decoded = decoder.Decode(0x00400000, binary, sizeof(binary));
  ASSERT_NE(decoded, nullptr);
  Instruction& ir = *decoded;

  EXPECT_EQ(ir.Mnemonic(), "nop");
  // NOP is excluded from your mapped switch cases, so it hits default
  EXPECT_EQ(ir.Category(), InstructionCategory::kUnknown); 
}

TEST(ZydisInstructionDecoderTest, InvalidOpcodeFailsGracefully) {
  ZydisInstructionDecoder decoder;
  // 0xFF 0xFF is an invalid x86 opcode
  uint8_t binary[] = {0xFF, 0xFF};

  EXPECT_EQ(decoder.Decode(0x00400000, binary, sizeof(binary)), nullptr);
}

TEST(InstructionOperandTest, DefaultConstructorIsZeroed) {
  InstructionOperand op;
  EXPECT_EQ(op.Type, OperandType::kUnkown);
  EXPECT_EQ(op.Reg, 0u);
  EXPECT_EQ(op.Imm, 0u);
  EXPECT_EQ(op.Mem.Base, 0u);
  EXPECT_EQ(op.Mem.Index, 0u);
  EXPECT_EQ(op.Mem.Scale, 0u);
  EXPECT_EQ(op.Mem.Offset, 0u);
}

TEST(ZydisInstructionDecoderTest, DecodeAll_MultipleInstructions) {
  ZydisInstructionDecoder decoder;
  // add rax, rbx ; mov rcx, 4 ; ret
  const uint8_t binary[] = {
      0x48, 0x01, 0xD8,                          // add rax, rbx
      0x48, 0xC7, 0xC1, 0x04, 0x00, 0x00, 0x00,  // mov rcx, 4
      0xC3                                       // ret
  };

  const auto instructions = decoder.DecodeAll(0x00400000, binary, sizeof(binary));

  ASSERT_EQ(instructions.size(), 3u);

  EXPECT_EQ(instructions[0].Mnemonic(), "add");
  EXPECT_EQ(instructions[0].Address(), 0x00400000u);

  EXPECT_EQ(instructions[1].Mnemonic(), "mov");
  EXPECT_EQ(instructions[1].Address(), 0x00400003u);

  EXPECT_EQ(instructions[2].Mnemonic(), "ret");
  EXPECT_EQ(instructions[2].Address(), 0x0040000Au);
}

TEST(ZydisInstructionDecoderTest, DecodeAll_StopsOnInvalidOpcode) {
  ZydisInstructionDecoder decoder;
  // ret ; then an invalid opcode
  const uint8_t binary[] = {0xC3, 0xFF, 0xFF};

  const auto instructions = decoder.DecodeAll(0x00400000, binary, sizeof(binary));

  ASSERT_EQ(instructions.size(), 1u);
  EXPECT_EQ(instructions[0].Mnemonic(), "ret");
}

TEST(ZydisInstructionDecoderTest, DecodeAll_EmptyBufferReturnsEmptyVector) {
  ZydisInstructionDecoder decoder;
  const uint8_t binary[] = {0x00};

  const auto instructions = decoder.DecodeAll(0x00400000, binary, 0);

  EXPECT_TRUE(instructions.empty());
}

TEST(ZydisInstructionDecoderTest, DecodeAll_NullBufferReturnsEmptyVector) {
  ZydisInstructionDecoder decoder;

  const auto instructions = decoder.DecodeAll(0x00400000, nullptr, 10);

  EXPECT_TRUE(instructions.empty());
}
