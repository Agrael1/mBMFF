#pragma once
#include <span>
#include "common.hpp"

#define MBMFF_ITERATE_BOX_TYPES(MACRO) \
    MACRO(alis, "alis")                \
    MACRO(apcn, "apcn")                \
    MACRO(av1C, "av1C")                \
    MACRO(avc1, "avc1")                \
    MACRO(avc3, "avc3")                \
    MACRO(avcC, "avcC")                \
    MACRO(btrt, "btrt")                \
    MACRO(cdsc, "cdsc")                \
    MACRO(co64, "co64")                \
    MACRO(colr, "colr")                \
    MACRO(ctts, "ctts")                \
    MACRO(dimg, "dimg")                \
    MACRO(dinf, "dinf")                \
    MACRO(dref, "dref")                \
    MACRO(edts, "edts")                \
    MACRO(elst, "elst")                \
    MACRO(esds, "esds")                \
    MACRO(fiel, "fiel")                \
    MACRO(fiin, "fiin")                \
    MACRO(free, "free")                \
    MACRO(frma, "frma")                \
    MACRO(ftyp, "ftyp")                \
    MACRO(gmhd, "gmhd")                \
    MACRO(gmin, "gmin")                \
    MACRO(hdlr, "hdlr")                \
    MACRO(hev1, "hev1")                \
    MACRO(hvcC, "hvcC")                \
    MACRO(iinf, "iinf")                \
    MACRO(iloc, "iloc")                \
    MACRO(ilst, "ilst")                \
    MACRO(infe, "infe")                \
    MACRO(ipco, "ipco")                \
    MACRO(ipma, "ipma")                \
    MACRO(ipro, "ipro")                \
    MACRO(iprp, "iprp")                \
    MACRO(iref, "iref")                \
    MACRO(irot, "irot")                \
    MACRO(ispe, "ispe")                \
    MACRO(load, "load")                \
    MACRO(mdat, "mdat")                \
    MACRO(mdhd, "mdhd")                \
    MACRO(mdia, "mdia")                \
    MACRO(meco, "meco")                \
    MACRO(mere, "mere")                \
    MACRO(meta, "meta")                \
    MACRO(mfra, "mfra")                \
    MACRO(minf, "minf")                \
    MACRO(moof, "moof")                \
    MACRO(moov, "moov")                \
    MACRO(mp4a, "mp4a")                \
    MACRO(mp4v, "mp4v")                \
    MACRO(mvex, "mvex")                \
    MACRO(mvhd, "mvhd")                \
    MACRO(paen, "paen")                \
    MACRO(pasp, "pasp")                \
    MACRO(pitm, "pitm")                \
    MACRO(pixi, "pixi")                \
    MACRO(sbgp, "sbgp")                \
    MACRO(schi, "schi")                \
    MACRO(schm, "schm")                \
    MACRO(sdtp, "sdtp")                \
    MACRO(sgpd, "sgpd")                \
    MACRO(sinf, "sinf")                \
    MACRO(smhd, "smhd")                \
    MACRO(stbl, "stbl")                \
    MACRO(stco, "stco")                \
    MACRO(stsc, "stsc")                \
    MACRO(stsd, "stsd")                \
    MACRO(stss, "stss")                \
    MACRO(stsz, "stsz")                \
    MACRO(strk, "strk")                \
    MACRO(stts, "stts")                \
    MACRO(tapt, "tapt")                \
    MACRO(thmb, "thmb")                \
    MACRO(tkhd, "tkhd")                \
    MACRO(tmcd, "tmcd")                \
    MACRO(traf, "traf")                \
    MACRO(trak, "trak")                \
    MACRO(tref, "tref")                \
    MACRO(udta, "udta")                \
    MACRO(urn, "urn ")                 \
    MACRO(url, "url ")                 \
    MACRO(vmhd, "vmhd")                \
    MACRO(wide, "wide")

#define MBMFF_FLAG_OPERATORS(EnumType)                                                   \
    constexpr auto operator|(EnumType a, EnumType b) noexcept -> EnumType                \
    {                                                                                    \
        return static_cast<EnumType>(mbmff::to_underlying(a) | mbmff::to_underlying(b)); \
    }                                                                                    \
    constexpr auto operator&(EnumType a, EnumType b) noexcept -> EnumType                \
    {                                                                                    \
        return static_cast<EnumType>(mbmff::to_underlying(a) & mbmff::to_underlying(b)); \
    }                                                                                    \
    constexpr auto operator+(EnumType a) noexcept -> std::underlying_type_t<EnumType>    \
    {                                                                                    \
        return mbmff::to_underlying(a);                                                  \
    }                                                                                    \
    constexpr auto has(EnumType a, EnumType check) noexcept -> bool                      \
    {                                                                                    \
        return (a & check) != EnumType::none;                                            \
    }

namespace mbmff {
#define MBMFF_ITERATE_ENUM(name, fourcc_str) name = mbmff::fourcc(fourcc_str),
/// @brief Enumeration of known ISOBMFF box types, generated from `MBMFF_ITERATE_BOX_TYPES`.
enum class box_type : std::uint32_t {
    /// @brief Unknown / unrecognised box type.
    unknown,
    MBMFF_ITERATE_BOX_TYPES(MBMFF_ITERATE_ENUM)
};

//------------------------------------------------------------------------------------------------------------
/// @brief Describes behavioural properties of a box type.
enum class box_properties : std::uint32_t {
    /// @brief No special properties.
    none = 0,

    /// @brief The box has a FullBox header (version + flags) after the size+type header.
    full_box = 1 << 0,

    /// @brief The box is a container: its payload consists of child boxes.
    container = 1 << 1,
};
MBMFF_FLAG_OPERATORS(box_properties)

//------------------------------------------------------------------------------------------------------------
/// @brief Header common to all ISOBMFF boxes: size, type, and (for FullBox) version+flags.
struct box_header {
    /// @brief Size of the box in bytes (including header). 0 means "remaining of file".
    std::uint64_t size_ = 0;

    /// @brief The box type.
    mbmff::box_type type_ = mbmff::box_type::unknown;

    /// @brief FullBox version (valid only for FullBox types after fill_full_header is called).
    std::uint8_t version_ = 0;

    /// @brief FullBox flags (3 bytes, valid only for FullBox types after fill_full_header is called).
    std::array<std::uint8_t, 3> flags_{};

public:
    /// @brief Returns the box type as a fourcc string.
    constexpr auto type_string() const noexcept -> mbmff::fourcc_string
    {
        return mbmff::fourcc_string::from_uint32(mbmff::to_underlying(type_));
    }
    /// @brief Returns the box size in bytes.
    constexpr auto size() const noexcept -> std::uint64_t
    {
        return size_;
    }
    /// @brief Returns the box type.
    constexpr auto type() const noexcept -> mbmff::box_type
    {
        return type_;
    }
    /// @brief Returns the FullBox flags as a 24-bit value (only valid after fill_full_header).
    constexpr auto flags() const noexcept -> std::uint32_t
    {
        return (static_cast<std::uint32_t>(flags_[0]) << 16) | (static_cast<std::uint32_t>(flags_[1]) << 8)
             | static_cast<std::uint32_t>(flags_[2]);
    }
    /// @brief Returns the FullBox version (only valid after fill_full_header).
    constexpr auto version() const noexcept -> std::uint8_t
    {
        return version_;
    }
    /// @brief Returns true if the box type is known (not `box_type::unknown`).
    constexpr auto valid() const noexcept -> bool
    {
        return type_ != mbmff::box_type::unknown;
    }
    /// @brief Equivalent to `valid()`.
    constexpr operator bool() const noexcept
    {
        return valid();
    }

public:
    /// @brief Extracts the version and flags from the first 4 bytes of a FullBox payload.
    /// @param[in] data Byte span containing at least 4 bytes of FullBox header data.
    constexpr void fill_full_header(std::span<const std::byte> data) noexcept
    {
        version_ = static_cast<std::uint8_t>(data[0]);
        flags_[0] = static_cast<std::uint8_t>(data[1]);
        flags_[1] = static_cast<std::uint8_t>(data[2]);
        flags_[2] = static_cast<std::uint8_t>(data[3]);
    }
};

struct box_view_base;
using any_box_view = mbmff::box_view_base;

/// @brief Base view for a parsed box: combines box_header with a payload span.
///
/// Every specialized `basic_box_view<Type>` inherits from this. The default
/// `validate()` accepts any payload; box-specific validators trim and check it.
struct box_view_base : mbmff::box_header {
    /// @brief Pointer and size of the box payload (after the header, after any type-specific validation).
    std::span<const std::byte> payload{};

public:
    /// @brief Default validator: accepts any payload as-is.
    /// @param[in] box The box view to validate.
    /// @returns The same box view unmodified.
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>
    {
        return {box};
    }
};

/// @brief Primary template for typed box views. Each implemented box type has a specialization.
///
/// The primary template marks the box as `not_implemented = true` so that the
/// compile-time test can detect missing specializations.
/// @tparam Type The box type enum value.
template <mbmff::box_type Type>
struct basic_box_view : public mbmff::box_view_base {
    /// @brief Properties of this box type (none by default).
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
    /// @brief Set to `true` for unimplemented box types (caught by static_assert in tests).
    constexpr static bool not_implemented = true;
};

//------------------------------------------------------------------------------------------------------------
/// @brief Parses the 8-byte (or 16-byte for 64-bit size) box header at the start of a byte span.
/// @param[in] data Byte span containing at least 8 bytes.
/// @returns A parsed box_header and the number of bytes consumed (8 or 16), or an error.
constexpr inline auto parse_box_header(std::span<const std::byte> data) noexcept
    -> mbmff::result<mbmff::parsed<mbmff::box_header>>
{
    if (data.size() < 8) {
        return mbmff::make_result<mbmff::parsed<mbmff::box_header>>(mbmff::error_code::need_more_data, 8);
    }

    std::uint64_t size = mbmff::read_be<std::uint32_t>(data);
    if (size == 0) {
        return mbmff::make_result<mbmff::parsed<mbmff::box_header>>(mbmff::error_code::invalid_format);
    }

    char type_str[4]{
        static_cast<char>(data[4]),
        static_cast<char>(data[5]),
        static_cast<char>(data[6]),
        static_cast<char>(data[7]),
    };
    mbmff::box_type type = mbmff::box_type(mbmff::fourcc(type_str));
    if (size == 1) {
        if (data.size() < 16) {
            return mbmff::make_result<mbmff::parsed<mbmff::box_header>>(mbmff::error_code::need_more_data, 16);
        }
        size = mbmff::read_be<std::uint64_t>(data.subspan(8));
    }

    return {mbmff::parsed<mbmff::box_header>{{size, type}, size == 1 ? 16u : 8u}};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
// parse_box_header must reject data < 8 bytes
static_assert(!mbmff::parse_box_header(std::span<const std::byte>{}), "parse_box_header should reject empty data");
static_assert(
    []() {
        std::array<std::byte, 7> data{};
        return !mbmff::parse_box_header(std::span(data));
    }(),
    "parse_box_header should reject data with less than 8 bytes"
);
static_assert(
    []() {
        std::array<std::byte, 16> data{
            std::byte(1),
        };
        mbmff::parse_box_header(std::span(data));
        return true;
    }(),
    "parse_box_header should accept data with 16 bytes (for large size)"
);

// Try parsing a valid box header
static_assert(
    []() {
        std::array<std::byte, 8> data{
            std::byte(0),
            std::byte(0),
            std::byte(0),
            std::byte(12), // size = 12
            std::byte('m'),
            std::byte('d'),
            std::byte('a'),
            std::byte('t'), // type = "mdat"
        };
        auto result = mbmff::parse_box_header(std::span(data));
        return result && result->value.size() == 12 && result->value.type() == mbmff::box_type::mdat
            && result->consumed == 8;
    }(),
    "parse_box_header should parse a valid box header correctly"
);

#endif

} // namespace mbmff
