#ifndef DEV_INC_FRONTEND_IRBUILDER_HPP_
#define DEV_INC_FRONTEND_IRBUILDER_HPP_

#include <cstdint>
#include <map>

#include "frontend/Instruction.hpp"
#include "frontend/IrBasicBlock.hpp"
#include "frontend/IrFunction.hpp"
#include "frontend/ControlFlowGraph.hpp"
#include "frontend/IrInstruction.hpp"

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

  // Helpers
  IrOperand LowerOperand(const instruction::InstructionOperand& operand, IrBasicBlock& dst);
  IrOperand LowerMemoryOperand(const instruction::InstructionOperand& operand, IrBasicBlock& dst);
  IrOpcode MapOpcode(const instruction::Instruction& inst) const;
  IrOperand NewVirtualRegister(uint32_t reg, uint8_t size_bits);

  cfg::ControlFlowGraph graph_;
  std::map<uint32_t, IrOperand> register_map_;
  uint64_t next_vreg_int_ = 0;
};
}  // namespace rosetta
}  // namespace frontend
}  // namespace ir

#endif  // DEV_INC_FRONTEND_IRBUILDER_HPP_
