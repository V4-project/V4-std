/**
 * @file test_ddt.cpp
 * @brief Tests for DDT search and management
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "v4std/ddt.hpp"

using namespace v4std;

// Mock DDT provider for testing
class MockDdtProvider : public DdtProvider {
public:
  span<const v4dev_desc_t> get_devices() const override {
    static constexpr v4dev_desc_t devices[] = {
        // STATUS LED (GPIO7, active-high)
        {V4DEV_LED, V4ROLE_STATUS, 0, 0, 7},
        // USER LED (GPIO8, active-high)
        {V4DEV_LED, V4ROLE_USER, 0, 0, 8},
        // Second USER LED (GPIO10, active-low)
        {V4DEV_LED, V4ROLE_USER, 1, V4DEV_FLAG_ACTIVE_LOW, 10},
        // USER BUTTON (GPIO9, active-low)
        {V4DEV_BUTTON, V4ROLE_USER, 0, V4DEV_FLAG_ACTIVE_LOW, 9},
        // CONSOLE UART (UART0)
        {V4DEV_UART, V4ROLE_CONSOLE, 0, 0, 0},
        // TIMER
        {V4DEV_TIMER, V4ROLE_STATUS, 0, 0, 0},
    };

    return span<const v4dev_desc_t>{devices, 6};
  }
};

// Global provider instance for tests
static MockDdtProvider g_provider;

TEST_CASE("DDT: get_all_devices") {
  Ddt::set_provider(&g_provider);

  auto devices = Ddt::get_all_devices();
  CHECK(devices.size() == 6);
}

TEST_CASE("DDT: find_device - exact match") {
  Ddt::set_provider(&g_provider);

  auto *led = Ddt::find_device(V4DEV_LED, V4ROLE_STATUS, 0);
  REQUIRE(led != nullptr);
  CHECK(led->kind == V4DEV_LED);
  CHECK(led->role == V4ROLE_STATUS);
  CHECK(led->index == 0);
  CHECK(led->handle == 7);
}

TEST_CASE("DDT: find_device - different index") {
  Ddt::set_provider(&g_provider);

  auto *led = Ddt::find_device(V4DEV_LED, V4ROLE_USER, 1);
  REQUIRE(led != nullptr);
  CHECK(led->index == 1);
  CHECK(led->handle == 10);
  CHECK((led->flags & V4DEV_FLAG_ACTIVE_LOW) != 0);
}

TEST_CASE("DDT: find_device - not found") {
  Ddt::set_provider(&g_provider);

  auto *led = Ddt::find_device(V4DEV_LED, V4ROLE_STATUS, 99);
  CHECK(led == nullptr);
}

TEST_CASE("DDT: find_default_device") {
  Ddt::set_provider(&g_provider);

  auto *button = Ddt::find_default_device(V4DEV_BUTTON, V4ROLE_USER);
  REQUIRE(button != nullptr);
  CHECK(button->kind == V4DEV_BUTTON);
  CHECK(button->role == V4ROLE_USER);
  CHECK(button->index == 0);
  CHECK(button->handle == 9);
}

TEST_CASE("DDT: count_devices") {
  Ddt::set_provider(&g_provider);

  SUBCASE("LED count") {
    size_t led_count = Ddt::count_devices(V4DEV_LED);
    CHECK(led_count == 3);
  }

  SUBCASE("BUTTON count") {
    size_t button_count = Ddt::count_devices(V4DEV_BUTTON);
    CHECK(button_count == 1);
  }

  SUBCASE("UART count") {
    size_t uart_count = Ddt::count_devices(V4DEV_UART);
    CHECK(uart_count == 1);
  }

  SUBCASE("TIMER count") {
    size_t timer_count = Ddt::count_devices(V4DEV_TIMER);
    CHECK(timer_count == 1);
  }

  SUBCASE("I2C count (not present)") {
    size_t i2c_count = Ddt::count_devices(V4DEV_I2C);
    CHECK(i2c_count == 0);
  }
}

TEST_CASE("DDT: Find UART by role") {
  Ddt::set_provider(&g_provider);

  auto *uart = Ddt::find_default_device(V4DEV_UART, V4ROLE_CONSOLE);
  REQUIRE(uart != nullptr);
  CHECK(uart->kind == V4DEV_UART);
  CHECK(uart->role == V4ROLE_CONSOLE);
}

TEST_CASE("DDT: Find TIMER") {
  Ddt::set_provider(&g_provider);

  auto *timer = Ddt::find_default_device(V4DEV_TIMER, V4ROLE_STATUS);
  REQUIRE(timer != nullptr);
  CHECK(timer->kind == V4DEV_TIMER);
}

TEST_CASE("DDT: Null provider behavior") {
  Ddt::set_provider(nullptr);

  SUBCASE("get_all_devices returns empty") {
    auto devices = Ddt::get_all_devices();
    CHECK(devices.empty());
  }

  SUBCASE("find_device returns nullptr") {
    auto *led = Ddt::find_device(V4DEV_LED, V4ROLE_STATUS, 0);
    CHECK(led == nullptr);
  }

  SUBCASE("count_devices returns 0") {
    size_t count = Ddt::count_devices(V4DEV_LED);
    CHECK(count == 0);
  }

  // Restore provider for other tests
  Ddt::set_provider(&g_provider);
}
