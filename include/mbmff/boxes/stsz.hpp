#pragma once
#include "box_view.hpp"

namespace mbmff {

struct stsz_data {
    std::uint32_t sample_size = 0;
    std::uint32_t sample_count = 0;
    std::span<const std::byte> entries_data{};

    constexpr auto size() const noexcept -> std::uint32_t
    {
        return sample_count;
    }
    constexpr auto operator[](std::size_t index) const noexcept -> std::uint32_t
    {
        if (sample_size != 0) {
            return sample_size;
        }
        auto offset = index * 4;
        if (offset + 4 > entries_data.size()) {
            return 0;
        }
        return mbmff::read_be<std::uint32_t>(entries_data.subspan(offset));
    }
};

template <>
struct mbmff::basic_box_view<mbmff::box_type::stsz> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::stsz_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::stsz>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    if (box.payload.size() < 8) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 8);
    }

    auto sample_size = mbmff::read_be<std::uint32_t>(box.payload);
    auto sample_count = mbmff::read_be<std::uint32_t>(box.payload.subspan(4));
    auto needed = std::size_t{8} + (sample_size == 0 ? sample_count * 4 : 0);
    if (box.payload.size() < needed) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, needed);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::stsz>::value() const noexcept -> mbmff::stsz_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    auto sample_size = mbmff::read_be<std::uint32_t>(payload);
    auto sample_count = mbmff::read_be<std::uint32_t>(payload.subspan(4));
    auto entries_data = sample_size == 0 ? payload.subspan(8) : std::span<const std::byte>{};
    return {sample_size, sample_count, entries_data};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::stsz>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(!mbmff::basic_box_view<mbmff::box_type::stsz>::validate(
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
    constexpr std::array<std::byte, 8> data{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x64},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x05},
    };
    mbmff::basic_box_view<mbmff::box_type::stsz> stsz;
    stsz.version_ = 0;
    stsz.payload = std::span(data);
    auto v = stsz.value();
    if (v.size() != 5) {
        return false;
    }
    if (v[0] != 100 || v[4] != 100) {
        return false;
    }
    return true;
}());

// variable sample_size == 0, entry table
static_assert([] {
    constexpr std::array<std::byte, 16> data{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x02},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x0A},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x14},
    };
    mbmff::basic_box_view<mbmff::box_type::stsz> stsz;
    stsz.version_ = 0;
    stsz.payload = std::span(data);
    auto v = stsz.value();
    if (v.size() != 2) {
        return false;
    }
    return v[0] == 10 && v[1] == 20;
}());
#endif

} // namespace mbmff
