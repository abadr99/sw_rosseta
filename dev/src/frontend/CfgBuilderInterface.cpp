#include <vector>

#include "frontend/CfgBuilderInterface.hpp"

using rosetta::frontend::cfg::ICfgBuilder;
using rosetta::frontend::instruction::Instruction;

ICfgBuilder::ICfgBuilder(const std::vector<Instruction>& instructions)
  : instructions_(instructions) {
}

ICfgBuilder::~ICfgBuilder() = default;
