#pragma once
#include "box_view.hpp"

namespace mbmff {

struct fiel_data {
    std::uint16_t field_count;
    std::uint8_t field_details;
};

template <>
struct mbmff::basic_box_view<mbmff::box_type::fiel> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::fiel_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::fiel>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 2) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 2);
    }
    auto field_count = mbmff::read_be<std::uint16_t>(box.payload);
    if (field_count == 2 && box.payload.size() < 3) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 3);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::fiel>::value() const noexcept -> mbmff::fiel_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    auto field_count = mbmff::read_be<std::uint16_t>(payload);
    auto field_details = (field_count == 2 && payload.size() >= 3) ? static_cast<std::uint8_t>(payload[2])
                                                                   : std::uint8_t{0};
    return {field_count, field_details};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::fiel>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::fiel>::validate(
    {mbmff::box_header{},
     std::span<const std::byte>(
         std::array<std::byte, 2>{
             std::byte{0x00},
             std::byte{0x01},
         }
             .data(),
         2
     )}
)));

static_assert([] {
    constexpr std::array<std::byte, 2> data{
        std::byte{0x00},
        std::byte{0x01},
    };
    mbmff::basic_box_view<mbmff::box_type::fiel> fiel;
    fiel.payload = std::span(data);
    auto v = fiel.value();
    return v.field_count == 1 && v.field_details == 0;
}());
#endif

} // namespace mbmff
