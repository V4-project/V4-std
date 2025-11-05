cmake_minimum_required(VERSION 3.16)
project(
  V4Std
  VERSION 0.1.0
  LANGUAGES CXX)

# ============================================================================
# Project Configuration
# ============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(V4STD_BUILD_TESTS "Build unit tests" ON)
option(V4STD_BUILD_EXAMPLES "Build example programs" ON)

# ============================================================================
# Compiler Flags
# ============================================================================

if(MSVC)
  add_compile_options(/W4 /GR- /EHs-c-)
else()
  add_compile_options(-Wall -Wextra -fno-exceptions -fno-rtti)
endif()

# ============================================================================
# V4Std Library (placeholder for now)
# ============================================================================

# Will be populated in subsequent commits
set(V4STD_SOURCES
    # src/ddt.cpp
    # src/sys_handlers.cpp
    # src/sys_led.cpp
    # src/sys_button.cpp
    # src/sys_timer.cpp
    # src/capability.cpp
)

if(V4STD_SOURCES)
  add_library(v4std STATIC ${V4STD_SOURCES})

  target_include_directories(
    v4std
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
    PRIVATE ${PROJECT_SOURCE_DIR}/src)
endif()

# ============================================================================
# Tests
# ============================================================================

if(V4STD_BUILD_TESTS)
  enable_testing()

  # DDT types test
  add_executable(test_ddt_types tests/test_ddt_types.cpp)
  target_include_directories(test_ddt_types PRIVATE ${PROJECT_SOURCE_DIR}/include)
  add_test(NAME test_ddt_types COMMAND test_ddt_types)
endif()

# ============================================================================
# Examples
# ============================================================================

if(V4STD_BUILD_EXAMPLES)
  # Examples will be added in subsequent commits
  # add_subdirectory(examples)
endif()

# ============================================================================
# Status Summary
# ============================================================================

message(STATUS "")
message(STATUS "V4Std Configuration:")
message(STATUS "  Version:       ${PROJECT_VERSION}")
message(STATUS "  Build tests:   ${V4STD_BUILD_TESTS}")
message(STATUS "  Build examples:${V4STD_BUILD_EXAMPLES}")
message(STATUS "")
