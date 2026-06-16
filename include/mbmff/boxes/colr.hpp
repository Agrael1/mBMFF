#pragma once
#include "box_view.hpp"

namespace mbmff {

struct colr_data {
    mbmff::fourcc_string colour_type;
    std::uint16_t colour_primaries;
    std::uint16_t transfer_characteristics;
    std::uint16_t matrix_coefficients;
    bool full_range_flag;
    std::span<const std::byte> icc_profile;
};

template <>
struct basic_box_view<mbmff::box_type::colr> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::colr_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::colr>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    auto colour_type = mbmff::read_be<std::uint32_t>(box.payload);
    if (colour_type == mbmff::fourcc("nclx")) {
        if (box.payload.size() < 10) {
            return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 10);
        }
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::colr>::value() const noexcept -> mbmff::colr_data
{
    if (payload.size() < 4) {
        return {};
    }
    auto colour_type_raw = mbmff::read_be<std::uint32_t>(payload);
    auto colour_type = mbmff::fourcc_string::from_uint32(colour_type_raw);
    if (colour_type_raw == mbmff::fourcc("nclx")) {
        return {
            colour_type,
            mbmff::read_be<std::uint16_t>(payload.subspan(4)),
            mbmff::read_be<std::uint16_t>(payload.subspan(6)),
            mbmff::read_be<std::uint16_t>(payload.subspan(8)),
            static_cast<bool>(payload[10] & std::byte{0x80}),
            {},
        };
    }
    return {
        colour_type,
        0,
        0,
        0,
        false,
        payload.subspan(4),
    };
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::colr>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::colr>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 10>{}.data(), 10)}
)));
#endif

} // namespace mbmff
