/**
 * @file test_ddt_types.cpp
 * @brief Tests for DDT POD types
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "v4std/ddt_types.h"

// Test C++ compilation
static_assert(sizeof(v4dev_desc_t) == 8, "v4dev_desc_t must be 8 bytes");

TEST_CASE("v4dev_desc_t: Struct size") { CHECK(sizeof(v4dev_desc_t) == 8); }

TEST_CASE("v4dev_desc_t: Device kind enum values") {
  CHECK(V4DEV_LED == 1);
  CHECK(V4DEV_BUTTON == 2);
  CHECK(V4DEV_TIMER == 4);
}

TEST_CASE("v4dev_desc_t: Device role enum values") {
  CHECK(V4ROLE_STATUS == 1);
  CHECK(V4ROLE_USER == 2);
  CHECK(V4ROLE_CONSOLE == 4);
}

TEST_CASE("v4dev_desc_t: Device flags") { CHECK(V4DEV_FLAG_ACTIVE_LOW == 1); }

TEST_CASE("v4dev_desc_t: Descriptor initialization") {
  v4dev_desc_t led = {
      V4DEV_LED,     // kind
      V4ROLE_STATUS, // role
      0,             // index
      0,             // flags
      7              // handle (GPIO7)
  };

  CHECK(led.kind == V4DEV_LED);
  CHECK(led.role == V4ROLE_STATUS);
  CHECK(led.index == 0);
  CHECK(led.flags == 0);
  CHECK(led.handle == 7);
}

TEST_CASE("v4dev_desc_t: Active-low flag") {
  v4dev_desc_t button = {
      V4DEV_BUTTON,          // kind
      V4ROLE_USER,           // role
      0,                     // index
      V4DEV_FLAG_ACTIVE_LOW, // flags
      9                      // handle (GPIO9)
  };

  CHECK((button.flags & V4DEV_FLAG_ACTIVE_LOW) != 0);
}
