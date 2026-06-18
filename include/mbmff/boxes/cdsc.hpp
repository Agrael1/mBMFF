#pragma once
#include "box_view.hpp"

namespace mbmff {

struct cdsc_data {
    std::span<const std::byte> value{};
};

template <>
struct basic_box_view<mbmff::box_type::cdsc> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::cdsc_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::cdsc>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::cdsc>::value() const noexcept -> mbmff::cdsc_data
{
    return {payload};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::cdsc>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::cdsc>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 4>{}.data(), 4)}
)));

// cdsc with empty payload
static_assert([] {
    mbmff::basic_box_view<mbmff::box_type::cdsc> cdsc_box;
    cdsc_box.payload = {};
    auto v = cdsc_box.value();
    return v.value.empty();
}());
#endif

} // namespace mbmff
