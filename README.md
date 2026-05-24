# mBMFF

mBMFF is a library that primarily serves as an ISO-BMFF parser.
For now it only supportss AVIF.
It is designed with extensibility in mind and can be extended to support additional media formats and features in future versions.

## How it works

The library operates without any dependencies, making it lightweight and easy to integrate into various projects. 
It never allocates memory on its own, instead it relies on the caller to provide memory to parse.

The API is built around iterators. Boxes are parsed lazily, meaning that the library only parses the boxes that are requested by the caller.
Parsing is done in a resursive manner:

- The caller uses box_iterator to iterate over box headers. This allows the caller to quickly skip over boxes that are not of interest.
- Then the caller can use box_cast to cast the box header to a specific box type.
- After the box is cast to the type of interest, the caller can use the box's API to access its contents, which are also parsed lazily.

## Requirements

- C\+\+23 or later (use of std::span, std::expected and other C++23 features)
- yes, all you need is a C++23 compliant compiler and the standard library.

## Example

The example is in the `test` directory, but here is a simple example of how to use the library to parse an AVIF file:

```cpp
#include <mbmff/mbmff.hpp>

int main() {
    std::ifstream avif_file("assets/avif_sample_8_420.avif", std::ios::binary);
    if (!avif_file) {
        std::cerr << "Failed to open the AVIF file.\n";
        return 1; // Failed to open the file
    }

    // Read the entire file into a vector of chars
    std::vector<char> file_data((std::istreambuf_iterator<char>(avif_file)), std::istreambuf_iterator<char>());
    std::span<const std::byte> file_data_span(reinterpret_cast<const std::byte*>(file_data.data()), file_data.size());

    for (const auto& box_expected : mbmff::box_iterator(file_data_span, mbmff::iterator_flags::recursive)) {
        if (!box_expected) {
            break;
        }
        const auto& box = box_expected.value();

        // Get concrete types
        switch (box.box_header.type) {
        case mbmff::box_type::ftyp: {
            auto ftyp = mbmff::box_cast<mbmff::box_type::ftyp>(box);
            // Access the contents of the FTYP box using the ftyp variable
            std::cout << "Found FTYP box!" << std::endl;
        } break;
        }
    }
}
```