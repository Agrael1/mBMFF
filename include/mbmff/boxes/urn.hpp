#pragma once
#include "box_view.hpp"

namespace mbmff {

struct urn_data {
    mbmff::byte_view name{};
    mbmff::byte_view location{};
};

template <>
struct basic_box_view<mbmff::box_type::urn> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::urn_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::urn>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::urn>::value() const noexcept -> mbmff::urn_data
{
    std::size_t offset = 0;
    auto name = mbmff::byte_view::from_c_str(payload, offset);
    offset += name.size() + 1;
    auto location = mbmff::byte_view::from_c_str(payload, offset);
    return {name, location};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::urn>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::urn>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 4>{}.data(), 4)}
)));

// urn with empty payload
static_assert([] {
    mbmff::basic_box_view<mbmff::box_type::urn> urn_box;
    urn_box.version_ = 0;
    urn_box.payload = {};
    auto v = urn_box.value();
    return v.name.empty() && v.location.empty();
}());
#endif

} // namespace mbmff
