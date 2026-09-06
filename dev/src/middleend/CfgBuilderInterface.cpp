#include <vector>

#include "middleend/CfgBuilderInterface.hpp"

using rosetta::middleend::cfg::ICfgBuilder;
using rosetta::frontend::decode::X86Instruction;

ICfgBuilder::ICfgBuilder(const std::vector<X86Instruction>& instructions)
  : instructions_(instructions) {
}

ICfgBuilder::~ICfgBuilder() = default;
