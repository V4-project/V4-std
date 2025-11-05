/**
 * @file sys_led.hpp
 * @brief LED SYS call implementations
 *
 * Provides platform-independent LED control through DDT.
 * Platform HAL implementations provide the actual hardware control.
 *
 * @copyright Copyright 2025 V4 Project
 * @license Dual-licensed under MIT or Apache-2.0
 */

#ifndef V4STD_SYS_LED_HPP
#define V4STD_SYS_LED_HPP

#include <cstdint>

namespace v4std
{

/**
 * @brief LED HAL interface
 *
 * Platform implementations provide LED control functions.
 * These are called by the SYS handlers after DDT lookup.
 */
class LedHal
{
 public:
  virtual ~LedHal() = default;

  /**
   * @brief Set LED state
   *
   * @param handle Platform-specific handle (e.g., GPIO number)
   * @param state true = on, false = off
   * @param active_low true if LED is active-low
   * @return true on success, false on error
   */
  virtual bool set_led(uint32_t handle, bool state, bool active_low) = 0;

  /**
   * @brief Get LED state
   *
   * @param handle Platform-specific handle
   * @param active_low true if LED is active-low
   * @return LED state (true = on, false = off)
   */
  virtual bool get_led(uint32_t handle, bool active_low) = 0;
};

/**
 * @brief Set LED HAL implementation
 *
 * Must be called during initialization before any LED operations.
 * The HAL pointer must remain valid for the program lifetime.
 *
 * @param hal Pointer to platform-specific LED HAL
 */
void set_led_hal(LedHal* hal);

/**
 * @brief Register LED SYS call handlers
 *
 * Registers handlers for:
 * - V4SYS_LED_ON
 * - V4SYS_LED_OFF
 * - V4SYS_LED_TOGGLE
 * - V4SYS_LED_SET
 * - V4SYS_LED_GET
 *
 * Must be called after set_led_hal() and Ddt::set_provider().
 */
void register_led_sys_handlers();

}  // namespace v4std

#endif  // V4STD_SYS_LED_HPP
