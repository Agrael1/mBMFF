#pragma once
#include "box_view.hpp"

namespace mbmff {
struct ftyp_data {
    mbmff::fourcc_string major_brand{};
    std::uint32_t minor_version = 0;
    std::span<const mbmff::fourcc_string> compatible_brands{};
};

template <>
struct basic_box_view<mbmff::box_type::ftyp> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::ftyp_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::ftyp>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 8) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 8);
    }

    if (box.payload.size() % 4 != 0) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::invalid_format);
    }

    return {box};
}

constexpr auto mbmff::basic_box_view<mbmff::box_type::ftyp>::value() const noexcept -> mbmff::ftyp_data
{
    const auto compatible_data = payload.subspan(8);
    std::span<const mbmff::fourcc_string> compatible_brands(
        reinterpret_cast<const mbmff::fourcc_string*>(compatible_data.data()),
        compatible_data.size() / 4
    );
    return mbmff::ftyp_data{
        mbmff::fourcc_string::from_data(payload),
        mbmff::read_be<uint32_t>(payload.subspan(4)),
        compatible_brands
    };
}
#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
// validate must reject payload < 8 bytes
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::ftyp>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

// validate must reject payload not divisible by 4
static_assert([] {
    constexpr std::byte bad_div[10]{};
    auto r = mbmff::basic_box_view<mbmff::box_type::ftyp>::validate({mbmff::box_header{}, std::span(bad_div)});
    return !r;
}());
#endif

} // namespace mbmff
