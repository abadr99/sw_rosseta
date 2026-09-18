#ifndef DEV_INC_FRONTEND_IRBASICBLOCK_HPP_
#define DEV_INC_FRONTEND_IRBASICBLOCK_HPP_

#include <cstdint>
#include <vector>

#include "frontend/IrInstruction.hpp"
#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace ir {

class IrBasicBlock {
 public:
  using Instructions = std::vector<IrInstruction>;
  using Addresses = std::vector<rosetta::utils::Address>;

  IrBasicBlock() = default;
  explicit IrBasicBlock(rosetta::utils::Address start_address);
  ~IrBasicBlock() = default;
  
  rosetta::utils::Address StartAddress() const;

  // Instruction sequence accessors & mutators
  void AddInstruction(IrInstruction instruction);
  const Instructions& InstructionList() const;
  Instructions& InstructionList();

  // Control Flow Graph (CFG) connectivity
  void AddSuccessor(rosetta::utils::Address successor);
  void AddPredecessor(rosetta::utils::Address predecessor);
  const Addresses& Successors() const;
  const Addresses& Predecessors() const;

 private:
  rosetta::utils::Address start_address_ = 0;
  Instructions instructions_;
  Addresses successors_;
  Addresses predecessors_;
};

}  // namespace ir
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_IRBASICBLOCK_HPP_
