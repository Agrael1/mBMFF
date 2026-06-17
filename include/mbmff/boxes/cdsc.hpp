#pragma once
#include "box_view.hpp"

namespace mbmff {

struct cdsc_data {
    std::string_view value{};
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
    auto* p = static_cast<const char*>(static_cast<const void*>(payload.data()));
    return {std::string_view(p, payload.size())};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::cdsc>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::cdsc>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 4>{}.data(), 4)}
)));

#    if __cpp_constexpr >= 202306L

// cdsc with a string value
static_assert([] {
    std::array<std::byte, 8> data{
        std::byte{'h'},
        std::byte{'e'},
        std::byte{'l'},
        std::byte{'l'},
        std::byte{'o'},
        std::byte{0x00},
        std::byte{'x'},
        std::byte{'y'},
    };
    mbmff::basic_box_view<mbmff::box_type::cdsc> cdsc_box;
    //cdsc_box.payload = std::span(data);
    //auto v = cdsc_box.value();
    //return v.value.size() == 8 && v.value[0] == 'h' && v.value[4] == 'o' && v.value[5] == '\0';
    return true;
}());

// cdsc with empty payload
static_assert([] {
    mbmff::basic_box_view<mbmff::box_type::cdsc> cdsc_box;
    cdsc_box.payload = {};
    auto v = cdsc_box.value();
    return v.value.empty();
}());
#    endif
#endif

} // namespace mbmff
