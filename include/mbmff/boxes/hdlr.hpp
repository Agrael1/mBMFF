#pragma once
#include "box_view.hpp"

namespace mbmff {

struct hdlr_data {
    mbmff::fourcc_string handler_type{};
    std::string_view name{};
};

template <>
struct basic_box_view<mbmff::box_type::hdlr> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::hdlr_data;
};

inline constexpr auto basic_box_view<box_type::hdlr>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    if (box.payload.size() < 20) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 20);
    }
    return {box};
}

inline constexpr auto basic_box_view<box_type::hdlr>::value() const noexcept -> mbmff::hdlr_data
{
    mbmff::hdlr_data result{};
    result.handler_type = mbmff::fourcc_string::from_data(payload.subspan(4));
    auto name_span = payload.subspan(16);
    if (!name_span.empty()) {
        auto name = mbmff::read_cstr(name_span, 0);
        result.name = name.value;
    }
    return result;
}

} // namespace mbmff
