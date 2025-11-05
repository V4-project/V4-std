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
# Code Generation
# ============================================================================

# Generate sys_ids.h from v4sys_ids.def
set(V4SYS_IDS_DEF "${PROJECT_SOURCE_DIR}/include/v4std/v4sys_ids.def")
set(V4SYS_IDS_H "${PROJECT_BINARY_DIR}/generated/v4std/sys_ids.h")

add_custom_command(
  OUTPUT "${V4SYS_IDS_H}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/generated/v4std"
  COMMAND ${CMAKE_COMMAND}
          -DINPUT_FILE=${V4SYS_IDS_DEF}
          -DOUTPUT_FILE=${V4SYS_IDS_H}
          -P ${PROJECT_SOURCE_DIR}/cmake/generate_sys_ids.cmake
  DEPENDS "${V4SYS_IDS_DEF}" "${PROJECT_SOURCE_DIR}/cmake/generate_sys_ids.cmake"
  COMMENT "Generating sys_ids.h from v4sys_ids.def"
  VERBATIM
)

add_custom_target(generate_sys_ids DEPENDS "${V4SYS_IDS_H}")

# ============================================================================
# V4Std Library
# ============================================================================

# V4Std library sources
set(V4STD_SOURCES
    src/ddt.cpp
    # src/sys_handlers.cpp
    # src/sys_led.cpp
    # src/sys_button.cpp
    # src/sys_timer.cpp
    # src/capability.cpp
)

add_library(v4std STATIC ${V4STD_SOURCES} "${V4SYS_IDS_H}")

target_include_directories(
  v4std
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>
         $<INSTALL_INTERFACE:include>
  PRIVATE ${PROJECT_SOURCE_DIR}/src)

add_dependencies(v4std generate_sys_ids)

# ============================================================================
# Tests
# ============================================================================

if(V4STD_BUILD_TESTS)
  enable_testing()

  # DDT types test
  add_executable(test_ddt_types tests/test_ddt_types.cpp)
  target_include_directories(test_ddt_types PRIVATE ${PROJECT_SOURCE_DIR}/include)
  add_test(NAME test_ddt_types COMMAND test_ddt_types)

  # DDT API test
  add_executable(test_ddt tests/test_ddt.cpp)
  target_link_libraries(test_ddt PRIVATE v4std)
  add_test(NAME test_ddt COMMAND test_ddt)

  # SYS IDs test
  add_executable(test_sys_ids tests/test_sys_ids.cpp)
  target_link_libraries(test_sys_ids PRIVATE v4std)
  add_dependencies(test_sys_ids generate_sys_ids)
  add_test(NAME test_sys_ids COMMAND test_sys_ids)
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
