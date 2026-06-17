# mBMFF

[![CI](https://img.shields.io/github/actions/workflow/status/Agrael1/mBMFF/ci.yml?branch=master&label=CI&logo=github)](https://github.com/Agrael1/mBMFF/actions/workflows/ci.yml)
[![License](https://img.shields.io/github/license/Agrael1/mBMFF)](LICENSE.txt)
[![Version](https://img.shields.io/badge/version-0.1.2-blue)](VERSION)
[![C++](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus)](CMakeLists.txt)
[![Header-only](https://img.shields.io/badge/header--only-yellowgreen)](include/mbmff)

A lightweight, dependency-free ISOBMFF (ISO Base Media File Format) parser written in modern C++20. Parses AVIF, HEIF, MP4, and any other ISO-BMFF container.

## Features

- **Zero dependencies** — no external libraries required
- **Zero allocations** — operates directly on caller-provided memory
- **Lazy parsing** — boxes are parsed on demand, skip what you don't need
- **Single-header amalgamation** — generate a single `mbmff.hpp` via the `amalgamate` CMake target for easy drop-in
- **Constexpr validation** — box structures are validated at compile time where possible (behind `MBMFF_ENABLE_CONSTEXPR_TEST`)
- **Iterators** — traverse boxes with `box_iterator` (flat or recursive)

## Quick start

```cpp
#include <mbmff/mbmff.hpp>
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
    std::ifstream file(argv[1], std::ios::binary);
    std::vector<char> data((std::istreambuf_iterator<char>(file)), {});

    for (const auto& result : mbmff::box_iterator(
             std::span(reinterpret_cast<const std::byte*>(data.data()), data.size()),
             mbmff::iterator_flags::recursive))
    {
        if (!result) break;
        auto& box = *result;

        switch (box.type()) {
        case mbmff::box_type::ftyp:
            std::cout << "ftyp: " << mbmff::box_cast<mbmff::box_type::ftyp>(box).value().major_brand.view() << '\n';
            break;
        case mbmff::box_type::hdlr:
            std::cout << "hdlr: " << mbmff::box_cast<mbmff::box_type::hdlr>(box).value().handler_type.view() << '\n';
            break;
        default:
            break;
        }
    }
}
```

## Requirements

- C++20 compiler (tested with MSVC 2022, GCC 14, Clang 18)

## Consumption

The library is header-only. Choose your path:

| Method | Instructions |
|---|---|
| **Copy headers** | Grab `include/mbmff/` and include `<mbmff/mbmff.hpp>` |
| **Single header** | `cmake --build build --target amalgamate` → `build/mbmff.hpp` |
| **CMake** | `add_subdirectory` or `FetchContent` → `target_link_libraries(foo PRIVATE mbmff::mbmff)` |
| **Conan** | `conan create .` → `requires = "mbmff/0.1.1"` |
| **Release** | Download single-header release |

## Project and contribution

Built in my free time. If you need a lean, fast ISOBMFF parser — for Vulkan Video or anything else — PRs are welcome.

## License

MIT — see [LICENSE.txt](LICENSE.txt).
