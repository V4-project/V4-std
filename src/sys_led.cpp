/**
 * @file sys_led.cpp
 * @brief LED SYS call implementation
 */

#include "v4std/sys_led.hpp"
#include "v4std/ddt.hpp"
#include "v4std/ddt_types.h"
#include "v4std/sys_handlers.hpp"
#include "v4std/sys_ids.h"

namespace v4std
{

// Global LED HAL pointer
static LedHal* led_hal = nullptr;

void set_led_hal(LedHal* hal)
{
  led_hal = hal;
}

// Helper: Find LED device and validate
static const v4dev_desc_t* find_led(int32_t kind, int32_t role, int32_t index)
{
  if (kind != V4DEV_LED) {
    return nullptr;
  }

  return Ddt::find_device(static_cast<v4dev_kind_t>(kind),
                           static_cast<v4dev_role_t>(role),
                           static_cast<uint8_t>(index));
}

// SYS_LED_ON handler
static int32_t sys_led_on(uint16_t sys_id,
                           int32_t arg0,
                           int32_t arg1,
                           int32_t arg2)
{
  (void)sys_id;  // Unused

  if (!led_hal) {
    return 0;  // Failure: no HAL
  }

  const v4dev_desc_t* led = find_led(arg0, arg1, arg2);
  if (!led) {
    return 0;  // Failure: device not found
  }

  bool active_low = (led->flags & V4DEV_FLAG_ACTIVE_LOW) != 0;
  bool success = led_hal->set_led(led->handle, true, active_low);

  return success ? 1 : 0;
}

// SYS_LED_OFF handler
static int32_t sys_led_off(uint16_t sys_id,
                            int32_t arg0,
                            int32_t arg1,
                            int32_t arg2)
{
  (void)sys_id;

  if (!led_hal) {
    return 0;
  }

  const v4dev_desc_t* led = find_led(arg0, arg1, arg2);
  if (!led) {
    return 0;
  }

  bool active_low = (led->flags & V4DEV_FLAG_ACTIVE_LOW) != 0;
  bool success = led_hal->set_led(led->handle, false, active_low);

  return success ? 1 : 0;
}

// SYS_LED_TOGGLE handler
static int32_t sys_led_toggle(uint16_t sys_id,
                               int32_t arg0,
                               int32_t arg1,
                               int32_t arg2)
{
  (void)sys_id;

  if (!led_hal) {
    return 0;
  }

  const v4dev_desc_t* led = find_led(arg0, arg1, arg2);
  if (!led) {
    return 0;
  }

  bool active_low = (led->flags & V4DEV_FLAG_ACTIVE_LOW) != 0;

  // Get current state and toggle
  bool current_state = led_hal->get_led(led->handle, active_low);
  bool success = led_hal->set_led(led->handle, !current_state, active_low);

  return success ? 1 : 0;
}

// SYS_LED_SET handler
static int32_t sys_led_set(uint16_t sys_id,
                            int32_t arg0,
                            int32_t arg1,
                            int32_t arg2)
{
  (void)sys_id;

  if (!led_hal) {
    return 0;
  }

  // For SET, we expect 4 arguments, but SysHandler only takes 3
  // So we use a different convention: arg2 is both index and state
  // Lower 8 bits = index, bit 8 = state
  // This is a limitation we'll document
  //
  // Actually, let's reconsider the API. Looking at the .def file:
  // V4SYS_LED_SET: "Set LED state (0=off, 1=on) by kind/role/index"
  // Stack effect should be: ( kind role index state -- success )
  //
  // But our handler signature is: (sys_id, arg0, arg1, arg2) -> result
  // That's only 3 args after sys_id!
  //
  // The V4 VM should pass 4 values on stack for LED_SET.
  // We need to handle this properly. Let me assume:
  // - arg0 = kind
  // - arg1 = role
  // - arg2 = (index << 16) | (state & 0xFFFF)
  //
  // This is a temporary solution; ideally SysHandler should take more args.

  uint8_t index = (arg2 >> 16) & 0xFF;
  bool state = (arg2 & 0xFFFF) != 0;

  const v4dev_desc_t* led = find_led(arg0, arg1, index);
  if (!led) {
    return 0;
  }

  bool active_low = (led->flags & V4DEV_FLAG_ACTIVE_LOW) != 0;
  bool success = led_hal->set_led(led->handle, state, active_low);

  return success ? 1 : 0;
}

// SYS_LED_GET handler
static int32_t sys_led_get(uint16_t sys_id,
                            int32_t arg0,
                            int32_t arg1,
                            int32_t arg2)
{
  (void)sys_id;

  if (!led_hal) {
    return 0;
  }

  const v4dev_desc_t* led = find_led(arg0, arg1, arg2);
  if (!led) {
    return 0;
  }

  bool active_low = (led->flags & V4DEV_FLAG_ACTIVE_LOW) != 0;
  bool state = led_hal->get_led(led->handle, active_low);

  return state ? 1 : 0;
}

void register_led_sys_handlers()
{
  register_sys_handler(V4SYS_LED_ON, sys_led_on);
  register_sys_handler(V4SYS_LED_OFF, sys_led_off);
  register_sys_handler(V4SYS_LED_TOGGLE, sys_led_toggle);
  register_sys_handler(V4SYS_LED_SET, sys_led_set);
  register_sys_handler(V4SYS_LED_GET, sys_led_get);
}

}  // namespace v4std
