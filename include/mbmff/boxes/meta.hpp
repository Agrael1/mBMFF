#pragma once
#include "box_view.hpp"

namespace mbmff {
template <>
struct mbmff::basic_box_view<mbmff::box_type::meta> : public mbmff::box_view_base {
    // full_box is not ticked here because `meta` can
    // be a regular box in some formats (e.g. QTFF)
    constexpr static mbmff::box_properties properties = mbmff::box_properties::container;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::meta>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    constexpr auto hdlr_type = "hdlr";
    bool has_full_box = false;

    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }

    for (int i = 0; i < 4; ++i) {
        if (box.payload[i] != std::byte(hdlr_type[i])) {
            has_full_box = true;
            break;
        }
    }

    if (has_full_box) {
        box.fill_full_header(box.payload);
        box.payload = box.payload.subspan(4);
    }
    return {box};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
// payload starting with "hdlr" → not full_box → passes validation
static_assert([] {
    constexpr std::byte no_fullbox[4]{std::byte{'h'}, std::byte{'d'}, std::byte{'l'}, std::byte{'r'}};
    auto r = mbmff::basic_box_view<mbmff::box_type::meta>::validate({mbmff::box_header{}, std::span(no_fullbox)});
    return static_cast<bool>(r);
}());
#endif

} // namespace mbmff
