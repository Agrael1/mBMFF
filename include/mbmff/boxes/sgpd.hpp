#pragma once
#include "box_view.hpp"

namespace mbmff {

struct sgpd_data {
    std::uint32_t grouping_type;
    std::uint32_t default_length = 0;
    std::uint32_t default_sample_description_index = 0;
    std::uint32_t entry_count = 0;
    std::span<const std::byte> entries_data{};

    constexpr auto size() const noexcept -> std::uint32_t
    {
        return entry_count;
    }
    constexpr auto entry_data(std::size_t index) const noexcept -> std::span<const std::byte>
    {
        auto offset = std::size_t{0};
        if (default_length == 0) {
            // variable-length entries with self-describing sizes
            for (std::size_t i = 0; i < index && offset < entries_data.size(); ++i) {
                if (offset + 4 > entries_data.size()) {
                    return {};
                }
                auto len = mbmff::read_be<std::uint32_t>(entries_data.subspan(offset));
                offset += 4 + len;
            }
            if (offset + 4 > entries_data.size()) {
                return {};
            }
            auto len = mbmff::read_be<std::uint32_t>(entries_data.subspan(offset));
            if (offset + 4 + len > entries_data.size()) {
                return {};
            }
            return entries_data.subspan(offset + 4, len);
        } else {
            // fixed-length entries
            offset = index * default_length;
            if (offset + default_length > entries_data.size()) {
                return {};
            }
            return entries_data.subspan(offset, default_length);
        }
    }
};

template <>
struct basic_box_view<mbmff::box_type::sgpd> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::sgpd_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::sgpd>::validate(mbmff::any_box_view box) noexcept
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
    // grouping_type
    auto header_size = std::size_t{4};
    if (box.version() >= 1) {
        header_size += 4; // default_length
    }
    if (box.version() >= 2) {
        header_size += 4; // default_sample_description_index
    }
    if (box.payload.size() < header_size + 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, header_size + 4);
    }
    // entry_count
    auto entry_count = mbmff::read_be<std::uint32_t>(box.payload.subspan(header_size));
    auto entries_offset = header_size + 4;
    if (box.version() >= 1) {
        auto default_length = mbmff::read_be<std::uint32_t>(box.payload.subspan(4));
        if (default_length == 0) {
            // variable-length entries: each entry self-describes its size
            auto offset = entries_offset;
            for (std::uint32_t i = 0; i < entry_count; ++i) {
                if (offset + 4 > box.payload.size()) {
                    return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, offset + 4);
                }
                auto entry_size = mbmff::read_be<std::uint32_t>(box.payload.subspan(offset));
                offset += 4 + entry_size;
                if (offset > box.payload.size()) {
                    return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, offset);
                }
            }
        } else {
            // fixed-length entries
            if (box.payload.size() < entries_offset + entry_count * default_length) {
                return mbmff::make_result<mbmff::any_box_view>(
                    mbmff::error_code::need_more_data,
                    entries_offset + entry_count * default_length
                );
            }
        }
    } else {
        // version 0: no default_length, each entry has fixed size unknown to us
        // accept any remaining data
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::sgpd>::value() const noexcept -> mbmff::sgpd_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    auto grouping_type = mbmff::read_be<std::uint32_t>(payload);
    auto header_size = std::size_t{4};
    std::uint32_t default_length = 0;
    if (version() >= 1) {
        default_length = mbmff::read_be<std::uint32_t>(payload.subspan(4));
        header_size += 4;
    }
    std::uint32_t default_sample_description_index = 0;
    if (version() >= 2) {
        default_sample_description_index = mbmff::read_be<std::uint32_t>(payload.subspan(header_size));
        header_size += 4;
    }
    auto entry_count = mbmff::read_be<std::uint32_t>(payload.subspan(header_size));
    return {
        grouping_type,
        default_length,
        default_sample_description_index,
        entry_count,
        payload.subspan(header_size + 4)
    };
}

} // namespace mbmff
