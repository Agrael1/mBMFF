#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include "formatters.hpp"



int main()
{
    std::ifstream avif_file("assets/avif_sample_8_420.avif", std::ios::binary);
    if (!avif_file) {
        std::cerr << "Failed to open the AVIF file.\n";
        return 1; // Failed to open the file
    }

    std::vector<char> file_data((std::istreambuf_iterator<char>(avif_file)), std::istreambuf_iterator<char>());
    std::span<const std::byte> file_data_span(reinterpret_cast<const std::byte*>(file_data.data()), file_data.size());

    uint32_t primary_item_id = 0;
    mbmff::iloc_box iloc_box;

    for (const auto& box_expected : mbmff::box_iterator(file_data_span, mbmff::iterator_flags::recursive)) {
        if (!box_expected) {
            break;
        }
        const auto& box = box_expected.value();

        // Get concrete types
        switch (box.box_header.type) {
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
            primary_item_id = pitm.item_id();
        } break;
        case mbmff::box_type::iinf: {
            auto iinf = mbmff::box_cast<mbmff::box_type::iinf>(box);
            std::cout << std::format("{}\n", iinf);
        } break;
        case mbmff::box_type::iloc: {
            auto iloc = mbmff::box_cast<mbmff::box_type::iloc>(box);
            std::cout << std::format("{}\n", iloc);
            iloc_box = iloc;
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
        case mbmff::box_type::ispe: {
            auto ispe = mbmff::box_cast<mbmff::box_type::ispe>(box);
            std::cout << std::format("{}\n", ispe);
        } break;
        case mbmff::box_type::av1C: {
            auto av1C = mbmff::box_cast<mbmff::box_type::av1C>(box);
            std::cout << std::format("{}\n", av1C);

            // scan OBUs in the av1C payload
            for (std::uint32_t i = 0; const auto& obu : mbmff::av1::obu_iterator(av1C.header())) {
                if (!obu) {
                    std::cout << std::format(
                        "    OBU parsing error: code={}, needed={}\n",
                        obu.error().code,
                        obu.error().needed
                    );
                    break;
                }

                if (obu->type == mbmff::av1::obu_type::sequence_header) {
                    auto seq_header_obu = mbmff::av1::obu_cast<mbmff::av1::obu_type::sequence_header>(*obu);
                    std::cout << std::format("    Sequence Header OBU: {}\n", seq_header_obu);
                    break;
                }

                std::cout << std::format("    {}:{}\n", i++, obu.value());
            }

        } break;
        case mbmff::box_type::pixi: {
            auto pixi = mbmff::box_cast<mbmff::box_type::pixi>(box);
            std::cout << std::format("{}\n", pixi);
        } break;
        case mbmff::box_type::ipma: {
            auto ipma = mbmff::box_cast<mbmff::box_type::ipma>(box);
            // ipma doesn't have a custom formatter, because we need to intrude into its payload to parse the actual
            // entries, so we print it manually here

            for (auto entry : mbmff::ipma_entry_iterator(ipma)) {
                std::cout << std::format("{}\n", entry);
            }
        } break;
        case mbmff::box_type::pasp: {
            auto pasp = mbmff::box_cast<mbmff::box_type::pasp>(box);
            std::cout << std::format("{}\n", pasp);
        } break;
        default:
            std::cout << "Box type: " << box.box_header.type_string().view() << ", size: " << box.box_header.size
                      << '\n';
            break;
        }
    }

    // parse iloc box to find the offset and size of the primary item in the mdat box
    uint64_t item_data_offset = 0;
    uint64_t item_data_size = 0;
    for (const auto& item : mbmff::iloc_item_iterator(iloc_box)) {
        if (item.item_id == primary_item_id) {
            std::cout << std::format(
                "Primary item found in iloc box: {}\n",
                item
            );
            item_data_offset = item.base_offset;
            item_data_size = item[0].length;
            break;
        }
    }

    // Seek to the primary item in the mdat box and print the first few bytes of its data
    auto primary_item_span = file_data_span.subspan(item_data_offset, item_data_size);
    auto primary_obu = mbmff::av1::obu_iterator(primary_item_span);

    for (std::uint32_t i = 0; const auto& obu : primary_obu) {
        if (!obu) {
            std::cout << std::format(
                "Primary item OBU parsing error: code={}, needed={}\n",
                obu.error().code,
                obu.error().needed
            );
            break;
        }
        std::cout << std::format("Primary item OBU {}: {}\n", i++, obu.value());
    }

    return 0;
}
