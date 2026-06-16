#pragma once
#include "box_view.hpp"

namespace mbmff {

struct infe_data {
    std::uint32_t item_id = 0;
    std::uint16_t item_protection_index = 0;
    mbmff::fourcc_string item_type{};
    std::string_view item_name{};
    std::string_view content_type{};
    std::string_view content_encoding{};
    std::string_view item_uri_type{};
};

template <>
struct mbmff::basic_box_view<mbmff::box_type::infe> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::infe_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::infe>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    auto need = (box.version() == 0 || box.version() == 1 || box.version() == 2) ? std::size_t{4} : std::size_t{6};
    if (box.payload.size() < need) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, need);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::infe>::value() const noexcept -> mbmff::infe_data
{
    mbmff::infe_data result{};
    auto xversion = version();
    std::size_t offset = 0;

    if (xversion == 0 || xversion == 1 || xversion == 2) {
        result.item_id = mbmff::read_be<std::uint16_t>(payload);
        offset = 2;
    } else {
        result.item_id = mbmff::read_be<std::uint32_t>(payload);
        offset = 4;
    }

    result.item_protection_index = mbmff::read_be<std::uint16_t>(payload.subspan(offset));
    offset += 2;

    if (xversion >= 2) {
        result.item_type = mbmff::fourcc_string::from_data(payload.subspan(offset));
        offset += 4;
    }

    auto name = mbmff::read_cstr(payload, offset);
    result.item_name = name.value;
    offset = name.next;

    if (xversion >= 2) {
        if (result.item_type.view() == "mime") {
            auto type = mbmff::read_cstr(payload, offset);
            result.content_type = type.value;
            offset = type.next;

            auto encoding = mbmff::read_cstr(payload, offset);
            result.content_encoding = encoding.value;
            offset = encoding.next;
        } else if (result.item_type.view() == "uri ") {
            auto uri = mbmff::read_cstr(payload, offset);
            result.item_uri_type = uri.value;
            offset = uri.next;
        }
    }

    return result;
}

} // namespace mbmff
