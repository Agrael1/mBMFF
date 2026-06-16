#pragma once
#include "box_view.hpp"

namespace mbmff {

struct iinf_data {
    std::uint32_t entry_count = 0;
};

template <>
struct basic_box_view<mbmff::box_type::iinf> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box
                                                      | mbmff::box_properties::container;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::iinf_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::iinf>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    auto entry_size = (box.version() == 0) ? std::size_t{2} : std::size_t{4};
    if (box.payload.size() < entry_size) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, entry_size);
    }
    box.payload = box.payload.subspan(entry_size);
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::iinf>::value() const noexcept -> mbmff::iinf_data
{
    mbmff::iinf_data result{};
    auto entry_size = (version() == 0) ? std::size_t{2} : std::size_t{4};
    if (payload.size() == 0 && payload.data() == nullptr) {
        return result;
    }
    auto* data = payload.data() - static_cast<std::ptrdiff_t>(entry_size);
    if (version() == 0) {
        result.entry_count = mbmff::read_be<std::uint16_t>(std::span<const std::byte>(data, 2));
    } else {
        result.entry_count = mbmff::read_be<std::uint32_t>(std::span<const std::byte>(data, 4));
    }
    return result;
}

} // namespace mbmff
