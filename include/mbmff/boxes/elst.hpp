#pragma once
#include "box_view.hpp"

namespace mbmff {

struct elst_entry {
    std::uint64_t segment_duration;
    std::int64_t media_time;
    std::int16_t media_rate_integer;
    std::int16_t media_rate_fraction;
};

struct elst_data {
    std::uint32_t entry_count = 0;
    std::uint8_t version_ = 0;
    std::span<const std::byte> entries_data{};

    constexpr auto size() const noexcept -> std::uint32_t
    {
        return entry_count;
    }
    constexpr auto operator[](std::size_t index) const noexcept -> mbmff::elst_entry
    {
        auto entry_size = (version_ == 1) ? std::size_t{20} : std::size_t{12};
        auto offset = index * entry_size;
        if (offset + entry_size > entries_data.size()) {
            return {};
        }
        auto span = entries_data.subspan(offset);
        if (version_ == 1) {
            return {
                mbmff::read_be<std::uint64_t>(span.subspan(0)),
                static_cast<std::int64_t>(mbmff::read_be<std::uint64_t>(span.subspan(8))),
                static_cast<std::int16_t>(mbmff::read_be<std::uint16_t>(span.subspan(16))),
                static_cast<std::int16_t>(mbmff::read_be<std::uint16_t>(span.subspan(18))),
            };
        }
        return {
            mbmff::read_be<std::uint32_t>(span.subspan(0)),
            static_cast<std::int64_t>(static_cast<std::int32_t>(mbmff::read_be<std::uint32_t>(span.subspan(4)))),
            static_cast<std::int16_t>(mbmff::read_be<std::uint16_t>(span.subspan(8))),
            static_cast<std::int16_t>(mbmff::read_be<std::uint16_t>(span.subspan(10))),
        };
    }
};

template <>
struct basic_box_view<mbmff::box_type::elst> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::elst_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::elst>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }

    auto entry_size = (box.version() == 1) ? std::size_t{20} : std::size_t{12};
    auto entry_count = mbmff::read_be<std::uint32_t>(box.payload);
    auto needed = std::size_t{4} + entry_count * entry_size;
    if (box.payload.size() < needed) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, needed);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::elst>::value() const noexcept -> mbmff::elst_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    auto entry_count = mbmff::read_be<std::uint32_t>(payload);
    return {entry_count, version(), payload.subspan(4)};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::elst>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

// validate must reject when entry_count=1 but payload too small for one entry
static_assert(!mbmff::basic_box_view<mbmff::box_type::elst>::validate(
    {mbmff::box_header{},
     std::span<const std::byte>(
         std::array<std::byte, 8>{
             std::byte{0x00},
             std::byte{0x00},
             std::byte{0x00},
             std::byte{0x00}, // full header: v0, flags=0
             std::byte{0x00},
             std::byte{0x00},
             std::byte{0x00},
             std::byte{0x01}, // entry_count=1
         }
             .data(),
         8
     )}
));

// value with 1 version 0 entry (from ISOBMFF test)
static_assert([] {
    mbmff::basic_box_view<mbmff::box_type::elst> elst;
    elst.version_ = 0;
    constexpr std::array<std::byte, 16> data{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x01},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x18},
        std::byte{0xc8},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x01},
        std::byte{0x00},
        std::byte{0x00},
    };
    elst.payload = std::span(data);
    auto d = elst.value();
    if (d.size() != 1) {
        return false;
    }
    auto e = d[0];
    return e.segment_duration == 6344 && e.media_time == 0 && e.media_rate_integer == 1 && e.media_rate_fraction == 0;
}());

// value with 2 version 0 entries (second entry has negative media_time)
static_assert([] {
    mbmff::basic_box_view<mbmff::box_type::elst> elst;
    elst.version_ = 0;
    constexpr std::array<std::byte, 28> data{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x02}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x03}, std::byte{0xe8}, std::byte{0xff}, std::byte{0xff}, std::byte{0xff}, std::byte{0xff},
        std::byte{0x00}, std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x13}, std::byte{0x88}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01}, std::byte{0xf4},
        std::byte{0x00}, std::byte{0x02}, std::byte{0x80}, std::byte{0x00},
    };
    elst.payload = std::span(data);
    auto d = elst.value();
    if (d.size() != 2) {
        return false;
    }
    auto e0 = d[0];
    if (e0.segment_duration != 1000 || e0.media_time != -1) {
        return false;
    }
    auto e1 = d[1];
    return e1.segment_duration == 5000 && e1.media_time == 500 && e1.media_rate_integer == 2
        && e1.media_rate_fraction == static_cast<std::int16_t>(0x8000);
}());
#endif

} // namespace mbmff
