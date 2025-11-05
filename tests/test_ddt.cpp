/**
 * @file test_ddt.cpp
 * @brief Tests for DDT search and management
 */

#include "v4std/ddt.hpp"
#include <cassert>
#include <cstdio>

using namespace v4std;

// Mock DDT provider for testing
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

int main()
{
  // Initialize mock provider
  MockDdtProvider provider;
  Ddt::set_provider(&provider);

  // Test: get_all_devices
  {
    auto devices = Ddt::get_all_devices();
    assert(devices.size() == 6);
    printf("✓ get_all_devices() returns 6 devices\n");
  }

  // Test: find_device - exact match
  {
    auto* led = Ddt::find_device(V4DEV_LED, V4ROLE_STATUS, 0);
    assert(led != nullptr);
    assert(led->kind == V4DEV_LED);
    assert(led->role == V4ROLE_STATUS);
    assert(led->index == 0);
    assert(led->handle == 7);
    printf("✓ find_device() finds STATUS LED at GPIO7\n");
  }

  // Test: find_device - different index
  {
    auto* led = Ddt::find_device(V4DEV_LED, V4ROLE_USER, 1);
    assert(led != nullptr);
    assert(led->index == 1);
    assert(led->handle == 10);
    assert(led->flags & V4DEV_FLAG_ACTIVE_LOW);
    printf("✓ find_device() finds USER LED index=1 at GPIO10\n");
  }

  // Test: find_device - not found
  {
    auto* led = Ddt::find_device(V4DEV_LED, V4ROLE_STATUS, 99);
    assert(led == nullptr);
    printf("✓ find_device() returns nullptr for non-existent device\n");
  }

  // Test: find_default_device
  {
    auto* button = Ddt::find_default_device(V4DEV_BUTTON, V4ROLE_USER);
    assert(button != nullptr);
    assert(button->kind == V4DEV_BUTTON);
    assert(button->role == V4ROLE_USER);
    assert(button->index == 0);
    assert(button->handle == 9);
    printf("✓ find_default_device() finds USER BUTTON at GPIO9\n");
  }

  // Test: count_devices
  {
    size_t led_count = Ddt::count_devices(V4DEV_LED);
    assert(led_count == 3);
    printf("✓ count_devices(LED) returns 3\n");

    size_t button_count = Ddt::count_devices(V4DEV_BUTTON);
    assert(button_count == 1);
    printf("✓ count_devices(BUTTON) returns 1\n");

    size_t uart_count = Ddt::count_devices(V4DEV_UART);
    assert(uart_count == 1);
    printf("✓ count_devices(UART) returns 1\n");

    size_t timer_count = Ddt::count_devices(V4DEV_TIMER);
    assert(timer_count == 1);
    printf("✓ count_devices(TIMER) returns 1\n");

    size_t i2c_count = Ddt::count_devices(V4DEV_I2C);
    assert(i2c_count == 0);
    printf("✓ count_devices(I2C) returns 0 (not present)\n");
  }

  // Test: Find UART by role
  {
    auto* uart = Ddt::find_default_device(V4DEV_UART, V4ROLE_CONSOLE);
    assert(uart != nullptr);
    assert(uart->kind == V4DEV_UART);
    assert(uart->role == V4ROLE_CONSOLE);
    printf("✓ find_default_device() finds CONSOLE UART\n");
  }

  // Test: Find TIMER
  {
    auto* timer = Ddt::find_default_device(V4DEV_TIMER, V4ROLE_STATUS);
    assert(timer != nullptr);
    assert(timer->kind == V4DEV_TIMER);
    printf("✓ find_default_device() finds TIMER\n");
  }

  // Test: Null provider behavior
  {
    Ddt::set_provider(nullptr);

    auto devices = Ddt::get_all_devices();
    assert(devices.empty());

    auto* led = Ddt::find_device(V4DEV_LED, V4ROLE_STATUS, 0);
    assert(led == nullptr);

    size_t count = Ddt::count_devices(V4DEV_LED);
    assert(count == 0);

    printf("✓ Null provider returns empty results\n");

    // Restore provider for other tests
    Ddt::set_provider(&provider);
  }

  printf("\n✅ All DDT tests passed!\n");
  return 0;
}
