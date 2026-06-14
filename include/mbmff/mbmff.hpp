#pragma once
#include "boxes/ftyp.hpp"
#include "boxes/meta.hpp"
#include "boxes/mdat.hpp"
#include "boxes/containers.hpp"
#include "boxes/av1C.hpp"
#include "boxes/iinf.hpp"
#include "boxes/iref.hpp"
#include "boxes/iloc.hpp"
#include "boxes/hdlr.hpp"
#include "boxes/pitm.hpp"
#include "boxes/ispe.hpp"
#include "boxes/pixi.hpp"
#include "boxes/pasp.hpp"
#include "boxes/ipma.hpp"
#include "boxes/infe.hpp"

namespace mbmff {

#define MBMFF_ITERATE_USING(name) using name##_box = basic_box_view<box_type::name>;
MBMFF_ITERATE_BOX_TYPES(MBMFF_ITERATE_USING)
#undef MBMFF_ITERATE_USING

inline constexpr auto get_box_properties(mbmff::box_type type) noexcept -> mbmff::box_properties
{
#define MBMFF_CASE_PROP(name) \
    case mbmff::box_type::name: return mbmff::basic_box_view<mbmff::box_type::name>::properties;
    switch (type) {
        MBMFF_ITERATE_BOX_TYPES(MBMFF_CASE_PROP)
    default:
        return mbmff::box_properties::none;
    }
#undef MBMFF_CASE_PROP
}

inline constexpr auto parse(std::span<const std::byte> data) noexcept -> mbmff::result<mbmff::any_box_view>
{
    auto header_result = mbmff::parse_box_header(data);
    if (!header_result) {
        return mbmff::make_result<mbmff::any_box_view>(header_result.code, header_result.needed);
    }
    auto header = header_result->value;
    auto header_size = header_result->consumed;

    if (header.size() != 0 && data.size() < header.size()) {
        return mbmff::make_result<mbmff::any_box_view>(
            mbmff::error_code::need_more_data,
            static_cast<std::size_t>(header.size())
        );
    }
    if (header.size() != 0 && header.size() < header_size) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::invalid_format);
    }

    auto payload_size = (header.size() == 0) ? (data.size() - header_size)
                                             : (static_cast<std::size_t>(header.size()) - header_size);
    auto payload_span = data.subspan(header_size, payload_size);

    mbmff::any_box_view box{{header.size_, header.type_}, payload_span};

#define MBMFF_CASE_VALIDATE(name) \
    case mbmff::box_type::name:   \
        return mbmff::basic_box_view<mbmff::box_type::name>::validate(box);

    switch (header.type_) {
        MBMFF_ITERATE_BOX_TYPES(MBMFF_CASE_VALIDATE)
    default:
        return {box};
    }
#undef MBMFF_CASE_VALIDATE
}

template <mbmff::box_type Box>
constexpr auto box_cast(const mbmff::any_box_view& box) noexcept -> mbmff::basic_box_view<Box>
{
    if (box.type() != Box) {
        return {};
    }
    return static_cast<mbmff::basic_box_view<Box>>(box);
}

enum class iterator_flags : std::uint32_t {
    none = 0,
    recursive = 1 << 0,
};
MBMFF_FLAG_OPERATORS(iterator_flags)

struct box_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = mbmff::result<mbmff::any_box_view>;
    using difference_type = std::ptrdiff_t;

private:
    std::span<const std::byte> remaining_;
    mbmff::iterator_flags flags_ = mbmff::iterator_flags::none;

public:
    constexpr box_iterator() noexcept = default;
    constexpr explicit box_iterator(
        std::span<const std::byte> data,
        mbmff::iterator_flags flags = mbmff::iterator_flags::none
    ) noexcept
        : remaining_(data)
        , flags_(flags)
    {}

public:
    constexpr auto begin() const noexcept -> mbmff::box_iterator
    {
        return *this;
    }
    constexpr auto end() const noexcept -> mbmff::box_iterator
    {
        return {};
    }
    constexpr auto try_get() const noexcept -> mbmff::result<mbmff::any_box_view>
    {
        return mbmff::parse(remaining_);
    }
    constexpr auto operator*() const noexcept -> mbmff::result<mbmff::any_box_view>
    {
        return try_get();
    }
    constexpr auto operator++() noexcept -> mbmff::box_iterator&
    {
        if (remaining_.empty()) {
            remaining_ = {};
            return *this;
        }
        auto op_result = mbmff::parse(remaining_);
        if (!op_result) {
            remaining_ = {};
            return *this;
        }

        if (mbmff::has(flags_, mbmff::iterator_flags::recursive)) {
            auto properties = mbmff::get_box_properties(op_result->type_);
            if (mbmff::has(properties, mbmff::box_properties::container)) {
                auto* current_end = &remaining_.back() + 1;
                auto* payload_start = op_result->payload.data();
                remaining_ = std::span<const std::byte>(payload_start, current_end);
                return *this;
            }
        }

        auto box_size = (op_result->size_ == 0) ? remaining_.size() : static_cast<std::size_t>(op_result->size_);
        if (box_size == 0 || box_size > remaining_.size()) {
            remaining_ = {};
        } else {
            remaining_ = remaining_.subspan(box_size);
        }
        return *this;
    }
    constexpr auto operator++(int) noexcept -> mbmff::box_iterator
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }
    constexpr bool operator==(const mbmff::box_iterator& other) const noexcept
    {
        if (remaining_.empty() && other.remaining_.empty()) {
            return true;
        }
        return remaining_.data() == other.remaining_.data() && remaining_.size() == other.remaining_.size();
    }
};

} // namespace mbmff

#undef MBMFF_ITERATE_BOX_TYPES
