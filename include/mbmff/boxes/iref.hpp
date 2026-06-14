#pragma once
#include "box_view.hpp"

namespace mbmff {

template <>
struct basic_box_view<mbmff::box_type::iref> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties
        = mbmff::box_properties::full_box | mbmff::box_properties::container;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
};

inline constexpr auto basic_box_view<box_type::iref>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);
    return {box};
}

} // namespace mbmff
