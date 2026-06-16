#pragma once
#include "box_view.hpp"

namespace mbmff {

struct pasp_data {
    std::uint32_t h_spacing = 0;
    std::uint32_t v_spacing = 0;
};

template <>
struct mbmff::basic_box_view<mbmff::box_type::pasp> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::pasp_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::pasp>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 8) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 8);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::pasp>::value() const noexcept -> mbmff::pasp_data
{
    mbmff::pasp_data result{};
    result.h_spacing = mbmff::read_be<std::uint32_t>(payload);
    result.v_spacing = mbmff::read_be<std::uint32_t>(payload.subspan(4));
    return result;
}

} // namespace mbmff
