#include <gtest/gtest.h>
#include <string>

#include "Decoder.hpp"
#include "Instructions.hpp"

using rosetta::frontend::decode::Decoder;
using rosetta::frontend::decode::X86Instruction;

TEST(DecoderTest, Decode64BitAdd) {
    Decoder decoder;
    // 64-bit: add rax, rbx (0x48 is the REX.W prefix)
    const uint8_t binary[] = {0x48, 0x01, 0xD8};

    // 1. Ensure it decodes successfully
    const auto decoded = decoder.decode(0x00400000, binary, sizeof(binary));
    ASSERT_TRUE(decoded.has_value());
    const X86Instruction& ir = *decoded;

    // 2. Check Core Identity (using new getter methods)
    EXPECT_EQ(ir.get_mnemonic(), ZYDIS_MNEMONIC_ADD);
    EXPECT_EQ(ir.get_address(), 0x00400000);
    EXPECT_EQ(ir.get_text(), "add rax, rbx");
    EXPECT_EQ(ir.get_length(), 3);
    EXPECT_EQ(ir.get_operand_count(), 2);

    // 3. Check Destination (rax)
    EXPECT_EQ(ir.get_operand(0).type, ZYDIS_OPERAND_TYPE_REGISTER);
    EXPECT_EQ(ir.get_operand(0).reg.value, ZYDIS_REGISTER_RAX);
    EXPECT_EQ(ir.get_operand(0).size, 64);

    // 4. Check Source (rbx)
    EXPECT_EQ(ir.get_operand(1).type, ZYDIS_OPERAND_TYPE_REGISTER);
    EXPECT_EQ(ir.get_operand(1).reg.value, ZYDIS_REGISTER_RBX);
    EXPECT_EQ(ir.get_operand(1).size, 64);
}

TEST(DecoderTest, InvalidOpcodeFailsGracefully) {
    Decoder decoder;

    // 0xFF 0xFF is an invalid x86 opcode instruction
    uint8_t binary[] = {0xFF, 0xFF};

    // Should return false and not crash the emulator.
    EXPECT_FALSE(
        decoder.decode(0x00400000, binary, sizeof(binary)).has_value());
}

TEST(InstructionTest, DefaultConstructorIsZeroed) {
    X86Instruction ir;
    // Verifies that your new zero-initialization list works perfectly
    EXPECT_EQ(ir.get_address(), 0);
    EXPECT_EQ(ir.get_length(), 0);
    EXPECT_EQ(ir.get_operand_count(), 0);
    EXPECT_EQ(ir.get_mnemonic(), ZYDIS_MNEMONIC_INVALID);
}
