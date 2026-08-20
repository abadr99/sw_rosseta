#include <gtest/gtest.h>
#include "Decoder.hpp"
#include "Instructions.hpp"

TEST(DecoderTest, Decode64BitAdd) {
    Decoder decoder;
    X86Instruction ir;
    
    // 64-bit: add rax, rcx (0x48 is the REX.W prefix)
    uint8_t binary[] = {0x48, 0x01, 0xC8};
    
    // 1. Ensure it decodes successfully
    bool success = decoder.decode_instruction(binary, sizeof(binary), ir);
    EXPECT_TRUE(success);
    
    // 2. Check Core Identity
    EXPECT_EQ(ir.name, "add");
    EXPECT_EQ(ir.length, 3);
    EXPECT_EQ(ir.operand_count, 2);
    
    // 3. Check Destination (rax)
    EXPECT_TRUE(ir.is_operand_register(0));
    EXPECT_EQ(ir.operands[0].reg.value, ZYDIS_REGISTER_RAX);
    EXPECT_EQ(ir.operands[0].size, 64);
    
    // 4. Check Source (rcx)
    EXPECT_TRUE(ir.is_operand_register(1));
    EXPECT_EQ(ir.operands[1].reg.value, ZYDIS_REGISTER_RCX);
    EXPECT_EQ(ir.operands[1].size, 64);
}

TEST(DecoderTest, InvalidOpcodeFailsGracefully) {
    Decoder decoder;
    X86Instruction ir;
    
    // 0xFF 0xFF is an invalid x86 opcode instruction
    uint8_t binary[] = {0xFF, 0xFF};
    
    // Should return false and NOT crash the emulator
    bool success = decoder.decode_instruction(binary, sizeof(binary), ir);
    EXPECT_FALSE(success);
}

TEST(InstructionTest, FormatsZydisFlagMasks) {
    X86Instruction instruction;

    const std::string flags = instruction.get_flags_string(
        ZYDIS_CPUFLAG_CF | ZYDIS_CPUFLAG_ZF | ZYDIS_CPUFLAG_OF);

    EXPECT_NE(flags.find("CF"), std::string::npos);
    EXPECT_NE(flags.find("ZF"), std::string::npos);
    EXPECT_NE(flags.find("OF"), std::string::npos);
}
