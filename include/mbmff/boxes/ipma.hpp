#pragma once
#include "box_view.hpp"

namespace mbmff {

struct ipma_association {
    std::uint16_t essential      : 1 = false;
    std::uint16_t property_index : 15 = 0;
};

struct ipma_entry {
    std::uint32_t item_id = 0;
    std::uint8_t entry_count = 0;
    std::uint8_t entry_size = 0;
    std::span<const std::byte> association_values{};

public:
    constexpr auto operator[](std::size_t index) const noexcept -> mbmff::ipma_association;
    constexpr auto size() const noexcept -> std::size_t
    {
        return entry_count;
    }
};

constexpr auto ipma_entry::operator[](std::size_t index) const noexcept -> mbmff::ipma_association
{
    if (index >= entry_count) {
        return {};
    }
    mbmff::ipma_association association;
    if (entry_size == 1) {
        auto value = static_cast<std::uint8_t>(association_values[index]);
        association.essential = (value >> 7) & 0x01;
        association.property_index = value & 0x7F;
    } else {
        auto value = mbmff::read_be<std::uint16_t>(association_values.subspan(index * 2, 2));
        association.essential = (value >> 15) & 0x01;
        association.property_index = value & 0x7FFF;
    }
    return association;
}

struct ipma_data {
    std::uint32_t entry_count = 0;
    std::uint8_t index_size = 2;
    std::uint8_t asoc_size = 1;
    std::span<const std::byte> entry_data{};
};

template <>
struct basic_box_view<mbmff::box_type::ipma> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::ipma_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::ipma>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);

    // entry_count (4 bytes) + at least one entry
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::ipma>::value() const noexcept -> mbmff::ipma_data
{
    mbmff::ipma_data result{};
    result.entry_count = mbmff::read_be<std::uint32_t>(payload);
    result.index_size = (version() == 0) ? std::uint8_t{2} : std::uint8_t{4};
    result.asoc_size = (flags() & 0x01) ? std::uint8_t{2} : std::uint8_t{1};
    result.entry_data = payload.subspan(4);
    return result;
}

class ipma_entry_iterator
{
    using iterator_category = std::forward_iterator_tag;
    using value_type = mbmff::ipma_entry;
    using difference_type = std::ptrdiff_t;

private:
    std::uint32_t entry_count_ = 0;
    std::uint8_t index_size_ = 0;
    std::uint8_t asoc_size_ = 0;
    std::span<const std::byte> remaining_;

public:
    constexpr ipma_entry_iterator() noexcept = default;
    constexpr explicit ipma_entry_iterator(const mbmff::ipma_data& data) noexcept
        : entry_count_(data.entry_count)
        , index_size_(data.index_size)
        , asoc_size_(data.asoc_size)
        , remaining_(data.entry_data)
    {}
    constexpr explicit ipma_entry_iterator(const mbmff::basic_box_view<mbmff::box_type::ipma>& box) noexcept
        : ipma_entry_iterator(box.value())
    {}

public:
    constexpr auto begin() const noexcept -> mbmff::ipma_entry_iterator
    {
        return *this;
    }
    constexpr auto end() const noexcept -> mbmff::ipma_entry_iterator
    {
        return {};
    }
    constexpr auto get() const noexcept -> mbmff::ipma_entry
    {
        mbmff::ipma_entry entry{};
        if (entry_count_ == 0 || remaining_.empty()) {
            return entry;
        }
        if (index_size_ == 2) {
            entry.item_id = mbmff::read_be<std::uint16_t>(remaining_);
            entry.association_values = remaining_.subspan(2);
        } else {
            entry.item_id = mbmff::read_be<std::uint32_t>(remaining_);
            entry.association_values = remaining_.subspan(4);
        }

        entry.entry_count = static_cast<std::uint8_t>(entry.association_values[0]);
        entry.association_values = entry.association_values.subspan(1);
        entry.entry_size = asoc_size_;
        return entry;
    }
    constexpr auto operator*() const noexcept -> mbmff::ipma_entry
    {
        return get();
    }
    constexpr auto operator++() noexcept -> mbmff::ipma_entry_iterator&
    {
        if (entry_count_ == 0 || remaining_.empty()) {
            entry_count_ = 0;
            remaining_ = {};
            return *this;
        }
        entry_count_--;
        auto current_entry = get();

        std::size_t offset = static_cast<std::size_t>(current_entry.entry_count) * current_entry.entry_size
                           + index_size_ + 1;

        remaining_ = (offset >= remaining_.size()) ? std::span<const std::byte>{} : remaining_.subspan(offset);
        return *this;
    }
    constexpr auto operator++(int) noexcept -> mbmff::ipma_entry_iterator
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }
    constexpr bool operator==(const mbmff::ipma_entry_iterator& other) const noexcept
    {
        if (entry_count_ == 0 && other.entry_count_ == 0) {
            return true;
        }
        return entry_count_ == other.entry_count_ && remaining_.data() == other.remaining_.data()
            && remaining_.size() == other.remaining_.size();
    }
};

} // namespace mbmff
