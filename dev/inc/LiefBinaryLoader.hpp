#pragma once

#include <LIEF/LIEF.hpp>
#include <LIEF/ELF.hpp>
#include <LIEF/PE.hpp>

#include "BinaryLoaderInterface.hpp"

namespace rosetta {
namespace frontend {
namespace loader {
// Class that parses binaries using LIEF
class LiefBinaryParser : public IBinaryParser {
 public:
  explicit LiefBinaryParser(const std::filesystem::path& filepath);
  ~LiefBinaryParser() = default;
  BinarySection GetExecutableCode() const override;
  Architecture GetArchitecture() const override;
  uint64_t GetEntryPoint() const override;
  LoadableSegments GetLoadableSegments() const override;
 private:
  std::unique_ptr<LIEF::Binary> pBinary_;
  BinarySection GetExecutableCodeLinux(const LIEF::ELF::Binary& elf) const;
  BinarySection GetExecutableCodeWindows(const LIEF::PE::Binary& pe) const;
  LoadableSegments GetLoadableSegmentsLinux(const LIEF::ELF::Binary& elf) const;
  LoadableSegments GetLoadableSegmentsWindows(const LIEF::PE::Binary& pe) const;
};

}}} // namespace rosetta::frontend::loader