/**
 * @file ddt_types.h
 * @brief Device Descriptor Table (DDT) POD type definitions
 *
 * C/C++ compatible header defining DDT structures and enums.
 * All types are POD (Plain Old Data) for portability.
 *
 * @copyright Copyright 2025 V4 Project
 * @license Dual-licensed under MIT or Apache-2.0
 */

#ifndef V4STD_DDT_TYPES_H
#define V4STD_DDT_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device kind/type
 *
 * Identifies the functional category of a device.
 */
typedef enum {
  V4DEV_NONE = 0,     /**< Undefined/invalid device */
  V4DEV_LED = 1,      /**< LED (digital output) */
  V4DEV_BUTTON = 2,   /**< Button (digital input) */
  V4DEV_BUZZER = 3,   /**< Buzzer (PWM output) */
  V4DEV_TIMER = 4,    /**< Timer (millis/micros) */
  V4DEV_UART = 5,     /**< UART (serial communication) */
  V4DEV_I2C = 6,      /**< I2C bus */
  V4DEV_SPI = 7,      /**< SPI bus */
  V4DEV_ADC = 8,      /**< ADC (analog input) */
  V4DEV_PWM = 9,      /**< PWM output */
  V4DEV_STORAGE = 10, /**< Storage (key-value store) */
  V4DEV_DISPLAY = 11, /**< Display controller */
  V4DEV_RNG = 12,     /**< Random number generator */
} v4dev_kind_t;

/**
 * @brief Device role
 *
 * Distinguishes the purpose/usage of devices of the same kind.
 * Allows multiple LEDs, buttons, etc. to be differentiated.
 */
typedef enum {
  V4ROLE_NONE = 0,    /**< Undefined/invalid role */
  V4ROLE_STATUS = 1,  /**< Status indicator (e.g., status LED) */
  V4ROLE_USER = 2,    /**< User interaction (e.g., user button) */
  V4ROLE_POWER = 3,   /**< Power control */
  V4ROLE_CONSOLE = 4, /**< Console communication (e.g., console UART) */
  V4ROLE_DEBUG = 5,   /**< Debug interface */
} v4dev_role_t;

/**
 * @brief Device descriptor flags
 */
#define V4DEV_FLAG_ACTIVE_LOW                                                  \
  (1 << 0) /**< Active-low signal (inverted logic) */

/**
 * @brief Device descriptor (8 bytes, POD type)
 *
 * Compact representation of a hardware device.
 * Platform-specific DDT providers populate arrays of these descriptors.
 */
typedef struct {
  uint8_t kind;    /**< Device type (v4dev_kind_t) */
  uint8_t role;    /**< Device role (v4dev_role_t) */
  uint8_t index;   /**< Index within kind/role combination (0-based) */
  uint8_t flags;   /**< Configuration flags (V4DEV_FLAG_*) */
  uint32_t handle; /**< Platform-specific handle (GPIO pin, pointer, etc.) */
} v4dev_desc_t;

#ifdef __cplusplus
}
#endif

#endif /* V4STD_DDT_TYPES_H */
