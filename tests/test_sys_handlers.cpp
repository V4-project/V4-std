/**
 * @file test_sys_handlers.cpp
 * @brief Tests for SYS handler registration and invocation
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "v4std/sys_handlers.hpp"
#include "v4std/sys_ids.h"

using namespace v4std;

// Mock handlers for testing

static int32_t mock_led_on(uint16_t sys_id, int32_t arg0, int32_t arg1,
                           int32_t arg2) {
  (void)sys_id; // Unused
  (void)arg0;   // kind
  (void)arg1;   // role
  (void)arg2;   // index
  return 1;     // Success
}

static int32_t mock_led_off(uint16_t sys_id, int32_t arg0, int32_t arg1,
                            int32_t arg2) {
  (void)sys_id;
  (void)arg0;
  (void)arg1;
  (void)arg2;
  return 0; // Off
}

static int32_t mock_button_read(uint16_t sys_id, int32_t arg0, int32_t arg1,
                                int32_t arg2) {
  (void)sys_id;
  (void)arg0;
  (void)arg1;
  (void)arg2;
  return 1; // Pressed
}

static int32_t mock_echo_args(uint16_t sys_id, int32_t arg0, int32_t arg1,
                              int32_t arg2) {
  // Echo back arg0 for testing
  (void)sys_id;
  (void)arg1;
  (void)arg2;
  return arg0;
}

TEST_CASE("SYS Handlers: Initial state - no handlers") {
  clear_sys_handlers();

  CHECK(get_sys_handler_count() == 0);
  CHECK(get_sys_handler(V4SYS_LED_ON) == nullptr);
}

TEST_CASE("SYS Handlers: Register single handler") {
  clear_sys_handlers();

  bool result = register_sys_handler(V4SYS_LED_ON, mock_led_on);
  CHECK(result == true);
  CHECK(get_sys_handler_count() == 1);

  SysHandler handler = get_sys_handler(V4SYS_LED_ON);
  REQUIRE(handler != nullptr);
  CHECK(handler == mock_led_on);
}

TEST_CASE("SYS Handlers: Invoke registered handler") {
  clear_sys_handlers();
  register_sys_handler(V4SYS_LED_ON, mock_led_on);

  int32_t result = invoke_sys_handler(V4SYS_LED_ON, 0, 0, 0);
  CHECK(result == 1); // mock_led_on returns 1
}

TEST_CASE("SYS Handlers: Register multiple handlers") {
  clear_sys_handlers();

  register_sys_handler(V4SYS_LED_ON, mock_led_on);
  register_sys_handler(V4SYS_LED_OFF, mock_led_off);
  register_sys_handler(V4SYS_BUTTON_READ, mock_button_read);

  CHECK(get_sys_handler_count() == 3);
  CHECK(get_sys_handler(V4SYS_LED_ON) == mock_led_on);
  CHECK(get_sys_handler(V4SYS_LED_OFF) == mock_led_off);
  CHECK(get_sys_handler(V4SYS_BUTTON_READ) == mock_button_read);
}

TEST_CASE("SYS Handlers: Invoke different handlers") {
  clear_sys_handlers();

  register_sys_handler(V4SYS_LED_ON, mock_led_on);
  register_sys_handler(V4SYS_LED_OFF, mock_led_off);
  register_sys_handler(V4SYS_BUTTON_READ, mock_button_read);

  int32_t result_on = invoke_sys_handler(V4SYS_LED_ON, 0, 0, 0);
  int32_t result_off = invoke_sys_handler(V4SYS_LED_OFF, 0, 0, 0);
  int32_t result_btn = invoke_sys_handler(V4SYS_BUTTON_READ, 0, 0, 0);

  CHECK(result_on == 1);
  CHECK(result_off == 0);
  CHECK(result_btn == 1);
}

TEST_CASE("SYS Handlers: Invoke unregistered handler") {
  clear_sys_handlers();

  int32_t result = invoke_sys_handler(V4SYS_TIMER_START, 0, 0, 0);
  CHECK(result == -1); // Error: no handler
}

TEST_CASE("SYS Handlers: Replace existing handler") {
  clear_sys_handlers();

  register_sys_handler(V4SYS_LED_ON, mock_led_on);
  size_t count_before = get_sys_handler_count();

  // Replace LED_ON handler with echo handler
  register_sys_handler(V4SYS_LED_ON, mock_echo_args);

  CHECK(get_sys_handler_count() == count_before); // Count unchanged
  CHECK(get_sys_handler(V4SYS_LED_ON) == mock_echo_args);

  int32_t result = invoke_sys_handler(V4SYS_LED_ON, 42, 0, 0);
  CHECK(result == 42); // mock_echo_args returns arg0
}

TEST_CASE("SYS Handlers: Unregister handler") {
  clear_sys_handlers();

  register_sys_handler(V4SYS_LED_ON, mock_led_on);
  register_sys_handler(V4SYS_LED_OFF, mock_led_off);
  register_sys_handler(V4SYS_BUTTON_READ, mock_button_read);

  unregister_sys_handler(V4SYS_LED_ON);

  CHECK(get_sys_handler(V4SYS_LED_ON) == nullptr);
  CHECK(get_sys_handler_count() == 2); // LED_OFF and BUTTON_READ remain

  int32_t result = invoke_sys_handler(V4SYS_LED_ON, 0, 0, 0);
  CHECK(result == -1); // No longer registered
}

TEST_CASE("SYS Handlers: Unregister non-existent handler (no-op)") {
  clear_sys_handlers();

  register_sys_handler(V4SYS_LED_ON, mock_led_on);
  size_t count_before = get_sys_handler_count();

  unregister_sys_handler(V4SYS_TIMER_START); // Never registered
  CHECK(get_sys_handler_count() == count_before);
}

TEST_CASE("SYS Handlers: Register null handler fails") {
  clear_sys_handlers();

  bool result = register_sys_handler(V4SYS_TIMER_START, nullptr);
  CHECK(result == false);
  CHECK(get_sys_handler(V4SYS_TIMER_START) == nullptr);
}

TEST_CASE("SYS Handlers: Handler argument passing") {
  clear_sys_handlers();

  register_sys_handler(V4SYS_CAP_COUNT, mock_echo_args);

  int32_t result1 = invoke_sys_handler(V4SYS_CAP_COUNT, 123, 456, 789);
  CHECK(result1 == 123); // Returns arg0
}

TEST_CASE("SYS Handlers: Clear all handlers") {
  clear_sys_handlers();

  register_sys_handler(V4SYS_LED_ON, mock_led_on);
  register_sys_handler(V4SYS_LED_OFF, mock_led_off);
  register_sys_handler(V4SYS_BUTTON_READ, mock_button_read);

  clear_sys_handlers();

  CHECK(get_sys_handler_count() == 0);
  CHECK(get_sys_handler(V4SYS_LED_OFF) == nullptr);
  CHECK(get_sys_handler(V4SYS_BUTTON_READ) == nullptr);
}

TEST_CASE("SYS Handlers: Register many handlers") {
  clear_sys_handlers();

  register_sys_handler(V4SYS_LED_ON, mock_led_on);
  register_sys_handler(V4SYS_LED_OFF, mock_led_off);
  register_sys_handler(V4SYS_LED_TOGGLE, mock_echo_args);
  register_sys_handler(V4SYS_BUTTON_READ, mock_button_read);
  register_sys_handler(V4SYS_TIMER_START, mock_echo_args);
  register_sys_handler(V4SYS_UART_READ, mock_echo_args);
  register_sys_handler(V4SYS_CAP_COUNT, mock_echo_args);

  CHECK(get_sys_handler_count() == 7);

  // Verify all are accessible
  CHECK(get_sys_handler(V4SYS_LED_ON) != nullptr);
  CHECK(get_sys_handler(V4SYS_LED_OFF) != nullptr);
  CHECK(get_sys_handler(V4SYS_LED_TOGGLE) != nullptr);
  CHECK(get_sys_handler(V4SYS_BUTTON_READ) != nullptr);
  CHECK(get_sys_handler(V4SYS_TIMER_START) != nullptr);
  CHECK(get_sys_handler(V4SYS_UART_READ) != nullptr);
  CHECK(get_sys_handler(V4SYS_CAP_COUNT) != nullptr);
}
