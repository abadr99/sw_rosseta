#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

#include "frontend/BasicBlock.hpp"
#include "frontend/CfgBuilder.hpp"
#include "frontend/Instruction.hpp"

using rosetta::frontend::basicblock::BasicBlock;
using rosetta::frontend::cfg::CfgBuilder;
using rosetta::frontend::cfg::ControlFlowGraph;
using rosetta::frontend::instruction::Instruction;
using rosetta::frontend::instruction::InstructionCategory;
using rosetta::frontend::instruction::InstructionOperand;
using rosetta::frontend::instruction::OperandType;
using rosetta::utils::Address;

namespace {

Instruction MakeMockInst(
    Address address,
    uint32_t size,
    InstructionCategory category,
    const std::string& mnemonic) {

  return Instruction(
      0,
      {},
      address,
      size,
      category,
      mnemonic,
      mnemonic);
}

Instruction MakeMockJump(
    Address address,
    uint32_t size,
    InstructionCategory category,
    Address target,
    const std::string& mnemonic) {

  InstructionOperand operand;
  operand.Type = OperandType::kImmediate;
  operand.Imm = target - (address + size);

  return Instruction(
      0,
      {operand},
      address,
      size,
      category,
      mnemonic,
      mnemonic);
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

TEST(CfgBuilderTest, EmptyInstructionStreamProducesEmptyGraph) {
  std::vector<Instruction> instructions;

  CfgBuilder builder(instructions);
  ControlFlowGraph graph = builder.Build();

  EXPECT_TRUE(graph.BlocksList().empty());
  EXPECT_EQ(graph.EntryAddress(), 0);
}

TEST(CfgBuilderTest, StraightLineCodeCreatesSingleBasicBlock) {
  std::vector<Instruction> instructions = {
      MakeMockInst(
          0x1000, 5,
          InstructionCategory::kDataTransfer,
          "mov"),
      MakeMockInst(
          0x1005, 3,
          InstructionCategory::kArithmetic,
          "add"),
      MakeMockInst(
          0x1008, 1,
          InstructionCategory::kReturn,
          "ret")
  };

  CfgBuilder builder(instructions);
  ControlFlowGraph graph = builder.Build();

  ASSERT_EQ(graph.BlocksList().size(), 1);
  ASSERT_TRUE(graph.HasBlock(0x1000));

  EXPECT_EQ(graph.EntryAddress(), 0x1000);

  const BasicBlock& block = graph.Block(0x1000);

  EXPECT_EQ(block.StartAddress(), 0x1000);
  EXPECT_EQ(block.EndAddress(), 0x1009);

  ASSERT_EQ(block.InstructionList().size(), 3);

  EXPECT_EQ(block.InstructionList()[0].Address(), 0x1000);
  EXPECT_EQ(block.InstructionList()[1].Address(), 0x1005);
  EXPECT_EQ(block.InstructionList()[2].Address(), 0x1008);

  EXPECT_TRUE(block.Successors().empty());
  EXPECT_TRUE(block.Predecessors().empty());
}

TEST(CfgBuilderTest, ConditionalBranchCreatesCorrectBlocksAndEdges) {
  std::vector<Instruction> instructions = {
      MakeMockInst(
          0x1000, 3,
          InstructionCategory::kLogical,
          "cmp"),

      MakeMockJump(
          0x1003, 2,
          InstructionCategory::kCondControlFlow,
          0x1020,
          "jz"),

      MakeMockInst(
          0x1005, 3,
          InstructionCategory::kDataTransfer,
          "mov"),

      MakeMockJump(
          0x1008, 2,
          InstructionCategory::kUnCondControlFlow,
          0x1025,
          "jmp"),

      MakeMockInst(
          0x1020, 5,
          InstructionCategory::kLogical,
          "xor"),

      MakeMockInst(
          0x1025, 1,
          InstructionCategory::kReturn,
          "ret")
  };

  CfgBuilder builder(instructions);
  ControlFlowGraph graph = builder.Build();

  ASSERT_EQ(graph.BlocksList().size(), 4);

  const BasicBlock& entry = graph.Block(0x1000);

  ASSERT_EQ(entry.InstructionList().size(), 2);
  EXPECT_EQ(entry.InstructionList()[0].Address(), 0x1000);
  EXPECT_EQ(entry.InstructionList()[1].Address(), 0x1003);

  ASSERT_EQ(entry.Successors().size(), 2);
  EXPECT_TRUE(ContainsAddress(entry.Successors(), 0x1020));
  EXPECT_TRUE(ContainsAddress(entry.Successors(), 0x1005));

  const BasicBlock& else_block = graph.Block(0x1005);

  ASSERT_EQ(else_block.InstructionList().size(), 2);
  EXPECT_EQ(else_block.InstructionList()[0].Address(), 0x1005);
  EXPECT_EQ(else_block.InstructionList()[1].Address(), 0x1008);

  ASSERT_EQ(else_block.Successors().size(), 1);
  EXPECT_EQ(else_block.Successors()[0], 0x1025);

  const BasicBlock& then_block = graph.Block(0x1020);

  ASSERT_EQ(then_block.InstructionList().size(), 1);
  EXPECT_EQ(then_block.InstructionList()[0].Address(), 0x1020);

  ASSERT_EQ(then_block.Successors().size(), 1);
  EXPECT_EQ(then_block.Successors()[0], 0x1025);

  const BasicBlock& exit_block = graph.Block(0x1025);

  ASSERT_EQ(exit_block.InstructionList().size(), 1);
  EXPECT_EQ(exit_block.InstructionList()[0].Address(), 0x1025);

  EXPECT_TRUE(exit_block.Successors().empty());

  ASSERT_EQ(exit_block.Predecessors().size(), 2);
  EXPECT_TRUE(ContainsAddress(exit_block.Predecessors(), 0x1005));
  EXPECT_TRUE(ContainsAddress(exit_block.Predecessors(), 0x1020));
}

TEST(CfgBuilderTest, UnconditionalJumpHasOnlyTargetSuccessor) {
  std::vector<Instruction> instructions = {
      MakeMockJump(
          0x1000, 2,
          InstructionCategory::kUnCondControlFlow,
          0x1010,
          "jmp"),

      MakeMockInst(
          0x1002, 3,
          InstructionCategory::kDataTransfer,
          "mov"),

      MakeMockInst(
          0x1010, 1,
          InstructionCategory::kReturn,
          "ret")
  };

  CfgBuilder builder(instructions);
  ControlFlowGraph graph = builder.Build();

  ASSERT_EQ(graph.BlocksList().size(), 3);

  const BasicBlock& jump_block = graph.Block(0x1000);

  ASSERT_EQ(jump_block.Successors().size(), 1);
  EXPECT_EQ(jump_block.Successors()[0], 0x1010);

  EXPECT_FALSE(
      ContainsAddress(jump_block.Successors(), 0x1002));
}

TEST(CfgBuilderTest, LoopCreatesBackEdgeAndExitEdge) {
  std::vector<Instruction> instructions = {
      MakeMockInst(
          0x1000, 5,
          InstructionCategory::kDataTransfer,
          "mov"),

      MakeMockInst(
          0x1005, 2,
          InstructionCategory::kArithmetic,
          "dec"),

      MakeMockJump(
          0x1007, 2,
          InstructionCategory::kCondControlFlow,
          0x1005,
          "jnz"),

      MakeMockInst(
          0x1009, 1,
          InstructionCategory::kReturn,
          "ret")
  };

  CfgBuilder builder(instructions);
  ControlFlowGraph graph = builder.Build();

  ASSERT_EQ(graph.BlocksList().size(), 3);

  const BasicBlock& entry = graph.Block(0x1000);

  ASSERT_EQ(entry.Successors().size(), 1);
  EXPECT_EQ(entry.Successors()[0], 0x1005);

  const BasicBlock& loop = graph.Block(0x1005);

  ASSERT_EQ(loop.Successors().size(), 2);

  EXPECT_TRUE(ContainsAddress(loop.Successors(), 0x1005));
  EXPECT_TRUE(ContainsAddress(loop.Successors(), 0x1009));

  ASSERT_EQ(loop.Predecessors().size(), 2);

  EXPECT_TRUE(ContainsAddress(loop.Predecessors(), 0x1000));
  EXPECT_TRUE(ContainsAddress(loop.Predecessors(), 0x1005));

  const BasicBlock& exit = graph.Block(0x1009);

  EXPECT_TRUE(exit.Successors().empty());

  ASSERT_EQ(exit.Predecessors().size(), 1);
  EXPECT_EQ(exit.Predecessors()[0], 0x1005);
}

TEST(CfgBuilderTest, ReturnInstructionHasNoSuccessor) {
  std::vector<Instruction> instructions = {
      MakeMockInst(
          0x1000, 5,
          InstructionCategory::kDataTransfer,
          "mov"),

      MakeMockInst(
          0x1005, 1,
          InstructionCategory::kReturn,
          "ret")
  };

  CfgBuilder builder(instructions);
  ControlFlowGraph graph = builder.Build();

  ASSERT_EQ(graph.BlocksList().size(), 1);

  const BasicBlock& block = graph.Block(0x1000);

  EXPECT_TRUE(block.Successors().empty());
}

TEST(CfgBuilderTest, InstructionAfterConditionalBranchStartsNewBlock) {
  std::vector<Instruction> instructions = {
      MakeMockJump(
          0x1000, 2,
          InstructionCategory::kCondControlFlow,
          0x1010,
          "jz"),

      MakeMockInst(
          0x1002, 3,
          InstructionCategory::kDataTransfer,
          "add"),

      MakeMockInst(
          0x1005, 1,
          InstructionCategory::kReturn,
          "ret"),

      MakeMockInst(
          0x1010, 1,
          InstructionCategory::kReturn,
          "ret")
  };

  CfgBuilder builder(instructions);
  ControlFlowGraph graph = builder.Build();

  ASSERT_TRUE(graph.HasBlock(0x1000));
  ASSERT_TRUE(graph.HasBlock(0x1002));
  ASSERT_TRUE(graph.HasBlock(0x1010));

  const BasicBlock& branch = graph.Block(0x1000);

  ASSERT_EQ(branch.InstructionList().size(), 1);
  EXPECT_EQ(branch.InstructionList()[0].Address(), 0x1000);
}
