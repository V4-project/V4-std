/**
 * @file ddt.hpp
 * @brief Device Descriptor Table (DDT) C++ API
 *
 * Provides a type-safe C++17 interface for device enumeration and search.
 * Platform-specific implementations provide DdtProvider subclasses.
 *
 * @copyright Copyright 2025 V4 Project
 * @license Dual-licensed under MIT or Apache-2.0
 */

#ifndef V4STD_DDT_HPP
#define V4STD_DDT_HPP

#include "v4std/ddt_types.h"
#include "v4std/span.hpp"
#include <cstddef>

namespace v4std {

/**
 * @brief DDT provider interface
 *
 * Platform-specific implementations inherit from this interface
 * and return their device table via get_devices().
 *
 * Example:
 * @code
 * class Esp32c6DdtProvider : public DdtProvider {
 * public:
 *   std::span<const v4dev_desc_t> get_devices() const override {
 *     static constexpr v4dev_desc_t devices[] = { ... };
 *     return std::span{devices};
 *   }
 * };
 * @endcode
 */
class DdtProvider {
public:
  virtual ~DdtProvider() = default;

  /**
   * @brief Get device table
   * @return Span of device descriptors
   */
  virtual span<const v4dev_desc_t> get_devices() const = 0;
};

/**
 * @brief DDT search and management
 *
 * Static class providing device search and enumeration.
 * Thread-safe for const operations after set_provider().
 */
class Ddt {
public:
  /**
   * @brief Set DDT provider
   *
   * Must be called once during initialization before any queries.
   * The provider pointer must remain valid for the lifetime of the program.
   *
   * @param provider Pointer to platform-specific provider (must not be null)
   */
  static void set_provider(DdtProvider *provider);

  /**
   * @brief Find device by kind, role, and index
   *
   * Searches for a device matching all three criteria.
   *
   * @param kind Device kind (e.g., V4DEV_LED)
   * @param role Device role (e.g., V4ROLE_STATUS)
   * @param index Index within kind/role combination (0-based)
   * @return Pointer to descriptor if found, nullptr otherwise
   */
  static const v4dev_desc_t *find_device(v4dev_kind_t kind, v4dev_role_t role,
                                         uint8_t index);

  /**
   * @brief Find default device (index 0)
   *
   * Convenience function for finding the first device of a given kind/role.
   *
   * @param kind Device kind
   * @param role Device role
   * @return Pointer to descriptor if found, nullptr otherwise
   */
  static const v4dev_desc_t *find_default_device(v4dev_kind_t kind,
                                                 v4dev_role_t role);

  /**
   * @brief Count devices of a given kind
   *
   * @param kind Device kind to count
   * @return Number of devices of that kind
   */
  static size_t count_devices(v4dev_kind_t kind);

  /**
   * @brief Get all devices
   *
   * @return Span of all device descriptors
   */
  static span<const v4dev_desc_t> get_all_devices();

private:
  static DdtProvider *provider_;
};

} // namespace v4std

#endif // V4STD_DDT_HPP
