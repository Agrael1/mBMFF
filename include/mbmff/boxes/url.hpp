#pragma once
#include "box_view.hpp"

namespace mbmff {

struct url_data {};

template <>
struct basic_box_view<mbmff::box_type::url> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::url_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::url>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::url>::value() const noexcept -> mbmff::url_data
{
    return {};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::url>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(!mbmff::basic_box_view<mbmff::box_type::url>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 3>{}.data(), 3)}
));

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::url>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 4>{}.data(), 4)}
)));
#endif

} // namespace mbmff
