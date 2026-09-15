#include "middleend/IRBuilder.hpp"

#include <string>

namespace rosetta {
namespace middleend {

void IRBuilder::Build(BasicBlock& block) {
    for (const auto& instruction : block.GetInstructions()) {
        TranslateInstruction(instruction, block);
    }
}

uint32_t IRBuilder::GetVirtualRegister(uint32_t register_number) {
    const auto it = register_map_.find(register_number);

    if (it != register_map_.end()) {
        return it->second;
    }

    const uint32_t virtual_register = next_virtual_register_++;
    register_map_[register_number] = virtual_register;

    return virtual_register;
}

IROperand IRBuilder::TranslateOperand(
    const frontend::instruction::InstructionOperand& operand) {

    switch (operand.Type) {
        case frontend::instruction::OperandType::kRegister:
            return {
                IROperandType::kVirtualRegister,
                GetVirtualRegister(operand.Reg)
            };

        case frontend::instruction::OperandType::kImmediate:
            return {
                IROperandType::kImmediate,
                operand.Imm
            };

        case frontend::instruction::OperandType::kMemory:
            // Memory operands are not supported by the first IR milestone.
            return {
                IROperandType::kImmediate,
                0
            };

        default:
            return {
                IROperandType::kImmediate,
                0
            };
    }
}

void IRBuilder::TranslateInstruction(
    const frontend::instruction::Instruction& instruction,
    BasicBlock& block) {

    const std::string& mnemonic = instruction.Mnemonic();
    const auto& operands = instruction.Operands();

    if (mnemonic == "MOV") {
        if (operands.size() != 2) {
            return;
        }

        IRInstruction ir;
        ir.Opcode = IROpcode::kMov;

        ir.Operands.push_back(
            TranslateOperand(operands[0]));

        ir.Operands.push_back(
            TranslateOperand(operands[1]));

        block.AddIRInstruction(ir);
    }
}

}  // namespace middleend
}  // namespace rosetta
