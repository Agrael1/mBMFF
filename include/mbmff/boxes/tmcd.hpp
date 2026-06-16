#pragma once
#include "box_view.hpp"

namespace mbmff {

struct tmcd_data {
    std::uint16_t data_reference_index;
    std::uint32_t flags;
    std::uint32_t timescale;
    std::uint32_t frame_duration;
    std::uint16_t number_of_frames;
};

template <>
struct basic_box_view<mbmff::box_type::tmcd> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::tmcd_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::tmcd>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 24) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 24);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::tmcd>::value() const noexcept -> mbmff::tmcd_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    return {
        mbmff::read_be<std::uint16_t>(payload.subspan(8)),
        mbmff::read_be<std::uint32_t>(payload.subspan(10)),
        mbmff::read_be<std::uint32_t>(payload.subspan(14)),
        mbmff::read_be<std::uint32_t>(payload.subspan(18)),
        mbmff::read_be<std::uint16_t>(payload.subspan(22)),
    };
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::tmcd>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::tmcd>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 24>{}.data(), 24)}
)));

static_assert([] {
    constexpr std::array<std::byte, 24> data{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x03},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x19}, std::byte{0x00}, std::byte{0x1E},
    };
    mbmff::basic_box_view<mbmff::box_type::tmcd> tmcd;
    tmcd.payload = std::span(data);
    auto v = tmcd.value();
    return v.data_reference_index == 1 && v.flags == 2 && v.timescale == 3 && v.frame_duration == 25
        && v.number_of_frames == 30;
}());
#endif

} // namespace mbmff
