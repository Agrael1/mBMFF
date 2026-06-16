#pragma once
#include "box_view.hpp"

namespace mbmff {

struct stsc_entry {
    std::uint32_t first_chunk;
    std::uint32_t samples_per_chunk;
    std::uint32_t sample_description_index;
};

struct stsc_data {
    std::uint32_t entry_count = 0;
    std::span<const std::byte> entries_data{};

    constexpr auto size() const noexcept -> std::uint32_t
    {
        return entry_count;
    }
    constexpr auto operator[](std::size_t index) const noexcept -> mbmff::stsc_entry
    {
        auto offset = index * 12;
        auto span = entries_data.subspan(offset, 12);
        return {
            mbmff::read_be<std::uint32_t>(span.subspan(0, 4)),
            mbmff::read_be<std::uint32_t>(span.subspan(4, 4)),
            mbmff::read_be<std::uint32_t>(span.subspan(8, 4)),
        };
    }
};

template <>
struct basic_box_view<mbmff::box_type::stsc> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::stsc_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::stsc>::validate(mbmff::any_box_view box) noexcept
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
    auto needed = std::size_t{4} + entry_count * 12;
    if (box.payload.size() < needed) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, needed);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::stsc>::value() const noexcept -> mbmff::stsc_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    auto entry_count = mbmff::read_be<std::uint32_t>(payload);
    return {entry_count, payload.subspan(4)};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::stsc>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(!mbmff::basic_box_view<mbmff::box_type::stsc>::validate(
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
    constexpr std::array<std::byte, 16> data{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x01},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x01},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x02},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x01},
    };
    mbmff::basic_box_view<mbmff::box_type::stsc> stsc;
    stsc.version_ = 0;
    stsc.payload = std::span(data);
    auto v = stsc.value();
    if (v.size() != 1) {
        return false;
    }
    return v[0].first_chunk == 1 && v[0].samples_per_chunk == 2 && v[0].sample_description_index == 1;
}());
#endif

} // namespace mbmff
