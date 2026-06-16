#pragma once
#include "box_view.hpp"

namespace mbmff {

struct mp4a_data {
    std::uint16_t data_reference_index;
    std::uint16_t channelcount;
    std::uint16_t samplesize;
    std::uint32_t samplerate;
};

template <>
struct basic_box_view<mbmff::box_type::mp4a> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::container;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::mp4a_data;
};

inline constexpr auto basic_box_view<mbmff::box_type::mp4a>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 28) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 28);
    }
    box.payload = box.payload.subspan(28);
    return {box};
}

inline constexpr auto basic_box_view<mbmff::box_type::mp4a>::value() const noexcept -> mbmff::mp4a_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    auto span = std::span<const std::byte>(payload.data() - 28, 28);
    return {
        mbmff::read_be<std::uint16_t>(span.subspan(6)), // data_reference_index
        mbmff::read_be<std::uint16_t>(span.subspan(16)), // channelcount
        mbmff::read_be<std::uint16_t>(span.subspan(18)), // samplesize
        mbmff::read_be<std::uint32_t>(span.subspan(24)), // samplerate
    };
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::mp4a>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::mp4a>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 28>{}.data(), 28)}
)));
#endif

} // namespace mbmff
