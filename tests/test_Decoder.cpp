#include <gtest/gtest.h>
#include "Decoder.hpp"
#include "Instructions.hpp"

TEST(DecoderTest, Decode64BitAdd) {
    Decoder decoder;
    // 64-bit: add rax, rbx (0x48 is the REX.W prefix)
    const uint8_t binary[] = {0x48, 0x01, 0xD8};
    
    // 1. Ensure it decodes successfully
    const auto decoded = decoder.decode(0x00400000, binary, sizeof(binary));
    ASSERT_TRUE(decoded.has_value());
    const X86Instruction& ir = *decoded;
    
    // 2. Check Core Identity
    EXPECT_EQ(ir.name, "add");
    EXPECT_EQ(ir.mnemonic, ZYDIS_MNEMONIC_ADD);
    EXPECT_EQ(ir.address, 0x00400000);
    EXPECT_EQ(ir.text, "add rax, rbx");
    EXPECT_EQ(ir.length, 3);
    EXPECT_EQ(ir.operand_count, 2);
    
    // 3. Check Destination (rax)
    EXPECT_TRUE(ir.is_operand_register(0));
    EXPECT_EQ(ir.operands[0].reg.value, ZYDIS_REGISTER_RAX);
    EXPECT_EQ(ir.operands[0].size, 64);
    
    // 4. Check Source (rbx)
    EXPECT_TRUE(ir.is_operand_register(1));
    EXPECT_EQ(ir.operands[1].reg.value, ZYDIS_REGISTER_RBX);
    EXPECT_EQ(ir.operands[1].size, 64);
}

TEST(DecoderTest, InvalidOpcodeFailsGracefully) {
    Decoder decoder;
    
    // 0xFF 0xFF is an invalid x86 opcode instruction
    uint8_t binary[] = {0xFF, 0xFF};
    
    // Should return false and NOT crash the emulator
    EXPECT_FALSE(decoder.decode(0x00400000, binary, sizeof(binary)).has_value());
}

TEST(InstructionTest, FormatsZydisFlagMasks) {
    X86Instruction instruction;

    const std::string flags = instruction.get_flags_string(
        ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_OF);

    EXPECT_NE(flags.find("CF"), std::string::npos);
    EXPECT_NE(flags.find("ZF"), std::string::npos);
    EXPECT_NE(flags.find("OF"), std::string::npos);
}
