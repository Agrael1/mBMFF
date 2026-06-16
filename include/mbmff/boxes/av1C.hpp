#pragma once
#include "box_view.hpp"

namespace mbmff {
struct av1C_data {
    std::uint8_t marker                               : 1 = 0;
    std::uint8_t version                              : 7 = 0;
    std::uint8_t seq_profile                          : 3 = 0;
    std::uint8_t seq_level_idx_0                      : 5 = 0;
    std::uint8_t seq_tier_0                           : 1 = 0;
    std::uint8_t high_bitdepth                        : 1 = 0;
    std::uint8_t twelve_bit                           : 1 = 0;
    std::uint8_t monochrome                           : 1 = 0;
    std::uint8_t chroma_subsampling_x                 : 1 = 0;
    std::uint8_t chroma_subsampling_y                 : 1 = 0;
    std::uint8_t chroma_sample_position               : 2 = 0;
    std::uint8_t reserved                             : 3 = 0;
    std::uint8_t initial_presentation_delay_present   : 1 = 0;
    std::uint8_t initial_presentation_delay_minus_one : 4 = 0;

    std::span<const std::byte> config_obus{};
};

template <>
struct mbmff::basic_box_view<mbmff::box_type::av1C> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::av1C_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::av1C>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    return {box};
}

constexpr auto mbmff::basic_box_view<mbmff::box_type::av1C>::value() const noexcept -> mbmff::av1C_data
{
    av1C_data result{};
    std::uint8_t read_byte = static_cast<std::uint8_t>(payload[0]);
    result.marker = (read_byte >> 7) & 0x01;
    result.version = read_byte & 0x7F;

    read_byte = static_cast<std::uint8_t>(payload[1]);
    result.seq_profile = (read_byte >> 5) & 0x07;
    result.seq_level_idx_0 = read_byte & 0x1F;

    read_byte = static_cast<std::uint8_t>(payload[2]);
    result.seq_tier_0 = (read_byte >> 7) & 0x01;
    result.high_bitdepth = (read_byte >> 6) & 0x01;
    result.twelve_bit = (read_byte >> 5) & 0x01;
    result.monochrome = (read_byte >> 4) & 0x01;
    result.chroma_subsampling_x = (read_byte >> 3) & 0x01;
    result.chroma_subsampling_y = (read_byte >> 2) & 0x01;
    result.chroma_sample_position = read_byte & 0x03;

    read_byte = static_cast<std::uint8_t>(payload[3]);
    result.reserved = (read_byte >> 5) & 0x07;
    result.initial_presentation_delay_present = (read_byte >> 4) & 0x01;
    if (result.initial_presentation_delay_present) {
        result.initial_presentation_delay_minus_one = read_byte & 0x0F;
    }

    result.config_obus = payload.subspan(4);
    return result;
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
// validate must reject empty payload
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::av1C>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

// validate must accept 4-byte payload
static_assert([] {
    constexpr std::byte min_data[4]{};
    auto r = mbmff::basic_box_view<mbmff::box_type::av1C>::validate({mbmff::box_header{}, std::span(min_data)});
    return static_cast<bool>(r);
}());
#endif

} // namespace mbmff
