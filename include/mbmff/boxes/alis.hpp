#pragma once
#include "box_view.hpp"

namespace mbmff {

struct alis_data {};

template <>
struct basic_box_view<mbmff::box_type::alis> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::alis_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::alis>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    (void)box.payload.subspan(0);
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::alis>::value() const noexcept -> mbmff::alis_data
{
    return {};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::alis>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);
#endif

} // namespace mbmff
