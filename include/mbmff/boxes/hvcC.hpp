#pragma once
#include "box_view.hpp"

namespace mbmff {

struct hvcC_data {
    std::uint8_t configuration_version;
    std::uint8_t general_profile_space;
    std::uint8_t general_tier_flag;
    std::uint8_t general_profile_idc;
    std::uint32_t general_profile_compatibility_flags;
    std::uint64_t general_constraint_indicator_flags;
    std::uint8_t general_level_idc;
    std::uint16_t min_spatial_segmentation_idc;
    std::uint8_t parallelism_type;
    std::uint8_t chroma_format;
    std::uint8_t bit_depth_luma_minus8;
    std::uint8_t bit_depth_chroma_minus8;
    std::uint16_t avg_frame_rate;
    std::uint8_t constant_frame_rate;
    std::uint8_t num_temporal_layers;
    std::uint8_t temporal_id_nested;
    std::uint8_t length_size_minus_one;
    std::uint8_t num_arrays;
    std::span<const std::byte> arrays_data;
};

template <>
struct mbmff::basic_box_view<mbmff::box_type::hvcC> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::hvcC_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::hvcC>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 23) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 23);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::hvcC>::value() const noexcept -> mbmff::hvcC_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    auto span = payload;
    auto b1 = static_cast<std::uint8_t>(span[1]);
    auto b21 = static_cast<std::uint8_t>(span[21]);
    auto ci_hi = mbmff::read_be<std::uint16_t>(span.subspan(6));
    auto ci_lo = mbmff::read_be<std::uint32_t>(span.subspan(8));
    return {
        .configuration_version = static_cast<std::uint8_t>(span[0]),
        .general_profile_space = static_cast<std::uint8_t>((b1 >> 6) & 3),
        .general_tier_flag = static_cast<std::uint8_t>((b1 >> 5) & 1),
        .general_profile_idc = static_cast<std::uint8_t>(b1 & 0x1F),
        .general_profile_compatibility_flags = mbmff::read_be<std::uint32_t>(span.subspan(2)),
        .general_constraint_indicator_flags = (static_cast<std::uint64_t>(ci_hi) << 32) | ci_lo,
        .general_level_idc = static_cast<std::uint8_t>(span[12]),
        .min_spatial_segmentation_idc = static_cast<std::uint16_t>(
            mbmff::read_be<std::uint16_t>(span.subspan(13)) & 0x0FFF
        ),
        .parallelism_type = static_cast<std::uint8_t>(static_cast<std::uint8_t>(span[15]) & 3),
        .chroma_format = static_cast<std::uint8_t>(static_cast<std::uint8_t>(span[16]) & 3),
        .bit_depth_luma_minus8 = static_cast<std::uint8_t>(static_cast<std::uint8_t>(span[17]) & 7),
        .bit_depth_chroma_minus8 = static_cast<std::uint8_t>(static_cast<std::uint8_t>(span[18]) & 7),
        .avg_frame_rate = mbmff::read_be<std::uint16_t>(span.subspan(19)),
        .constant_frame_rate = static_cast<std::uint8_t>((b21 >> 6) & 3),
        .num_temporal_layers = static_cast<std::uint8_t>((b21 >> 3) & 7),
        .temporal_id_nested = static_cast<std::uint8_t>((b21 >> 2) & 1),
        .length_size_minus_one = static_cast<std::uint8_t>(b21 & 3),
        .num_arrays = static_cast<std::uint8_t>(span[22]),
        .arrays_data = span.subspan(23),
    };
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::hvcC>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::hvcC>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 23>{}.data(), 23)}
)));

static_assert([] {
    constexpr std::array<std::byte, 23> data{
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    };
    mbmff::basic_box_view<mbmff::box_type::hvcC> hvcC;
    hvcC.payload = std::span(data);
    auto v = hvcC.value();
    return v.configuration_version == 1;
}());
#endif

} // namespace mbmff
