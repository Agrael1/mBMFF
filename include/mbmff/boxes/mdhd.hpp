#pragma once
#include "box_view.hpp"

namespace mbmff {

struct mdhd_data {
    std::uint64_t creation_time = 0;
    std::uint64_t modification_time = 0;
    std::uint32_t timescale = 0;
    std::uint64_t duration = 0;
    mbmff::fourcc_string language{};
    std::uint16_t predefined = 0;
};

template <>
struct basic_box_view<mbmff::box_type::mdhd> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::mdhd_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::mdhd>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    auto needed = (box.version() == 1) ? std::size_t{32} : std::size_t{20};
    if (box.payload.size() < needed) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, needed);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::mdhd>::value() const noexcept -> mbmff::mdhd_data
{
    mbmff::mdhd_data result{};
    if (payload.empty() && payload.data() == nullptr) {
        return result;
    }

    if (version() == 1) {
        result.creation_time = mbmff::read_be<std::uint64_t>(payload.subspan(0));
        result.modification_time = mbmff::read_be<std::uint64_t>(payload.subspan(8));
        result.timescale = mbmff::read_be<std::uint32_t>(payload.subspan(16));
        result.duration = mbmff::read_be<std::uint64_t>(payload.subspan(20));
        result.language = mbmff::fourcc_string::from_language(mbmff::read_be<std::uint16_t>(payload.subspan(28)));
        result.predefined = mbmff::read_be<std::uint16_t>(payload.subspan(30));
    } else {
        result.creation_time = mbmff::read_be<std::uint32_t>(payload.subspan(0));
        result.modification_time = mbmff::read_be<std::uint32_t>(payload.subspan(4));
        result.timescale = mbmff::read_be<std::uint32_t>(payload.subspan(8));
        result.duration = mbmff::read_be<std::uint32_t>(payload.subspan(12));
        result.language = mbmff::fourcc_string::from_language(mbmff::read_be<std::uint16_t>(payload.subspan(16)));
        result.predefined = mbmff::read_be<std::uint16_t>(payload.subspan(18));
    }
    return result;
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::mdhd>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(!mbmff::basic_box_view<mbmff::box_type::mdhd>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 4>{std::byte{0x00}}.data(), 4)}
));

static_assert(mbmff::fourcc_string::from_language(0x51A3).view() == "und");
static_assert(mbmff::fourcc_string::from_language(0x11A6).view() == "eng");

// value on a valid version 0 mdhd
static_assert([] {
    constexpr std::array<std::byte, 20> data{
        std::byte{0xe4}, std::byte{0x00}, std::byte{0xb3}, std::byte{0x34}, std::byte{0xe4},
        std::byte{0x00}, std::byte{0xb3}, std::byte{0x34}, std::byte{0x00}, std::byte{0x01},
        std::byte{0x5f}, std::byte{0x90}, std::byte{0x00}, std::byte{0x01}, std::byte{0xef},
        std::byte{0x13}, std::byte{0x51}, std::byte{0xA3}, // lang='und' (0x51A3)
        std::byte{0x00}, std::byte{0x00},
    };
    mbmff::basic_box_view<mbmff::box_type::mdhd> mdhd;
    mdhd.version_ = 0;
    mdhd.payload = std::span(data);
    auto v = mdhd.value();
    return v.creation_time == 0xe400b334 && v.modification_time == 0xe400b334 && v.timescale == 90000
        && v.duration == 126739 && v.language.view() == "und" && v.predefined == 0;
}());
#endif

} // namespace mbmff
