#pragma once
#include "box_view.hpp"

namespace mbmff {

struct frma_data {
    std::uint32_t data_format;
};

template <>
struct basic_box_view<mbmff::box_type::frma> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::frma_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::frma>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::frma>::value() const noexcept -> mbmff::frma_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    return {mbmff::read_be<std::uint32_t>(payload)};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(!mbmff::basic_box_view<mbmff::box_type::frma>::validate({mbmff::box_header{}, std::span<const std::byte>{}}));

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::frma>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 4>{}.data(), 4)}
)));
#endif

} // namespace mbmff
