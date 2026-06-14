#pragma once
#include "box_view.hpp"

namespace mbmff {

struct pitm_data {
    std::uint32_t item_id = 0;
};

template <>
struct basic_box_view<mbmff::box_type::pitm> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::pitm_data;
};

inline constexpr auto basic_box_view<box_type::pitm>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    auto need = (box.version() == 0) ? std::size_t{2} : std::size_t{4};
    if (box.payload.size() < need) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, need);
    }
    return {box};
}

inline constexpr auto basic_box_view<box_type::pitm>::value() const noexcept -> mbmff::pitm_data
{
    mbmff::pitm_data result{};
    if (version() == 0) {
        result.item_id = mbmff::read_be<std::uint16_t>(payload);
    } else {
        result.item_id = mbmff::read_be<std::uint32_t>(payload);
    }
    return result;
}

} // namespace mbmff
