#pragma once
#include "box_view.hpp"

namespace mbmff {

struct wide_data {};

template <>
struct basic_box_view<mbmff::box_type::wide> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
};

} // namespace mbmff
