#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

#include "frontend/Instruction.hpp"
#include "middleend/BasicBlock.hpp"
#include "middleend/CfgBuilder.hpp"
#include "middleend/ControlFlowGraph.hpp"

using rosetta::frontend::decode::X86Instruction;
using rosetta::frontend::utils::Address;
using rosetta::middleend::basicblock::BasicBlock;
using rosetta::middleend::cfg::CfgBuilder;
using rosetta::middleend::cfg::ControlFlowGraph;

namespace {

// Static storage so the operand pointers remain valid.
static ZydisDecodedOperand dummy_ops[ZYDIS_MAX_OPERAND_COUNT]{};

X86Instruction MakeMockInst(
    Address addr,
    uint8_t len,
    ZydisMnemonic mnemonic) {

    ZydisDecodedInstruction zinst{};
    decltype(*zinst.cpu_flags) cpu_flags{};

    zinst.mnemonic = mnemonic;
    zinst.length = len;
    zinst.operand_count = 0;
    zinst.operand_count_visible = 0;
    zinst.cpu_flags = &cpu_flags;

    return X86Instruction(addr, zinst, dummy_ops, "mock");
}

X86Instruction MakeMockJump(
    Address addr,
    uint8_t len,
    ZydisMnemonic mnemonic,
    Address target) {

    ZydisDecodedInstruction zinst{};
    decltype(*zinst.cpu_flags) cpu_flags{};
    ZydisDecodedOperand jump_ops[ZYDIS_MAX_OPERAND_COUNT]{};

    zinst.mnemonic = mnemonic;
    zinst.length = len;
    zinst.operand_count = 1;
    zinst.operand_count_visible = 1;
    zinst.cpu_flags = &cpu_flags;

    jump_ops[0].type = ZYDIS_OPERAND_TYPE_IMMEDIATE;
    jump_ops[0].imm.is_relative = false;
    jump_ops[0].imm.value.u = target;

    return X86Instruction(
        addr,
        zinst,
        jump_ops,
        "mock_jump");
}

bool ContainsAddress(
    const std::vector<Address>& addresses,
    Address target) {

    return std::find(
        addresses.begin(),
        addresses.end(),
        target) != addresses.end();
}

}  // namespace


// -----------------------------------------------------------------------------
// 1. Empty input
// -----------------------------------------------------------------------------

TEST(CfgBuilderTest, EmptyInstructionStreamProducesEmptyGraph) {
    std::vector<X86Instruction> instructions;

    CfgBuilder builder(instructions);
    ControlFlowGraph cfg = builder.Build();

    EXPECT_TRUE(cfg.GetBlocks().empty());
}


// -----------------------------------------------------------------------------
// 2. Straight-line code
// -----------------------------------------------------------------------------

TEST(CfgBuilderTest, StraightLineCodeCreatesSingleBasicBlock) {
    std::vector<X86Instruction> instructions = {
        MakeMockInst(0x1000, 5, ZYDIS_MNEMONIC_MOV),
        MakeMockInst(0x1005, 3, ZYDIS_MNEMONIC_ADD),
        MakeMockInst(0x1008, 1, ZYDIS_MNEMONIC_RET)
    };

    CfgBuilder builder(instructions);
    ControlFlowGraph cfg = builder.Build();

    ASSERT_EQ(cfg.GetBlocks().size(), 1);
    ASSERT_TRUE(cfg.HasBlock(0x1000));

    EXPECT_EQ(cfg.GetEntryAddress(), 0x1000);

    const BasicBlock& block = cfg.GetBlock(0x1000);

    EXPECT_EQ(block.GetStartAddress(), 0x1000);
    EXPECT_EQ(block.GetEndAddress(), 0x1009);

    ASSERT_EQ(block.GetInstructions().size(), 3);

    EXPECT_EQ(
        block.GetInstructions()[0].get_address(),
        0x1000);

    EXPECT_EQ(
        block.GetInstructions()[1].get_address(),
        0x1005);

    EXPECT_EQ(
        block.GetInstructions()[2].get_address(),
        0x1008);

    EXPECT_TRUE(block.GetSuccessors().empty());
    EXPECT_TRUE(block.GetPredecessors().empty());
}


// -----------------------------------------------------------------------------
// 3. Conditional branch
// -----------------------------------------------------------------------------

TEST(CfgBuilderTest, ConditionalBranchCreatesCorrectBlocksAndEdges) {
    //
    // 0x1000: cmp
    // 0x1003: jz 0x1020
    //
    // 0x1005: mov
    // 0x1008: jmp 0x1025
    //
    // 0x1020: xor
    //
    // 0x1025: ret
    //

    std::vector<X86Instruction> instructions = {
        MakeMockInst(0x1000, 3, ZYDIS_MNEMONIC_CMP),
        MakeMockJump(0x1003, 2, ZYDIS_MNEMONIC_JZ, 0x1020),
        MakeMockInst(0x1005, 3, ZYDIS_MNEMONIC_MOV),
        MakeMockJump(0x1008, 2, ZYDIS_MNEMONIC_JMP, 0x1025),
        MakeMockInst(0x1020, 5, ZYDIS_MNEMONIC_XOR),
        MakeMockInst(0x1025, 1, ZYDIS_MNEMONIC_RET)
    };

    CfgBuilder builder(instructions);
    ControlFlowGraph cfg = builder.Build();

    // Four leaders:
    //
    // 0x1000  first instruction
    // 0x1005  fallthrough after jz
    // 0x1020  target of jz
    // 0x1025  target of jmp
    //
    ASSERT_EQ(cfg.GetBlocks().size(), 4);

    // -------------------------------------------------------------------------
    // Block 0x1000
    // -------------------------------------------------------------------------

    const BasicBlock& entry = cfg.GetBlock(0x1000);

    ASSERT_EQ(entry.GetInstructions().size(), 2);

    EXPECT_EQ(
        entry.GetInstructions()[0].get_address(),
        0x1000);

    EXPECT_EQ(
        entry.GetInstructions()[1].get_address(),
        0x1003);

    ASSERT_EQ(entry.GetSuccessors().size(), 2);

    EXPECT_TRUE(
        ContainsAddress(entry.GetSuccessors(), 0x1020));

    EXPECT_TRUE(
        ContainsAddress(entry.GetSuccessors(), 0x1005));

    // -------------------------------------------------------------------------
    // Block 0x1005
    // -------------------------------------------------------------------------

    const BasicBlock& else_block = cfg.GetBlock(0x1005);

    ASSERT_EQ(else_block.GetInstructions().size(), 2);

    EXPECT_EQ(
        else_block.GetInstructions()[0].get_address(),
        0x1005);

    EXPECT_EQ(
        else_block.GetInstructions()[1].get_address(),
        0x1008);

    ASSERT_EQ(else_block.GetSuccessors().size(), 1);

    EXPECT_EQ(else_block.GetSuccessors()[0], 0x1025);

    // -------------------------------------------------------------------------
    // Block 0x1020
    // -------------------------------------------------------------------------

    const BasicBlock& then_block = cfg.GetBlock(0x1020);

    ASSERT_EQ(then_block.GetInstructions().size(), 1);

    EXPECT_EQ(
        then_block.GetInstructions()[0].get_address(),
        0x1020);

    ASSERT_EQ(then_block.GetSuccessors().size(), 1);

    EXPECT_EQ(then_block.GetSuccessors()[0], 0x1025);

    // -------------------------------------------------------------------------
    // Block 0x1025
    // -------------------------------------------------------------------------

    const BasicBlock& exit_block = cfg.GetBlock(0x1025);

    ASSERT_EQ(exit_block.GetInstructions().size(), 1);

    EXPECT_EQ(
        exit_block.GetInstructions()[0].get_address(),
        0x1025);

    EXPECT_TRUE(exit_block.GetSuccessors().empty());

    ASSERT_EQ(exit_block.GetPredecessors().size(), 2);

    EXPECT_TRUE(
        ContainsAddress(exit_block.GetPredecessors(), 0x1005));

    EXPECT_TRUE(
        ContainsAddress(exit_block.GetPredecessors(), 0x1020));
}


// -----------------------------------------------------------------------------
// 4. Unconditional jump
// -----------------------------------------------------------------------------

TEST(CfgBuilderTest, UnconditionalJumpHasOnlyTargetSuccessor) {
    //
    // 0x1000: jmp 0x1010
    // 0x1002: mov       <- should NOT be a successor
    // 0x1010: ret
    //

    std::vector<X86Instruction> instructions = {
        MakeMockJump(0x1000, 2, ZYDIS_MNEMONIC_JMP, 0x1010),
        MakeMockInst(0x1002, 3, ZYDIS_MNEMONIC_MOV),
        MakeMockInst(0x1010, 1, ZYDIS_MNEMONIC_RET)
    };

    CfgBuilder builder(instructions);
    ControlFlowGraph cfg = builder.Build();

    ASSERT_EQ(cfg.GetBlocks().size(), 3);

    const BasicBlock& jump_block = cfg.GetBlock(0x1000);

    ASSERT_EQ(jump_block.GetSuccessors().size(), 1);
    EXPECT_EQ(jump_block.GetSuccessors()[0], 0x1010);

    EXPECT_FALSE(
        ContainsAddress(jump_block.GetSuccessors(), 0x1002));
}


// -----------------------------------------------------------------------------
// 5. Loop / back edge
// -----------------------------------------------------------------------------

TEST(CfgBuilderTest, LoopCreatesBackEdgeAndExitEdge) {
    //
    // 0x1000: mov
    //
    // 0x1005: dec
    // 0x1007: jnz 0x1005
    //
    // 0x1009: ret
    //

    std::vector<X86Instruction> instructions = {
        MakeMockInst(0x1000, 5, ZYDIS_MNEMONIC_MOV),
        MakeMockInst(0x1005, 2, ZYDIS_MNEMONIC_DEC),
        MakeMockJump(0x1007, 2, ZYDIS_MNEMONIC_JNZ, 0x1005),
        MakeMockInst(0x1009, 1, ZYDIS_MNEMONIC_RET)
    };

    CfgBuilder builder(instructions);
    ControlFlowGraph cfg = builder.Build();

    ASSERT_EQ(cfg.GetBlocks().size(), 3);

    // Entry block
    const BasicBlock& entry = cfg.GetBlock(0x1000);

    ASSERT_EQ(entry.GetSuccessors().size(), 1);
    EXPECT_EQ(entry.GetSuccessors()[0], 0x1005);

    // Loop header/body
    const BasicBlock& loop = cfg.GetBlock(0x1005);

    ASSERT_EQ(loop.GetSuccessors().size(), 2);

    EXPECT_TRUE(
        ContainsAddress(loop.GetSuccessors(), 0x1005));

    EXPECT_TRUE(
        ContainsAddress(loop.GetSuccessors(), 0x1009));

    ASSERT_EQ(loop.GetPredecessors().size(), 2);

    EXPECT_TRUE(
        ContainsAddress(loop.GetPredecessors(), 0x1000));

    EXPECT_TRUE(
        ContainsAddress(loop.GetPredecessors(), 0x1005));

    // Exit
    const BasicBlock& exit = cfg.GetBlock(0x1009);

    EXPECT_TRUE(exit.GetSuccessors().empty());

    ASSERT_EQ(exit.GetPredecessors().size(), 1);

    EXPECT_EQ(exit.GetPredecessors()[0], 0x1005);
}


// -----------------------------------------------------------------------------
// 6. RET terminates a block
// -----------------------------------------------------------------------------

TEST(CfgBuilderTest, ReturnInstructionHasNoSuccessor) {
    //
    // 0x1000: mov
    // 0x1005: ret
    //

    std::vector<X86Instruction> instructions = {
        MakeMockInst(0x1000, 5, ZYDIS_MNEMONIC_MOV),
        MakeMockInst(0x1005, 1, ZYDIS_MNEMONIC_RET)
    };

    CfgBuilder builder(instructions);
    ControlFlowGraph cfg = builder.Build();

    ASSERT_EQ(cfg.GetBlocks().size(), 1);

    const BasicBlock& block = cfg.GetBlock(0x1000);

    EXPECT_TRUE(block.GetSuccessors().empty());
}


// -----------------------------------------------------------------------------
// 7. Block boundaries are created at branch fallthrough
// -----------------------------------------------------------------------------

TEST(CfgBuilderTest, InstructionAfterConditionalBranchStartsNewBlock) {
    //
    // 0x1000: jz 0x1010
    // 0x1002: add       <- must start new block
    // 0x1005: ret
    // 0x1010: ret
    //

    std::vector<X86Instruction> instructions = {
        MakeMockJump(0x1000, 2, ZYDIS_MNEMONIC_JZ, 0x1010),
        MakeMockInst(0x1002, 3, ZYDIS_MNEMONIC_ADD),
        MakeMockInst(0x1005, 1, ZYDIS_MNEMONIC_RET),
        MakeMockInst(0x1010, 1, ZYDIS_MNEMONIC_RET)
    };

    CfgBuilder builder(instructions);
    ControlFlowGraph cfg = builder.Build();

    ASSERT_TRUE(cfg.HasBlock(0x1000));
    ASSERT_TRUE(cfg.HasBlock(0x1002));
    ASSERT_TRUE(cfg.HasBlock(0x1010));

    const BasicBlock& branch = cfg.GetBlock(0x1000);

    ASSERT_EQ(branch.GetInstructions().size(), 1);

    EXPECT_EQ(
        branch.GetInstructions()[0].get_address(),
        0x1000);
}
