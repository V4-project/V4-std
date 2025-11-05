/**
 * @file test_sys_handlers.cpp
 * @brief Tests for SYS handler registration and invocation
 */

#include "v4std/sys_handlers.hpp"
#include "v4std/sys_ids.h"
#include <cassert>
#include <cstdio>

using namespace v4std;

// Mock handlers for testing

static int32_t mock_led_on(uint16_t sys_id, int32_t arg0, int32_t arg1, int32_t arg2)
{
  (void)sys_id;  // Unused
  (void)arg0;    // kind
  (void)arg1;    // role
  (void)arg2;    // index
  return 1;      // Success
}

static int32_t mock_led_off(uint16_t sys_id, int32_t arg0, int32_t arg1, int32_t arg2)
{
  (void)sys_id;
  (void)arg0;
  (void)arg1;
  (void)arg2;
  return 0;  // Off
}

static int32_t mock_button_read(uint16_t sys_id,
                                 int32_t arg0,
                                 int32_t arg1,
                                 int32_t arg2)
{
  (void)sys_id;
  (void)arg0;
  (void)arg1;
  (void)arg2;
  return 1;  // Pressed
}

static int32_t mock_echo_args(uint16_t sys_id,
                               int32_t arg0,
                               int32_t arg1,
                               int32_t arg2)
{
  // Echo back arg0 for testing
  (void)sys_id;
  (void)arg1;
  (void)arg2;
  return arg0;
}

int main()
{
  // Test: Initial state - no handlers
  {
    assert(get_sys_handler_count() == 0);
    assert(get_sys_handler(V4SYS_LED_ON) == nullptr);
    printf("✓ Initial state: no handlers registered\n");
  }

  // Test: Register single handler
  {
    bool result = register_sys_handler(V4SYS_LED_ON, mock_led_on);
    assert(result == true);
    assert(get_sys_handler_count() == 1);

    SysHandler handler = get_sys_handler(V4SYS_LED_ON);
    assert(handler != nullptr);
    assert(handler == mock_led_on);

    printf("✓ register_sys_handler() registers handler\n");
  }

  // Test: Invoke registered handler
  {
    int32_t result = invoke_sys_handler(V4SYS_LED_ON, 0, 0, 0);
    assert(result == 1);  // mock_led_on returns 1
    printf("✓ invoke_sys_handler() calls registered handler\n");
  }

  // Test: Register multiple handlers
  {
    register_sys_handler(V4SYS_LED_OFF, mock_led_off);
    register_sys_handler(V4SYS_BUTTON_READ, mock_button_read);

    assert(get_sys_handler_count() == 3);

    assert(get_sys_handler(V4SYS_LED_ON) == mock_led_on);
    assert(get_sys_handler(V4SYS_LED_OFF) == mock_led_off);
    assert(get_sys_handler(V4SYS_BUTTON_READ) == mock_button_read);

    printf("✓ Multiple handlers can be registered\n");
  }

  // Test: Invoke different handlers
  {
    int32_t result_on = invoke_sys_handler(V4SYS_LED_ON, 0, 0, 0);
    int32_t result_off = invoke_sys_handler(V4SYS_LED_OFF, 0, 0, 0);
    int32_t result_btn = invoke_sys_handler(V4SYS_BUTTON_READ, 0, 0, 0);

    assert(result_on == 1);
    assert(result_off == 0);
    assert(result_btn == 1);

    printf("✓ Different handlers return different results\n");
  }

  // Test: Invoke unregistered handler
  {
    int32_t result = invoke_sys_handler(V4SYS_TIMER_START, 0, 0, 0);
    assert(result == -1);  // Error: no handler
    printf("✓ invoke_sys_handler() returns -1 for unregistered SYS ID\n");
  }

  // Test: Replace existing handler
  {
    size_t count_before = get_sys_handler_count();

    // Replace LED_ON handler with echo handler
    register_sys_handler(V4SYS_LED_ON, mock_echo_args);

    assert(get_sys_handler_count() == count_before);  // Count unchanged
    assert(get_sys_handler(V4SYS_LED_ON) == mock_echo_args);

    int32_t result = invoke_sys_handler(V4SYS_LED_ON, 42, 0, 0);
    assert(result == 42);  // mock_echo_args returns arg0

    printf("✓ Existing handler can be replaced\n");
  }

  // Test: Unregister handler
  {
    unregister_sys_handler(V4SYS_LED_ON);

    assert(get_sys_handler(V4SYS_LED_ON) == nullptr);
    assert(get_sys_handler_count() == 2);  // LED_OFF and BUTTON_READ remain

    int32_t result = invoke_sys_handler(V4SYS_LED_ON, 0, 0, 0);
    assert(result == -1);  // No longer registered

    printf("✓ unregister_sys_handler() removes handler\n");
  }

  // Test: Unregister non-existent handler (no-op)
  {
    size_t count_before = get_sys_handler_count();
    unregister_sys_handler(V4SYS_TIMER_START);  // Never registered
    assert(get_sys_handler_count() == count_before);

    printf("✓ Unregistering non-existent handler is a no-op\n");
  }

  // Test: Register null handler fails
  {
    bool result = register_sys_handler(V4SYS_TIMER_START, nullptr);
    assert(result == false);
    assert(get_sys_handler(V4SYS_TIMER_START) == nullptr);

    printf("✓ register_sys_handler() rejects null handler\n");
  }

  // Test: Handler argument passing
  {
    register_sys_handler(V4SYS_CAP_COUNT, mock_echo_args);

    int32_t result1 = invoke_sys_handler(V4SYS_CAP_COUNT, 123, 456, 789);
    assert(result1 == 123);  // Returns arg0

    printf("✓ Handler receives correct arguments\n");
  }

  // Test: Clear all handlers
  {
    clear_sys_handlers();

    assert(get_sys_handler_count() == 0);
    assert(get_sys_handler(V4SYS_LED_OFF) == nullptr);
    assert(get_sys_handler(V4SYS_BUTTON_READ) == nullptr);
    assert(get_sys_handler(V4SYS_CAP_COUNT) == nullptr);

    printf("✓ clear_sys_handlers() removes all handlers\n");
  }

  // Test: Register many handlers
  {
    clear_sys_handlers();

    register_sys_handler(V4SYS_LED_ON, mock_led_on);
    register_sys_handler(V4SYS_LED_OFF, mock_led_off);
    register_sys_handler(V4SYS_LED_TOGGLE, mock_echo_args);
    register_sys_handler(V4SYS_BUTTON_READ, mock_button_read);
    register_sys_handler(V4SYS_TIMER_START, mock_echo_args);
    register_sys_handler(V4SYS_UART_READ, mock_echo_args);
    register_sys_handler(V4SYS_CAP_COUNT, mock_echo_args);

    assert(get_sys_handler_count() == 7);

    // Verify all are accessible
    assert(get_sys_handler(V4SYS_LED_ON) != nullptr);
    assert(get_sys_handler(V4SYS_LED_OFF) != nullptr);
    assert(get_sys_handler(V4SYS_LED_TOGGLE) != nullptr);
    assert(get_sys_handler(V4SYS_BUTTON_READ) != nullptr);
    assert(get_sys_handler(V4SYS_TIMER_START) != nullptr);
    assert(get_sys_handler(V4SYS_UART_READ) != nullptr);
    assert(get_sys_handler(V4SYS_CAP_COUNT) != nullptr);

    printf("✓ Multiple handlers can coexist\n");
  }

  // Cleanup
  clear_sys_handlers();

  printf("\n✅ All SYS handler tests passed!\n");
  return 0;
}
