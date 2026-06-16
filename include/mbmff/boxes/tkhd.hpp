#pragma once
#include <array>
#include "box_view.hpp"

namespace mbmff {

struct tkhd_data {
    std::uint64_t creation_time = 0;
    std::uint64_t modification_time = 0;
    std::uint32_t track_id = 0;
    std::uint64_t duration = 0;
    std::uint16_t layer = 0;
    std::uint16_t alternate_group = 0;
    std::uint16_t volume = 0;
    std::array<std::uint32_t, 9> matrix{};
    std::uint32_t width = 0;
    std::uint32_t height = 0;
};

template <>
struct mbmff::basic_box_view<mbmff::box_type::tkhd> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::tkhd_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::tkhd>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    auto needed = (box.version() == 1) ? std::size_t{92} : std::size_t{80};
    if (box.payload.size() < needed) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, needed);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::tkhd>::value() const noexcept -> mbmff::tkhd_data
{
    mbmff::tkhd_data result{};
    if (payload.empty() && payload.data() == nullptr) {
        return result;
    }

    if (version() == 1) {
        result.creation_time = mbmff::read_be<std::uint64_t>(payload.subspan(0));
        result.modification_time = mbmff::read_be<std::uint64_t>(payload.subspan(8));
        result.track_id = mbmff::read_be<std::uint32_t>(payload.subspan(16));
        result.duration = mbmff::read_be<std::uint64_t>(payload.subspan(24));
        result.layer = mbmff::read_be<std::uint16_t>(payload.subspan(40));
        result.alternate_group = mbmff::read_be<std::uint16_t>(payload.subspan(42));
        result.volume = mbmff::read_be<std::uint16_t>(payload.subspan(44));
        for (std::size_t i = 0; i < 9; ++i) {
            result.matrix[i] = mbmff::read_be<std::uint32_t>(payload.subspan(48 + i * 4));
        }
        result.width = mbmff::read_be<std::uint32_t>(payload.subspan(84));
        result.height = mbmff::read_be<std::uint32_t>(payload.subspan(88));
    } else {
        result.creation_time = mbmff::read_be<std::uint32_t>(payload.subspan(0));
        result.modification_time = mbmff::read_be<std::uint32_t>(payload.subspan(4));
        result.track_id = mbmff::read_be<std::uint32_t>(payload.subspan(8));
        result.duration = mbmff::read_be<std::uint32_t>(payload.subspan(16));
        result.layer = mbmff::read_be<std::uint16_t>(payload.subspan(28));
        result.alternate_group = mbmff::read_be<std::uint16_t>(payload.subspan(30));
        result.volume = mbmff::read_be<std::uint16_t>(payload.subspan(32));
        for (std::size_t i = 0; i < 9; ++i) {
            result.matrix[i] = mbmff::read_be<std::uint32_t>(payload.subspan(36 + i * 4));
        }
        result.width = mbmff::read_be<std::uint32_t>(payload.subspan(72));
        result.height = mbmff::read_be<std::uint32_t>(payload.subspan(76));
    }
    return result;
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::tkhd>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

// validate must reject version 0 payload < 80 bytes (after 4-byte full header)
static_assert(!mbmff::basic_box_view<mbmff::box_type::tkhd>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 4>{std::byte{0x00}}.data(), 4)}
));

// value on a valid version 0 tkhd (data from ISOBMFF test)
static_assert([] {
    constexpr std::array<std::byte, 80> data{
        std::byte{0xe4}, std::byte{0x00}, std::byte{0xb3}, std::byte{0x34}, std::byte{0xe4}, std::byte{0x00},
        std::byte{0xb3}, std::byte{0x34}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x37}, std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x40}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x06}, std::byte{0xc0}, std::byte{0x00}, std::byte{0x00}, std::byte{0x09}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00},
    };
    mbmff::basic_box_view<mbmff::box_type::tkhd> tkhd;
    tkhd.version_ = 0;
    tkhd.payload = std::span(data);
    auto v = tkhd.value();
    return v.creation_time == 0xe400b334 && v.modification_time == 0xe400b334 && v.track_id == 1 && v.duration == 0x3702
        && v.layer == 0 && v.alternate_group == 0 && v.volume == 0 && v.width == 0x06c00000 && v.height == 0x09000000;
}());
#endif

} // namespace mbmff
