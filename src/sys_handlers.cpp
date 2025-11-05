/**
 * @file sys_handlers.cpp
 * @brief SYS call handler registry implementation
 */

#include "v4std/sys_handlers.hpp"
#include <unordered_map>

namespace v4std
{

// Global handler registry
// Note: In production embedded systems, consider using a fixed-size
// array or other allocation-free structure. std::unordered_map is used
// here for simplicity and flexibility during development.
static std::unordered_map<uint16_t, SysHandler> handler_registry;

bool register_sys_handler(uint16_t sys_id, SysHandler handler)
{
  if (!handler) {
    return false;
  }

  handler_registry[sys_id] = handler;
  return true;
}

void unregister_sys_handler(uint16_t sys_id)
{
  handler_registry.erase(sys_id);
}

SysHandler get_sys_handler(uint16_t sys_id)
{
  auto it = handler_registry.find(sys_id);
  if (it != handler_registry.end()) {
    return it->second;
  }
  return nullptr;
}

int32_t invoke_sys_handler(uint16_t sys_id,
                            int32_t arg0,
                            int32_t arg1,
                            int32_t arg2)
{
  SysHandler handler = get_sys_handler(sys_id);
  if (!handler) {
    return -1;  // Error: no handler registered
  }

  return handler(sys_id, arg0, arg1, arg2);
}

void clear_sys_handlers()
{
  handler_registry.clear();
}

size_t get_sys_handler_count()
{
  return handler_registry.size();
}

}  // namespace v4std
