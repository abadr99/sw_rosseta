/**
 * @file BinaryLoader.hpp
 * @brief Loads and parses x86-64 binaries (ELF, PE) via LIEF.
 *
 * Provides BinaryLoader, a non-throwing wrapper around LIEF's abstract
 * binary layer. Extracts the entry point and allocatable sections needed
 * by the DBT frontend before decoding begins.
 *
 * @author a.atta
 * @date 2026-08-20
 */

#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <LIEF/LIEF.hpp>

#include "utils/types.h"

namespace rosetta {
namespace frontend {
namespace loader {

struct Permissions {
  utils::Flag is_readable : 1;
  utils::Flag is_writable : 1;
  utils::Flag is_executable : 1;
};

enum class Architecture : uint8_t {
  kX86_64,
  kRiscV64,
  kUnknown,
};

struct LoadableSegment {
  utils::Address virtual_address;
  utils::Size memory_size;
  Permissions permissions;
  utils::Bytes data;
};

struct BinarySection {
  public:
    BinarySection(const std::string& name, utils::Address virtual_address, const utils::Bytes& data);
    std::string GetName() const;
    utils::Address GetVirtualAddress() const;
    utils::Bytes GetContent() const;
  private:
    std::string name_;
    utils::Address virtual_address_;
    utils::Bytes data_;

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
};

}  // namespace loader
}  // namespace frontend
}  // namespace rosetta
