/**
 * @file BinaryLoader.cpp
 * @brief Implementation of BinaryLoader.
 *
 * Parses the input file with LIEF's format-agnostic parser, verifies the
 * target is 64-bit x86-64 (checking the ELF- or PE-specific machine field
 * depending on binary->format()), then copies the entry point and every
 * allocatable section into host memory.
 *
 * @author a.atta
 * @date 2026-08-20
*/

#include <memory>
#include <vector>
#include <utility>
#include <string>

#include <LIEF/LIEF.hpp>
#include "frontend/BinaryLoader.hpp"

using rosetta::frontend::loader::BinaryLoader;
using rosetta::frontend::loader::Section;

std::optional<BinaryLoader>
BinaryLoader::create(const std::filesystem::path& filepath) noexcept {
  std::error_code ec;
  if (!std::filesystem::exists(filepath, ec) || ec) {
    return std::nullopt;
  }

  std::unique_ptr<LIEF::Binary> binary = LIEF::Parser::parse(filepath.string());
  if (binary == nullptr) {
    return std::nullopt;
  }

  if (binary->header().architecture() != LIEF::Header::ARCHITECTURES::X86_64) {
    return std::nullopt;
  }

  BinaryLoader loader;
  loader.entry_point_ = binary->entrypoint();

  for (const LIEF::Section& section : binary->sections()) {
    const auto& content = section.content();
    loader.sections_.push_back(Section{
        section.name(),
        section.virtual_address(),
        section.size(),
        std::vector<uint8_t>(content.begin(), content.end()),
    });
  }

  bool has_text = false;
  for (const auto& section : loader.sections_) {
    if (section.name == ".text") {
      has_text = true;
      break;
    }
  }
  if (!has_text) {
    return std::nullopt;
  }

  return loader;
}

BinaryLoader::~BinaryLoader() noexcept = default;

BinaryLoader::BinaryLoader(BinaryLoader&& other) noexcept
    : entry_point_(other.entry_point_)
    , sections_(std::move(other.sections_)) {
  other.entry_point_ = 0;
}

BinaryLoader& BinaryLoader::operator=(BinaryLoader&& other) noexcept {
  if (this != &other) {
    entry_point_ = other.entry_point_;
    sections_ = std::move(other.sections_);

    other.entry_point_ = 0;
  }
  return *this;
}

std::optional<std::reference_wrapper<const Section>>
BinaryLoader::get_section(std::string_view name) const noexcept {
  for (const auto& section : sections_) {
    if (section.name == name) {
      return std::cref(section);
    }
  }
  return std::nullopt;
}

std::optional<uint64_t>
BinaryLoader::vma_to_offset(uint64_t vma) const noexcept {
  for (const auto& section : sections_) {
    if (vma >= section.vma && vma < section.vma + section.size) {
      return vma - section.vma;
    }
  }
  return std::nullopt;
}
