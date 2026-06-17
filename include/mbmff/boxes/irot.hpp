#pragma once
#include "box_view.hpp"

namespace mbmff {

struct irot_data {
    std::uint8_t angle;
};

template <>
struct basic_box_view<mbmff::box_type::irot> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::irot_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::irot>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 1) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 1);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::irot>::value() const noexcept -> mbmff::irot_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    return {static_cast<std::uint8_t>(static_cast<std::uint8_t>(payload[0]) & 0x3)};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(!mbmff::basic_box_view<mbmff::box_type::irot>::validate({mbmff::box_header{}, std::span<const std::byte>{}}));

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::irot>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 1>{}.data(), 1)}
)));
#endif

} // namespace mbmff
