#pragma once
#include <array>
#include "box_view.hpp"

namespace mbmff {

struct vmhd_data {
    std::uint16_t graphicsmode = 0;
    std::array<std::uint16_t, 3> opcolor{};
};

template <>
struct mbmff::basic_box_view<mbmff::box_type::vmhd> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::vmhd_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::vmhd>::validate(mbmff::any_box_view box) noexcept
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
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::vmhd>::value() const noexcept -> mbmff::vmhd_data
{
    mbmff::vmhd_data result{};
    if (payload.empty() && payload.data() == nullptr) {
        return result;
    }
    result.graphicsmode = mbmff::read_be<std::uint16_t>(payload.subspan(0));
    for (int i = 0; i < 3; ++i) {
        result.opcolor[i] = mbmff::read_be<std::uint16_t>(payload.subspan(2 + i * 2));
    }
    return result;
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::vmhd>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(!mbmff::basic_box_view<mbmff::box_type::vmhd>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 4>{std::byte{0x00}}.data(), 4)}
));

static_assert([] {
    constexpr std::array<std::byte, 8> data{
        std::byte{0x00},
        std::byte{0x00}, // graphicsmode = 0 (copy)
        std::byte{0x00},
        std::byte{0x00}, // opcolor[0] = 0
        std::byte{0x00},
        std::byte{0x00}, // opcolor[1] = 0
        std::byte{0x00},
        std::byte{0x00}, // opcolor[2] = 0
    };
    mbmff::basic_box_view<mbmff::box_type::vmhd> vmhd;
    vmhd.version_ = 0;
    vmhd.payload = std::span(data);
    auto v = vmhd.value();
    return v.graphicsmode == 0 && v.opcolor[0] == 0 && v.opcolor[1] == 0 && v.opcolor[2] == 0;
}());
#endif

} // namespace mbmff
