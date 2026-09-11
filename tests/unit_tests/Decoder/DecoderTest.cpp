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

TEST(ZydisInstructionDecoderTest, Decode64BitAdd) {
  ZydisInstructionDecoder decoder;
  // 64-bit: add rax, rbx (0x48 is the REX.W prefix)
  const uint8_t binary[] = {0x48, 0x01, 0xD8};

  // 1. Ensure it decodes successfully
  const auto decoded = decoder.Decode(0x00400000, binary, sizeof(binary));
  ASSERT_NE(decoded, nullptr);
  Instruction& ir = *decoded;

  // 2. Check Core Identity
  EXPECT_EQ(ir.Mnemonic(), "add");
  EXPECT_EQ(ir.Address(), 0x00400000);
  EXPECT_EQ(ir.AssemblyText(), "add %rbx, %rax");  // AT&T syntax
  EXPECT_EQ(ir.Size(), 3);
  EXPECT_EQ(ir.Operands().size(), 2u);
  EXPECT_EQ(ir.Category(), InstructionCategory::kArithmetic);

  // 3. Check Destination (rax) - Operands() keeps Zydis's decode order
  // (dest, src), independent of AT&T's reversed display order above.
  const InstructionOperand& dst = ir.Operands()[0];
  EXPECT_EQ(dst.Type, OperandType::kRegister);
  EXPECT_EQ(dst.Reg, 1u);  // project numbering: rax = 1

  // 4. Check Source (rbx)
  const InstructionOperand& src = ir.Operands()[1];
  EXPECT_EQ(src.Type, OperandType::kRegister);
  EXPECT_EQ(src.Reg, 2u);  // project numbering: rbx = 2
}

TEST(ZydisInstructionDecoderTest, InvalidOpcodeFailsGracefully) {
  ZydisInstructionDecoder decoder;

  // 0xFF 0xFF is an invalid x86 opcode instruction
  uint8_t binary[] = {0xFF, 0xFF};

  // Should return nullptr and not crash the emulator.
  EXPECT_EQ(decoder.Decode(0x00400000, binary, sizeof(binary)), nullptr);
}

TEST(InstructionOperandTest, DefaultConstructorIsZeroed) {
  // Instruction itself has no default constructor - InstructionOperand
  // does, and this is what actually gets zero-initialized in this design.
  InstructionOperand op;
  EXPECT_EQ(op.Type, OperandType::kUnkown);
  EXPECT_EQ(op.Reg, 0u);
  EXPECT_EQ(op.Imm, 0u);
  EXPECT_EQ(op.Mem.Base, 0u);
  EXPECT_EQ(op.Mem.Index, 0u);
  EXPECT_EQ(op.Mem.Scale, 0u);
  EXPECT_EQ(op.Mem.Offset, 0u);
}
