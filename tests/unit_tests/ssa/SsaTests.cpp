#include <gtest/gtest.h>

#include "frontend/BasicBlock.hpp"
#include "frontend/SsaBlock.hpp"
#include "frontend/SsaStatement.hpp"

using rosetta::frontend::basicblock::BasicBlock;
using rosetta::frontend::ssa::SsaBlock;
using rosetta::frontend::ssa::SsaDataType;
using rosetta::frontend::ssa::SsaOpcode;
using rosetta::frontend::ssa::SsaOperand;
using rosetta::frontend::ssa::SsaOperandType;
using rosetta::frontend::ssa::SsaPhiIncoming;
using rosetta::frontend::ssa::SsaStatement;

TEST(SsaOperandTest, DefaultConstructorIsEmpty) {
  SsaOperand operand;

  EXPECT_EQ(operand.Type, SsaOperandType::kNone);
  EXPECT_EQ(operand.DataType, SsaDataType::kNone);
  EXPECT_EQ(operand.Value, 0u);
}

TEST(SsaPhiIncomingTest, DefaultConstructorIsEmpty) {
  SsaPhiIncoming incoming;

  EXPECT_EQ(incoming.PredecessorBlock, 0u);
  EXPECT_EQ(incoming.Value.Type, SsaOperandType::kNone);
  EXPECT_EQ(incoming.Value.DataType, SsaDataType::kNone);
  EXPECT_EQ(incoming.Value.Value, 0u);
}

TEST(SsaStatementTest, DefaultConstructorIsEmpty) {
  SsaStatement statement;

  EXPECT_EQ(statement.Opcode(), SsaOpcode::kNone);
  EXPECT_TRUE(statement.Operands().empty());
  EXPECT_FALSE(statement.HasResult());
  EXPECT_EQ(statement.TrueTarget(), 0u);
  EXPECT_EQ(statement.FalseTarget(), 0u);
  EXPECT_EQ(statement.GuestPc(), 0u);
  EXPECT_TRUE(statement.PhiIncomingList().empty());
}

TEST(SsaStatementTest, ConstStoresOperandAndResult) {
  SsaOperand constant;
  constant.Type = SsaOperandType::kConstant;
  constant.DataType = SsaDataType::kI64;
  constant.Value = 4;

  SsaOperand result;
  result.Type = SsaOperandType::kVirtualReg;
  result.DataType = SsaDataType::kI64;
  result.Value = 0;

  // %0 = const i64 4
  SsaStatement statement(
      SsaOpcode::kConst, {constant}, result, 0, 0, 0x00400000);

  EXPECT_EQ(statement.Opcode(), SsaOpcode::kConst);
  ASSERT_EQ(statement.Operands().size(), 1u);

  const SsaOperand& operand = statement.Operands()[0];
  EXPECT_EQ(operand.Type, SsaOperandType::kConstant);
  EXPECT_EQ(operand.DataType, SsaDataType::kI64);
  EXPECT_EQ(operand.Value, 4u);

  EXPECT_TRUE(statement.HasResult());
  EXPECT_EQ(statement.Result().Type, SsaOperandType::kVirtualReg);
  EXPECT_EQ(statement.Result().DataType, SsaDataType::kI64);
  EXPECT_EQ(statement.Result().Value, 0u);
  EXPECT_EQ(statement.GuestPc(), 0x00400000u);
}

TEST(SsaStatementTest, ArithmeticStoresTwoOperands) {
  SsaOperand lhs;
  lhs.Type = SsaOperandType::kVirtualReg;
  lhs.DataType = SsaDataType::kI64;
  lhs.Value = 0;

  SsaOperand rhs;
  rhs.Type = SsaOperandType::kConstant;
  rhs.DataType = SsaDataType::kI64;
  rhs.Value = 1;

  SsaOperand result;
  result.Type = SsaOperandType::kVirtualReg;
  result.DataType = SsaDataType::kI64;
  result.Value = 1;

  // %1 = sub i64 %0, 1
  SsaStatement statement(SsaOpcode::kSub, {lhs, rhs}, result);

  EXPECT_EQ(statement.Opcode(), SsaOpcode::kSub);
  ASSERT_EQ(statement.Operands().size(), 2u);

  EXPECT_EQ(statement.Operands()[0].Type, SsaOperandType::kVirtualReg);
  EXPECT_EQ(statement.Operands()[0].Value, 0u);

  EXPECT_EQ(statement.Operands()[1].Type, SsaOperandType::kConstant);
  EXPECT_EQ(statement.Operands()[1].Value, 1u);

  EXPECT_TRUE(statement.HasResult());
  EXPECT_EQ(statement.Result().Value, 1u);
}

TEST(SsaStatementTest, CondBrStoresTargetsAndCondition) {
  SsaOperand condition;
  condition.Type = SsaOperandType::kVirtualReg;
  condition.DataType = SsaDataType::kI1;
  condition.Value = 2;

  // condbr %2, 0x00400020, 0x00400010
  SsaStatement statement(
      SsaOpcode::kCondBr,
      {condition},
      SsaOperand(),
      0x00400020,
      0x00400010,
      0x0040000E);

  EXPECT_EQ(statement.Opcode(), SsaOpcode::kCondBr);
  ASSERT_EQ(statement.Operands().size(), 1u);

  EXPECT_EQ(statement.Operands()[0].Type, SsaOperandType::kVirtualReg);
  EXPECT_EQ(statement.Operands()[0].DataType, SsaDataType::kI1);
  EXPECT_EQ(statement.Operands()[0].Value, 2u);

  EXPECT_FALSE(statement.HasResult());
  EXPECT_EQ(statement.TrueTarget(), 0x00400020u);
  EXPECT_EQ(statement.FalseTarget(), 0x00400010u);
  EXPECT_EQ(statement.GuestPc(), 0x0040000Eu);
}

TEST(SsaStatementTest, PhiStoresIncomingValues) {
  SsaOperand result;
  result.Type = SsaOperandType::kVirtualReg;
  result.DataType = SsaDataType::kI64;
  result.Value = 2;

  SsaOperand first_value;
  first_value.Type = SsaOperandType::kVirtualReg;
  first_value.DataType = SsaDataType::kI64;
  first_value.Value = 0;

  SsaOperand second_value;
  second_value.Type = SsaOperandType::kVirtualReg;
  second_value.DataType = SsaDataType::kI64;
  second_value.Value = 1;

  SsaPhiIncoming first;
  first.PredecessorBlock = 0x00400000;
  first.Value = first_value;

  SsaPhiIncoming second;
  second.PredecessorBlock = 0x00400020;
  second.Value = second_value;

  // %2 = phi [%0, 0x00400000], [%1, 0x00400020]
  SsaStatement statement(SsaOpcode::kPhi, {}, result);
  statement.AddPhiIncoming(first);
  statement.AddPhiIncoming(second);

  EXPECT_EQ(statement.Opcode(), SsaOpcode::kPhi);
  EXPECT_TRUE(statement.Operands().empty());
  EXPECT_TRUE(statement.HasResult());

  const auto& incoming = statement.PhiIncomingList();
  ASSERT_EQ(incoming.size(), 2u);

  EXPECT_EQ(incoming[0].PredecessorBlock, 0x00400000u);
  EXPECT_EQ(incoming[0].Value.Value, 0u);

  EXPECT_EQ(incoming[1].PredecessorBlock, 0x00400020u);
  EXPECT_EQ(incoming[1].Value.Value, 1u);
}

TEST(SsaBlockTest, StartsWithNoStatements) {
  BasicBlock basic_block;
  SsaBlock block(basic_block);

  EXPECT_TRUE(block.StatementList().empty());
}

TEST(SsaBlockTest, AddStatementPreservesOrder) {
  BasicBlock basic_block;
  SsaBlock block(basic_block);

  SsaStatement first(SsaOpcode::kConst, {}, SsaOperand());
  SsaStatement second(SsaOpcode::kSub, {}, SsaOperand());
  SsaStatement third(SsaOpcode::kMul, {}, SsaOperand());

  block.AddStatement(first);
  block.AddStatement(second);
  block.AddStatement(third);

  ASSERT_EQ(block.StatementList().size(), 3u);

  EXPECT_EQ(block.StatementList()[0].Opcode(), SsaOpcode::kConst);
  EXPECT_EQ(block.StatementList()[1].Opcode(), SsaOpcode::kSub);
  EXPECT_EQ(block.StatementList()[2].Opcode(), SsaOpcode::kMul);
}

TEST(SsaStatementTest, BrStoresTargetWithoutResult) {
  // br 0x00400020
  SsaStatement statement(
      SsaOpcode::kBr,
      {},
      SsaOperand(),
      0x00400020,
      0,
      0x00400010);

  EXPECT_EQ(statement.Opcode(), SsaOpcode::kBr);
  EXPECT_TRUE(statement.Operands().empty());
  EXPECT_FALSE(statement.HasResult());
  EXPECT_EQ(statement.TrueTarget(), 0x00400020u);
  EXPECT_EQ(statement.FalseTarget(), 0u);
  EXPECT_EQ(statement.GuestPc(), 0x00400010u);
}

TEST(SsaStatementTest, IndirectBrStoresTargetOperand) {
  SsaOperand target;
  target.Type = SsaOperandType::kVirtualReg;
  target.DataType = SsaDataType::kI64;
  target.Value = 3;

  // indirectbr %3
  SsaStatement statement(
      SsaOpcode::kIndirectBr,
      {target},
      SsaOperand(),
      0,
      0,
      0x00400020);

  EXPECT_EQ(statement.Opcode(), SsaOpcode::kIndirectBr);
  ASSERT_EQ(statement.Operands().size(), 1u);

  EXPECT_EQ(statement.Operands()[0].Type, SsaOperandType::kVirtualReg);
  EXPECT_EQ(statement.Operands()[0].DataType, SsaDataType::kI64);
  EXPECT_EQ(statement.Operands()[0].Value, 3u);

  EXPECT_FALSE(statement.HasResult());
  EXPECT_EQ(statement.GuestPc(), 0x00400020u);
}

TEST(SsaStatementTest, PhiIncomingPreservesInsertionOrder) {
  SsaPhiIncoming first;
  first.PredecessorBlock = 0x00400010;
  first.Value.Value = 1;

  SsaPhiIncoming second;
  second.PredecessorBlock = 0x00400020;
  second.Value.Value = 2;

  SsaStatement statement(
    SsaOpcode::kPhi,
    {},
    SsaOperand());

  statement.AddPhiIncoming(first);
  statement.AddPhiIncoming(second);

  const auto& incoming = statement.PhiIncomingList();
  ASSERT_EQ(incoming.size(), 2u);

  EXPECT_EQ(incoming[0].PredecessorBlock, 0x00400010u);
  EXPECT_EQ(incoming[0].Value.Value, 1u);

  EXPECT_EQ(incoming[1].PredecessorBlock, 0x00400020u);
  EXPECT_EQ(incoming[1].Value.Value, 2u);
}