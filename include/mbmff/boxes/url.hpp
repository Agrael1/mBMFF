#pragma once
#include "box_view.hpp"

namespace mbmff {

struct url_data {
    std::string_view location{};
};

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
    if ((flags() & 0x000001) || payload.empty()) {
        return {};
    }
    auto location = mbmff::read_cstr(payload, 0);
    return {location.value};
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

// url with flag 0x000001 - no location string
static_assert([] {
    constexpr std::array<std::byte, 4> data{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x01},
    };
    mbmff::basic_box_view<mbmff::box_type::url> url_box;
    url_box.version_ = 0;
    url_box.flags_[2] = std::uint8_t{1};
    url_box.payload = std::span(data).subspan(4);
    auto v = url_box.value();
    return v.location.empty();
}());
#endif

} // namespace mbmff
