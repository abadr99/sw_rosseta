#include <LIEF/LIEF.hpp>
#include <LIEF/ELF.hpp>
#include <LIEF/PE.hpp>

#include "utils/Macros.h"
#include "utils/Types.h"
#include "BinaryLoaderInterface.hpp"
#include "LiefBinaryLoader.hpp"

using namespace rosetta::frontend;  // NOLINT
using namespace rosetta::frontend::loader;  // NOLINT

LiefBinaryParser::LiefBinaryParser(const std::filesystem::path& filepath)
  : IBinaryParser(filepath)
  , pBinary_(LIEF::Parser::parse(filepath_.string())) {
}

BinarySection LiefBinaryParser::GetExecutableCodeLinux(const LIEF::ELF::Binary& elf) const {
  // How to get the executable code in Linux? (usually named .text)
  // 1) Iterate over all segments that **Loadable and executable**
  // 2) entry point is in this section 
  // NOTE: We have to use segments not sections as they might by stripped from binary 
  //       using `strip`
  using lief = LIEF::ELF::Segment;
  for (const auto& segment : elf.segments()) {
    if (segment.type() != lief::TYPE::LOAD) {
      continue;  // SKIP NON-LOADABLE SEGMENTS
    }
    if (!segment.has(lief::FLAGS::X)) {
      continue;  // SKIP NON EXECUTABLE SEGMENTS
    }
    if (segment.virtual_size() == 0) {
      continue;  // SKIP EMPTY SEGMENTS
    }
    utils::Address start_address =segment.virtual_address();
    utils::Address end_address = start_address + segment.virtual_size();
    if (pBinary_->entrypoint() >= start_address && pBinary_->entrypoint() < end_address) {
      // Entry point is in this section 
      // const std::string& name, utils::Address va, const utils::Bytes& data
      return BinarySection("exe_section",   // TODO(@abadr99): Check if this is the correct name
                            segment.virtual_address(), 
                            utils::Bytes(segment.content().begin(), segment.content().end()));
    }
  }
  // INFO("[LiefBinaryParser]: No executable section found");
  return BinarySection("", 0, utils::Bytes(0)); 
}

BinarySection LiefBinaryParser::GetExecutableCodeWindows(const LIEF::PE::Binary& pe) const {
  // How to get the executable code in windows? (usually named .text)
  // 1) Iterate over all sections (no segments here) it should be executable or CNT_CODE
  // 2) section should contains entry point
  using C = LIEF::PE::Section::CHARACTERISTICS;
  auto IsExecutable = [&](const LIEF::PE::Section& section) {
    return section.has_characteristic(C::MEM_EXECUTE) || section.has_characteristic(C::CNT_CODE);
  };

  for (const auto& section : pe.sections()) {
    if (!IsExecutable(section)) {
      continue;  // SKIP NON EXECUTABLE SECTIONS 
    }
    // NOTE: In windows virtual address is RVA (relative virtual address)
    const utils::Address start_address = pe.imagebase() + section.virtual_address();
    const utils::Address end_address = start_address + pe.virtual_size();
    if (pBinary_->entrypoint() >= start_address && pBinary_->entrypoint() < end_address) {
      return BinarySection(section.name(),
                            start_address, 
                            utils::Bytes(section.content().begin(), section.content().end()));
    }
  }
  // INFO("[LiefBinaryParser]: No executable section found");
  return BinarySection("", 0, utils::Bytes(0)); 
}

BinarySection LiefBinaryParser::GetExecutableCode() const {
  assert(pBinary_ != nullptr);
  switch (pBinary_->format()) {
    case LIEF::Binary::FORMATS::ELF:  {
      const auto& elf = static_cast<const LIEF::ELF::Binary&>(*pBinary_);
      return GetExecutableCodeLinux(elf);
    }
    case LIEF::Binary::FORMATS::PE:   {
      const auto& pe = static_cast<const LIEF::PE::Binary&>(*pBinary_);
      return GetExecutableCodeWindows(pe);
    }
    default:
      UNREACHABLE("Unsupported binary format");
  }
}

Architecture LiefBinaryParser::GetArchitecture() const {
  using arch = LIEF::Header::ARCHITECTURES;
  return pBinary_->header().architecture() == arch::X86_64 ? Architecture::kX86_64 
                                                           : Architecture::kUnknown;
}

uint64_t LiefBinaryParser::GetEntryPoint() const {
  return pBinary_->entrypoint();
}

IBinaryParser::LoadableSegments 
LiefBinaryParser::GetLoadableSegmentsLinux(const LIEF::ELF::Binary& elf) const {
  UNIMPLEMENTED();
  return {};
}

IBinaryParser::LoadableSegments 
LiefBinaryParser::GetLoadableSegmentsWindows(const LIEF::PE::Binary& pe) const {
  UNIMPLEMENTED();
  return {};
}

IBinaryParser::LoadableSegments LiefBinaryParser::GetLoadableSegments() const {
  assert(pBinary_ != nullptr);
  switch (pBinary_->format()) {
    case LIEF::Binary::FORMATS::ELF:  {
      const auto& elf = static_cast<const LIEF::ELF::Binary&>(*pBinary_);
      return GetLoadableSegmentsLinux(elf);
    }
    case LIEF::Binary::FORMATS::PE:   {
      const auto& pe = static_cast<const LIEF::PE::Binary&>(*pBinary_);
      return GetLoadableSegmentsWindows(pe);
    }
    default:
      UNREACHABLE("Unsupported binary format");
  }
}