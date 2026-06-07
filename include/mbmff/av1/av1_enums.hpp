#pragma once
#include <cstdint>
#include <string_view>

namespace mbmff::av1 {
enum class obu_type : std::uint8_t {
    sequence_header = 1,
    temporal_delimiter = 2,
    frame_header = 3,
    tile_group = 4,
    metadata = 5,
    frame = 6,
    redundant_frame_header = 7,
    tile_list = 8,
    // 9-14 are reserved
    padding = 15, // special type used for padding OBUs that may be present in the stream. These should be ignored by
                  // decoders.
};
constexpr bool operator==(obu_type type, std::uint8_t value) noexcept
{
    return static_cast<std::uint8_t>(type) == value;
}
constexpr bool operator==(std::uint8_t value, av1::obu_type type) noexcept
{
    return type == value;
}

constexpr static auto to_string(av1::obu_type type) noexcept -> std::string_view
{
    switch (type) {
    case obu_type::sequence_header:
        return "Sequence Header";
    case obu_type::temporal_delimiter:
        return "Temporal Delimiter";
    case obu_type::frame_header:
        return "Frame Header";
    case obu_type::tile_group:
        return "Tile Group";
    case obu_type::metadata:
        return "Metadata";
    case obu_type::frame:
        return "Frame";
    case obu_type::redundant_frame_header:
        return "Redundant Frame Header";
    case obu_type::tile_list:
        return "Tile List";
    case obu_type::padding:
        return "Padding";
    default:
        return "Unknown OBU Type";
    }
}

enum class color_primaries : std::uint8_t {
    CP_RESERVED_0 = 0,
    CP_BT_709 = 1,
    CP_UNSPECIFIED = 2,
    CP_RESERVED_3 = 3,
    CP_BT_470_M = 4,
    CP_BT_470_B_G = 5,
    CP_BT_601 = 6,
    CP_SMPTE_240 = 7,
    CP_GENERIC_FILM = 8,
    CP_BT_2020 = 9,
    CP_XYZ = 10,
    CP_SMPTE_431 = 11,
    CP_SMPTE_432 = 12,
    CP_EBU_3213 = 22,
};

constexpr auto to_string(av1::color_primaries primaries) noexcept -> std::string_view
{
    switch (primaries) {
    case av1::color_primaries::CP_RESERVED_0:
        return "Reserved (0)";
    case av1::color_primaries::CP_BT_709:
        return "BT.709";
    case av1::color_primaries::CP_UNSPECIFIED:
        return "Unspecified";
    case av1::color_primaries::CP_RESERVED_3:
        return "Reserved (3)";
    case av1::color_primaries::CP_BT_470_M:
        return "BT.470 System M (historical)";
    case av1::color_primaries::CP_BT_470_B_G:
        return "BT.470 System B/G (historical)";
    case av1::color_primaries::CP_BT_601:
        return "BT.601";
    case av1::color_primaries::CP_SMPTE_240:
        return "SMPTE 240M";
    case av1::color_primaries::CP_GENERIC_FILM:
        return "Generic film (color filters using illuminant C)";
    case av1::color_primaries::CP_BT_2020:
        return "BT.2020, BT.2100";
    case av1::color_primaries::CP_XYZ:
        return "SMPTE 428 (CIE 1921 XYZ)";
    case av1::color_primaries::CP_SMPTE_431:
        return "SMPTE RP 431-2";
    case av1::color_primaries::CP_SMPTE_432:
        return "SMPTE EG 432-1";
    case av1::color_primaries::CP_EBU_3213:
        return "EBU Tech 3213-E";
    default:
        return "Unknown color primaries";
    }
}

enum class transfer_characteristics : std::uint8_t {
    TC_RESERVED_0 = 0,
    TC_BT_709 = 1,
    TC_UNSPECIFIED = 2,
    TC_RESERVED_3 = 3,
    TC_BT_470_M = 4,
    TC_BT_470_B_G = 5,
    TC_BT_601 = 6,
    TC_SMPTE_240 = 7,
    TC_LINEAR = 8,
    TC_LOG_100 = 9,
    TC_LOG_100_SQRT10 = 10,
    TC_IEC_61966 = 11,
    TC_BT_1361 = 12,
    TC_SRGB = 13,
    TC_BT_2020_10_BIT = 14,
    TC_BT_2020_12_BIT = 15,
    TC_SMPTE_2084 = 16,
    TC_SMPTE_428 = 17,
    TC_HLG = 18,
};

constexpr auto to_string(av1::transfer_characteristics tc) noexcept -> std::string_view
{
    switch (tc) {
    case av1::transfer_characteristics::TC_RESERVED_0:
        return "Reserved (0)";
    case av1::transfer_characteristics::TC_BT_709:
        return "BT.709";
    case av1::transfer_characteristics::TC_UNSPECIFIED:
        return "Unspecified";
    case av1::transfer_characteristics::TC_RESERVED_3:
        return "Reserved (3)";
    case av1::transfer_characteristics::TC_BT_470_M:
        return "BT.470 System M (historical)";
    case av1::transfer_characteristics::TC_BT_470_B_G:
        return "BT.470 System B/G (historical)";
    case av1::transfer_characteristics::TC_BT_601:
        return "BT.601";
    case av1::transfer_characteristics::TC_SMPTE_240:
        return "SMPTE 240M";
    case av1::transfer_characteristics::TC_LINEAR:
        return "Linear";
    case av1::transfer_characteristics::TC_LOG_100:
        return "Logarithmic (100:1 range)";
    case av1::transfer_characteristics::TC_LOG_100_SQRT10:
        return "Logarithmic (100 * sqrt(10):1 range)";
    case av1::transfer_characteristics::TC_IEC_61966:
        return "IEC 61966-2-4";
    case av1::transfer_characteristics::TC_BT_1361:
        return "BT.1361";
    case av1::transfer_characteristics::TC_SRGB:
        return "sRGB or sYCC";
    case av1::transfer_characteristics::TC_BT_2020_10_BIT:
        return "BT.2020 10-bit system";
    case av1::transfer_characteristics::TC_BT_2020_12_BIT:
        return "BT.2020 12-bit system";
    case av1::transfer_characteristics::TC_SMPTE_2084:
        return "SMPTE ST 2084, ITU BT.2100 PQ";
    case av1::transfer_characteristics::TC_SMPTE_428:
        return "SMPTE ST 428";
    case av1::transfer_characteristics::TC_HLG:
        return "BT.2100 HLG, ARIB STD-B67";
    default:
        return "Unknown transfer characteristics";
    }
}

enum class matrix_coefficients : std::uint8_t {
    MC_IDENTITY = 0,
    MC_BT_709 = 1,
    MC_UNSPECIFIED = 2,
    MC_RESERVED_3 = 3,
    MC_FCC = 4,
    MC_BT_470_B_G = 5,
    MC_BT_601 = 6,
    MC_SMPTE_240 = 7,
    MC_SMPTE_YCGCO = 8,
    MC_BT_2020_NCL = 9,
    MC_BT_2020_CL = 10,
    MC_SMPTE_2085 = 11,
    MC_CHROMAT_NCL = 12,
    MC_CHROMAT_CL = 13,
    MC_ICTCP = 14,
};

constexpr auto to_string(av1::matrix_coefficients mc) noexcept -> std::string_view
{
    switch (mc) {
    case av1::matrix_coefficients::MC_IDENTITY:
        return "Identity";
    case av1::matrix_coefficients::MC_BT_709:
        return "BT.709";
    case av1::matrix_coefficients::MC_UNSPECIFIED:
        return "Unspecified";
    case av1::matrix_coefficients::MC_RESERVED_3:
        return "Reserved (3)";
    case av1::matrix_coefficients::MC_FCC:
        return "US FCC 73.628";
    case av1::matrix_coefficients::MC_BT_470_B_G:
        return "BT.470 System B/G (historical)";
    case av1::matrix_coefficients::MC_BT_601:
        return "BT.601";
    case av1::matrix_coefficients::MC_SMPTE_240:
        return "SMPTE 240M";
    case av1::matrix_coefficients::MC_SMPTE_YCGCO:
        return "YCgCo";
    case av1::matrix_coefficients::MC_BT_2020_NCL:
        return "BT.2020 non-constant luminance system";
    case av1::matrix_coefficients::MC_BT_2020_CL:
        return "BT.2020 constant luminance system";
    case av1::matrix_coefficients::MC_SMPTE_2085:
        return "SMPTE ST 2085 YDzDx";
    case av1::matrix_coefficients::MC_CHROMAT_NCL:
        return "Chromaticity-derived non-constant luminance system";
    case av1::matrix_coefficients::MC_CHROMAT_CL:
        return "Chromaticity-derived constant luminance system";
    case av1::matrix_coefficients::MC_ICTCP:
        return "ICTCP (BT.2100)";
    default:
        return "Unknown matrix coefficients";
    }
}

enum class chroma_sample_position : std::uint8_t {
    CSP_UNKNOWN = 0,
    CSP_VERTICAL = 1,
    CSP_COLOCATED = 2,
    CSP_RESERVED = 3,
};

constexpr auto to_string(av1::chroma_sample_position csp) noexcept -> std::string_view
{
    switch (csp) {
    case av1::chroma_sample_position::CSP_UNKNOWN:
        return "Unknown";
    case av1::chroma_sample_position::CSP_VERTICAL:
        return "Vertical";
    case av1::chroma_sample_position::CSP_COLOCATED:
        return "Colocated";
    case av1::chroma_sample_position::CSP_RESERVED:
        return "Reserved";
    default:
        return "Invalid chroma sample position";
    }
}

enum class metadata_type : std::uint32_t {
    METADATA_TYPE_HDR_CLL = 1,
    METADATA_TYPE_HDR_MDCV = 2,
    METADATA_TYPE_SCALABILITY = 3,
    METADATA_TYPE_ITUT_T35 = 4,
    METADATA_TYPE_TIMECODE = 5,
};

constexpr auto to_string(av1::metadata_type type) noexcept -> std::string_view
{
    switch (type) {
    case av1::metadata_type::METADATA_TYPE_ITUT_T35:
        return "ITU-T T.35";
    case av1::metadata_type::METADATA_TYPE_HDR_CLL:
        return "HDR Content Light Level (CLL)";
    case av1::metadata_type::METADATA_TYPE_HDR_MDCV:
        return "HDR Mastering Display Color Volume (MDCV)";
    case av1::metadata_type::METADATA_TYPE_SCALABILITY:
        return "Scalability";
    case av1::metadata_type::METADATA_TYPE_TIMECODE:
        return "Timecode";
    default:
        return "Unknown metadata type";
    }
}

enum class frame_type : std::uint8_t {
    KEY_FRAME,
    INTER_FRAME,
    INTRA_ONLY_FRAME,
    SWITCH_FRAME,
};

constexpr auto to_string(av1::frame_type type) noexcept -> std::string_view
{
    switch (type) {
    case frame_type::KEY_FRAME:
        return "Key Frame";
    case frame_type::INTER_FRAME:
        return "Inter Frame";
    case frame_type::INTRA_ONLY_FRAME:
        return "Intra-only Frame";
    case frame_type::SWITCH_FRAME:
        return "Switch Frame";
    default:
        return "Unknown frame type";
    }
}

enum class ref_frame : std::int8_t {
    NONE = -1,
    INTRA_FRAME = 0,
    LAST_FRAME = 1,
    LAST2_FRAME = 2,
    LAST3_FRAME = 3,
    GOLDEN_FRAME = 4,
    BWDREF_FRAME = 5,
    ALTREF2_FRAME = 6,
    ALTREF_FRAME = 7,
};

constexpr auto to_string(av1::ref_frame ref) noexcept -> std::string_view
{
    switch (ref) {
    case av1::ref_frame::NONE:
        return "None";
    case av1::ref_frame::INTRA_FRAME:
        return "Intra Frame";
    case av1::ref_frame::LAST_FRAME:
        return "Last Frame";
    case av1::ref_frame::LAST2_FRAME:
        return "Last2 Frame";
    case av1::ref_frame::LAST3_FRAME:
        return "Last3 Frame";
    case av1::ref_frame::GOLDEN_FRAME:
        return "Golden Frame";
    case av1::ref_frame::BWDREF_FRAME:
        return "BWDREF Frame";
    case av1::ref_frame::ALTREF2_FRAME:
        return "ALTREF2 Frame";
    case av1::ref_frame::ALTREF_FRAME:
        return "ALTREF Frame";
    default:
        return "Unknown reference frame";
    }
}

constexpr std::int8_t operator+(av1::ref_frame ref) noexcept
{
    return std::to_underlying(ref);
}

enum class interpolation_filter : std::uint8_t {
    EIGHTTAP = 0,
    EIGHTTAP_SMOOTH = 1,
    EIGHTTAP_SHARP = 2,
    BILINEAR = 3,
    SWITCHABLE = 4,
};

constexpr auto to_string(av1::interpolation_filter filter) noexcept -> std::string_view
{
    switch (filter) {
    case av1::interpolation_filter::EIGHTTAP:
        return "8-tap";
    case av1::interpolation_filter::EIGHTTAP_SMOOTH:
        return "8-tap smooth";
    case av1::interpolation_filter::EIGHTTAP_SHARP:
        return "8-tap sharp";
    case av1::interpolation_filter::BILINEAR:
        return "Bilinear";
    case av1::interpolation_filter::SWITCHABLE:
        return "Switchable";
    default:
        return "Unknown interpolation filter";
    }
}

// Value that indicates the allow_screen_content_tools syntax element is coded
static constexpr std::uint8_t SELECT_SCREEN_CONTENT_TOOLS = 2;

// Value that indicates the force_integer_mv syntax element is coded
static constexpr std::uint8_t SELECT_INTEGER_MV = 2;

// The maximum number of reference frames that can be used for inter prediction in AV1.
static constexpr std::uint32_t NUM_REF_FRAMES = 8;

// Value of primary_ref_frame indicating that there is no primary reference frame
static constexpr std::uint8_t PRIMARY_REF_NONE = 7;

// Smallest denominator for upscaling ratio
static constexpr std::uint8_t SUPERRES_DENOM_MIN = 9;

// Smallest denominator for upscaling ratio
static constexpr std::uint8_t SUPERRES_NUM = 8;

// Smallest denominator for upscaling ratio
static constexpr std::uint8_t REFS_PER_FRAME = 8;

// Maximum width of a tile in units of luma samples
static constexpr std::uint32_t MAX_TILE_WIDTH = 4096;

// Maximum area of a tile in units of luma samples
static constexpr std::uint32_t MAX_TILE_AREA = 4096 * 2304;

// Maximum number of tile rows
static constexpr std::uint32_t MAX_TILE_ROWS = 64;

// Maximum number of tile columns
static constexpr std::uint32_t MAX_TILE_COLS = 64;

// Number of segments allowed in segmentation map
static constexpr std::uint32_t MAX_SEGMENTS = 8;

// Number of segment features
static constexpr std::uint32_t SEG_LVL_MAX = 8;

} // namespace mbmff::av1

// Bring to_string functions into mbmff namespace for easier access when formatting
namespace mbmff {
using av1::to_string;
}
