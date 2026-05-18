#pragma once
#include <array>
#include <bit>
#include <concepts>
#include <cstring>
#include <format>
#include <string_view>
#include <type_traits>
#include "av1.hpp"

#define MBMFF_ITERATE_BOX_TYPES(MACRO) \
    MACRO(ftyp)                        \
    MACRO(meta)                        \
    MACRO(mdat)                        \
    MACRO(moov)                        \
    MACRO(trak)                        \
    MACRO(mdia)                        \
    MACRO(minf)                        \
    MACRO(stbl)                        \
    MACRO(dinf)                        \
    MACRO(edts)                        \
    MACRO(udta)                        \
    MACRO(mvex)                        \
    MACRO(moof)                        \
    MACRO(traf)                        \
    MACRO(mfra)                        \
    MACRO(iprp)                        \
    MACRO(ipco)                        \
    MACRO(iinf)                        \
    MACRO(iref)                        \
    MACRO(iloc)                        \
    MACRO(hdlr)                        \
    MACRO(pitm)                        \
    MACRO(ispe)                        \
    MACRO(av1C)                        \
    MACRO(pixi)                        \
    MACRO(ipma)                        \
    MACRO(infe)

namespace mbmff {

//------------------------------------------------------------------------------------------------------------
constexpr auto fourcc(const char* str) noexcept -> uint32_t
{
    return (static_cast<uint32_t>(str[3]) << 24) | (static_cast<uint32_t>(str[2]) << 16)
         | (static_cast<uint32_t>(str[1]) << 8) | static_cast<uint32_t>(str[0]);
}

#define MBMFF_ITERATE_ENUM(name) name = fourcc(#name),

enum class box_type : uint32_t { unknown, MBMFF_ITERATE_BOX_TYPES(MBMFF_ITERATE_ENUM) };

//------------------------------------------------------------------------------------------------------------
enum class properties : uint32_t {
    none = 0,
    full_box = 1 << 0,
    container = 1 << 1,
};
constexpr auto operator|(properties a, properties b) noexcept -> properties
{
    return static_cast<properties>(std::to_underlying(a) | std::to_underlying(b));
}
constexpr auto operator&(properties a, properties b) noexcept -> properties
{
    return static_cast<properties>(std::to_underlying(a) & std::to_underlying(b));
}
constexpr auto operator+(properties a) noexcept -> std::underlying_type_t<properties>
{
    return std::to_underlying(a);
}
constexpr auto has(properties a, properties check) noexcept -> bool
{
    return (a & check) != properties::none;
}

//------------------------------------------------------------------------------------------------------------
struct fourcc_string {
    std::array<char, 4> data{};

public:
    constexpr auto view() const noexcept -> std::string_view
    {
        return std::string_view(data.data(), data.size());
    }
    constexpr operator std::string_view() const noexcept
    {
        return view();
    }
    constexpr static auto from_data(std::span<const std::byte> data) noexcept -> fourcc_string
    {
        return fourcc_string{std::array<char, 4>{
            static_cast<char>(data[0]),
            static_cast<char>(data[1]),
            static_cast<char>(data[2]),
            static_cast<char>(data[3]),
        }};
    }
};

//------------------------------------------------------------------------------------------------------------
struct box_header {
    std::uint64_t size = 0;
    mbmff::box_type type{};
    std::uint8_t version = 0;
    std::array<std::uint8_t, 3> flags{};

public:
    constexpr auto type_string() const noexcept -> fourcc_string
    {
        std::uint32_t value = std::to_underlying(type);
        std::array<char, 4> str{
            char(value & 0xFF),
            char((value >> 8) & 0xFF),
            char((value >> 16) & 0xFF),
            char((value >> 24) & 0xFF),
        };
        return {str};
    }
    constexpr auto flags_value() const noexcept -> std::uint32_t
    {
        return (static_cast<std::uint32_t>(flags[0]) << 16) | (static_cast<std::uint32_t>(flags[1]) << 8)
             | static_cast<std::uint32_t>(flags[2]);
    }

    // Assumes there is enough data in the buffer (i.e. the caller should have already checked that the full header is
    // present)
    constexpr void fill_full_header(std::span<const std::byte> data) noexcept
    {
        version = static_cast<std::uint8_t>(data[0]);
        flags[0] = static_cast<std::uint8_t>(data[1]);
        flags[1] = static_cast<std::uint8_t>(data[2]);
        flags[2] = static_cast<std::uint8_t>(data[3]);
    }
};

struct infe_header {
    std::uint32_t item_id = 0;
    std::uint16_t item_protection_index = 0;
    mbmff::fourcc_string item_type{};
    std::string_view item_name{};
    std::string_view content_type{};
    std::string_view content_encoding{};
    std::string_view item_uri_type{};
};

struct ftyp_header {
    mbmff::fourcc_string major_brand{};
    std::uint32_t minor_version = 0;
    std::span<const mbmff::fourcc_string> compatible_brands{};
};

struct hdlr_header {
    mbmff::fourcc_string handler_type{};
    std::string_view name{};
};

struct iloc_item {
    std::uint32_t item_id = 0;
    std::uint8_t construction_method = 0;
    std::uint64_t base_offset = 0;
    std::uint32_t extent_count = 0;
};

struct iloc_header {
    std::uint8_t offset_size = 0;
    std::uint8_t length_size = 0;
    std::uint8_t base_offset_size = 0;
    std::uint8_t index_size = 0;
    std::uint32_t item_count = 0;
    std::span<const std::byte> item_data{};

public:
    // TODO: item iterator
};

struct ispe_header {
    std::uint32_t image_width = 0;
    std::uint32_t image_height = 0;
};

//------------------------------------------------------------------------------------------------------------

using any_box_view = struct box_view_base;

struct box_view_base {
    mbmff::box_header header{};
    std::span<const std::byte> payload{};

public:
    constexpr operator bool() const noexcept
    {
        return header.size != 0;
    }
    constexpr static auto parse(any_box_view box) noexcept -> std::expected<any_box_view, unexpected>
    {
        return box;
    }
    constexpr auto version() const noexcept -> std::uint8_t
    {
        return header.version;
    }
};

template <box_type Type>
struct basic_box_view : public box_view_base {
    constexpr static properties properties = properties::none;
};

//------------------------------------------------------------------------------------------------------------
// Box-specific views
//------------------------------------------------------------------------------------------------------------
template <>
struct basic_box_view<box_type::ftyp> : public box_view_base {
    constexpr static properties properties = properties::none;
    constexpr static auto parse(any_box_view box) noexcept -> std::expected<any_box_view, unexpected>;

public:
    constexpr auto header() const noexcept -> ftyp_header;
    constexpr auto major_brand() const noexcept -> fourcc_string;
    constexpr auto minor_version() const noexcept -> std::uint32_t;
    constexpr auto compatible_brands() const noexcept -> std::span<const fourcc_string>;
};

template <>
struct basic_box_view<box_type::meta> : public box_view_base {
    // full_box is not ticked here because `meta` can
    // be a regular box in some formats (e.g. QTFF)
    constexpr static properties properties = properties::container;
    constexpr static auto parse(any_box_view box) noexcept -> std::expected<any_box_view, unexpected>;
};

template <>
struct basic_box_view<box_type::mdat> : public box_view_base {
    constexpr auto data_size() const noexcept -> std::size_t;
    constexpr static properties properties = properties::none;
};

template <>
struct basic_box_view<box_type::iinf> : public box_view_base {
    constexpr static properties properties = properties::container | properties::full_box;
    constexpr static auto parse(any_box_view box) noexcept -> std::expected<any_box_view, unexpected>;
    constexpr auto entry_count() const noexcept -> std::uint32_t;
};

template <>
struct basic_box_view<box_type::infe> : public box_view_base {
    constexpr static properties properties = properties::full_box;
    constexpr auto header() const noexcept -> infe_header;
    constexpr auto item_id() const noexcept -> std::uint32_t;
    constexpr auto item_protection_index() const noexcept -> std::uint16_t;
};

template <>
struct basic_box_view<box_type::hdlr> : public box_view_base {
    constexpr static properties properties = properties::full_box;
    constexpr auto header() const noexcept -> hdlr_header;
};

template <>
struct basic_box_view<box_type::pitm> : public box_view_base {
    constexpr static properties properties = properties::full_box;
    constexpr auto item_id() const noexcept -> uint32_t;
};

template <>
struct basic_box_view<box_type::iloc> : public box_view_base {
    constexpr static properties properties = properties::full_box;
    constexpr auto header() const noexcept -> iloc_header;
};

template <>
struct basic_box_view<box_type::iprp> : public box_view_base {
    constexpr static properties properties = properties::container;
};

template <>
struct basic_box_view<box_type::ipco> : public box_view_base {
    constexpr static properties properties = properties::container;
};

template <>
struct basic_box_view<box_type::ispe> : public box_view_base {
    constexpr static properties properties = properties::full_box;
    constexpr auto header() const noexcept -> ispe_header;
};

template <>
struct basic_box_view<box_type::av1C> : public box_view_base {
    constexpr static properties properties = properties::none;
    constexpr auto header() const noexcept -> av1C_header;
};

//------------------------------------------------------------------------------------------------------------
#define MBMFF_ITERATE_USING(box) using box##_box = basic_box_view<box_type::box>;
MBMFF_ITERATE_BOX_TYPES(MBMFF_ITERATE_USING)
#undef MBMFF_ITERATE_USING

//------------------------------------------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------------------------------------------
template <std::integral T>
constexpr auto read_be(std::span<const std::byte> data) noexcept -> T
{
    T value;
    std::memcpy(&value, data.data(), sizeof(value)); // strict aliasing
    return std::byteswap(value);
}

struct parsed_cstr {
    std::string_view value{};
    std::size_t next = 0;
};

constexpr auto read_cstr(std::span<const std::byte> data, std::size_t offset) noexcept -> parsed_cstr
{
    if (offset >= data.size()) {
        return {};
    }

    const auto* begin = reinterpret_cast<const char*>(data.data() + offset);
    std::size_t length = 0;
    for (std::size_t i = offset; i < data.size(); ++i) {
        if (data[i] == std::byte{0}) {
            break;
        }
        ++length;
    }

    std::size_t next = offset + length;
    if (next < data.size() && data[next] == std::byte{0}) {
        ++next;
    }

    return {std::string_view(begin, length), next};
}

//------------------------------------------------------------------------------------------------------------
constexpr inline auto parse_box_header(std::span<const std::byte> data) noexcept
    -> std::expected<parsed<box_header>, unexpected>
{
    if (data.size() < 8) {
        return std::unexpected(unexpected{error_code::need_more_data, 8});
    }

    std::uint64_t size = read_be<std::uint32_t>(data);
    if (size == 0) {
        return std::unexpected(unexpected{error_code::invalid_format, 8});
    }

    // Read the header type (4 bytes)
    box_type type = box_type(fourcc(reinterpret_cast<const char*>(data.data() + 4)));
    if (size == 1) {
        if (data.size() < 16) {
            return std::unexpected(unexpected{error_code::need_more_data, 16});
        }
        size = read_be<std::uint64_t>(data.subspan(8));
    }

    return parsed<box_header>{box_header{size, type}, size == 1 ? 16u : 8u};
}

inline constexpr auto get_box_properties(box_type type) noexcept -> properties
{
#define MBMFF_CASE_ITERATE(box) \
    case box_type::box:         \
        return basic_box_view<box_type::box>::properties;

    switch (type) {
        MBMFF_ITERATE_BOX_TYPES(MBMFF_CASE_ITERATE)
    default:
        return properties::none;
    }
#undef MBMFF_CASE_ITERATE
}

inline constexpr auto parse(std::span<const std::byte> data) noexcept -> std::expected<any_box_view, unexpected>
{
    auto header_result = parse_box_header(data);
    if (!header_result) {
        return std::unexpected(header_result.error());
    }

    auto header = header_result->value;
    if (header.size != 0 && data.size() < header.size) {
        return std::unexpected(unexpected{error_code::need_more_data, static_cast<std::size_t>(header.size)});
    }
    if (header.size != 0 && header.size < header_result->consumed) {
        return std::unexpected(unexpected{error_code::invalid_format, header_result->consumed});
    }

    std::size_t payload_size = (header.size == 0) ? (data.size() - header_result->consumed)
                                                  : (static_cast<std::size_t>(header.size) - header_result->consumed);
    auto payload_span = data.subspan(header_result->consumed, payload_size);

    properties box_properties = get_box_properties(header.type);
    if ((box_properties & properties::full_box) != properties::none) {
        if (payload_span.size() < 4) {
            return std::unexpected(unexpected{error_code::need_more_data, 4});
        }

        header.fill_full_header(payload_span);
        payload_span = payload_span.subspan(4);
    }

#define MBMFF_CASE_PARSE(box) \
    case box_type::box:       \
        return basic_box_view<box_type::box>::parse(any_box_view{header, payload_span});

    switch (header.type) {
        MBMFF_ITERATE_BOX_TYPES(MBMFF_CASE_PARSE)
    default:
        return basic_box_view<box_type::unknown>::parse(any_box_view{header, payload_span});
    }
#undef MBMFF_CASE_PARSE
}

//------------------------------------------------------------------------------------------------------------
// FTYP
inline constexpr auto basic_box_view<box_type::ftyp>::parse(any_box_view box) noexcept
    -> std::expected<any_box_view, unexpected>
{
    if (box.payload.size() < 8) {
        return std::unexpected(unexpected{error_code::need_more_data, 8});
    }

    if (box.payload.size() % 4 != 0) {
        return std::unexpected(unexpected{error_code::invalid_format});
    }

    return box;
}
constexpr auto basic_box_view<box_type::ftyp>::header() const noexcept -> ftyp_header
{
    return ftyp_header{major_brand(), minor_version(), compatible_brands()};
}
constexpr auto basic_box_view<box_type::ftyp>::major_brand() const noexcept -> fourcc_string
{
    return fourcc_string::from_data(payload);
}

constexpr auto basic_box_view<box_type::ftyp>::minor_version() const noexcept -> std::uint32_t
{
    return read_be<uint32_t>(payload.subspan(4));
}

constexpr auto basic_box_view<box_type::ftyp>::compatible_brands() const noexcept -> std::span<const fourcc_string>
{
    const auto compatible_data = payload.subspan(8);

    return std::span<const fourcc_string>(
        reinterpret_cast<const fourcc_string*>(compatible_data.data()),
        compatible_data.size() / 4
    );
}

//------------------------------------------------------------------------------------------------------------
// META
inline constexpr auto basic_box_view<box_type::meta>::parse(any_box_view box) noexcept
    -> std::expected<any_box_view, unexpected>
{
    // In a QT File, `meta` is a regular box. Its payload starts directly with a child box (e.g. `[size][hdlr]`).
    // In ISOBMFF, `meta` is a Full Box. Its payload starts with 4 bytes (version+flags), then the child box
    // `[size][hdlr]`.
    constexpr auto hdlr_type = "hdlr";
    bool full_box = std::memcmp(box.payload.data() + 4, hdlr_type, 4) != 0;

    if (full_box) {
        box.header.fill_full_header(box.payload);
        box.payload = box.payload.subspan(4);
    }
    return box;
}

//------------------------------------------------------------------------------------------------------------
// MDAT
constexpr auto basic_box_view<box_type::mdat>::data_size() const noexcept -> std::size_t
{
    return payload.size();
}

//------------------------------------------------------------------------------------------------------------
// IINF
inline constexpr auto basic_box_view<box_type::iinf>::parse(any_box_view box) noexcept
    -> std::expected<any_box_view, unexpected>
{
    if (box.version() == 0) {
        box.payload = box.payload.subspan(2); // skip 2 bytes of reserved data
    } else {
        box.payload = box.payload.subspan(4); // skip 4 bytes of reserved data
    }
    return box;
}

inline constexpr auto basic_box_view<box_type::iinf>::entry_count() const noexcept -> std::uint32_t
{
    std::size_t reserved_size = (version() == 0) ? 2 : 4;
    const std::byte* reserved_end = payload.data() - reserved_size;
    std::span<const std::byte> reserved_span(reserved_end, reserved_size);

    if (version() == 0) {
        return read_be<std::uint16_t>(reserved_span);
    }
    return read_be<std::uint32_t>(reserved_span);
}

//------------------------------------------------------------------------------------------------------------
// INFE
inline constexpr auto basic_box_view<box_type::infe>::item_id() const noexcept -> std::uint32_t
{
    auto xversion = version();
    if (xversion == 0 || xversion == 1 || xversion == 2) {
        return read_be<std::uint16_t>(payload);
    } else {
        return read_be<std::uint32_t>(payload);
    }
}

inline constexpr auto basic_box_view<box_type::infe>::item_protection_index() const noexcept -> std::uint16_t
{
    auto xversion = version();
    std::size_t offset = (xversion == 0 || xversion == 1 || xversion == 2) ? 2 : 4;
    return read_be<std::uint16_t>(payload.subspan(offset));
}

inline constexpr auto basic_box_view<box_type::infe>::header() const noexcept -> infe_header
{
    infe_header result{};
    auto xversion = version();
    std::size_t offset = 0;

    if (xversion == 0 || xversion == 1 || xversion == 2) {
        result.item_id = read_be<std::uint16_t>(payload);
        offset = 2;
    } else {
        result.item_id = read_be<std::uint32_t>(payload);
        offset = 4;
    }

    result.item_protection_index = read_be<std::uint16_t>(payload.subspan(offset));
    offset += 2;

    if (xversion >= 2) {
        result.item_type = fourcc_string::from_data(payload.subspan(offset));
        offset += 4;
    }

    auto name = read_cstr(payload, offset);
    result.item_name = name.value;
    offset = name.next;

    if (xversion >= 2) {
        if (result.item_type.view() == "mime") {
            auto type = read_cstr(payload, offset);
            result.content_type = type.value;
            offset = type.next;

            auto encoding = read_cstr(payload, offset);
            result.content_encoding = encoding.value;
            offset = encoding.next;
        } else if (result.item_type.view() == "uri ") {
            auto uri = read_cstr(payload, offset);
            result.item_uri_type = uri.value;
            offset = uri.next;
        }
    }

    return result;
}

//------------------------------------------------------------------------------------------------------------
// HDLR
constexpr auto basic_box_view<box_type::hdlr>::header() const noexcept -> hdlr_header
{
    hdlr_header result{};
    // Skip archaic pre-ISOBMFF reserved fields (4 bytes) "mhlr" or "alis"
    uint32_t reserved = read_be<uint32_t>(payload);

    // Handler type is at offset 4
    result.handler_type = fourcc_string::from_data(payload.subspan(4));

    // skip another 3x4 bytes of reserved data
    auto subspan = payload.subspan(16);
    if (!subspan.empty()) {
        auto name = read_cstr(subspan, 0);
        result.name = name.value;
    }
    return result;
}

//------------------------------------------------------------------------------------------------------------
// PITM
constexpr auto basic_box_view<box_type::pitm>::item_id() const noexcept -> uint32_t
{
    if (version() == 0) {
        return read_be<uint16_t>(payload);
    }
    return read_be<uint32_t>(payload);
}

//------------------------------------------------------------------------------------------------------------
// ILOC
constexpr auto basic_box_view<box_type::iloc>::header() const noexcept -> iloc_header
{
    iloc_header result{};
    uint8_t first_byte = static_cast<uint8_t>(payload[0]);
    result.offset_size = (first_byte >> 4) & 0x0F;
    result.length_size = first_byte & 0x0F;
    uint8_t second_byte = static_cast<uint8_t>(payload[1]);
    result.base_offset_size = (second_byte >> 4) & 0x0F;
    result.index_size = second_byte & 0x0F;

    auto xpayload = this->payload.subspan(2);
    if (version() < 2) {
        result.item_count = read_be<uint16_t>(xpayload);
        result.item_data = xpayload.subspan(2);
    } else {
        result.item_count = read_be<uint32_t>(xpayload);
        result.item_data = xpayload.subspan(4);
    }
    return result;
}

//------------------------------------------------------------------------------------------------------------
// ISPE
constexpr auto basic_box_view<box_type::ispe>::header() const noexcept -> ispe_header
{
    ispe_header result{};
    result.image_width = read_be<uint32_t>(payload);
    result.image_height = read_be<uint32_t>(payload.subspan(4));
    return result;
}

//------------------------------------------------------------------------------------------------------------
// AV1C
constexpr auto basic_box_view<box_type::av1C>::header() const noexcept -> av1C_header
{
    av1C_header result{};
    if (payload.size() < 3) {
        return result;
    }

    // Byte 0
    std::uint8_t read_byte = static_cast<uint8_t>(payload[0]);
    result.marker = (read_byte >> 7) & 0x01;
    result.version = read_byte & 0x7F;

    // Byte 1
    read_byte = static_cast<uint8_t>(payload[1]);
    result.seq_profile = (read_byte >> 5) & 0x07;
    result.seq_level_idx_0 = read_byte & 0x1F;

    // Byte 2
    read_byte = static_cast<uint8_t>(payload[2]);
    result.seq_tier_0 = (read_byte >> 7) & 0x01;
    result.high_bitdepth = (read_byte >> 6) & 0x01;
    result.twelve_bit = (read_byte >> 5) & 0x01;
    result.monochrome = (read_byte >> 4) & 0x01;
    result.chroma_subsampling_x = (read_byte >> 3) & 0x01;
    result.chroma_subsampling_y = (read_byte >> 2) & 0x01;
    result.chroma_sample_position = read_byte & 0x03;

    // Byte 3
    read_byte = static_cast<uint8_t>(payload[3]);
    result.reserved = (read_byte >> 5) & 0x07;
    result.initial_presentation_delay_present = (read_byte >> 4) & 0x01;
    if (result.initial_presentation_delay_present) {
        result.initial_presentation_delay_minus_one = read_byte & 0x0F;
    }

    result.config_obus = payload.subspan(4);
    return result;
}

//------------------------------------------------------------------------------------------------------------
template <box_type Box>
constexpr auto box_cast(const any_box_view& box) noexcept -> basic_box_view<Box>
{
    if (box.header.type != Box) {
        return {};
    }
    return static_cast<basic_box_view<Box>>(box);
}

//------------------------------------------------------------------------------------------------------------
enum class iterator_flags : uint32_t {
    none = 0,
    recursive = 1 << 0,
};
constexpr auto operator|(iterator_flags a, iterator_flags b) noexcept -> iterator_flags
{
    return static_cast<iterator_flags>(std::to_underlying(a) | std::to_underlying(b));
}
constexpr auto operator&(iterator_flags a, iterator_flags b) noexcept -> iterator_flags
{
    return static_cast<iterator_flags>(std::to_underlying(a) & std::to_underlying(b));
}
constexpr auto operator+(iterator_flags a) noexcept -> std::underlying_type_t<iterator_flags>
{
    return std::to_underlying(a);
}
constexpr auto has(iterator_flags a, iterator_flags check) noexcept -> bool
{
    return (a & check) != iterator_flags::none;
}

struct box_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::expected<any_box_view, unexpected>;
    using difference_type = std::ptrdiff_t;

    std::span<const std::byte> remaining;
    iterator_flags flags = iterator_flags::none;

public:
    constexpr box_iterator() noexcept = default;
    constexpr explicit box_iterator(
        std::span<const std::byte> data,
        iterator_flags flags = iterator_flags::none
    ) noexcept
        : remaining(data)
        , flags(flags)
    {}

public:
    constexpr static auto begin(std::span<const std::byte> data) noexcept -> box_iterator
    {
        return box_iterator{data};
    }
    constexpr static auto end(std::span<const std::byte> data) noexcept -> box_iterator
    {
        return box_iterator{data.subspan(data.size())};
    }
    constexpr auto begin() const noexcept -> box_iterator
    {
        return *this;
    }
    constexpr auto end() const noexcept -> box_iterator
    {
        return {};
    }
    constexpr auto try_get() const noexcept -> std::expected<any_box_view, unexpected>
    {
        return parse(remaining);
    }
    constexpr auto operator*() const noexcept -> std::expected<any_box_view, unexpected>
    {
        return try_get();
    }
    constexpr auto operator++() noexcept -> box_iterator&
    {
        if (remaining.empty()) {
            remaining = {};
            return *this;
        }
        auto result = parse(remaining);
        if (!result) {
            remaining = {};
            return *this;
        }

        // If recursive flag is set and the box is a container, iterate into it instead of moving to the next sibling
        if (has(flags, iterator_flags::recursive)) {
            auto properties = get_box_properties(result->header.type);

            if (has(properties, properties::container)) {
                auto* current_end = &remaining.back() + 1;
                auto* payload_start = &result->payload[0];

                remaining = std::span<const std::byte>(payload_start, current_end);
                return *this;
            }
        }

        std::size_t box_size = (result->header.size == 0) ? remaining.size()
                                                          : static_cast<std::size_t>(result->header.size);
        if (box_size == 0 || box_size > remaining.size()) {
            remaining = {};
        } else {
            remaining = remaining.subspan(box_size);
        }
        return *this;
    }
    constexpr auto operator++(int) noexcept -> box_iterator
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }
    constexpr bool operator==(const box_iterator& other) const noexcept
    {
        // special case: both iterators are at the end (empty)
        if (remaining.empty() && other.remaining.empty()) {
            return true;
        }
        return remaining.data() == other.remaining.data() && remaining.size() == other.remaining.size();
    }
};
} // namespace mbmff

#undef MBMFF_ITERATE_BOX_TYPES

//------------------------------------------------------------------------------------------------------------
template <>
struct std::formatter<mbmff::error_code> : std::formatter<std::string_view> {
    auto format(mbmff::error_code code, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(mbmff::get_error_message(code), ctx);
    }
};

template <>
struct std::formatter<mbmff::box_header> : std::formatter<std::string_view> {
    auto format(const mbmff::box_header& header, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format("{} ({} bytes)", header.type_string().view(), header.size),
            ctx
        );
    }
};

template <>
struct std::formatter<mbmff::ftyp_box> : std::formatter<std::string_view> {
    auto format(const mbmff::ftyp_box& box, std::format_context& ctx) const
    {
        std::string output = std::format(
            "FTYP: {} minor={} compatible=[",
            box.major_brand().view(),
            box.minor_version()
        );

        auto compatible_brands = box.compatible_brands();

        for (std::size_t i = 0; i < compatible_brands.size(); ++i) {
            if (i != 0) {
                output.append(", ");
            }
            output.append(compatible_brands[i].view());
        }
        output.push_back(']');
        return std::formatter<std::string_view>::format(output, ctx);
    }
};

template <>
struct std::formatter<mbmff::meta_box> : std::formatter<std::string_view> {
    auto format(const mbmff::meta_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format("META: version={} flags=0x{:06X}", box.header.version, box.header.flags_value()),
            ctx
        );
    }
};

template <>
struct std::formatter<mbmff::mdat_box> : std::formatter<std::string_view> {
    auto format(const mbmff::mdat_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(std::format("MDAT: payload={} bytes", box.data_size()), ctx);
    }
};

template <>
struct std::formatter<mbmff::iinf_box> : std::formatter<std::string_view> {
    auto format(const mbmff::iinf_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format(
                "IINF: version={} flags=0x{:06X}, entries={}",
                box.header.version,
                box.header.flags_value(),
                box.entry_count()
            ),
            ctx
        );
    }
};

template <>
struct std::formatter<mbmff::infe_box> : std::formatter<std::string_view> {
    auto format(const mbmff::infe_box& box, std::format_context& ctx) const
    {
        auto header = box.header();
        auto version = box.version();
        std::string output = std::format(
            "INFE: version={} id={} protection={}",
            version,
            header.item_id,
            header.item_protection_index
        );

        if (version < 2) {
            output.append(std::format(" name={}", header.item_name));
        }

        if (version >= 2) {
            output.append(std::format(" type={}", header.item_type.view()));
            if (header.item_type.view() == "mime") {
                output.append(std::format(" content_type={}", header.content_type));
                if (!header.content_encoding.empty()) {
                    output.append(std::format(" content_encoding={}", header.content_encoding));
                }
            } else if (header.item_type.view() == "uri ") {
                output.append(std::format(" uri={}", header.item_uri_type));
            }
        }

        return std::formatter<std::string_view>::format(output, ctx);
    }
};

template <>
struct std::formatter<mbmff::hdlr_box> : std::formatter<std::string_view> {
    auto format(const mbmff::hdlr_box& box, std::format_context& ctx) const
    {
        auto header = box.header();
        std::string output = std::format("HDLR: handler_type={}", header.handler_type.view());
        if (!header.name.empty()) {
            output.append(std::format(" name={}", header.name));
        }
        return std::formatter<std::string_view>::format(output, ctx);
    }
};

template <>
struct std::formatter<mbmff::pitm_box> : std::formatter<std::string_view> {
    auto format(const mbmff::pitm_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(std::format("PITM: item_id={}", box.item_id()), ctx);
    }
};

template <>
struct std::formatter<mbmff::iloc_box> : std::formatter<std::string_view> {
    auto format(const mbmff::iloc_box& box, std::format_context& ctx) const
    {
        auto header = box.header();
        std::string output = std::format(
            "ILOC: version={} offset_size={} length_size={} base_offset_size={} index_size={} item_count={}",
            box.version(),
            header.offset_size,
            header.length_size,
            header.base_offset_size,
            header.index_size,
            header.item_count
        );
        return std::formatter<std::string_view>::format(output, ctx);
    }
};

template <>
struct std::formatter<mbmff::iprp_box> : std::formatter<std::string_view> {
    auto format(const mbmff::iprp_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format("IPRP: Container", ctx);
    }
};

template <>
struct std::formatter<mbmff::ipco_box> : std::formatter<std::string_view> {
    auto format(const mbmff::ipco_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format("IPCO: Container", ctx);
    }
};

template <>
struct std::formatter<mbmff::ispe_box> : std::formatter<std::string_view> {
    auto format(const mbmff::ispe_box& box, std::format_context& ctx) const
    {
        auto header = box.header();
        return std::formatter<std::string_view>::format(
            std::format("ISPE: image_width={} image_height={}", header.image_width, header.image_height),
            ctx
        );
    }
};

template <>
struct std::formatter<mbmff::av1C_box> : std::formatter<std::string_view> {
    auto format(const mbmff::av1C_box& box, std::format_context& ctx) const
    {
        auto header = box.header();
        std::string output = std::format(
            "av1C: profile={} level={} tier={} bitdepth={} monochrome={} chroma_subsampling=({}, {})",
            std::uint8_t(header.seq_profile),
            std::uint8_t(header.seq_level_idx_0),
            header.seq_tier_0 ? "high" : "main",
            header.high_bitdepth ? (header.twelve_bit ? 12 : 10) : 8,
            header.monochrome ? "yes" : "no",
            header.chroma_subsampling_x ? "1" : "0",
            header.chroma_subsampling_y ? "1" : "0"
        );
        return std::formatter<std::string_view>::format(output, ctx);
    }
};
