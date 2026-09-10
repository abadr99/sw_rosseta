#include <gtest/gtest.h>

#include <cstdint>
#include <memory>

#include "frontend/DecoderInterface.hpp"
#include "frontend/Instruction.hpp"
#include "frontend/ZydisDecoder.hpp"

using namespace rosetta::frontend::decode;  // NOLINT

TEST(ZydisDecoderTest, Decode64BitAdd) {
  ZydisInstructionDecoder decoder;
  // 64-bit: add rax, rbx (0x48 is the REX.W prefix)
  const uint8_t binary[] = {0x48, 0x01, 0xD8};

  // 1. Ensure it decodes successfully
  const auto decoded = decoder.Decode(0x00400000, binary, sizeof(binary));
  ASSERT_NE(decoded, nullptr);
  const Instruction& ir = *decoded;

  // 2. Check Core Identity
  EXPECT_EQ(ir.GetMnemonic(), "add");
  EXPECT_EQ(ir.GetAddress(), 0x00400000);
  EXPECT_EQ(ir.GetText(), "add rax, rbx");
  EXPECT_EQ(ir.GetLength(), 3);
  EXPECT_EQ(ir.GetOperandCount(), 2);

  // 3. Check Destination (rax)
  EXPECT_EQ(ir.GetOperand(0).kind, OperandKind::kRegister);
  EXPECT_EQ(ir.GetOperand(0).reg, "rax");
  EXPECT_EQ(ir.GetOperand(0).size_bits, 64);

  // 4. Check Source (rbx)
  EXPECT_EQ(ir.GetOperand(1).kind, OperandKind::kRegister);
  EXPECT_EQ(ir.GetOperand(1).reg, "rbx");
  EXPECT_EQ(ir.GetOperand(1).size_bits, 64);
}

TEST(ZydisDecoderTest, DecodeImmediateOperand) {
  ZydisInstructionDecoder decoder;
  // 64-bit: add rax, 5
  const uint8_t binary[] = {0x48, 0x83, 0xC0, 0x05};

  const auto decoded = decoder.Decode(0x00400000, binary, sizeof(binary));
  ASSERT_NE(decoded, nullptr);
  const Instruction& ir = *decoded;

  ASSERT_EQ(ir.GetOperandCount(), 2);
  EXPECT_EQ(ir.GetOperand(0).kind, OperandKind::kRegister);
  EXPECT_EQ(ir.GetOperand(0).reg, "rax");

  EXPECT_EQ(ir.GetOperand(1).kind, OperandKind::kImmediate);
  EXPECT_EQ(ir.GetOperand(1).immediate, 5);
}

TEST(ZydisDecoderTest, DecodeMemoryOperandWithBaseAndIndex) {
  ZydisInstructionDecoder decoder;
  // 64-bit: mov rax, [rax + rcx*4]
  const uint8_t binary[] = {0x48, 0x8B, 0x04, 0x88};

  const auto decoded = decoder.Decode(0x00400000, binary, sizeof(binary));
  ASSERT_NE(decoded, nullptr);
  const Instruction& ir = *decoded;

  ASSERT_EQ(ir.GetOperandCount(), 2);
  const Operand& mem = ir.GetOperand(1);
  EXPECT_EQ(mem.kind, OperandKind::kMemory);
  EXPECT_EQ(mem.reg, "rax");
  EXPECT_EQ(mem.index, "rcx");
  EXPECT_EQ(mem.scale, 4);
  EXPECT_EQ(mem.displacement, 0);
}

TEST(ZydisDecoderTest, DecodeMemoryOperandWithNoBaseOrIndex) {
  ZydisInstructionDecoder decoder;
  // 32-bit: mov eax, [0x12345678] (absolute addressing, no base/index)
  const uint8_t binary[] = {0x8B, 0x04, 0x25, 0x78, 0x56, 0x34, 0x12};

  const auto decoded = decoder.Decode(0x00400000, binary, sizeof(binary));
  ASSERT_NE(decoded, nullptr);
  const Instruction& ir = *decoded;

  ASSERT_EQ(ir.GetOperandCount(), 2);
  const Operand& mem = ir.GetOperand(1);
  EXPECT_EQ(mem.kind, OperandKind::kMemory);
  EXPECT_TRUE(mem.reg.empty());
  EXPECT_TRUE(mem.index.empty());
  EXPECT_EQ(mem.displacement, 0x12345678);
}

TEST(ZydisDecoderTest, InvalidOpcodeFailsGracefully) {
  ZydisInstructionDecoder decoder;

  // 0xFF 0xFF is an invalid x86 opcode instruction
  uint8_t binary[] = {0xFF, 0xFF};

  // Should return nullptr and not crash the emulator.
  EXPECT_EQ(decoder.Decode(0x00400000, binary, sizeof(binary)), nullptr);
}

TEST(ZydisDecoderTest, NullBufferFailsGracefully) {
  ZydisInstructionDecoder decoder;
  EXPECT_EQ(decoder.Decode(0x00400000, nullptr, 0), nullptr);
}
