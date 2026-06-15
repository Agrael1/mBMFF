#pragma once
#include "boxes/av1C.hpp"
#include "boxes/containers.hpp"
#include "boxes/ftyp.hpp"
#include "boxes/hdlr.hpp"
#include "boxes/iinf.hpp"
#include "boxes/iloc.hpp"
#include "boxes/infe.hpp"
#include "boxes/ipma.hpp"
#include "boxes/iref.hpp"
#include "boxes/ispe.hpp"
#include "boxes/mdat.hpp"
#include "boxes/meta.hpp"
#include "boxes/mvhd.hpp"
#include "boxes/pasp.hpp"
#include "boxes/pitm.hpp"
#include "boxes/pixi.hpp"

namespace mbmff {

enum class iterator_flags : std::uint32_t {
    none = 0,
    recursive = 1 << 0,
    allow_partial = 1 << 1,
};
MBMFF_FLAG_OPERATORS(iterator_flags)

#define MBMFF_ITERATE_USING(name) using name##_box = basic_box_view<box_type::name>;
MBMFF_ITERATE_BOX_TYPES(MBMFF_ITERATE_USING)
#undef MBMFF_ITERATE_USING

inline constexpr auto get_box_properties(mbmff::box_type type) noexcept -> mbmff::box_properties
{
#define MBMFF_CASE_PROP(name)   \
    case mbmff::box_type::name: \
        return mbmff::basic_box_view<mbmff::box_type::name>::properties;
    switch (type) {
        MBMFF_ITERATE_BOX_TYPES(MBMFF_CASE_PROP)
    default:
        return mbmff::box_properties::none;
    }
#undef MBMFF_CASE_PROP
}

inline constexpr auto parse(
    std::span<const std::byte> data,
    mbmff::iterator_flags flags = mbmff::iterator_flags::none
) noexcept -> mbmff::result<mbmff::any_box_view>
{
    auto header_result = mbmff::parse_box_header(data);
    if (!header_result) {
        return mbmff::make_result<mbmff::any_box_view>(header_result.code, header_result.needed);
    }
    auto header = header_result->value;
    auto header_size = header_result->consumed;

    bool partial = false;
    std::size_t payload_size;

    if (header.size() != 0 && data.size() < header.size()) {
        if (!mbmff::has(flags, mbmff::iterator_flags::allow_partial)) {
            return mbmff::make_result<mbmff::any_box_view>(
                mbmff::error_code::need_more_data,
                static_cast<std::size_t>(header.size())
            );
        }
        partial = true;
        payload_size = data.size() - header_size;
    } else {
        if (header.size() != 0 && header.size() < header_size) {
            return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::invalid_format);
        }
        payload_size = (header.size() == 0) ? (data.size() - header_size)
                                            : (static_cast<std::size_t>(header.size()) - header_size);
    }

    auto payload_span = data.subspan(header_size, payload_size);
    mbmff::any_box_view box{{header.size_, header.type_}, payload_span};

    auto validate_result = [&]() -> mbmff::result<mbmff::any_box_view> {
#define MBMFF_CASE_VALIDATE(name) \
    case mbmff::box_type::name:   \
        return mbmff::basic_box_view<mbmff::box_type::name>::validate(box);
        switch (header.type_) {
            MBMFF_ITERATE_BOX_TYPES(MBMFF_CASE_VALIDATE)
        default:
            return {box};
        }
#undef MBMFF_CASE_VALIDATE
    }();

    if (!validate_result) {
        return validate_result;
    }
    if (partial) {
        return mbmff::make_result<mbmff::any_box_view>(*validate_result, mbmff::error_code::truncated);
    }
    return validate_result;
}

template <mbmff::box_type Box>
constexpr auto box_cast(const mbmff::any_box_view& box) noexcept -> mbmff::basic_box_view<Box>
{
    if (box.type() != Box) {
        return {};
    }
    return static_cast<mbmff::basic_box_view<Box>>(box);
}

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
        return mbmff::parse(remaining_, flags_);
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
        auto op_result = mbmff::parse(remaining_, flags_);
        if (!op_result && op_result.code != mbmff::error_code::truncated) {
            remaining_ = {};
            return *this;
        }

        if (mbmff::has(flags_, mbmff::iterator_flags::recursive)) {
            auto properties = mbmff::get_box_properties(op_result->type_);
            if (mbmff::has(properties, mbmff::box_properties::container)) {
                auto* payload_start = op_result->payload.data();
                auto* payload_end = payload_start + op_result->payload.size();
                remaining_ = std::span<const std::byte>(payload_start, payload_end);
                return *this;
            }
        }

        if (op_result.code == mbmff::error_code::truncated) {
            remaining_ = {};
            return *this;
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

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST

// parse() without flags rejects truncated data (backward compat)
static_assert([] {
    std::array<std::byte, 12> data{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x03}, std::byte{0xE8},
        std::byte{'m'}, std::byte{'d'}, std::byte{'a'}, std::byte{'t'},
    };
    auto r = parse(std::span<const std::byte>(data));
    return !r && r.code == error_code::need_more_data && r.needed == 1000;
}());

// parse() with allow_partial returns truncated for incomplete box
static_assert([] {
    std::array<std::byte, 12> data{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x03},
        std::byte{0xE8},
        std::byte{'m'},
        std::byte{'d'},
        std::byte{'a'},
        std::byte{'t'},
    };
    auto r = parse(std::span<const std::byte>(data), iterator_flags::allow_partial);
    return !r && r.code == error_code::truncated && r->type_ == box_type::mdat && r->size_ == 1000
        && r->payload.size() == 4;
}());

// parse() with allow_partial returns success for complete box
static_assert([] {
    std::array<std::byte, 12> data{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x0C},
        std::byte{'m'},
        std::byte{'d'},
        std::byte{'a'},
        std::byte{'t'},
        std::byte{0x01},
        std::byte{0x02},
        std::byte{0x03},
        std::byte{0x04},
    };
    auto r = parse(std::span<const std::byte>(data), iterator_flags::allow_partial);
    return r && r.code == error_code::success && r->type_ == box_type::mdat;
}());

// allow_partial still rejects when even the validate minimum is absent
static_assert([] {
    std::array<std::byte, 12> data{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x10},
        std::byte{'f'},
        std::byte{'t'},
        std::byte{'y'},
        std::byte{'p'},
        std::byte{'a'},
        std::byte{'v'},
        std::byte{'0'},
        std::byte{'1'},
    };
    auto r = parse(std::span<const std::byte>(data), iterator_flags::allow_partial);
    return !r && r.code == error_code::need_more_data && r.needed == 8;
}());

// box_iterator with allow_partial continues past truncated boxes
static_assert([] {
    std::array<std::byte, 24> data{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x0C}, std::byte{'m'},  std::byte{'d'},
        std::byte{'a'},  std::byte{'t'},  std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x03}, std::byte{0xE8}, std::byte{'m'},  std::byte{'o'},
        std::byte{'o'},  std::byte{'v'},  std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}, std::byte{0xDD},
    };
    auto it = box_iterator(std::span<const std::byte>(data), iterator_flags::allow_partial);
    auto end = box_iterator{};

    if (it == end) {
        return false;
    }
    auto r1 = *it;
    if (!r1 || r1->type_ != box_type::mdat) {
        return false;
    }
    ++it;

    if (it == end) {
        return false;
    }
    auto r2 = *it;
    if (r2.code != error_code::truncated || r2->type_ != box_type::moov) {
        return false;
    }
    ++it;

    return it == end;
}());
// box_iterator without allow_partial stops at truncated data
static_assert([] {
    std::array<std::byte, 24> data{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x0C},
        std::byte{'m'}, std::byte{'d'}, std::byte{'a'}, std::byte{'t'},
        std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x03}, std::byte{0xE8},
        std::byte{'m'}, std::byte{'o'}, std::byte{'o'}, std::byte{'v'},
        std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}, std::byte{0xDD},
    };
    auto it = box_iterator(std::span<const std::byte>(data));
    auto end = box_iterator{};

    if (it == end) return false;
    if (!(*it)) return false;  // mdat
    ++it;

    // truncated moov — parse fails → iterator terminates
    if (it == end) return false;
    if ((*it).code != error_code::need_more_data) return false;
    ++it;

    return it == end;
}());
// recursive + allow_partial descends into a truncated container's children
static_assert([] {
    std::array<std::byte, 20> data{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x03}, std::byte{0xE8}, std::byte{'m'},
        std::byte{'o'},  std::byte{'o'},  std::byte{'v'},  std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x0C}, std::byte{'m'},  std::byte{'d'},  std::byte{'a'},
        std::byte{'t'},  std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04},
    };
    auto it = box_iterator(std::span<const std::byte>(data), iterator_flags::allow_partial | iterator_flags::recursive);
    auto end = box_iterator{};

    if (it == end) {
        return false;
    }
    if ((*it).code != error_code::truncated || (*it)->type_ != box_type::moov) {
        return false;
    }
    ++it;

    if (it == end) {
        return false;
    }
    if (!(*it) || (*it)->type_ != box_type::mdat) {
        return false;
    }
    ++it;

    return it == end;
}());

#endif

} // namespace mbmff

#undef MBMFF_ITERATE_BOX_TYPES
