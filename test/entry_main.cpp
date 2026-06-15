#include <mbmff/mbmff.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include "formatters.hpp"

static constexpr auto is_box_implemented(mbmff::box_type type) noexcept -> bool
{
    switch (type) {
    case mbmff::box_type::ftyp:
    case mbmff::box_type::meta:
    case mbmff::box_type::mdat:
    case mbmff::box_type::av1C:
    case mbmff::box_type::moov:
    case mbmff::box_type::trak:
    case mbmff::box_type::mdia:
    case mbmff::box_type::minf:
    case mbmff::box_type::stbl:
    case mbmff::box_type::dinf:
    case mbmff::box_type::edts:
    case mbmff::box_type::udta:
    case mbmff::box_type::mvex:
    case mbmff::box_type::moof:
    case mbmff::box_type::traf:
    case mbmff::box_type::mfra:
    case mbmff::box_type::ipco:
    case mbmff::box_type::iprp:
    case mbmff::box_type::iinf:
    case mbmff::box_type::iref:
    case mbmff::box_type::iloc:
    case mbmff::box_type::hdlr:
    case mbmff::box_type::pitm:
    case mbmff::box_type::ispe:
    case mbmff::box_type::pixi:
    case mbmff::box_type::pasp:
    case mbmff::box_type::ipma:
    case mbmff::box_type::infe:
    case mbmff::box_type::mvhd:
        return true;
    default:
        return false;
    }
}

static auto display_fourcc(const mbmff::fourcc_string& fc) -> std::string
{
    std::string out;
    out.reserve(12);
    bool all_printable = true;
    for (std::size_t i = 0; i < 4; ++i) {
        auto c = fc[i];
        if (static_cast<unsigned char>(c) < 0x20 || static_cast<unsigned char>(c) > 0x7e) {
            all_printable = false;
            break;
        }
    }
    if (all_printable) {
        out.append(fc.view());
    } else {
        out += '[';
        for (std::size_t i = 0; i < 4; ++i) {
            if (i) out += ' ';
            auto uc = static_cast<unsigned char>(fc[i]);
            auto hex = "0123456789abcdef";
            out += "0x";
            out += hex[uc >> 4];
            out += hex[uc & 0xf];
        }
        out += ']';
    }
    return out;
}

static void print_box(const mbmff::any_box_view& box, std::size_t depth)
{
    for (std::size_t i = 0; i < depth; ++i) std::cout << "  ";

    if (!is_box_implemented(box.type_)) {
        std::cout << "\x1b[33m[!]\x1b[0m ";
    }

    std::cout << '<' << display_fourcc(box.type_string());

    switch (box.type_) {
    case mbmff::box_type::ftyp:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::ftyp>(box));
        break;
    case mbmff::box_type::meta:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::meta>(box));
        break;
    case mbmff::box_type::mdat:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::mdat>(box));
        break;
    case mbmff::box_type::iinf:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::iinf>(box));
        break;
    case mbmff::box_type::iloc:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::iloc>(box));
        break;
    case mbmff::box_type::hdlr:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::hdlr>(box));
        break;
    case mbmff::box_type::pitm:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::pitm>(box));
        break;
    case mbmff::box_type::ispe:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::ispe>(box));
        break;
    case mbmff::box_type::pixi:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::pixi>(box));
        break;
    case mbmff::box_type::pasp:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::pasp>(box));
        break;
    case mbmff::box_type::ipma:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::ipma>(box));
        break;
    case mbmff::box_type::infe:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::infe>(box));
        break;
    case mbmff::box_type::av1C:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::av1C>(box));
        break;
    case mbmff::box_type::mvhd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::mvhd>(box));
        break;
    default:
        std::cout << " size=\"" << box.size_ << '"';
        break;
    }

    bool is_container = mbmff::has(mbmff::get_box_properties(box.type_), mbmff::box_properties::container);
    std::cout << (is_container ? ">" : " />") << '\n';

    if (box.type_ == mbmff::box_type::iloc) {
        auto iloc = mbmff::box_cast<mbmff::box_type::iloc>(box);
        auto data = iloc.value();
        auto iter = mbmff::iloc_item_iterator(data, iloc.version());
        for (; iter != mbmff::iloc_item_iterator{}; ++iter) {
            auto item = *iter;
            for (std::size_t i = 0; i < depth + 1; ++i) std::cout << "  ";
            std::cout << std::format("<item id=\"{}\" base_offset=\"{}\" construction_method=\"{}\""
                                     " data_reference_index=\"{}\" extents=\"{}\">\n",
                                     item.item_id, item.base_offset,
                                     item.construction_method, item.data_reference_index, item.size());
            for (std::size_t e = 0; e < item.size(); ++e) {
                auto ext = item[e];
                for (std::size_t i = 0; i < depth + 2; ++i) std::cout << "  ";
                std::cout << std::format("<extent offset=\"{}\" length=\"{}\"", ext.offset, ext.length);
                if (ext.index) {
                    std::cout << std::format(" index=\"{}\"", ext.index);
                }
                std::cout << " />\n";
            }
            for (std::size_t i = 0; i < depth + 1; ++i) std::cout << "  ";
            std::cout << "</item>\n";
        }
    }
}

static void walk_boxes_impl(const mbmff::any_box_view& box, std::size_t depth)
{
    print_box(box, depth);
    auto props = mbmff::get_box_properties(box.type_);
    if (mbmff::has(props, mbmff::box_properties::container) && !box.payload.empty()) {
        for (const auto& child : mbmff::box_iterator(box.payload)) {
            if (!child) {
                std::cerr << "\x1b[31;1m" << std::string(depth * 2, ' ')
                          << "Parse error: " << mbmff::to_string(child.code)
                          << " (needed " << child.needed << " bytes)\x1b[0m\n";
                return;
            }
            walk_boxes_impl(*child, depth + 1);
        }
    }
}

static void walk_boxes(std::span<const std::byte> data)
{
    for (const auto& result : mbmff::box_iterator(data)) {
        if (!result) {
            std::cerr << "\x1b[31;1mParse error at top level: "
                      << mbmff::to_string(result.code)
                      << " (needed " << result.needed << " bytes)\x1b[0m\n";
            return;
        }
        walk_boxes_impl(*result, 0);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: mbmff-test <file>\n";
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << argv[1] << '\n';
        return 1;
    }

    std::vector<char> file_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::span<const std::byte> file_span(
        reinterpret_cast<const std::byte*>(file_data.data()),
        file_data.size()
    );

    walk_boxes(file_span);
    return 0;
}
