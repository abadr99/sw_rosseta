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
  using Addresses = std::vector<frontend::utils::Address>;

  BasicBlock() = default;

  void AddInstruction(const frontend::instruction::Instruction& instruction);
  bool IsEmpty() const;

  frontend::utils::Address StartAddress() const;
  frontend::utils::Address EndAddress() const;
  const Instructions& Instructions() const;

  void AddSuccessor(frontend::utils::Address successor_address);
  void AddPredecessor(frontend::utils::Address predecessor_address);
  const Addresses& Successors() const;
  const Addresses& Predecessors() const;

 private:
  Instructions instructions_;
  frontend::utils::Address start_address_ = 0;
  frontend::utils::Address end_address_ = 0;
  Addresses successors_;
  Addresses predecessors_;
};

}  // namespace basicblock
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_BASICBLOCK_HPP_
