#pragma once

namespace rosetta {
namespace middleend {

enum class IROpcode {
    kMov,
    kAdd,
    kSub,
    kMul,
    kCmp,
    kBranch,
    kJump,
    kReturn
};

}  // namespace middleend
}  // namespace rosetta
