#pragma once

#include <cstdint>
#include <unordered_map>

#include "frontend/Instruction.hpp"
#include "middleend/BasicBlock.hpp"

namespace rosetta {
namespace middleend {

class IRBuilder {
public:
    void Build(BasicBlock& block);

private:
    void TranslateInstruction(
        const frontend::instruction::Instruction& instruction,
        BasicBlock& block);

    IROperand TranslateOperand(
        const frontend::instruction::InstructionOperand& operand);

    uint32_t GetVirtualRegister(uint32_t register_number);

    std::unordered_map<uint32_t, uint32_t> register_map_;
    uint32_t next_virtual_register_ = 0;
};

}  // namespace middleend
}  // namespace rosetta
