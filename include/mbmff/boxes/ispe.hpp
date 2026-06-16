#pragma once
#include "box_view.hpp"

namespace mbmff {

struct ispe_data {
    std::uint32_t image_width = 0;
    std::uint32_t image_height = 0;
};

template <>
struct basic_box_view<mbmff::box_type::ispe> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::ispe_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::ispe>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    if (box.payload.size() < 8) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 8);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::ispe>::value() const noexcept -> mbmff::ispe_data
{
    mbmff::ispe_data result{};
    result.image_width = mbmff::read_be<std::uint32_t>(payload);
    result.image_height = mbmff::read_be<std::uint32_t>(payload.subspan(4));
    return result;
}

} // namespace mbmff
