/**
 * @file sys_handlers.hpp
 * @brief SYS call handler registration and dispatch
 *
 * Provides platform-independent SYS call handler registration.
 * Platform implementations register handlers for specific SYS IDs,
 * and the V4 VM invokes them via invoke_sys_handler().
 *
 * @copyright Copyright 2025 V4 Project
 * @license Dual-licensed under MIT or Apache-2.0
 */

#ifndef V4STD_SYS_HANDLERS_HPP
#define V4STD_SYS_HANDLERS_HPP

#include <cstddef>
#include <cstdint>

namespace v4std
{

/**
 * @brief SYS call handler function signature
 *
 * Handlers receive the SYS ID and two arguments (arg0, arg1).
 * The arguments are typically device identifiers (kind, role, index)
 * or operation parameters.
 *
 * @param sys_id SYS call ID (e.g., V4SYS_LED_ON)
 * @param arg0 First argument (often: kind)
 * @param arg1 Second argument (often: role)
 * @param arg2 Third argument (often: index or value)
 * @return Result value (operation-specific)
 */
using SysHandler = int32_t (*)(uint16_t sys_id,
                                int32_t arg0,
                                int32_t arg1,
                                int32_t arg2);

/**
 * @brief Register a SYS call handler
 *
 * Registers a handler function for a specific SYS ID.
 * If a handler is already registered for this ID, it will be replaced.
 *
 * Thread safety: Not thread-safe. Registration should occur during
 * initialization before concurrent VM execution.
 *
 * @param sys_id SYS call ID (e.g., V4SYS_LED_ON)
 * @param handler Handler function pointer (must not be null)
 * @return true if registration succeeded, false if handler is null
 */
bool register_sys_handler(uint16_t sys_id, SysHandler handler);

/**
 * @brief Unregister a SYS call handler
 *
 * Removes the handler for the specified SYS ID.
 * If no handler is registered, this is a no-op.
 *
 * @param sys_id SYS call ID
 */
void unregister_sys_handler(uint16_t sys_id);

/**
 * @brief Get registered handler for a SYS ID
 *
 * Looks up the handler function for the given SYS ID.
 *
 * @param sys_id SYS call ID
 * @return Handler function pointer, or nullptr if not registered
 */
SysHandler get_sys_handler(uint16_t sys_id);

/**
 * @brief Invoke a SYS call handler
 *
 * Looks up and invokes the handler for the given SYS ID.
 * If no handler is registered, returns an error code (-1).
 *
 * @param sys_id SYS call ID
 * @param arg0 First argument
 * @param arg1 Second argument
 * @param arg2 Third argument
 * @return Handler result, or -1 if no handler registered
 */
int32_t invoke_sys_handler(uint16_t sys_id,
                            int32_t arg0,
                            int32_t arg1,
                            int32_t arg2);

/**
 * @brief Clear all registered SYS handlers
 *
 * Removes all handler registrations.
 * Useful for testing or re-initialization.
 */
void clear_sys_handlers();

/**
 * @brief Get number of registered handlers
 *
 * @return Count of registered handlers
 */
size_t get_sys_handler_count();

}  // namespace v4std

#endif  // V4STD_SYS_HANDLERS_HPP
