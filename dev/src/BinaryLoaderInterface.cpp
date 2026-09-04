/**
 * @file BinaryLoaderInterface.cpp
 * @brief Implementation of BinaryLoaderInterface.
 */

#include <cassert>

#include "BinaryLoaderInterface.hpp"

using namespace rosetta::frontend;  // NOLINT
using namespace rosetta::frontend::loader;  // NOLINT

IBinaryParser::IBinaryParser(const std::filesystem::path& filepath)
  : filepath_(filepath) {
}

IBinaryParser::~IBinaryParser() = default;

