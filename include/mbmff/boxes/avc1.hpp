#pragma once
#include "box_view.hpp"

namespace mbmff {

struct avc1_data {
    std::uint16_t data_reference_index;
    std::uint16_t width;
    std::uint16_t height;
    std::uint32_t horizresolution;
    std::uint32_t vertresolution;
    std::uint16_t frame_count;
    mbmff::byte_view compressorname{};
    std::uint16_t depth;
};

template <>
struct basic_box_view<mbmff::box_type::avc1> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::container;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::avc1_data;
};

inline constexpr auto basic_box_view<mbmff::box_type::avc1>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 78) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 78);
    }
    box.payload = box.payload.subspan(78);
    return {box};
}

inline constexpr auto basic_box_view<mbmff::box_type::avc1>::value() const noexcept -> mbmff::avc1_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    auto span = std::span<const std::byte>(payload.data() - 78, 78);
    return {
        mbmff::read_be<std::uint16_t>(span.subspan(6)),
        mbmff::read_be<std::uint16_t>(span.subspan(24)),
        mbmff::read_be<std::uint16_t>(span.subspan(26)),
        mbmff::read_be<std::uint32_t>(span.subspan(28)),
        mbmff::read_be<std::uint32_t>(span.subspan(32)),
        mbmff::read_be<std::uint16_t>(span.subspan(40)),
        mbmff::byte_view::from_pascal_str({span.subspan(42, 32)}),
        mbmff::read_be<std::uint16_t>(span.subspan(74)),
    };
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::avc1>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::avc1>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 78>{}.data(), 78)}
)));
#endif

} // namespace mbmff
