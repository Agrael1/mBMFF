#pragma once
#include "box_view.hpp"

namespace mbmff {

struct avcC_data {
    std::uint8_t configurationVersion = 0;
    std::uint8_t AVCProfileIndication = 0;
    std::uint8_t profile_compatibility = 0;
    std::uint8_t AVCLevelIndication = 0;
    std::uint8_t lengthSizeMinusOne = 0;
    std::span<const std::byte> nalu_data{};
};

template <>
struct basic_box_view<mbmff::box_type::avcC> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::avcC_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::avcC>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 6) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 6);
    }
    return {box};
}

constexpr auto mbmff::basic_box_view<mbmff::box_type::avcC>::value() const noexcept -> mbmff::avcC_data
{
    avcC_data result{};
    result.configurationVersion = static_cast<std::uint8_t>(payload[0]);
    result.AVCProfileIndication = static_cast<std::uint8_t>(payload[1]);
    result.profile_compatibility = static_cast<std::uint8_t>(payload[2]);
    result.AVCLevelIndication = static_cast<std::uint8_t>(payload[3]);
    result.lengthSizeMinusOne = static_cast<std::uint8_t>(payload[4]) & 0x03;
    result.nalu_data = payload.subspan(6);
    return result;
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::avcC>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert([] {
    constexpr std::byte min_data[6]{};
    auto r = mbmff::basic_box_view<mbmff::box_type::avcC>::validate({mbmff::box_header{}, std::span(min_data)});
    return static_cast<bool>(r);
}());

static_assert([] {
    constexpr std::array<std::byte, 10> data{
        std::byte{0x01},
        std::byte{0x64},
        std::byte{0x00},
        std::byte{0x1e},
        std::byte{0xff},
        std::byte{0x01},
        std::byte{0x00},
        std::byte{0x04},
        std::byte{0xbe},
        std::byte{0xef},
    };
    mbmff::basic_box_view<mbmff::box_type::avcC> avcc;
    avcc.payload = std::span(data);
    auto v = avcc.value();
    return v.configurationVersion == 1 && v.AVCProfileIndication == 100 && v.profile_compatibility == 0
        && v.AVCLevelIndication == 30 && v.lengthSizeMinusOne == 3 && v.nalu_data.size() == 4;
}());
#endif

} // namespace mbmff
