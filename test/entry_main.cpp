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

    for (const auto& box_expected : mbmff::box_iterator(file_data_span, mbmff::iterator_flags::recursive)) {
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
        case mbmff::box_type::infe: {
            auto infe = mbmff::box_cast<mbmff::box_type::infe>(box);
            std::cout << std::format("{}\n", infe);
        } break;
        case mbmff::box_type::meta: {
            auto meta = mbmff::box_cast<mbmff::box_type::meta>(box);
            std::cout << std::format("{}\n", meta);
        } break;
        case mbmff::box_type::hdlr: {
            auto hdlr = mbmff::box_cast<mbmff::box_type::hdlr>(box);
            std::cout << std::format("{}\n", hdlr);
        } break;
        case mbmff::box_type::pitm: {
            auto pitm = mbmff::box_cast<mbmff::box_type::pitm>(box);
            std::cout << std::format("{}\n", pitm);
        } break;
        case mbmff::box_type::iinf: {
            auto iinf = mbmff::box_cast<mbmff::box_type::iinf>(box);
            std::cout << std::format("{}\n", iinf);
        } break;
        case mbmff::box_type::iloc: {
            auto iloc = mbmff::box_cast<mbmff::box_type::iloc>(box);
            std::cout << std::format("{}\n", iloc);
        } break;
        case mbmff::box_type::iprp: {
            auto iprp = mbmff::box_cast<mbmff::box_type::iprp>(box);
            std::cout << std::format("{}\n", iprp);
        } break;
        case mbmff::box_type::ipco: {
            auto ipco = mbmff::box_cast<mbmff::box_type::ipco>(box);
            std::cout << std::format("{}\n", ipco);
        } break;
        case mbmff::box_type::mdat: {
            auto mdat = mbmff::box_cast<mbmff::box_type::mdat>(box);
            std::cout << std::format("{}\n", mdat);
        } break;
        default:
            std::cout << "Box type: " << box.header.type_string().view()
                      << ", size: " << box.header.size << '\n';
            break;
        }
    }

    return 0;
}
