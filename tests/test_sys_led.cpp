/**
 * @file test_sys_led.cpp
 * @brief Tests for LED SYS call implementations
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "v4std/ddt.hpp"
#include "v4std/ddt_types.h"
#include "v4std/sys_handlers.hpp"
#include "v4std/sys_ids.h"
#include "v4std/sys_led.hpp"
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

// Global instances for tests
static MockLedHal g_hal;
static MockDdtProvider g_provider;

TEST_CASE("LED SYS: LED_ON (active-high LED)")
{
  g_hal.clear();
  set_led_hal(&g_hal);
  Ddt::set_provider(&g_provider);
  register_led_sys_handlers();

  int32_t result =
      invoke_sys_handler(V4SYS_LED_ON, V4DEV_LED, V4ROLE_STATUS, 0);

  CHECK(result == 1);  // Success
  CHECK(g_hal.led_states[7] == true);  // GPIO7 should be high
}

TEST_CASE("LED SYS: LED_OFF (active-high LED)")
{
  g_hal.clear();
  set_led_hal(&g_hal);
  Ddt::set_provider(&g_provider);
  register_led_sys_handlers();

  g_hal.led_states[7] = true;  // Start with LED on

  int32_t result =
      invoke_sys_handler(V4SYS_LED_OFF, V4DEV_LED, V4ROLE_STATUS, 0);

  CHECK(result == 1);
  CHECK(g_hal.led_states[7] == false);  // GPIO7 should be low
}

TEST_CASE("LED SYS: LED_ON (active-low LED)")
{
  g_hal.clear();
  set_led_hal(&g_hal);
  Ddt::set_provider(&g_provider);
  register_led_sys_handlers();

  int32_t result =
      invoke_sys_handler(V4SYS_LED_ON, V4DEV_LED, V4ROLE_USER, 1);

  CHECK(result == 1);
  // GPIO10 is active-low, so logical ON means physical LOW
  CHECK(g_hal.led_states[10] == false);
}

TEST_CASE("LED SYS: LED_OFF (active-low LED)")
{
  g_hal.clear();
  set_led_hal(&g_hal);
  Ddt::set_provider(&g_provider);
  register_led_sys_handlers();

  g_hal.led_states[10] = false;  // Start with LED on (physical low)

  int32_t result =
      invoke_sys_handler(V4SYS_LED_OFF, V4DEV_LED, V4ROLE_USER, 1);

  CHECK(result == 1);
  // Logical OFF means physical HIGH for active-low
  CHECK(g_hal.led_states[10] == true);
}

TEST_CASE("LED SYS: LED_TOGGLE")
{
  g_hal.clear();
  set_led_hal(&g_hal);
  Ddt::set_provider(&g_provider);
  register_led_sys_handlers();

  g_hal.led_states[8] = false;  // Start OFF

  // Toggle ON
  int32_t result1 =
      invoke_sys_handler(V4SYS_LED_TOGGLE, V4DEV_LED, V4ROLE_USER, 0);
  CHECK(result1 == 1);
  CHECK(g_hal.led_states[8] == true);

  // Toggle OFF
  int32_t result2 =
      invoke_sys_handler(V4SYS_LED_TOGGLE, V4DEV_LED, V4ROLE_USER, 0);
  CHECK(result2 == 1);
  CHECK(g_hal.led_states[8] == false);
}

TEST_CASE("LED SYS: LED_SET (set to ON)")
{
  g_hal.clear();
  set_led_hal(&g_hal);
  Ddt::set_provider(&g_provider);
  register_led_sys_handlers();

  // For LED_SET, we pack index and state into arg2
  // arg2 = (index << 16) | (state & 0xFFFF)
  int32_t arg2 = (0 << 16) | 1;  // index=0, state=1 (ON)

  int32_t result =
      invoke_sys_handler(V4SYS_LED_SET, V4DEV_LED, V4ROLE_STATUS, arg2);

  CHECK(result == 1);
  CHECK(g_hal.led_states[7] == true);
}

TEST_CASE("LED SYS: LED_SET (set to OFF)")
{
  g_hal.clear();
  set_led_hal(&g_hal);
  Ddt::set_provider(&g_provider);
  register_led_sys_handlers();

  g_hal.led_states[7] = true;  // Start ON

  int32_t arg2 = (0 << 16) | 0;  // index=0, state=0 (OFF)

  int32_t result =
      invoke_sys_handler(V4SYS_LED_SET, V4DEV_LED, V4ROLE_STATUS, arg2);

  CHECK(result == 1);
  CHECK(g_hal.led_states[7] == false);
}

TEST_CASE("LED SYS: LED_GET")
{
  g_hal.clear();
  set_led_hal(&g_hal);
  Ddt::set_provider(&g_provider);
  register_led_sys_handlers();

  g_hal.led_states[8] = true;  // LED is ON

  int32_t result =
      invoke_sys_handler(V4SYS_LED_GET, V4DEV_LED, V4ROLE_USER, 0);

  CHECK(result == 1);  // Returns 1 for ON

  g_hal.led_states[8] = false;  // LED is OFF

  result = invoke_sys_handler(V4SYS_LED_GET, V4DEV_LED, V4ROLE_USER, 0);

  CHECK(result == 0);  // Returns 0 for OFF
}

TEST_CASE("LED SYS: LED_GET with active-low")
{
  g_hal.clear();
  set_led_hal(&g_hal);
  Ddt::set_provider(&g_provider);
  register_led_sys_handlers();

  // GPIO10 is active-low
  // Physical LOW = Logical ON
  g_hal.led_states[10] = false;

  int32_t result =
      invoke_sys_handler(V4SYS_LED_GET, V4DEV_LED, V4ROLE_USER, 1);

  CHECK(result == 1);  // Should return 1 (logical ON)
}

TEST_CASE("LED SYS: Device not found")
{
  g_hal.clear();
  set_led_hal(&g_hal);
  Ddt::set_provider(&g_provider);
  register_led_sys_handlers();

  // Try to control non-existent LED (index 99)
  int32_t result =
      invoke_sys_handler(V4SYS_LED_ON, V4DEV_LED, V4ROLE_STATUS, 99);

  CHECK(result == 0);  // Failure
}

TEST_CASE("LED SYS: Wrong device kind")
{
  g_hal.clear();
  set_led_hal(&g_hal);
  Ddt::set_provider(&g_provider);
  register_led_sys_handlers();

  // Try to use LED_ON with BUTTON kind
  int32_t result =
      invoke_sys_handler(V4SYS_LED_ON, V4DEV_BUTTON, V4ROLE_USER, 0);

  CHECK(result == 0);  // Failure
}

TEST_CASE("LED SYS: No HAL configured")
{
  g_hal.clear();
  Ddt::set_provider(&g_provider);
  register_led_sys_handlers();

  set_led_hal(nullptr);  // Remove HAL

  int32_t result =
      invoke_sys_handler(V4SYS_LED_ON, V4DEV_LED, V4ROLE_STATUS, 0);

  CHECK(result == 0);  // Failure

  // Restore HAL
  set_led_hal(&g_hal);
}

TEST_CASE("LED SYS: Multiple LEDs independent")
{
  g_hal.clear();
  set_led_hal(&g_hal);
  Ddt::set_provider(&g_provider);
  register_led_sys_handlers();

  // Turn on STATUS LED
  invoke_sys_handler(V4SYS_LED_ON, V4DEV_LED, V4ROLE_STATUS, 0);

  // Turn off USER LED
  invoke_sys_handler(V4SYS_LED_OFF, V4DEV_LED, V4ROLE_USER, 0);

  CHECK(g_hal.led_states[7] == true);   // STATUS ON
  CHECK(g_hal.led_states[8] == false);  // USER OFF
}
