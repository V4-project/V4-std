/**
 * @file test_ddt_types.cpp
 * @brief Tests for DDT POD types
 */

#include "v4std/ddt_types.h"
#include <cassert>
#include <cstdio>

// Test C++ compilation
static_assert(sizeof(v4dev_desc_t) == 8, "v4dev_desc_t must be 8 bytes");

int main()
{
  // Test struct size
  assert(sizeof(v4dev_desc_t) == 8);
  printf("✓ v4dev_desc_t size: %zu bytes\n", sizeof(v4dev_desc_t));

  // Test enum values
  assert(V4DEV_LED == 1);
  assert(V4DEV_BUTTON == 2);
  assert(V4DEV_TIMER == 4);
  printf("✓ Device kind enum values correct\n");

  assert(V4ROLE_STATUS == 1);
  assert(V4ROLE_USER == 2);
  assert(V4ROLE_CONSOLE == 4);
  printf("✓ Device role enum values correct\n");

  // Test flags
  assert(V4DEV_FLAG_ACTIVE_LOW == 1);
  printf("✓ Device flags correct\n");

  // Test descriptor initialization
  v4dev_desc_t led = {
      .kind = V4DEV_LED,
      .role = V4ROLE_STATUS,
      .index = 0,
      .flags = 0,
      .handle = 7 // GPIO7
  };

  assert(led.kind == V4DEV_LED);
  assert(led.role == V4ROLE_STATUS);
  assert(led.index == 0);
  assert(led.flags == 0);
  assert(led.handle == 7);
  printf("✓ Descriptor initialization works\n");

  // Test active-low flag
  v4dev_desc_t button = {
      .kind = V4DEV_BUTTON,
      .role = V4ROLE_USER,
      .index = 0,
      .flags = V4DEV_FLAG_ACTIVE_LOW,
      .handle = 9 // GPIO9
  };

  assert(button.flags & V4DEV_FLAG_ACTIVE_LOW);
  printf("✓ Active-low flag works\n");

  printf("\n✅ All DDT type tests passed!\n");
  return 0;
}
