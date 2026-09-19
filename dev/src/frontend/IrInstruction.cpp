#include "frontend/IrInstruction.hpp"

#include <vector>
#include <utility>

namespace rosetta {
namespace frontend {
namespace ir {

IrInstruction::IrInstruction(IrOpcode opcode,
                             std::vector<IrOperand> operands,
                             IrOperand result,
                             utils::Address true_target,
                             utils::Address false_target,
                             utils::Address guest_pc,
                             std::vector<PhiIncoming> phi_incoming)
    : opcode_(opcode),
      operands_(std::move(operands)),
      result_(result),
      true_target_(true_target),
      false_target_(false_target),
      guest_pc_(guest_pc),
      phi_incoming_(std::move(phi_incoming)) {}

IrOpcode IrInstruction::Opcode() const {
  return opcode_;
}

const std::vector<IrOperand>& IrInstruction::Operands() const {
  return operands_;
}

IrOperand IrInstruction::Result() const {
  return result_;
}

bool IrInstruction::HasResult() const {
  return result_.Type != IrOperandType::kNone;
}

utils::Address IrInstruction::TrueTarget() const {
  return true_target_;
}

utils::Address IrInstruction::FalseTarget() const {
  return false_target_;
}

utils::Address IrInstruction::GuestPc() const {
  return guest_pc_;
}

const std::vector<PhiIncoming>& IrInstruction::PhiIncomingList() const {
  return phi_incoming_;
}

}  // namespace ir
}  // namespace frontend
}  // namespace rosetta
