#pragma once
#include "box_view.hpp"

namespace mbmff {
template <>
struct basic_box_view<mbmff::box_type::mdat> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
};

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
// mdat always accepts any payload
static_assert(
    mbmff::basic_box_view<mbmff::box_type::mdat>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);
#endif

} // namespace mbmff
