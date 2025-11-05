/**
 * @file ddt.cpp
 * @brief DDT implementation
 */

#include "v4std/ddt.hpp"

namespace v4std
{

// Static member initialization
DdtProvider* Ddt::provider_ = nullptr;

void Ddt::set_provider(DdtProvider* provider)
{
  provider_ = provider;
}

const v4dev_desc_t* Ddt::find_device(v4dev_kind_t kind,
                                     v4dev_role_t role,
                                     uint8_t index)
{
  if (!provider_)
    return nullptr;

  auto devices = provider_->get_devices();

  for (const auto& dev : devices) {
    if (dev.kind == kind && dev.role == role && dev.index == index) {
      return &dev;
    }
  }

  return nullptr;
}

const v4dev_desc_t* Ddt::find_default_device(v4dev_kind_t kind,
                                             v4dev_role_t role)
{
  return find_device(kind, role, 0);
}

size_t Ddt::count_devices(v4dev_kind_t kind)
{
  if (!provider_)
    return 0;

  auto devices = provider_->get_devices();
  size_t count = 0;

  for (const auto& dev : devices) {
    if (dev.kind == kind) {
      ++count;
    }
  }

  return count;
}

span<const v4dev_desc_t> Ddt::get_all_devices()
{
  if (!provider_)
    return {};

  return provider_->get_devices();
}

}  // namespace v4std
