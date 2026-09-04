/**
 * @file BinaryLoader.cpp
 * @brief Implementation of BinaryLoader.
 */

 #include <LIEF/LIEF.hpp>
 
#include "utils/Macros.h"
#include "utils/Types.h"
#include "BinaryLoader.hpp"

using namespace rosetta::frontend;  // NOLINT
using namespace rosetta::frontend::loader;  // NOLINT

// ------------------- BinarySection Implementation -------------------
BinarySection::BinarySection(const std::string& name, 
                             const utils::Address virtual_address, 
                             const utils::Bytes& data)
  : name_(name)
  , virtual_address_(virtual_address)
  , data_(data) 
  { /* EMPTY */}

std::string BinarySection::GetName() const { 
  return name_; 
}
utils::Address BinarySection::GetVirtualAddress() const { 
  return virtual_address_; 
}
utils::Bytes BinarySection::GetContent() const { 
  return data_; 
}

// ------------------- IBinaryParser Implementation -------------------
IBinaryParser::IBinaryParser(const std::filesystem::path& filepath)
  : filepath_(filepath) {
}

IBinaryParser::~IBinaryParser() = default;

// ------------------- LiefBinaryParser Implementation -------------------
LiefBinaryParser::LiefBinaryParser(const std::filesystem::path& filepath)
  : IBinaryParser(filepath)
  , pBinary_(LIEF::Parser::parse(filepath_.string())) {
}

BinarySection LiefBinaryParser::GetExecutableCode() const {
  using section = LIEF::Section;
  for (const auto& section : pBinary_->sections()) {
    // todo(@abadr99): we are assuming the the section that contains 
    // the executable code is named ".text"
    if (section.name() == ".text") {
      const auto content = section.content();
      return BinarySection(section.name(),
                           section.virtual_address(),
                           utils::Bytes(content.begin(), content.end()));
    }
  }
  UNREACHABLE("No executable code section found");
}

Architecture LiefBinaryParser::GetArchitecture() const {
  using arch = LIEF::Header::ARCHITECTURES;
  return pBinary_->header().architecture() == arch::X86_64 ? Architecture::kX86_64 
                                                           : Architecture::kUnknown;
}

uint64_t LiefBinaryParser::GetEntryPoint() const {
  return pBinary_->entrypoint();
}

typename IBinaryParser::LoadableSegments LiefBinaryParser::GetLoadableSegments() const {
  return LoadableSegments();
}