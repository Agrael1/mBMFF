#pragma once
#include "box_view.hpp"

namespace mbmff {

struct sdtp_entry {
    std::uint8_t is_leading;
    std::uint8_t sample_depends_on;
    std::uint8_t sample_is_depended_on;
    std::uint8_t sample_has_redundancy;
};

struct sdtp_data {
    std::span<const std::byte> entries_data{};

    constexpr auto size() const noexcept -> std::size_t
    {
        return entries_data.size();
    }
    constexpr auto operator[](std::size_t index) const noexcept -> mbmff::sdtp_entry
    {
        auto byte = static_cast<std::uint8_t>(entries_data[index]);
        return {
            .is_leading = static_cast<std::uint8_t>((byte >> 6) & 3),
            .sample_depends_on = static_cast<std::uint8_t>((byte >> 4) & 3),
            .sample_is_depended_on = static_cast<std::uint8_t>((byte >> 2) & 3),
            .sample_has_redundancy = static_cast<std::uint8_t>(byte & 3),
        };
    }
};

template <>
struct basic_box_view<mbmff::box_type::sdtp> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::sdtp_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::sdtp>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    (void)box.payload.subspan(0);
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::sdtp>::value() const noexcept -> mbmff::sdtp_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    return {payload.subspan(4)};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::sdtp>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert([] {
    constexpr std::array<std::byte, 8> data{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0xC0},
        std::byte{0xE0},
        std::byte{0x80},
        std::byte{0xFF},
    };
    mbmff::basic_box_view<mbmff::box_type::sdtp> sdtp;
    sdtp.version_ = 0;
    sdtp.payload = std::span(data);
    auto v = sdtp.value();
    if (v.size() != 4) {
        return false;
    }
    // byte 0xC0 = 1100 0000 -> is_leading=3, depends_on=0, depended_on=0, redundancy=0
    if (v[0].is_leading != 3 || v[0].sample_depends_on != 0 || v[0].sample_is_depended_on != 0
        || v[0].sample_has_redundancy != 0) {
        return false;
    }
    // byte 0xE0 = 1110 0000 -> is_leading=3, depends_on=2, depended_on=0, redundancy=0
    if (v[1].is_leading != 3 || v[1].sample_depends_on != 2 || v[1].sample_is_depended_on != 0
        || v[1].sample_has_redundancy != 0) {
        return false;
    }
    // byte 0x80 = 1000 0000 -> is_leading=2, depends_on=0, depended_on=0, redundancy=0
    if (v[2].is_leading != 2 || v[2].sample_depends_on != 0 || v[2].sample_is_depended_on != 0
        || v[2].sample_has_redundancy != 0) {
        return false;
    }
    // byte 0xFF = 1111 1111 -> all 3
    if (v[3].is_leading != 3 || v[3].sample_depends_on != 3 || v[3].sample_is_depended_on != 3
        || v[3].sample_has_redundancy != 3) {
        return false;
    }
    return true;
}());
#endif

} // namespace mbmff
