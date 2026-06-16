#pragma once
#include <array>
#include "box_view.hpp"

namespace mbmff {

struct mvhd_data {
    std::uint64_t creation_time = 0;
    std::uint64_t modification_time = 0;
    std::uint32_t timescale = 0;
    std::uint64_t duration = 0;
    std::uint32_t rate = 0;
    std::uint16_t volume = 0;
    std::array<std::uint32_t, 9> matrix{};
    std::uint32_t next_track_id = 0;
};

template <>
struct mbmff::basic_box_view<mbmff::box_type::mvhd> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::mvhd_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::mvhd>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    auto needed = (box.version() == 1) ? std::size_t{108} : std::size_t{96};
    if (box.payload.size() < needed) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, needed);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::mvhd>::value() const noexcept -> mbmff::mvhd_data
{
    mbmff::mvhd_data result{};
    if (payload.empty() && payload.data() == nullptr) {
        return result;
    }

    if (version() == 1) {
        result.creation_time = mbmff::read_be<std::uint64_t>(payload.subspan(0));
        result.modification_time = mbmff::read_be<std::uint64_t>(payload.subspan(8));
        result.timescale = mbmff::read_be<std::uint32_t>(payload.subspan(16));
        result.duration = mbmff::read_be<std::uint64_t>(payload.subspan(20));
        result.rate = mbmff::read_be<std::uint32_t>(payload.subspan(28));
        result.volume = mbmff::read_be<std::uint16_t>(payload.subspan(32));
        for (std::size_t i = 0; i < 9; ++i) {
            result.matrix[i] = mbmff::read_be<std::uint32_t>(payload.subspan(44 + i * 4));
        }
        result.next_track_id = mbmff::read_be<std::uint32_t>(payload.subspan(104));
    } else {
        result.creation_time = mbmff::read_be<std::uint32_t>(payload.subspan(0));
        result.modification_time = mbmff::read_be<std::uint32_t>(payload.subspan(4));
        result.timescale = mbmff::read_be<std::uint32_t>(payload.subspan(8));
        result.duration = mbmff::read_be<std::uint32_t>(payload.subspan(12));
        result.rate = mbmff::read_be<std::uint32_t>(payload.subspan(16));
        result.volume = mbmff::read_be<std::uint16_t>(payload.subspan(20));
        for (std::size_t i = 0; i < 9; ++i) {
            result.matrix[i] = mbmff::read_be<std::uint32_t>(payload.subspan(32 + i * 4));
        }
        result.next_track_id = mbmff::read_be<std::uint32_t>(payload.subspan(92));
    }
    return result;
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
// validate must reject payload < 4 bytes
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::mvhd>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

// validate must reject version 0 payload < 96 bytes (after 4-byte full header)
static_assert(!mbmff::basic_box_view<mbmff::box_type::mvhd>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 4>{std::byte{0x00}}.data(), 4)}
));

// value on a valid version 0 mvhd (data from ISOBMFF test)
static_assert([] {
    constexpr std::array<std::byte, 96> data{
        std::byte{0xe4}, std::byte{0x00}, std::byte{0xb3}, std::byte{0x34}, std::byte{0xe4}, std::byte{0x00},
        std::byte{0xb3}, std::byte{0x34}, std::byte{0x00}, std::byte{0x00}, std::byte{0x27}, std::byte{0x10},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x37}, std::byte{0x02}, std::byte{0x00}, std::byte{0x01},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x40}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x03},
    };
    mbmff::basic_box_view<mbmff::box_type::mvhd> mvhd;
    mvhd.version_ = 0;
    mvhd.payload = std::span(data);
    auto v = mvhd.value();
    return v.creation_time == 0xe400b334 && v.modification_time == 0xe400b334 && v.timescale == 10000
        && v.duration == 14082 && v.rate == 0x00010000 && v.volume == 0x0100 && v.next_track_id == 3;
}());
#endif

} // namespace mbmff
