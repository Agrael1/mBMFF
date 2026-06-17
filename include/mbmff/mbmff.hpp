#pragma once
#include "boxes/alis.hpp"
#include "boxes/apcn.hpp"
#include "boxes/av1C.hpp"
#include "boxes/avc1.hpp"
#include "boxes/avc3.hpp"
#include "boxes/avcC.hpp"
#include "boxes/btrt.hpp"
#include "boxes/cdsc.hpp"
#include "boxes/co64.hpp"
#include "boxes/colr.hpp"
#include "boxes/containers.hpp"
#include "boxes/ctts.hpp"
#include "boxes/dimg.hpp"
#include "boxes/dref.hpp"
#include "boxes/elst.hpp"
#include "boxes/esds.hpp"
#include "boxes/fiel.hpp"
#include "boxes/free.hpp"
#include "boxes/frma.hpp"
#include "boxes/ftyp.hpp"
#include "boxes/gmin.hpp"
#include "boxes/hdlr.hpp"
#include "boxes/hev1.hpp"
#include "boxes/hvcC.hpp"
#include "boxes/iinf.hpp"
#include "boxes/iloc.hpp"
#include "boxes/infe.hpp"
#include "boxes/ipma.hpp"
#include "boxes/iref.hpp"
#include "boxes/irot.hpp"
#include "boxes/ispe.hpp"
#include "boxes/load.hpp"
#include "boxes/mdat.hpp"
#include "boxes/mdhd.hpp"
#include "boxes/meta.hpp"
#include "boxes/mp4a.hpp"
#include "boxes/mp4v.hpp"
#include "boxes/mvhd.hpp"
#include "boxes/pasp.hpp"
#include "boxes/pitm.hpp"
#include "boxes/pixi.hpp"
#include "boxes/sbgp.hpp"
#include "boxes/schm.hpp"
#include "boxes/sdtp.hpp"
#include "boxes/sgpd.hpp"
#include "boxes/smhd.hpp"
#include "boxes/stco.hpp"
#include "boxes/stsc.hpp"
#include "boxes/stsd.hpp"
#include "boxes/stss.hpp"
#include "boxes/stsz.hpp"
#include "boxes/stts.hpp"
#include "boxes/thmb.hpp"
#include "boxes/tkhd.hpp"
#include "boxes/tmcd.hpp"
#include "boxes/url.hpp"
#include "boxes/urn.hpp"
#include "boxes/vmhd.hpp"
#include "boxes/wide.hpp"

namespace mbmff {

/// @brief A set of flags, determining the behavior of the box_iterator and parse() function
enum class iterator_flags : std::uint32_t {
    /// @brief Regular parsing: requires the whole box to be loaded, otherwise returns error_code::need_more_data.
    none = 0,

    /// @brief Recursive parsing: steps into the box_properties::container.
    recursive = 1 << 0,

    /// @brief Partial parsing: allows parsing partially loaded boxes, and returns error_code::truncated if the contents
    /// are not fully loaded. Still requires headers to be readable, returns error_code::need_more_data otherwise.
    allow_partial = 1 << 1,
};
MBMFF_FLAG_OPERATORS(iterator_flags)

#define MBMFF_ITERATE_USING(name, fourcc) using name##_box = mbmff::basic_box_view<mbmff::box_type::name>;
MBMFF_ITERATE_BOX_TYPES(MBMFF_ITERATE_USING)
#undef MBMFF_ITERATE_USING

/// @brief Returns the box_properties flags for a given box type (full_box, container, etc.).
/// @param[in] type type of the box
/// @returns If the box is not implemented by this library - `box_properties::none`
/// Otherwise, a combination of box_properties, describing the box
inline constexpr auto get_box_properties(mbmff::box_type type) noexcept -> mbmff::box_properties
{
#define MBMFF_CASE_PROP(name, fourcc) \
    case mbmff::box_type::name:       \
        return mbmff::basic_box_view<mbmff::box_type::name>::properties;

    switch (type) {
        MBMFF_ITERATE_BOX_TYPES(MBMFF_CASE_PROP)
    default:
        return mbmff::box_properties::none;
    }
#undef MBMFF_CASE_PROP
}

/// @brief Returns true if the given box type has a specialization registered in the library.
/// @param[in] type type of the box
/// @returns `true` if the box is registered and implemented in the library, `false` otherwise
inline constexpr bool is_box_implemented(mbmff::box_type type) noexcept
{
#define MBMFF_CASE_IMPL(name, fourcc) case mbmff::box_type::name:

    switch (type) {
        MBMFF_ITERATE_BOX_TYPES(MBMFF_CASE_IMPL)
        return true;
    default:
        return false;
    }
#undef MBMFF_CASE_IMPL
}

/// @brief Returns true if the box type can contain child boxes (e.g. moov, trak, stbl).
/// @param[in] type type of the box
/// @returns `true` if the box has `box_properties::container` listed in its properties
/// `false` if not implemented or otherwise
inline constexpr bool is_container(const mbmff::any_box_view& box) noexcept
{
    return mbmff::has(mbmff::get_box_properties(box.type()), mbmff::box_properties::container);
}

/// @brief Parses a single box from the beginning of the byte span.
///
/// Without `allow_partial`, the full box must be present in `data`; returns
/// `need_more_data` if the box extends beyond the span. With `allow_partial`,
/// returns `truncated` when the box header is valid but the payload is
/// incomplete, allowing the caller to proceed with partial data.
/// @tparam flags Iterator flags (`none`, `allow_partial`).
/// @param[in] data Byte span containing (at least the start of) a box.
/// @returns The parsed box view, or an error.
template <mbmff::iterator_flags flags = mbmff::iterator_flags::none>
inline constexpr auto parse(std::span<const std::byte> data) noexcept -> mbmff::result<mbmff::any_box_view>
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
        if constexpr (!mbmff::has(flags, mbmff::iterator_flags::allow_partial)) {
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
#define MBMFF_CASE_VALIDATE(name, fourcc) \
    case mbmff::box_type::name:           \
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

/// @brief Casts a generic any_box_view to a typed basic_box_view. Returns an empty view on type mismatch.
/// @tparam Box The target box type.
/// @param[in] box The generic box view.
/// @returns A typed box view, or an empty view if the type doesn't match.
template <mbmff::box_type Box>
constexpr auto box_cast(const mbmff::any_box_view& box) noexcept -> mbmff::basic_box_view<Box>
{
    if (box.type() != Box) {
        return {};
    }
    return static_cast<mbmff::basic_box_view<Box>>(box);
}

/// @brief Forward iterator over a sequence of ISOBMFF boxes in a byte span.
///
/// Iterates over top-level boxes. With the `recursive` flag, descends into
/// container boxes (yielding their children instead). With `allow_partial`,
/// continues past truncated boxes.
/// @tparam flags Iterator flags (`none`, `allow_partial`, `recursive`).
template <mbmff::iterator_flags flags = mbmff::iterator_flags::none>
struct box_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = mbmff::result<mbmff::any_box_view>;
    using difference_type = std::ptrdiff_t;
    using iterator = mbmff::box_iterator<flags>;

private:
    std::span<const std::byte> remaining_;
    const std::byte* original_start_ = nullptr;

public:
    constexpr box_iterator() noexcept = default;
    /// @brief Constructs an iterator over a byte span.
    /// @param[in] data The span of bytes to iterate over.
    constexpr explicit box_iterator(std::span<const std::byte> data) noexcept
        : remaining_(data)
        , original_start_(data.data())
    {}

public:
    /// @brief Returns an iterator to the beginning of the sequence.
    constexpr auto begin() const noexcept -> iterator
    {
        return *this;
    }
    /// @brief Returns a sentinel iterator for end-of-sequence.
    constexpr auto end() const noexcept -> iterator
    {
        return {};
    }
    /// @brief Parses the current box without advancing the iterator.
    /// @returns The parsed box or an error.
    constexpr auto try_get() const noexcept -> mbmff::result<mbmff::any_box_view>
    {
        return mbmff::parse<flags>(remaining_);
    }
    /// @brief Dereferences the iterator (equivalent to `try_get()`).
    constexpr auto operator*() const noexcept -> mbmff::result<mbmff::any_box_view>
    {
        return try_get();
    }
    /// @brief Returns the number of bytes consumed from the original data so far.
    constexpr auto offset() const noexcept -> std::size_t
    {
        if (original_start_ == nullptr) {
            return 0;
        }
        return static_cast<std::size_t>(remaining_.data() - original_start_);
    }
    /// @brief Returns the remaining unprocessed span (bytes yet to be iterated).
    constexpr auto remaining() const noexcept -> std::span<const std::byte>
    {
        return remaining_;
    }
    /// @brief Advances the iterator to the next box (pre-increment).
    constexpr auto operator++() noexcept -> iterator&
    {
        if (remaining_.empty()) {
            remaining_ = {};
            return *this;
        }
        auto op_result = mbmff::parse<flags>(remaining_);
        if (!op_result && op_result.code != mbmff::error_code::truncated) {
            remaining_ = {};
            return *this;
        }

        if constexpr (mbmff::has(flags, mbmff::iterator_flags::recursive)) {
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
    /// @brief Advances the iterator to the next box (post-increment).
    constexpr auto operator++(int) noexcept -> iterator
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }
    /// @brief Compares two iterators for equality (both exhausted, or same position).
    constexpr bool operator==(const iterator& other) const noexcept
    {
        if (remaining_.empty() && other.remaining_.empty()) {
            return true;
        }
        return remaining_.data() == other.remaining_.data() && remaining_.size() == other.remaining_.size();
    }
};

/// @brief Recursive box iterator: descends into container boxes' children.
using recursive_box_iterator = mbmff::box_iterator<mbmff::iterator_flags::recursive>;

/// @brief Partial box iterator: allows parsing of partially loaded boxes.
using partial_box_iterator = mbmff::box_iterator<mbmff::iterator_flags::allow_partial>;

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST

template <mbmff::box_type Box>
concept box_not_implemented = requires { mbmff::basic_box_view<Box>::not_implemented; };

// Test if all boxes are implemented
#    define MBMFF_TEST_ALL_IMPLEMENTED(name, fourcc) \
        static_assert(!box_not_implemented<mbmff::box_type::name>, "Box type " #name " is not implemented.");

MBMFF_ITERATE_BOX_TYPES(MBMFF_TEST_ALL_IMPLEMENTED)
#    undef MBMFF_TEST_ALL_IMPLEMENTED

// test if container property is correctly detected
static_assert(
    [] {
        std::array<std::byte, 8> data{
            std::byte{0x00},
            std::byte{0x00},
            std::byte{0x00},
            std::byte{0x08},
            std::byte{'m'},
            std::byte{'o'},
            std::byte{'o'},
            std::byte{'v'},
        };
        auto r = mbmff::parse(std::span<const std::byte>(data));
        return r && r->type_ == mbmff::box_type::moov
            && mbmff::has(mbmff::get_box_properties(r->type_), mbmff::box_properties::container);
    }(),
    "moov container property should be detected"
);

// parse() without flags rejects truncated data (backward compat)
static_assert(
    [] {
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
        auto r = mbmff::parse(std::span<const std::byte>(data));
        return !r && r.code == mbmff::error_code::need_more_data && r.needed == 1000;
    }(),
    "parse() without flags should reject truncated data"
);

// parse() with allow_partial returns truncated for incomplete box
static_assert(
    [] {
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
        auto r = mbmff::parse<mbmff::iterator_flags::allow_partial>(std::span<const std::byte>(data));
        return !r && r.code == mbmff::error_code::truncated && r->type_ == mbmff::box_type::mdat && r->size_ == 1000
            && r->payload.size() == 4;
    }(),
    "parse(allow_partial) should return truncated for incomplete box"
);

// parse() with allow_partial returns success for complete box
static_assert(
    [] {
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
        auto r = mbmff::parse<mbmff::iterator_flags::allow_partial>(std::span<const std::byte>(data));
        return r && r.code == mbmff::error_code::success && r->type_ == mbmff::box_type::mdat;
    }(),
    "parse(allow_partial) should return success for complete box"
);

// allow_partial still rejects when even the validate minimum is absent
static_assert(
    [] {
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
        auto r = mbmff::parse<mbmff::iterator_flags::allow_partial>(std::span<const std::byte>(data));
        return !r && r.code == mbmff::error_code::need_more_data && r.needed == 8;
    }(),
    "allow_partial should still reject data lacking validate minimum"
);

// box_iterator with allow_partial continues past truncated boxes
static_assert(
    [] {
        std::array<std::byte, 24> data{
            std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x0C}, std::byte{'m'},  std::byte{'d'},
            std::byte{'a'},  std::byte{'t'},  std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04},
            std::byte{0x00}, std::byte{0x00}, std::byte{0x03}, std::byte{0xE8}, std::byte{'m'},  std::byte{'o'},
            std::byte{'o'},  std::byte{'v'},  std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}, std::byte{0xDD},
        };
        auto it = mbmff::box_iterator<mbmff::iterator_flags::allow_partial>(std::span<const std::byte>(data));
        auto end = it.end();

        if (it == end) {
            return false;
        }
        auto r1 = *it;
        if (!r1 || r1->type_ != mbmff::box_type::mdat) {
            return false;
        }
        ++it;

        if (it == end) {
            return false;
        }
        auto r2 = *it;
        if (r2.code != mbmff::error_code::truncated || r2->type_ != mbmff::box_type::moov) {
            return false;
        }
        ++it;

        return it == end;
    }(),
    "box_iterator(allow_partial) should continue past truncated boxes"
);
// box_iterator without allow_partial stops at truncated data
static_assert(
    [] {
        std::array<std::byte, 24> data{
            std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x0C}, std::byte{'m'},  std::byte{'d'},
            std::byte{'a'},  std::byte{'t'},  std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04},
            std::byte{0x00}, std::byte{0x00}, std::byte{0x03}, std::byte{0xE8}, std::byte{'m'},  std::byte{'o'},
            std::byte{'o'},  std::byte{'v'},  std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}, std::byte{0xDD},
        };
        auto it = mbmff::box_iterator(std::span<const std::byte>(data));
        auto end = it.end();

        if (it == end) {
            return false;
        }
        if (!(*it)) {
            return false; // mdat
        }
        ++it;

        // truncated moov — parse fails → iterator terminates
        if (it == end) {
            return false;
        }
        if ((*it).code != mbmff::error_code::need_more_data) {
            return false;
        }
        ++it;

        return it == end;
    }(),
    "box_iterator should stop at truncated data"
);
// recursive + allow_partial descends into a truncated container's children
static_assert(
    [] {
        std::array<std::byte, 20> data{
            std::byte{0x00}, std::byte{0x00}, std::byte{0x03}, std::byte{0xE8}, std::byte{'m'},
            std::byte{'o'},  std::byte{'o'},  std::byte{'v'},  std::byte{0x00}, std::byte{0x00},
            std::byte{0x00}, std::byte{0x0C}, std::byte{'m'},  std::byte{'d'},  std::byte{'a'},
            std::byte{'t'},  std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04},
        };
        auto it = mbmff::box_iterator<mbmff::iterator_flags::allow_partial | mbmff::iterator_flags::recursive>(
            std::span<const std::byte>(data)

        );
        auto end = it.end();
        if (it == end) {
            return false;
        }

        if ((*it).code != mbmff::error_code::truncated || (*it)->type_ != mbmff::box_type::moov) {
            return false;
        }
        ++it;

        if (it == end) {
            return false;
        }
        if (!(*it) || (*it)->type_ != mbmff::box_type::mdat) {
            return false;
        }
        ++it;

        return it == end;
    }(),
    "recursive allow_partial should descend into truncated container"
);

#endif

} // namespace mbmff

#undef MBMFF_ITERATE_BOX_TYPES
