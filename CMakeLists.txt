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
  COMMAND ${CMAKE_COMMAND} -E make_directory
          "${PROJECT_BINARY_DIR}/generated/v4std"
  COMMAND
    ${CMAKE_COMMAND} -DINPUT_FILE=${V4SYS_IDS_DEF} -DOUTPUT_FILE=${V4SYS_IDS_H}
    -P ${PROJECT_SOURCE_DIR}/cmake/generate_sys_ids.cmake
  DEPENDS "${V4SYS_IDS_DEF}"
          "${PROJECT_SOURCE_DIR}/cmake/generate_sys_ids.cmake"
  COMMENT "Generating sys_ids.h from v4sys_ids.def"
  VERBATIM)

add_custom_target(generate_sys_ids DEPENDS "${V4SYS_IDS_H}")

# ============================================================================
# V4Std Library
# ============================================================================

# V4Std library sources
set(V4STD_SOURCES src/ddt.cpp src/sys_handlers.cpp src/sys_led.cpp
                  # src/sys_button.cpp src/sys_timer.cpp src/capability.cpp
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

  # Doctest include directory
  set(DOCTEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/tests/vendor/doctest)

  # Helper function to add test executables
  function(add_v4std_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_include_directories(${TEST_NAME} PRIVATE ${DOCTEST_INCLUDE_DIR})
    target_link_libraries(${TEST_NAME} PRIVATE v4std)
    target_compile_definitions(
      ${TEST_NAME} PRIVATE DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
  endfunction()

  # DDT types test
  add_executable(test_ddt_types tests/test_ddt_types.cpp)
  target_include_directories(
    test_ddt_types PRIVATE ${DOCTEST_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/include)
  target_compile_definitions(
    test_ddt_types PRIVATE DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS)
  add_test(NAME test_ddt_types COMMAND test_ddt_types)

  # DDT API test
  add_v4std_test(test_ddt tests/test_ddt.cpp)

  # SYS IDs test
  add_v4std_test(test_sys_ids tests/test_sys_ids.cpp)
  add_dependencies(test_sys_ids generate_sys_ids)

  # SYS handlers test
  add_v4std_test(test_sys_handlers tests/test_sys_handlers.cpp)

  # LED SYS test
  add_v4std_test(test_sys_led tests/test_sys_led.cpp)
endif()

# ============================================================================
# Examples
# ============================================================================

if(V4STD_BUILD_EXAMPLES)
  # Examples will be added in subsequent commits add_subdirectory(examples)
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
