#ifndef DEV_INC_FRONTEND_BASICBLOCK_HPP_
#define DEV_INC_FRONTEND_BASICBLOCK_HPP_

#include <cstdint>
#include <vector>

#include "frontend/Instruction.hpp"
#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace basicblock {

class BasicBlock {
 public:
  using Instructions = std::vector<frontend::instruction::Instruction>;
  using Addresses = std::vector<utils::Address>;

  BasicBlock() = default;

  void AddInstruction(const frontend::instruction::Instruction& instruction);
  bool IsEmpty() const;

  utils::Address StartAddress() const;
  utils::Address EndAddress() const;
  const Instructions& InstructionList() const;

  void AddSuccessor(utils::Address successor_address);
  void AddPredecessor(utils::Address predecessor_address);
  const Addresses& Successors() const;
  const Addresses& Predecessors() const;

 private:
  Instructions instructions_;
  utils::Address start_address_ = 0;
  utils::Address end_address_ = 0;
  Addresses successors_;
  Addresses predecessors_;
};

}  // namespace basicblock
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_BASICBLOCK_HPP_
