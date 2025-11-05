/**
 * @file test_sys_led.cpp
 * @brief Tests for LED SYS call implementations
 */

#include "v4std/ddt.hpp"
#include "v4std/ddt_types.h"
#include "v4std/sys_handlers.hpp"
#include "v4std/sys_ids.h"
#include "v4std/sys_led.hpp"
#include <cassert>
#include <cstdio>
#include <unordered_map>

using namespace v4std;

// Mock LED HAL for testing
class MockLedHal : public LedHal
{
 public:
  // Track LED states (indexed by handle)
  std::unordered_map<uint32_t, bool> led_states;

  bool set_led(uint32_t handle, bool state, bool active_low) override
  {
    // Apply active-low logic: if active_low, invert the logical state
    bool physical_state = active_low ? !state : state;
    led_states[handle] = physical_state;
    return true;
  }

  bool get_led(uint32_t handle, bool active_low) override
  {
    bool physical_state = led_states[handle];
    // Apply active-low logic when reading back
    return active_low ? !physical_state : physical_state;
  }

  void clear()
  {
    led_states.clear();
  }
};

// Mock DDT provider with LED devices
class MockDdtProvider : public DdtProvider
{
 public:
  span<const v4dev_desc_t> get_devices() const override
  {
    static constexpr v4dev_desc_t devices[] = {
        // STATUS LED (GPIO7, active-high)
        {V4DEV_LED, V4ROLE_STATUS, 0, 0, 7},
        // USER LED (GPIO8, active-high)
        {V4DEV_LED, V4ROLE_USER, 0, 0, 8},
        // Second USER LED (GPIO10, active-low)
        {V4DEV_LED, V4ROLE_USER, 1, V4DEV_FLAG_ACTIVE_LOW, 10},
    };

    return span<const v4dev_desc_t>{devices, 3};
  }
};

int main()
{
  MockLedHal hal;
  MockDdtProvider provider;

  // Initialize
  set_led_hal(&hal);
  Ddt::set_provider(&provider);
  register_led_sys_handlers();

  // Test: LED_ON (active-high LED)
  {
    hal.clear();

    int32_t result =
        invoke_sys_handler(V4SYS_LED_ON, V4DEV_LED, V4ROLE_STATUS, 0);

    assert(result == 1);  // Success
    assert(hal.led_states[7] == true);  // GPIO7 should be high

    printf("✓ LED_ON works for active-high LED\n");
  }

  // Test: LED_OFF (active-high LED)
  {
    hal.clear();
    hal.led_states[7] = true;  // Start with LED on

    int32_t result =
        invoke_sys_handler(V4SYS_LED_OFF, V4DEV_LED, V4ROLE_STATUS, 0);

    assert(result == 1);
    assert(hal.led_states[7] == false);  // GPIO7 should be low

    printf("✓ LED_OFF works for active-high LED\n");
  }

  // Test: LED_ON (active-low LED)
  {
    hal.clear();

    int32_t result =
        invoke_sys_handler(V4SYS_LED_ON, V4DEV_LED, V4ROLE_USER, 1);

    assert(result == 1);
    // GPIO10 is active-low, so logical ON means physical LOW
    assert(hal.led_states[10] == false);

    printf("✓ LED_ON works for active-low LED (inverts signal)\n");
  }

  // Test: LED_OFF (active-low LED)
  {
    hal.clear();
    hal.led_states[10] = false;  // Start with LED on (physical low)

    int32_t result =
        invoke_sys_handler(V4SYS_LED_OFF, V4DEV_LED, V4ROLE_USER, 1);

    assert(result == 1);
    // Logical OFF means physical HIGH for active-low
    assert(hal.led_states[10] == true);

    printf("✓ LED_OFF works for active-low LED (inverts signal)\n");
  }

  // Test: LED_TOGGLE
  {
    hal.clear();
    hal.led_states[8] = false;  // Start OFF

    // Toggle ON
    int32_t result1 =
        invoke_sys_handler(V4SYS_LED_TOGGLE, V4DEV_LED, V4ROLE_USER, 0);
    assert(result1 == 1);
    assert(hal.led_states[8] == true);

    // Toggle OFF
    int32_t result2 =
        invoke_sys_handler(V4SYS_LED_TOGGLE, V4DEV_LED, V4ROLE_USER, 0);
    assert(result2 == 1);
    assert(hal.led_states[8] == false);

    printf("✓ LED_TOGGLE works (toggles state)\n");
  }

  // Test: LED_SET (set to ON)
  {
    hal.clear();

    // For LED_SET, we pack index and state into arg2
    // arg2 = (index << 16) | (state & 0xFFFF)
    int32_t arg2 = (0 << 16) | 1;  // index=0, state=1 (ON)

    int32_t result =
        invoke_sys_handler(V4SYS_LED_SET, V4DEV_LED, V4ROLE_STATUS, arg2);

    assert(result == 1);
    assert(hal.led_states[7] == true);

    printf("✓ LED_SET works (set to ON)\n");
  }

  // Test: LED_SET (set to OFF)
  {
    hal.clear();
    hal.led_states[7] = true;  // Start ON

    int32_t arg2 = (0 << 16) | 0;  // index=0, state=0 (OFF)

    int32_t result =
        invoke_sys_handler(V4SYS_LED_SET, V4DEV_LED, V4ROLE_STATUS, arg2);

    assert(result == 1);
    assert(hal.led_states[7] == false);

    printf("✓ LED_SET works (set to OFF)\n");
  }

  // Test: LED_GET
  {
    hal.clear();
    hal.led_states[8] = true;  // LED is ON

    int32_t result =
        invoke_sys_handler(V4SYS_LED_GET, V4DEV_LED, V4ROLE_USER, 0);

    assert(result == 1);  // Returns 1 for ON

    hal.led_states[8] = false;  // LED is OFF

    result = invoke_sys_handler(V4SYS_LED_GET, V4DEV_LED, V4ROLE_USER, 0);

    assert(result == 0);  // Returns 0 for OFF

    printf("✓ LED_GET works (returns current state)\n");
  }

  // Test: LED_GET with active-low
  {
    hal.clear();
    // GPIO10 is active-low
    // Physical LOW = Logical ON
    hal.led_states[10] = false;

    int32_t result =
        invoke_sys_handler(V4SYS_LED_GET, V4DEV_LED, V4ROLE_USER, 1);

    assert(result == 1);  // Should return 1 (logical ON)

    printf("✓ LED_GET handles active-low correctly\n");
  }

  // Test: Device not found
  {
    hal.clear();

    // Try to control non-existent LED (index 99)
    int32_t result =
        invoke_sys_handler(V4SYS_LED_ON, V4DEV_LED, V4ROLE_STATUS, 99);

    assert(result == 0);  // Failure

    printf("✓ LED operations return 0 when device not found\n");
  }

  // Test: Wrong device kind
  {
    hal.clear();

    // Try to use LED_ON with BUTTON kind
    int32_t result =
        invoke_sys_handler(V4SYS_LED_ON, V4DEV_BUTTON, V4ROLE_USER, 0);

    assert(result == 0);  // Failure

    printf("✓ LED operations reject non-LED devices\n");
  }

  // Test: No HAL configured
  {
    set_led_hal(nullptr);  // Remove HAL

    int32_t result =
        invoke_sys_handler(V4SYS_LED_ON, V4DEV_LED, V4ROLE_STATUS, 0);

    assert(result == 0);  // Failure

    // Restore HAL
    set_led_hal(&hal);

    printf("✓ LED operations fail gracefully when HAL not configured\n");
  }

  // Test: Multiple LEDs independent
  {
    hal.clear();

    // Turn on STATUS LED
    invoke_sys_handler(V4SYS_LED_ON, V4DEV_LED, V4ROLE_STATUS, 0);

    // Turn off USER LED
    invoke_sys_handler(V4SYS_LED_OFF, V4DEV_LED, V4ROLE_USER, 0);

    assert(hal.led_states[7] == true);   // STATUS ON
    assert(hal.led_states[8] == false);  // USER OFF

    printf("✓ Multiple LEDs operate independently\n");
  }

  printf("\n✅ All LED SYS tests passed!\n");
  return 0;
}
