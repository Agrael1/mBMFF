#pragma once
#include "box_view.hpp"

namespace mbmff {

struct stts_entry {
    std::uint32_t sample_count;
    std::uint32_t sample_delta;
};

struct stts_data {
    std::uint32_t entry_count = 0;
    std::span<const std::byte> entries_data{};

    constexpr auto size() const noexcept -> std::uint32_t
    {
        return entry_count;
    }
    constexpr auto operator[](std::size_t index) const noexcept -> mbmff::stts_entry
    {
        auto offset = index * 8;
        if (offset + 8 > entries_data.size()) {
            return {};
        }
        auto span = entries_data.subspan(offset);
        return {
            mbmff::read_be<std::uint32_t>(span.subspan(0)),
            mbmff::read_be<std::uint32_t>(span.subspan(4)),
        };
    }
};

template <>
struct basic_box_view<mbmff::box_type::stts> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::stts_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::stts>::validate(mbmff::any_box_view box) noexcept
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

    auto entry_count = mbmff::read_be<std::uint32_t>(box.payload);
    auto needed = std::size_t{4} + entry_count * 8;
    if (box.payload.size() < needed) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, needed);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::stts>::value() const noexcept -> mbmff::stts_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    auto entry_count = mbmff::read_be<std::uint32_t>(payload);
    return {entry_count, payload.subspan(4)};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::stts>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(!mbmff::basic_box_view<mbmff::box_type::stts>::validate(
    {mbmff::box_header{},
     std::span<const std::byte>(
         std::array<std::byte, 8>{
             std::byte{0x00},
             std::byte{0x00},
             std::byte{0x00},
             std::byte{0x00},
             std::byte{0x00},
             std::byte{0x00},
             std::byte{0x00},
             std::byte{0x01},
         }
             .data(),
         8
     )}
));

static_assert([] {
    constexpr std::array<std::byte, 12> data{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x01},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x03},
        std::byte{0xe8},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x01},
    };
    mbmff::basic_box_view<mbmff::box_type::stts> stts;
    stts.version_ = 0;
    stts.payload = std::span(data);
    auto v = stts.value();
    if (v.size() != 1) {
        return false;
    }
    auto e = v[0];
    return e.sample_count == 1000 && e.sample_delta == 1;
}());
#endif

} // namespace mbmff
