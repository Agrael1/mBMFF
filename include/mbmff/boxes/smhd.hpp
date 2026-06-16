#pragma once
#include "box_view.hpp"

namespace mbmff {

struct smhd_data {
    std::int16_t balance = 0;
};

template <>
struct basic_box_view<mbmff::box_type::smhd> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::smhd_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::smhd>::validate(mbmff::any_box_view box) noexcept
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

    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::smhd>::value() const noexcept -> mbmff::smhd_data
{
    return {static_cast<std::int16_t>(mbmff::read_be<std::uint16_t>(payload))};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::smhd>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::smhd>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 8>{}.data(), 8)}
)));

static_assert([] {
    constexpr std::array<std::byte, 8> data{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00}, // full header
        std::byte{0x00},
        std::byte{0x00}, // balance = 0
        std::byte{0x00},
        std::byte{0x00}, // reserved
    };
    mbmff::basic_box_view<mbmff::box_type::smhd> smhd;
    smhd.version_ = 0;
    smhd.payload = std::span(data).subspan(4);
    auto v = smhd.value();
    return v.balance == 0;
}());
#endif

} // namespace mbmff
