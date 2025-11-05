# V4-std: Standard Hardware Library for V4 Forth VM

**Status**: Work in Progress
**Version**: 0.1.0
**Language**: C++17

## Overview

V4-std is a device-independent standard library for the V4 Forth VM, providing portable hardware abstraction through the Device Descriptor Table (DDT) mechanism. Write once, run on any V4-supported platform.

## Features

- **Device Descriptor Table (DDT)**: Hardware abstraction via table-driven device enumeration
- **SYS Call Architecture**: Standardized syscall interface (0x0100-0x0FFF range)
- **Portable Forth Words**: Standard word library that works across all platforms
- **Zero Dependencies**: No dynamic allocation, no exceptions, no RTTI
- **Modern C++17**: Type-safe APIs with `std::span`, constexpr, etc.
- **Multi-Platform**: Linux, macOS, Windows, ESP32-C6, and more

## Supported Devices

- **LED**: Digital output (status indicators, user LEDs)
- **Button**: Digital input (user buttons, switches)
- **Timer**: Millisecond/microsecond timing, delays
- **UART**: Serial communication (console, debugging)
- More coming soon: I2C, SPI, ADC, PWM, Display, Storage

## Quick Start

### Building

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build
```

### Usage Example

```forth
\ led-hello.fth - Portable LED blink
: hello
  ." V4-std LED Demo" cr
  button-wait
  3 blink ;

S" led" cap? if
  S" button" cap? if
    hello
  else
    ." No button available" cr
  then
then
```

## Architecture

```
Forth Application (led-hello.fth)
        ↓
V4-std Words (led-on, button?, ms)
        ↓
SYS Calls (0x0120, 0x0140, 0x0162)
        ↓
SYS Handlers (sys_led_on, sys_button_read)
        ↓
DDT (Device Descriptor Table)
        ↓
Platform Provider (ESP32-C6, etc.)
        ↓
V4-hal (Hardware Abstraction Layer)
```

## Repository Structure

```
V4-std/
├── include/v4std/       # Public headers
├── src/                 # Implementation
├── tests/               # Unit tests
├── forth/               # Forth word definitions
├── examples/            # Example programs
└── docs/                # Documentation
```

## Integration

### With V4-engine

```cpp
#include "v4std/ddt.hpp"
#include "v4std/sys_handlers.hpp"

// Set DDT provider
v4std::Ddt::set_provider(&my_platform_provider);

// Register SYS handlers
v4std::register_all_sys_handlers(vm);
```

### With V4-runtime (ESP32-C6)

See `V4-runtime/bsp/esp32c6/components/v4_std/` for platform integration.

## Development Status

- [x] Repository structure
- [ ] DDT type definitions
- [ ] DDT search API
- [ ] SYS call definitions
- [ ] LED SYS handlers
- [ ] Button SYS handlers
- [ ] Timer SYS handlers
- [ ] Forth word library
- [ ] ESP32-C6 integration
- [ ] Documentation

## Contributing

This project adheres to C++17 embedded coding standards:
- No exceptions (`-fno-exceptions`)
- No RTTI (`-fno-rtti`)
- No dynamic allocation
- Fixed-size arrays and `std::span`

## License

Dual-licensed under MIT OR Apache-2.0.

## Links

- [V4 Project](https://github.com/V4-project)
- [V4-engine](https://github.com/V4-project/V4-engine)
- [V4-hal](https://github.com/V4-project/V4-hal)
- [V4-link](https://github.com/V4-project/V4-link)
