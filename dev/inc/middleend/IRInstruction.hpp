#pragma once

#include <vector>

#include "middleend/IROperand.hpp"
#include "middleend/IROpcode.hpp"

namespace rosetta {
namespace middleend {

struct IRInstruction {
    IROpcode Opcode;
    std::vector<IROperand> Operands;
};

}  // namespace middleend
}  // namespace rosetta