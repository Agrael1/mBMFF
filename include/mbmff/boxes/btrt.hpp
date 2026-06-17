#pragma once
#include "box_view.hpp"

namespace mbmff {

struct btrt_data {
    std::uint32_t buffer_size_db;
    std::uint32_t max_bitrate;
    std::uint32_t avg_bitrate;
};

template <>
struct basic_box_view<mbmff::box_type::btrt> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::btrt_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::btrt>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 12) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 12);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::btrt>::value() const noexcept -> mbmff::btrt_data
{
    if (payload.size() < 12) {
        return {};
    }
    return {
        mbmff::read_be<std::uint32_t>(payload.subspan(0)),
        mbmff::read_be<std::uint32_t>(payload.subspan(4)),
        mbmff::read_be<std::uint32_t>(payload.subspan(8)),
    };
}

} // namespace mbmff
