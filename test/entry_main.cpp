#include <mbmff/mbmff.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

int main()
{
    std::ifstream avif_file("avif_sample.avif", std::ios::binary);
    if (!avif_file) {
        std::cerr << "Failed to open the AVIF file.\n";
        return 1; // Failed to open the file
    }

    std::vector<char> file_data((std::istreambuf_iterator<char>(avif_file)), std::istreambuf_iterator<char>());
    std::span<const std::byte> file_data_span(reinterpret_cast<const std::byte*>(file_data.data()), file_data.size());

    for (const auto& box_expected : mbmff::box_iterator(file_data_span)) {
        if (!box_expected) {
            break;
        }
        const auto& box = box_expected.value();

        // Get concrete types
        switch (box.header.type) {
        case mbmff::box_type::ftyp: {
            auto ftyp = mbmff::box_cast<mbmff::box_type::ftyp>(box);
            std::cout << std::format("{}\n", ftyp);
        } break;
        case mbmff::box_type::meta: {
            auto meta = mbmff::box_cast<mbmff::box_type::meta>(box);
            std::cout << std::format("{}\n", meta);

            // Meta is a container box, so we can iterate over its children
            for (const auto& child_box_expected : mbmff::box_iterator(meta.payload)) {
                if (!child_box_expected) {
                    break;
                }
                const auto& child_box = child_box_expected.value();

                switch (child_box.header.type) {
                case mbmff::box_type::iinf: {
                    auto iinf = mbmff::box_cast<mbmff::box_type::iinf>(child_box);
                    std::cout << std::format("  {}\n", iinf);

                    for (const auto& infe_box_expected : mbmff::box_iterator(iinf.payload)) {
                        if (!infe_box_expected) {
                            break;
                        }
                        const auto& infe_box = infe_box_expected.value();
                        switch (infe_box.header.type) {
                        case mbmff::box_type::infe: {
                            auto infe = mbmff::box_cast<mbmff::box_type::infe>(infe_box);
                            std::cout << std::format("    {}\n", infe);
                            break;
                        }
                        default: {
                            std::cout << "    Box type: " << infe_box.header.type_string().view()
                                      << ", size: " << infe_box.header.size << '\n';
                        }
                        }
                    }
                } break;
                default:
                    break;
                }



                std::cout << "Box type: " << child_box.header.type_string().view()
                          << ", size: " << child_box.header.size
                          << '\n';
            }

        } break;
        default:
            std::cout << "Box type: " << box.header.type_string().view()
                      << ", size: " << box.header.size << '\n';
            break;
        }
    }

    return 0;
}
