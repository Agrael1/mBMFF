#pragma once
#include "box_view.hpp"

namespace mbmff {

struct iloc_extent {
    std::uint64_t index = 0;
    std::uint64_t offset = 0;
    std::uint64_t length = 0;
};

struct iloc_item {
    std::uint64_t base_offset = 0;
    std::uint32_t item_id = 0;

    std::uint8_t iloc_version = 0;
    std::uint8_t offset_size = 0;
    std::uint8_t length_size = 0;
    std::uint8_t index_size = 0;

    std::uint8_t construction_method = 0;

    std::uint16_t data_reference_index = 0;
    std::uint32_t extent_count = 0;

    std::span<const std::byte> extent_data{};

public:
    constexpr auto get_extent(std::size_t index) const noexcept -> mbmff::iloc_extent;
    constexpr auto size() const noexcept -> std::size_t
    {
        return extent_count;
    }
    constexpr auto operator[](std::size_t index) const noexcept -> mbmff::iloc_extent
    {
        return get_extent(index);
    }
    constexpr auto extent_size() const noexcept -> std::size_t
    {
        return std::size_t(offset_size) + length_size + index_size;
    }
};

constexpr auto mbmff::iloc_item::get_extent(std::size_t index) const noexcept -> mbmff::iloc_extent
{
    auto entry_span = extent_data.subspan(index * extent_size(), extent_size());
    mbmff::iloc_extent extent{};
    std::uint32_t offset = 0;

    if ((iloc_version == 1 || iloc_version == 2) && index_size > 0) {
        if (index_size == 2) {
            extent.index = mbmff::read_be<std::uint16_t>(entry_span);
        } else if (index_size == 4) {
            extent.index = mbmff::read_be<std::uint32_t>(entry_span);
        } else if (index_size == 8) {
            extent.index = mbmff::read_be<std::uint64_t>(entry_span);
        }
        offset += index_size;
    }

    if (offset_size == 2) {
        extent.offset = mbmff::read_be<std::uint16_t>(entry_span.subspan(offset));
    } else if (offset_size == 4) {
        extent.offset = mbmff::read_be<std::uint32_t>(entry_span.subspan(offset));
    } else if (offset_size == 8) {
        extent.offset = mbmff::read_be<std::uint64_t>(entry_span.subspan(offset));
    }
    offset += offset_size;

    if (length_size == 2) {
        extent.length = mbmff::read_be<std::uint16_t>(entry_span.subspan(offset));
    } else if (length_size == 4) {
        extent.length = mbmff::read_be<std::uint32_t>(entry_span.subspan(offset));
    } else if (length_size == 8) {
        extent.length = mbmff::read_be<std::uint64_t>(entry_span.subspan(offset));
    }
    return extent;
}

struct iloc_data {
    std::uint8_t offset_size = 0;
    std::uint8_t length_size = 0;
    std::uint8_t base_offset_size = 0;
    std::uint8_t index_size = 0;
    std::uint32_t item_count = 0;
    std::span<const std::byte> item_data{};
};

template <>
struct basic_box_view<mbmff::box_type::iloc> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::iloc_data;
};

inline constexpr auto mbmff::basic_box_view<box_type::iloc>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    if (box.payload.size() < 2) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 2);
    }
    auto entry_size = (box.version() < 2) ? std::size_t{2} : std::size_t{4};
    if (box.payload.size() < 2 + entry_size) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 2 + entry_size);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<box_type::iloc>::value() const noexcept -> mbmff::iloc_data
{
    mbmff::iloc_data result{};
    if (payload.size() < 2) return result;

    auto first_byte = static_cast<std::uint8_t>(payload[0]);
    result.offset_size = (first_byte >> 4) & 0x0F;
    result.length_size = first_byte & 0x0F;
    auto second_byte = static_cast<std::uint8_t>(payload[1]);
    result.base_offset_size = (second_byte >> 4) & 0x0F;
    result.index_size = second_byte & 0x0F;

    auto data = payload.subspan(2);
    if (version() < 2) {
        result.item_count = mbmff::read_be<std::uint16_t>(data);
        result.item_data = data.subspan(2);
    } else {
        result.item_count = mbmff::read_be<std::uint32_t>(data);
        result.item_data = data.subspan(4);
    }
    return result;
}

class iloc_item_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = mbmff::iloc_item;
    using difference_type = std::ptrdiff_t;

private:
    std::span<const std::byte> remaining_;
    std::uint8_t iloc_version_ = 0;
    std::uint8_t offset_size_ = 0;
    std::uint8_t length_size_ = 0;
    std::uint8_t index_size_ = 0;
    std::uint8_t base_offset_size_ = 0;

public:
    constexpr iloc_item_iterator() noexcept = default;
    constexpr iloc_item_iterator(const mbmff::iloc_data& header, std::uint8_t version) noexcept
        : remaining_(header.item_data)
        , iloc_version_(version)
        , offset_size_(header.offset_size)
        , length_size_(header.length_size)
        , index_size_(header.index_size)
        , base_offset_size_(header.base_offset_size)
    {}
    constexpr explicit iloc_item_iterator(const mbmff::basic_box_view<mbmff::box_type::iloc>& box) noexcept
        : iloc_item_iterator(box.value(), box.version())
    {}

public:
    constexpr auto extent_size() const noexcept -> std::size_t
    {
        return std::size_t(offset_size_) + length_size_ + (index_size_ > 0 ? index_size_ : 0);
    }
    constexpr auto begin() const noexcept -> mbmff::iloc_item_iterator
    {
        return *this;
    }
    constexpr auto end() const noexcept -> mbmff::iloc_item_iterator
    {
        return {};
    }
    constexpr auto get() const noexcept -> mbmff::iloc_item
    {
        mbmff::iloc_item item{};
        if (remaining_.empty()) {
            return item;
        }

        std::size_t offset = 0;

        if (iloc_version_ < 2) {
            item.item_id = mbmff::read_be<std::uint16_t>(remaining_);
            offset += 2;
        } else {
            item.item_id = mbmff::read_be<std::uint32_t>(remaining_);
            offset += 4;
        }

        if (iloc_version_ == 1 || iloc_version_ == 2) {
            auto value = mbmff::read_be<std::uint16_t>(remaining_.subspan(offset));
            item.construction_method = static_cast<std::uint8_t>(value & 0x000F);
            offset += 2;
        }

        item.data_reference_index = mbmff::read_be<std::uint16_t>(remaining_.subspan(offset));
        offset += 2;

        switch (base_offset_size_) {
        default:
        case 0:
            item.base_offset = 0;
            break;
        case 2:
            item.base_offset = mbmff::read_be<std::uint16_t>(remaining_.subspan(offset));
            break;
        case 4:
            item.base_offset = mbmff::read_be<std::uint32_t>(remaining_.subspan(offset));
            break;
        case 8:
            item.base_offset = mbmff::read_be<std::uint64_t>(remaining_.subspan(offset));
            break;
        }
        offset += base_offset_size_;

        item.extent_count = mbmff::read_be<std::uint16_t>(remaining_.subspan(offset));
        item.extent_data = remaining_.subspan(offset + 2);

        item.iloc_version = iloc_version_;
        item.offset_size = offset_size_;
        item.length_size = length_size_;
        item.index_size = index_size_;
        return item;
    }

    constexpr auto operator*() const noexcept -> mbmff::iloc_item
    {
        return get();
    }

    constexpr auto operator++() noexcept -> mbmff::iloc_item_iterator&
    {
        if (remaining_.empty()) {
            remaining_ = {};
            return *this;
        }
        auto current_item = get();
        const std::byte* data = current_item.extent_data.data();
        std::size_t total_extent_size = current_item.extent_count * extent_size();

        remaining_ = {data + total_extent_size,
                      remaining_.size() - (data - remaining_.data()) - total_extent_size};

        return *this;
    }

    constexpr auto operator++(int) noexcept -> mbmff::iloc_item_iterator
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }
    constexpr bool operator==(const mbmff::iloc_item_iterator& other) const noexcept
    {
        if (remaining_.empty() && other.remaining_.empty()) {
            return true;
        }
        return remaining_.data() == other.remaining_.data()
            && remaining_.size() == other.remaining_.size();
    }
};

} // namespace mbmff
