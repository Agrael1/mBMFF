#pragma once
#include "box_view.hpp"

namespace mbmff {

struct dref_data {
    std::uint32_t entry_count = 0;
};

template <>
struct basic_box_view<mbmff::box_type::dref> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box
                                                      | mbmff::box_properties::container;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::dref_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::dref>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }

    auto entry_count = mbmff::read_be<std::uint32_t>(box.payload);
    box.payload = box.payload.subspan(4);
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::dref>::value() const noexcept -> mbmff::dref_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    return {mbmff::read_be<std::uint32_t>(std::span<const std::byte>(payload.data() - 4, 4))};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::dref>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(!mbmff::basic_box_view<mbmff::box_type::dref>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 4>{std::byte{0x00}}.data(), 4)}
));

static_assert([] {
    constexpr std::array<std::byte, 12> data{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00}, // full header: v=0, f=0
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x01}, // entry_count = 1
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x0c}, // child box header start
    };
    // Simulate what validate does: eat full header and entry_count
    mbmff::basic_box_view<mbmff::box_type::dref> dref;
    dref.version_ = 0;
    dref.payload = std::span(data).subspan(8);
    auto v = dref.value();
    return v.entry_count == 1;
}());
#endif

} // namespace mbmff
