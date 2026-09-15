#pragma once

#include <cstdint>

namespace rosetta {
namespace middleend {

enum class IROperandType {
    kVirtualRegister,
    kImmediate,
    kBasicBlock
};

struct IROperand {
    IROperandType Type;
    uint64_t Value;
};

}  // namespace middleend
}  // namespace rosetta