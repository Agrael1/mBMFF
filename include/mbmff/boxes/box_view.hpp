#pragma once
#include <span>
#include "common.hpp"

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
    MACRO(pasp)                        \
    MACRO(infe)                        \
    MACRO(mvhd)                        \
    MACRO(tkhd)                        \
    MACRO(elst)                        \
    MACRO(mdhd)                        \
    MACRO(vmhd)                        \
    MACRO(smhd)                        \
    MACRO(stts)                        \
    MACRO(stsd)                        \
    MACRO(dref)

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
#define MBMFF_ITERATE_ENUM(name) name = mbmff::fourcc(#name),
enum class box_type : std::uint32_t {
    unknown,
    MBMFF_ITERATE_BOX_TYPES(MBMFF_ITERATE_ENUM) url = mbmff::fourcc("url ")
};

//------------------------------------------------------------------------------------------------------------
enum class box_properties : std::uint32_t {
    none = 0,
    full_box = 1 << 0,
    container = 1 << 1,
};
MBMFF_FLAG_OPERATORS(box_properties)

//------------------------------------------------------------------------------------------------------------
struct box_header {
    std::uint64_t size_ = 0;
    mbmff::box_type type_ = mbmff::box_type::unknown;
    std::uint8_t version_ = 0;
    std::array<std::uint8_t, 3> flags_{};

public:
    constexpr auto type_string() const noexcept -> mbmff::fourcc_string
    {
        return mbmff::fourcc_string::from_uint32(mbmff::to_underlying(type_));
    }
    constexpr auto size() const noexcept -> std::uint64_t
    {
        return size_;
    }
    constexpr auto type() const noexcept -> mbmff::box_type
    {
        return type_;
    }
    constexpr auto flags() const noexcept -> std::uint32_t
    {
        return (static_cast<std::uint32_t>(flags_[0]) << 16) | (static_cast<std::uint32_t>(flags_[1]) << 8)
             | static_cast<std::uint32_t>(flags_[2]);
    }
    constexpr auto version() const noexcept -> std::uint8_t
    {
        return version_;
    }
    constexpr auto valid() const noexcept -> bool
    {
        return type_ != mbmff::box_type::unknown;
    }
    constexpr operator bool() const noexcept
    {
        return valid();
    }

public:
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

struct box_view_base : mbmff::box_header {
    std::span<const std::byte> payload{};

public:
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>
    {
        return {box};
    }
};

template <mbmff::box_type Type>
struct basic_box_view : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::none;
};

//------------------------------------------------------------------------------------------------------------
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
