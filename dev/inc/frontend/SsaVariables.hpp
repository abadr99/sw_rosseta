#ifndef DEV_INC_FRONTEND_SSAVARIABLES_HPP_
#define DEV_INC_FRONTEND_SSAVARIABLES_HPP_

#include <cstdint>
#include <map>
#include <vector>

#include "frontend/ControlFlowGraph.hpp"
#include "frontend/Instruction.hpp"
#include "frontend/SsaBlock.hpp"
#include "frontend/SsaStatement.hpp"

namespace rosetta {
namespace frontend {
namespace ssa {

struct PendingPhi {
  utils::Address block_address;
  size_t instruction_index;
  uint32_t machine_reg;
  PendingPhi()
    : block_address(0), instruction_index(0), machine_reg(0) {}
};

class SsaVariables {
 public:
  explicit SsaVariables(cfg::ControlFlowGraph& graph);
  ~SsaVariables() = default;

  void Build();

 private:
  // Local Lift
  void LiftBlock(SsaBlock& block);
  void LiftInstruction(const instruction::Instruction& inst,
                       SsaBlock& block);

  void LiftMove(const instruction::Instruction& inst,
                SsaBlock& block);
  void LiftSub(const instruction::Instruction& inst,
               SsaBlock& block);
  void LiftMul(const instruction::Instruction& inst,
               SsaBlock& block);
  void LiftCompareAndCondBr(
      const instruction::Instruction& cmp_inst,
      const instruction::Instruction& branch_inst,
      SsaBlock& block);
  void LiftReturn(SsaBlock& block);

  // Phi
  void ResolvePendingPhis();

  // Helpers
  SsaOperand LowerRegisterOperand(uint32_t machine_reg,
                                  SsaBlock& block);
  SsaOperand LowerImmediateOperand(uint64_t value) const;
  SsaOperand LowerMemoryOperand(
      const instruction::InstructionOperand& operand,
      SsaBlock& block);
  SsaOperand NewVirtualReg(SsaDataType data_type = SsaDataType::kI64);
  SsaOpcode MapOpcode(const instruction::Instruction& inst) const;

  SsaBlock& GetOrCreateBlock(utils::Address address);

  cfg::ControlFlowGraph& graph_;
  std::map<utils::Address, SsaBlock> ssa_blocks_;
  std::map<uint32_t, SsaOperand> register_map_;
  uint64_t next_vreg_id_ = 0;

  std::map<utils::Address, std::map<uint32_t, SsaOperand>> block_exit_state_;

  std::vector<PendingPhi> pending_phis_;
};
}  // namespace ssa
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_SSAVARIABLES_HPP_
