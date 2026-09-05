#ifndef DEV_INC_MIDDLEEND_BASICBLOCK_HPP_
#define DEV_INC_MIDDLEEND_BASICBLOCK_HPP_

#include <cstdint>
#include <vector>

#include <frontend/Instruction.hpp>
#include <utils/Types.hpp>

namespace rosetta {
namespace middleend {
namespace basicblock {

class BasicBlock {
 public:
  using Instructions = std::vector<frontend::decode::X86Instruction>;
  using Addresses = std::vector<frontend::utils::Address>;

  BasicBlock() = default;

  void AddInstruction(const frontend::decode::X86Instruction& instruction);
  bool IsEmpty() const;

  frontend::utils::Address GetStartAddress() const;
  frontend::utils::Address GetEndAddress() const;
  const Instructions& GetInstructions() const;

  void AddSuccessor(frontend::utils::Address successor_address);
  void AddPredecessor(frontend::utils::Address predecessor_address);
  const Addresses& GetSuccessors() const;
  const Addresses& GetPredecessors() const;

 private:
  Instructions instructions_;
  frontend::utils::Address start_address_ = 0;
  frontend::utils::Address end_address_ = 0;
  Addresses successors_;
  Addresses predecessors_;
};

}  // namespace basicblock
}  // namespace middleend
}  // namespace rosetta

#endif  // DEV_INC_MIDDLEEND_BASICBLOCK_HPP_
