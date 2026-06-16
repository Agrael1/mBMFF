#pragma once
#include "box_view.hpp"

namespace mbmff {

struct pixi_data {
    std::span<const std::uint8_t> bits_per_channel{};
};

template <>
struct mbmff::basic_box_view<mbmff::box_type::pixi> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::pixi_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::pixi>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    if (box.payload.size() < 1) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 1);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::pixi>::value() const noexcept -> mbmff::pixi_data
{
    mbmff::pixi_data result{};
    auto num_channels = static_cast<std::uint8_t>(payload[0]);
    auto remaining = payload.size() - 1;
    auto count = (num_channels < remaining) ? num_channels : static_cast<std::uint8_t>(remaining);
    result.bits_per_channel = std::span<const std::uint8_t>(
        static_cast<const std::uint8_t*>(static_cast<const void*>(payload.data() + 1)),
        count
    );
    return result;
}

} // namespace mbmff
