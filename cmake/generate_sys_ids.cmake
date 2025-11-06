#!/usr/bin/env cmake -P
# @file generate_sys_ids.cmake
# @brief Generate sys_ids.h from v4sys_ids.def
#
# Usage: cmake -DINPUT_FILE=... -DOUTPUT_FILE=... -P generate_sys_ids.cmake

if(NOT INPUT_FILE OR NOT OUTPUT_FILE)
  message(
    FATAL_ERROR
      "Usage: cmake -DINPUT_FILE=<def> -DOUTPUT_FILE=<h> -P generate_sys_ids.cmake"
  )
endif()

# Read input file
file(READ "${INPUT_FILE}" DEF_CONTENT)

# Initialize output content with header
set(OUTPUT_CONTENT
    "/**
 * @file sys_ids.h
 * @brief V4-std SYS call ID definitions (auto-generated)
 *
 * THIS FILE IS AUTO-GENERATED FROM v4sys_ids.def
 * DO NOT EDIT MANUALLY!
 *
 * @copyright Copyright 2025 V4 Project
 * @license Dual-licensed under MIT or Apache-2.0
 */

#ifndef V4STD_SYS_IDS_H
#define V4STD_SYS_IDS_H

#include <cstdint>

#ifdef __cplusplus
extern \"C\" {
#endif

")

# Process each line
string(REPLACE "\n" ";" DEF_LINES "${DEF_CONTENT}")

foreach(LINE ${DEF_LINES})
  # Skip comments, empty lines, and documentation
  if(LINE MATCHES "^[ \t]*//")
    # Keep comment lines
    string(APPEND OUTPUT_CONTENT "${LINE}\n")
  elseif(LINE MATCHES "^[ \t]*\\*")
    # Skip file header comments
  elseif(LINE MATCHES "^[ \t]*$")
    # Keep empty lines for formatting
    string(APPEND OUTPUT_CONTENT "\n")
  elseif(
    LINE MATCHES
    "V4SYS_DEF\\(([A-Z0-9_]+),[ \t]*0x([0-9A-Fa-f]+),[ \t]*\"([^\"]+)\"\\)")
    # Extract: NAME, HEX_VALUE, DESCRIPTION
    set(SYS_NAME ${CMAKE_MATCH_1})
    set(SYS_VALUE ${CMAKE_MATCH_2})
    set(SYS_DESC ${CMAKE_MATCH_3})

    # Generate C/C++ constant definition
    string(APPEND OUTPUT_CONTENT
           "#define V4SYS_${SYS_NAME} 0x${SYS_VALUE}  /**< ${SYS_DESC} */\n")
  elseif(LINE MATCHES "=====")
    # Keep separator lines for readability
    string(APPEND OUTPUT_CONTENT "${LINE}\n")
  endif()
endforeach()

# Add footer
string(
  APPEND
  OUTPUT_CONTENT
  "
#ifdef __cplusplus
}  // extern \"C\"
#endif

#endif  // V4STD_SYS_IDS_H
")

# Write output file
file(WRITE "${OUTPUT_FILE}" "${OUTPUT_CONTENT}")

message(STATUS "Generated ${OUTPUT_FILE} from ${INPUT_FILE}")
