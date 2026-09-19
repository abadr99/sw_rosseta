#ifndef DEV_INC_FRONTEND_IRBUILDER_HPP_
#define DEV_INC_FRONTEND_IRBUILDER_HPP_

#include <cstdint>
#include <map>
#include <vector>

#include "frontend/Instruction.hpp"
#include "frontend/ControlFlowGraph.hpp"
#include "frontend/IrInstruction.hpp"
#include "frontend/BasicBlock.hpp"

namespace rosetta {
namespace frontend {
namespace ir {

struct PendingPhi {
  utils::Address block_address;
  size_t instruction_index;
  uint32_t machine_reg;
  PendingPhi()
    : block_address(0), instruction_index(0), machine_reg(0) {}
};

class IrBuilder {
 public:
  explicit IrBuilder(cfg::ControlFlowGraph& graph);
  ~IrBuilder() = default;

  void Build();

 private:
  // Local Lift
  void LiftBlock(basicblock::BasicBlock& block);
  void LiftInstruction(const instruction::Instruction& inst,
                       basicblock::BasicBlock& block);

  void LiftMove(const instruction::Instruction& inst,
                basicblock::BasicBlock& block);
  void LiftSub(const instruction::Instruction& inst,
               basicblock::BasicBlock& block);
  void LiftMul(const instruction::Instruction& inst,
               basicblock::BasicBlock& block);
  void LiftCompareAndCondBr(
      const instruction::Instruction& cmp_inst,
      const instruction::Instruction& branch_inst,
      basicblock::BasicBlock& block);
  void LiftReturn(basicblock::BasicBlock& block);

  // Phi
  void ResolvePendingPhis();

  // Helpers
  IrOperand LowerRegisterOperand(uint32_t machine_reg,
                                 basicblock::BasicBlock& block);
  IrOperand LowerImmediateOperand(uint64_t value) const;
  IrOperand LowerMemoryOperand(
      const instruction::InstructionOperand& operand,
      basicblock::BasicBlock& block);
  IrOperand NewVirtualReg(IrDataType data_type = IrDataType::kI64);
  IrOpcode MapOpcode(const instruction::Instruction& inst) const;

  cfg::ControlFlowGraph& graph_;
  std::map<uint32_t, IrOperand> register_map_;
  uint64_t next_vreg_id_ = 0;

  std::map<utils::Address, std::map<uint32_t, IrOperand>> block_exit_state_;

  std::vector<PendingPhi> pending_phis_;
};
}  // namespace rosetta
}  // namespace frontend
}  // namespace ir

#endif  // DEV_INC_FRONTEND_IRBUILDER_HPP_
