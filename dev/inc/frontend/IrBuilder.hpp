#ifndef DEV_INC_FRONTEND_IRBUILDER_HPP_
#define DEV_INC_FRONTEND_IRBUILDER_HPP_

#include "frontend/IrFunction.hpp"
#include "frontend/ControlFlowGraph.hpp"

namespace rosetta {
namespace frontend {
namespace ir {

class IrBuilder {
 public:
  explicit IrBuilder(const cfg::ControlFlowGraph& graph);
  ~IrBuilder() = default;

  IrFunction Build();
 private:
  void LiftBlock(const basicblock::BasicBlock& src, IrBasicBlock& dst) const;
  void LiftInstruction(const instruction::Instruction& inst, IrBasicBlock& dst) const;

  void LiftArithmetic(const instruction::Instruction& inst, IrBasicBlock& dst);
  void LiftLogical(const instruction::Instruction& inst, IrBasicBlock& dst);
  void LiftDataTransfer(const instruction::Instruction& inst, IrBasicBlock& dst);
  void LiftCondBranch(const instruction::Instruction& inst, IrBasicBlock& dst);
  void LiftUnCondJump(const instruction::Instruction& inst, IrBasicBlock& dst);
  void LiftCall(const instruction::Instruction& inst, IrBasicBlock& dst);
  void LiftReturn(const instruction::Instruction& inst, IrBasicBlock& dst);

  cfg::ControlFlowGraph graph_;
};
}  // namespace rosetta
}  // namespace frontend
}  // namespace ir

#endif  // DEV_INC_FRONTEND_IRBUILDER_HPP_