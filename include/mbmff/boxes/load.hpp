#pragma once
#include "box_view.hpp"

namespace mbmff {

struct load_data {
    std::int32_t preload_time;
    std::int32_t preload_size;
    std::uint8_t default_loading;
    std::int32_t duration_to_load;
};

template <>
struct mbmff::basic_box_view<mbmff::box_type::load> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::load_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::load>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 16) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 16);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::load>::value() const noexcept -> mbmff::load_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    return {
        .preload_time = mbmff::read_be<std::int32_t>(payload),
        .preload_size = mbmff::read_be<std::int32_t>(payload.subspan(4)),
        .default_loading = static_cast<std::uint8_t>(payload[8]),
        .duration_to_load = mbmff::read_be<std::int32_t>(payload.subspan(12)),
    };
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::load>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::load>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 16>{}.data(), 16)}
)));

static_assert([] {
    constexpr std::array<std::byte, 16> data{
        std::byte{0xFF},
        std::byte{0xFF},
        std::byte{0xFF},
        std::byte{0xFF},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x64},
        std::byte{0x01},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x32},
    };
    mbmff::basic_box_view<mbmff::box_type::load> load;
    load.payload = std::span(data);
    auto v = load.value();
    return v.preload_time == -1 && v.preload_size == 100 && v.default_loading == 1 && v.duration_to_load == 50;
}());
#endif

} // namespace mbmff
