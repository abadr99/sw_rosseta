/**
 * @file BinaryLoader.hpp
 * @brief Loads and parses x86-64 binaries (ELF, PE) via LIEF.
 *
*/

#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <LIEF/LIEF.hpp>

#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace loader {

struct Perm {
  utils::Flag R : 1;
  utils::Flag W : 1;
  utils::Flag X : 1;
};

enum class Architecture : uint8_t {
  kX86_64,
  kRiscV64,
  kUnknown,
};

struct LoadableSegment {
  LoadableSegment(utils::Address va,
                  utils::Size siz,
                  const Perm& perm,
                  const utils::Bytes& data)
  : VirtualAddress(va)
  , Size(siz)
  , Permissions(perm)
  , Data(data)
  { /* EMPTY */ }
  utils::Address VirtualAddress;
  utils::Size Size;
  Perm Permissions;
  utils::Bytes Data;
};

struct BinarySection {
 public:
  BinarySection(const std::string& name, utils::Address va, const utils::Bytes& data)
  : Name(name)
  , VirtualAddress(va)
  , Data(data)
  { /* EMPTY */ }
  std::string Name;
  utils::Address VirtualAddress;
  utils::Bytes Data;
};

// Represents a section of a binary
// Interface for binary parsers
class IBinaryParser {
 public:
  using LoadableSegments = std::vector<LoadableSegment>;
  explicit IBinaryParser(const std::filesystem::path& filepath);
  virtual ~IBinaryParser();
  // returns the executable code section (.text)
  virtual BinarySection GetExecutableCode() const = 0;
  virtual Architecture GetArchitecture() const = 0;
  virtual uint64_t GetEntryPoint() const = 0;
  virtual LoadableSegments GetLoadableSegments() const = 0;
 protected:
  std::filesystem::path filepath_;
};


}  // namespace loader
}  // namespace frontend
}  // namespace rosetta
