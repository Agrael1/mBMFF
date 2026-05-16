#pragma once
#include <array>
#include <bit>
#include <concepts>
#include <cstring>
#include <expected>
#include <format>
#include <span>
#include <string_view>
#include <type_traits>

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
    MACRO(infe)

namespace mbmff {
enum class error_code {
    success = 0,
    invalid_format,
    need_more_data,
};

//------------------------------------------------------------------------------------------------------------
static constexpr auto get_error_message(error_code code) noexcept -> std::string_view
{
    switch (code) {
    case error_code::success:
        return "Success";
    case error_code::invalid_format:
        return "Invalid format";
    case error_code::need_more_data:
        return "Need more data";
    default:
        return "Unknown error code";
    }
}

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
template <typename Type>
struct parsed {
    Type value;
    std::size_t consumed;
};
struct unexpected {
    mbmff::error_code code;
    std::size_t needed;
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
    fourcc_string item_type{};
    std::string_view item_name{};
    std::string_view content_type{};
    std::string_view content_encoding{};
    std::string_view item_uri_type{};
};

struct ftyp_header {
    fourcc_string major_brand{};
    std::uint32_t minor_version = 0;
    std::span<const fourcc_string> compatible_brands{};
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
};

template <>
struct basic_box_view<box_type::infe> : public box_view_base {
    constexpr static properties properties = properties::full_box;
    constexpr auto header() const noexcept -> infe_header;
    constexpr auto item_id() const noexcept -> std::uint32_t;
    constexpr auto item_protection_index() const noexcept -> std::uint16_t;
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
    return ftyp_header{major_brand(),
        minor_version(),
        compatible_brands()
    };
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
template <box_type Box>
constexpr auto box_cast(const any_box_view& box) noexcept -> basic_box_view<Box>
{
    if (box.header.type != Box) {
        return {};
    }
    return static_cast<basic_box_view<Box>>(box);
}

//------------------------------------------------------------------------------------------------------------
struct box_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::expected<any_box_view, unexpected>;
    using difference_type = std::ptrdiff_t;

    std::span<const std::byte> remaining;

public:
    constexpr box_iterator() noexcept = default;
    constexpr explicit box_iterator(std::span<const std::byte> data) noexcept
        : remaining(data)
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
        if (result) {
            std::size_t box_size = (result->header.size == 0) ? remaining.size()
                                                              : static_cast<std::size_t>(result->header.size);
            if (box_size == 0 || box_size > remaining.size()) {
                remaining = {};
            } else {
                remaining = remaining.subspan(box_size);
            }
        } else {
            remaining = {};
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

// TODO: implement
struct recursive_box_iterator {
    using iterator_category = std::forward_iterator_tag;
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
            std::format("IINF: version={} flags=0x{:06X}", box.header.version, box.header.flags_value()),
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
