/**
 * @file binary_loader.hpp
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

#include <cstdint>
#include <filesystem>
#include <string_view>
#include <string>
#include <vector>
#include <optional>
#include <functional>

namespace loader {

/**
 * @brief A single loaded, allocatable section of a binary.
 */
struct Section {
  std::string name;           ///< Section name (e.g. ".text").
  uint64_t vma;               ///< Virtual memory address (base).
  uint64_t size;              ///< Size in bytes.
  std::vector<uint8_t> data;  ///< Raw section bytes.
};

/**
 * @brief Loads and parses a binary file (ELF, PE) via LIEF's abstract layer.
 *
 * Always targets 64-bit x86-64. Creation never throws; check the returned
 * std::optional afterward to confirm the load succeeded before using any
 * other member function.
 */
class BinaryLoader {
 public:
  /**
   * @brief Parses the binary at filepath and populates entry point/sections.
   * @param filepath Path to the binary to load.
   *
   * Does not throw. On failure, returns std::nullopt; entry_point_ and
   * sections_ are not instantiated.
   */
  static std::optional<BinaryLoader>
  create(const std::filesystem::path& filepath) noexcept;

  /// @brief Destructor.
  ~BinaryLoader() noexcept;

  /// @brief Copying is disabled to enforce unique ownership.
  BinaryLoader(const BinaryLoader&) = delete;
  BinaryLoader& operator=(const BinaryLoader&) = delete;

  /// @brief Transfers ownership of loaded state to a new instance.
  BinaryLoader(BinaryLoader&& other) noexcept;

  /// @brief Transfers ownership of loaded state, releasing any prior state.
  BinaryLoader& operator=(BinaryLoader&& other) noexcept;

  /**
   * @brief Looks up a loaded section by name.
   * @param name Section name (e.g. ".text", ".data").
   * @return Optional reference to the matching Section, or nullopt if not found.
   */
  [[nodiscard]] std::optional<std::reference_wrapper<const Section>>
    get_section(std::string_view name) const noexcept;

  /**
   * @brief Resolves a virtual memory address to an offset within its section.
   * @param vma Virtual address to resolve.
   * @return Offset from the containing section's base VMA.
   *
   * On failure (vma not inside any loaded section), sets status() to
   * nullopt and returns nullopt.
   */
  [[nodiscard]] std::optional<uint64_t>
  vma_to_offset(uint64_t vma) const noexcept;

  /// @brief Returns the binary's entry point virtual address.
  [[nodiscard]] uint64_t get_entry_point() const noexcept
  { return entry_point_; }

 private:
  explicit BinaryLoader() noexcept = default;

  uint64_t entry_point_{0};         ///< Entry point VMA.
  std::vector<Section> sections_;   ///< Loaded, allocatable sections.
};

}  // namespace loader
