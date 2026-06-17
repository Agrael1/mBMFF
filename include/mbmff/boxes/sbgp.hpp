#pragma once
#include "box_view.hpp"

namespace mbmff {

struct sbgp_entry {
    std::uint32_t sample_count;
    std::uint32_t group_description_index;
};

struct sbgp_data {
    std::uint32_t grouping_type;
    std::uint32_t grouping_type_parameter;
    std::uint32_t entry_count = 0;
    std::span<const std::byte> entries_data{};

    constexpr auto size() const noexcept -> std::uint32_t
    {
        return entry_count;
    }
    constexpr auto operator[](std::size_t index) const noexcept -> mbmff::sbgp_entry
    {
        auto offset = index * 8;
        if (offset + 8 > entries_data.size()) {
            return {};
        }
        return {
            mbmff::read_be<std::uint32_t>(entries_data.subspan(offset)),
            mbmff::read_be<std::uint32_t>(entries_data.subspan(offset + 4)),
        };
    }
};

template <>
struct basic_box_view<mbmff::box_type::sbgp> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::sbgp_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::sbgp>::validate(mbmff::any_box_view box) noexcept
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
    auto grouping_type = mbmff::read_be<std::uint32_t>(box.payload);
    (void)grouping_type;

    auto header_size = std::size_t{4};
    if (box.version() >= 1) {
        header_size += 4;
    }
    if (box.payload.size() < header_size + 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, header_size + 4);
    }
    auto entry_count = mbmff::read_be<std::uint32_t>(box.payload.subspan(header_size));
    auto needed = header_size + 4 + entry_count * 8;
    if (box.payload.size() < needed) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, needed);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::sbgp>::value() const noexcept -> mbmff::sbgp_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    auto grouping_type = mbmff::read_be<std::uint32_t>(payload);
    auto header_size = std::size_t{4};
    std::uint32_t grouping_type_parameter = 0;
    if (version() >= 1) {
        grouping_type_parameter = mbmff::read_be<std::uint32_t>(payload.subspan(4));
        header_size += 4;
    }
    auto entry_count = mbmff::read_be<std::uint32_t>(payload.subspan(header_size));
    return {grouping_type, grouping_type_parameter, entry_count, payload.subspan(header_size + 4)};
}

} // namespace mbmff
