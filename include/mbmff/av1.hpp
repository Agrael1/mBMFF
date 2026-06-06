#pragma once
#include <expected>
#include <format>
#include <span>
#include "common.hpp"

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
constexpr bool operator==(obu_type type, uint8_t value) noexcept
{
    return static_cast<uint8_t>(type) == value;
}
constexpr bool operator==(uint8_t value, obu_type type) noexcept
{
    return type == value;
}

constexpr static auto to_string(obu_type type) noexcept -> std::string_view
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

constexpr auto to_string(color_primaries primaries) noexcept -> std::string_view
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

constexpr auto to_string(transfer_characteristics tc) noexcept -> std::string_view
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

constexpr auto to_string(matrix_coefficients mc) noexcept -> std::string_view
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

constexpr auto to_string(chroma_sample_position csp) noexcept -> std::string_view
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

enum class metadata_type : uint32_t {
    METADATA_TYPE_HDR_CLL = 1,
    METADATA_TYPE_HDR_MDCV = 2,
    METADATA_TYPE_SCALABILITY = 3,
    METADATA_TYPE_ITUT_T35 = 4,
    METADATA_TYPE_TIMECODE = 5,
};

constexpr auto to_string(metadata_type type) noexcept -> std::string_view
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

constexpr auto to_string(frame_type type) noexcept -> std::string_view
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

constexpr auto to_string(ref_frame ref) noexcept -> std::string_view
{
    switch (ref) {
    case ref_frame::NONE:
        return "None";
    case ref_frame::INTRA_FRAME:
        return "Intra Frame";
    case ref_frame::LAST_FRAME:
        return "Last Frame";
    case ref_frame::LAST2_FRAME:
        return "Last2 Frame";
    case ref_frame::LAST3_FRAME:
        return "Last3 Frame";
    case ref_frame::GOLDEN_FRAME:
        return "Golden Frame";
    case ref_frame::BWDREF_FRAME:
        return "BWDREF Frame";
    case ref_frame::ALTREF2_FRAME:
        return "ALTREF2 Frame";
    case ref_frame::ALTREF_FRAME:
        return "ALTREF Frame";
    default:
        return "Unknown reference frame";
    }
}

constexpr std::int8_t operator+(ref_frame ref) noexcept
{
    return std::to_underlying(ref);
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

//------------------------------------------------------------------------------------------------------------
struct timing_info {
    std::uint32_t num_units_in_display_tick = 0;
    std::uint32_t time_scale = 0;
    std::uint32_t equal_picture_interval = 0;
    std::uint32_t num_ticks_per_picture_minus_1 = 0;
};

struct decoder_model_info {
    std::uint8_t buffer_delay_length_minus_1 = 0;
    std::uint32_t num_units_in_decoding_tick = 0;
    std::uint8_t buffer_removal_time_length_minus_1 = 0;
    std::uint8_t frame_presentation_time_length_minus_1 = 0;
};

struct operating_parameters_info {
    std::uint32_t decoder_buffer_delay = 0;
    std::uint32_t encoder_buffer_delay = 0;
    std::uint32_t low_delay_mode_flag = 0;
};

struct operating_point {
    std::uint16_t operating_point_idc = 0;
    std::uint8_t seq_level_idx                             : 5 = 0;
    std::uint8_t seq_tier                                  : 1 = 0;
    std::uint8_t decoder_model_present_for_this_op         : 1 = 0;
    std::uint8_t initial_display_delay_present_for_this_op : 1 = 0;
    std::uint8_t initial_display_delay_minus_1             : 4 = 0;

    operating_parameters_info operating_parameters{};
};

struct color_description {
    av1::color_primaries color_primaries = av1::color_primaries::CP_UNSPECIFIED;
    av1::transfer_characteristics transfer_characteristics = av1::transfer_characteristics::TC_UNSPECIFIED;
    av1::matrix_coefficients matrix_coefficients = av1::matrix_coefficients::MC_UNSPECIFIED;
};

struct color_config {
    std::uint8_t high_bitdepth                  : 1 = 0;
    std::uint8_t twelve_bit                     : 1 = 0;
    std::uint8_t monochrome                     : 1 = 0;
    std::uint8_t color_description_present_flag : 1 = 0;

    std::uint8_t color_range         : 1 = 0;
    std::uint8_t subsampling_x       : 1 = 0;
    std::uint8_t subsampling_y       : 1 = 0;
    std::uint8_t separate_uv_delta_q : 1 = 0;

    av1::chroma_sample_position chroma_sample_position = av1::chroma_sample_position::CSP_UNKNOWN;
    av1::color_description color_description_data{};
};

struct obu_sequence_header {
    std::uint8_t seq_profile                        : 3 = 0;
    std::uint8_t still_picture                      : 1 = 0;
    std::uint8_t reduced_still_picture_header       : 1 = 0;
    std::uint8_t timing_info_present_flag           : 1 = 0;
    std::uint8_t decoder_model_info_present_flag    : 1 = 0;
    std::uint8_t initial_display_delay_present_flag : 1 = 0;

    std::uint8_t frame_id_numbers_present_flag      : 1 = 0;
    std::uint8_t delta_frame_id_length_minus_2      : 4 = 0;
    std::uint8_t additional_frame_id_length_minus_1 : 3 = 0;

    std::uint8_t use_128x128_superblock     : 1 = 0;
    std::uint8_t enable_filter_intra        : 1 = 0;
    std::uint8_t enable_intra_edge_filter   : 1 = 0;
    std::uint8_t enable_interintra_compound : 1 = 0;
    std::uint8_t enable_masked_compound     : 1 = 0;
    std::uint8_t enable_warped_motion       : 1 = 0;
    std::uint8_t enable_dual_filter         : 1 = 0;
    std::uint8_t enable_order_hint          : 1 = 0;

    std::uint8_t order_hint_bits_minus_1         : 3 = 0;
    std::uint8_t enable_jnt_comp                 : 1 = 0;
    std::uint8_t enable_ref_frame_mvs            : 1 = 0;
    std::uint8_t seq_choose_screen_content_tools : 1 = 0;
    std::uint8_t seq_force_screen_content_tools  : 2 = SELECT_SCREEN_CONTENT_TOOLS;
    std::uint8_t seq_choose_integer_mv           : 1 = 0;
    std::uint8_t seq_force_integer_mv            : 2 = SELECT_INTEGER_MV;

    std::uint8_t enable_superres           : 1 = 0;
    std::uint8_t enable_cdef               : 1 = 0;
    std::uint8_t enable_restoration        : 1 = 0;
    std::uint8_t film_grain_params_present : 1 = 0;

    std::uint8_t operating_points_cnt_minus_1 : 5 = 0;
    std::array<operating_point, 32> operating_points{}; // max 32 operating points (as per spec)

    timing_info timing_info_data{};
    decoder_model_info decoder_model_info_data{};
    color_config color_config_data{};

    uint8_t frame_width_bits_minus_1  : 4 = 0;
    uint8_t frame_height_bits_minus_1 : 4 = 0;
    uint16_t max_frame_width_minus_1 = 0;
    uint16_t max_frame_height_minus_1 = 0;

public:
    constexpr std::uint8_t order_hint_bits() const noexcept
    {
        if (reduced_still_picture_header) {
            return 0;
        }

        if (enable_order_hint) {
            return order_hint_bits_minus_1 + 1;
        }
        return 0;
    }
};

struct obu_metadata {
    av1::metadata_type type = {};
    std::span<const std::byte> payload{};
};

struct frame_indices {
    std::uint32_t ref_frame_indices;

public:
    constexpr auto ref_frame_index(std::size_t i) const noexcept -> std::uint8_t
    {
        return (ref_frame_indices >> (i * 3)) & 0x07;
    }
    constexpr auto set_ref_frame_index(std::size_t i, std::uint8_t index) noexcept -> void
    {
        ref_frame_indices &= ~(0x07 << (i * 3)); // Clear the existing index at position i
        ref_frame_indices |= (index & 0x07) << (i * 3); // Set the new index at position i
    }
};

struct ref_frame_state {
    std::uint8_t valid; // RefValid
    std::uint8_t order_hint; // RefOrderHint
    std::uint8_t bit_depth; // RefBitDepth
    av1::frame_type frame_type; // RefFrameType

    std::uint32_t upscaled_width; // RefUpscaledWidth
    std::uint64_t frame_id; // RefFrameId

    std::uint32_t frame_width; // RefFrameWidth
    std::uint32_t frame_height; // RefFrameHeight
    std::uint32_t render_width; // RefRenderWidth
    std::uint32_t render_height; // RefRenderHeight

    std::uint32_t subsampling_x; // RefSubsamplingX
    std::uint32_t subsampling_y; // RefSubsamplingY
};

struct obu_frame_header {
    std::uint8_t show_existing_frame   : 1 = 0;
    std::uint8_t frame_to_show_map_idx : 3 = 0;
    std::uint8_t show_frame            : 1 = 1;
    std::uint8_t showable_frame        : 1 = 0;
    std::uint8_t error_resilient_mode  : 1 = 1;
    std::uint8_t disable_cdf_update    : 1 = 0;

    std::uint8_t allow_screen_content_tools       : 1 = 0;
    std::uint8_t force_integer_mv                 : 1 = 0;
    std::uint8_t frame_size_override_flag         : 1 = 0;
    std::uint8_t buffer_removal_time_present_flag : 1 = 0;
    std::uint8_t allow_high_precision_mv          : 1 = 0;
    std::uint8_t primary_ref_frame                : 3 = 0;

    std::uint8_t use_ref_frame_mvs          : 1 = 0;
    std::uint8_t allow_intrabc              : 1 = 0;
    std::uint8_t frame_refs_short_signaling : 1 = 0;
    std::uint8_t last_frame_idx             : 3 = 0;
    std::uint8_t gold_frame_idx             : 3 = 0;

    std::uint8_t refresh_frame_flags = 0;
    av1::frame_type frame_type = av1::frame_type::KEY_FRAME;
    std::uint8_t order_hint = 0;

    // frame size
    std::uint16_t frame_width_minus_1 = 0;
    std::uint16_t frame_height_minus_1 = 0;

    // superres params
    std::uint8_t use_superres : 1 = 0;
    std::uint8_t coded_denom  : 3 = 0;

    // render size
    std::uint8_t render_and_frame_size_different : 1 = 0;
    std::uint16_t render_width_minus_1 = 0;
    std::uint16_t render_height_minus_1 = 0;

    av1::frame_indices ref_frame_indices_data{};

    std::uint64_t display_frame_id = 0;
    std::uint64_t current_frame_id = 0;
    std::uint64_t frame_presentation_time = 0;
    std::array<std::uint64_t, 32> buffer_removal_time{};
    std::array<std::uint8_t, NUM_REF_FRAMES> ref_order_hint{};
    std::array<std::uint64_t, REFS_PER_FRAME> expectedFrameId{};
};

//------------------------------------------------------------------------------------------------------------
struct obu_view_base {
    constexpr static std::uint64_t max_obu_size = (1ULL << 32) - 1; // 4GB - 1 byte
public:
    std::uint8_t forbidden_bit  : 1 = 0; // should be 0
    std::uint8_t type           : 4 = 0;
    std::uint8_t extension_flag : 1 = 0;
    std::uint8_t has_size_field : 1 = 0;
    std::uint8_t reserved       : 1 = 0; // should be 0

    // obu extension header fields (if extension_flag is set)
    std::uint8_t temporal_id          : 3 = 0;
    std::uint8_t spatial_id           : 2 = 0;
    std::uint8_t extension_reserved_1 : 3 = 0; // should be 0

    // followed by payload
    std::span<const std::byte> payload{};

public:
    constexpr operator bool() const noexcept
    {
        return payload.data() != nullptr;
    }
};

//------------------------------------------------------------------------------------------------------------
struct obu_context {
    // Selected by the decoder
    std::uint32_t operating_point = 0;

    // Sequence header OBU, needed for parsing frame headers.
    av1::obu_view_base sequence_obu_header{};

    // Most recently parsed sequence header, needed for parsing frame headers
    av1::obu_sequence_header sequence_header{};

    std::uint8_t seen_frame_header : 1 = false;

    std::uint64_t prev_frame_id = 0;
    std::uint64_t current_frame_id = 0;
    std::uint32_t frame_width = 0;
    std::uint32_t frame_height = 0;
    std::uint32_t render_width = 0;
    std::uint32_t render_height = 0;
    std::uint32_t upscaled_width = 0;
    std::uint32_t mi_cols = 0;
    std::uint32_t mi_rows = 0;

    std::array<std::uint8_t, NUM_REF_FRAMES> order_hints{};
    av1::ref_frame_state ref[NUM_REF_FRAMES]{};
};

//------------------------------------------------------------------------------------------------------------
using any_obu_view = struct obu_view_base;

template <obu_type Type>
struct basic_obu_view : public obu_view_base {};

template <>
struct basic_obu_view<obu_type::sequence_header> : public obu_view_base {
    constexpr auto header() const noexcept -> obu_sequence_header;
};
template <>
struct basic_obu_view<obu_type::metadata> : public obu_view_base {
    constexpr auto header() const noexcept -> obu_metadata;
};
template <>
struct basic_obu_view<obu_type::frame_header> : public obu_view_base {
    constexpr auto header(obu_context& context) const noexcept -> obu_frame_header;
};
//------------------------------------------------------------------------------------------------------------
using obu_sequence_header_view = basic_obu_view<obu_type::sequence_header>;

//------------------------------------------------------------------------------------------------------------
template <obu_type Obu>
constexpr auto obu_cast(const any_obu_view& obu) noexcept -> basic_obu_view<Obu>
{
    if (static_cast<obu_type>(obu.type) != Obu) {
        return {};
    }
    return static_cast<basic_obu_view<Obu>>(obu);
}

struct av1C_header {
    // Byte 0
    std::uint8_t marker  : 1 = 0; // should be 1
    std::uint8_t version : 7 = 0; // should be 1

    // Byte 1
    std::uint8_t seq_profile     : 3 = 0;
    std::uint8_t seq_level_idx_0 : 5 = 0;

    // Byte 2
    std::uint8_t seq_tier_0             : 1 = 0;
    std::uint8_t high_bitdepth          : 1 = 0;
    std::uint8_t twelve_bit             : 1 = 0;
    std::uint8_t monochrome             : 1 = 0;
    std::uint8_t chroma_subsampling_x   : 1 = 0;
    std::uint8_t chroma_subsampling_y   : 1 = 0;
    std::uint8_t chroma_sample_position : 2 = 0;

    // Byte 3
    std::uint8_t reserved                             : 3 = 0; // should be 0
    std::uint8_t initial_presentation_delay_present   : 1 = 0;
    std::uint8_t initial_presentation_delay_minus_one : 4 = 0;

    // followed by config OBUs
    std::span<const std::byte> config_obus{};
};

//------------------------------------------------------------------------------------------------------------
constexpr auto leb128(std::span<const std::byte> data) noexcept -> mbmff::parsed<std::uint64_t>
{
    std::uint64_t value = 0;
    std::uint32_t leb128_bytes = 0;
    for (int i = 0; i < 8; i++) {
        std::uint8_t leb128_byte = static_cast<std::uint8_t>(data[i]);

        value |= static_cast<std::uint64_t>(leb128_byte & 0x7f) << (i * 7);
        leb128_bytes += 1;
        if (!(leb128_byte & 0x80)) {
            break;
        }
    }
    return {value, leb128_bytes};
}

constexpr auto parse_obu(std::span<const std::byte> data) noexcept -> std::expected<any_obu_view, unexpected>
{
    any_obu_view header{};
    std::uint64_t offset = 0;

    // Byte 0:
    std::uint8_t byte0 = static_cast<std::uint8_t>(data[offset++]);
    header.forbidden_bit = (byte0 >> 7) & 0x01;
    header.type = (byte0 >> 3) & 0x0F;
    header.extension_flag = (byte0 >> 2) & 0x01;
    header.has_size_field = (byte0 >> 1) & 0x01;
    header.reserved = byte0 & 0x01;

    if (header.forbidden_bit) {
        return std::unexpected(unexpected{error_code::invalid_format});
    }

    // read extension header if present
    if (header.extension_flag) {
        if (data.size() < offset + 1) {
            return std::unexpected(unexpected{error_code::need_more_data, offset + 1});
        }

        std::uint8_t ext_byte = static_cast<std::uint8_t>(data[offset++]);
        header.temporal_id = (ext_byte >> 5) & 0x07;
        header.spatial_id = (ext_byte >> 3) & 0x03;
        header.extension_reserved_1 = ext_byte & 0x07;
        if (header.extension_reserved_1 || header.reserved) {
            return std::unexpected(unexpected{error_code::invalid_format});
        }
    }

    std::uint64_t obu_size = 0;
    // read OBU size if present
    if (header.has_size_field) {
        if (data.size() < offset + 1) {
            return std::unexpected(unexpected{error_code::need_more_data, offset + 1});
        }
        auto [xobu_size, leb128_bytes] = leb128(data.subspan(offset));
        if (obu_size > any_obu_view::max_obu_size) {
            return std::unexpected(unexpected{error_code::invalid_format});
        }
        obu_size = xobu_size;
        offset += leb128_bytes;
    } else {
        // If no size field is present, the OBU extends to the end of the data
        obu_size = data.size() - offset;
    }

    // place the payload span after the header and any size field
    header.payload = data.subspan(offset, obu_size);
    return std::expected<any_obu_view, unexpected>{header};
}

//------------------------------------------------------------------------------------------------------------
// Sequence Header
constexpr auto basic_obu_view<obu_type::sequence_header>::header() const noexcept -> obu_sequence_header
{
    obu_sequence_header result{};
    bit_reader reader{payload};

    // Byte 0
    std::uint8_t read_byte = static_cast<std::uint8_t>(payload[0]);
    result.seq_profile = reader.read_bits<std::uint8_t>(3);
    result.still_picture = reader.read_bits<std::uint8_t>(1);
    result.reduced_still_picture_header = reader.read_bits<std::uint8_t>(1);

    if (result.reduced_still_picture_header) {
        result.operating_points[0].seq_level_idx = reader.read_bits<std::uint8_t>(5);
    } else {
        result.timing_info_present_flag = reader.read_bits<std::uint8_t>(1);
        if (result.timing_info_present_flag) {
            // Timing info
            result.timing_info_data.num_units_in_display_tick = reader.read_bits<std::uint32_t>(32);
            result.timing_info_data.time_scale = reader.read_bits<std::uint32_t>(32);
            result.timing_info_data.equal_picture_interval = reader.read_bits<std::uint32_t>(1);
            if (result.timing_info_data.equal_picture_interval) {
                result.timing_info_data.num_ticks_per_picture_minus_1 = reader.uvlc();
            }

            // Decoder model info
            result.decoder_model_info_present_flag = reader.read_bits<std::uint8_t>(1);
            if (result.decoder_model_info_present_flag) {
                result.decoder_model_info_data.buffer_delay_length_minus_1 = reader.read_bits<std::uint8_t>(5);
                result.decoder_model_info_data.num_units_in_decoding_tick = reader.read_bits<std::uint32_t>(32);
                result.decoder_model_info_data.buffer_removal_time_length_minus_1 = reader.read_bits<std::uint8_t>(5);
                result.decoder_model_info_data.frame_presentation_time_length_minus_1 = reader.read_bits<std::uint8_t>(
                    5
                );
            }

            result.initial_display_delay_present_flag = reader.read_bits<std::uint8_t>(1);

            // Operating points
            result.operating_points_cnt_minus_1 = reader.read_bits<std::uint8_t>(5);
            for (std::size_t i = 0; i <= result.operating_points_cnt_minus_1; i++) {
                auto& op = result.operating_points[i];
                op.operating_point_idc = reader.read_bits<std::uint16_t>(12);
                op.seq_level_idx = reader.read_bits<std::uint8_t>(5);
                if (op.seq_level_idx > 7) {
                    op.seq_tier = reader.read_bits<std::uint8_t>(1);
                }

                // Decoding parameters info
                if (result.decoder_model_info_present_flag) {
                    op.decoder_model_present_for_this_op = reader.read_bits<std::uint8_t>(1);
                    if (op.decoder_model_present_for_this_op) {
                        std::size_t bits_for_buffer_delay = result.decoder_model_info_data.buffer_delay_length_minus_1
                                                          + 1;

                        op.operating_parameters.decoder_buffer_delay = reader.read_bits<std::uint32_t>(
                            bits_for_buffer_delay
                        );
                        op.operating_parameters.encoder_buffer_delay = reader.read_bits<std::uint32_t>(
                            bits_for_buffer_delay
                        );
                        op.operating_parameters.low_delay_mode_flag = reader.read_bits<std::uint32_t>(1);
                    }
                }

                // Initial display delay
                if (result.initial_display_delay_present_flag) {
                    op.initial_display_delay_present_for_this_op = reader.read_bits<std::uint8_t>(1);
                    if (op.initial_display_delay_present_for_this_op) {
                        op.initial_display_delay_minus_1 = reader.read_bits<std::uint8_t>(4);
                    }
                }
            }
        }
    }

    // here by the spec we should select
    // operatingPoint = choose_operating_point( )
    // OperatingPointIdc = operating_point_idc[operatingPoint]

    result.frame_width_bits_minus_1 = reader.read_bits<std::uint8_t>(4);
    result.frame_height_bits_minus_1 = reader.read_bits<std::uint8_t>(4);
    result.max_frame_width_minus_1 = reader.read_bits<std::uint16_t>(result.frame_width_bits_minus_1 + 1);
    result.max_frame_height_minus_1 = reader.read_bits<std::uint16_t>(result.frame_height_bits_minus_1 + 1);

    if (!result.reduced_still_picture_header) {
        result.frame_id_numbers_present_flag = reader.read_bits<std::uint8_t>(1);
    }

    if (result.frame_id_numbers_present_flag) {
        result.delta_frame_id_length_minus_2 = reader.read_bits<std::uint8_t>(4);
        result.additional_frame_id_length_minus_1 = reader.read_bits<std::uint8_t>(3);
    }

    result.use_128x128_superblock = reader.read_bits<std::uint8_t>(1);
    result.enable_filter_intra = reader.read_bits<std::uint8_t>(1);
    result.enable_intra_edge_filter = reader.read_bits<std::uint8_t>(1);

    if (!result.reduced_still_picture_header) {
        result.enable_interintra_compound = reader.read_bits<std::uint8_t>(1);
        result.enable_masked_compound = reader.read_bits<std::uint8_t>(1);
        result.enable_warped_motion = reader.read_bits<std::uint8_t>(1);
        result.enable_dual_filter = reader.read_bits<std::uint8_t>(1);
        result.enable_order_hint = reader.read_bits<std::uint8_t>(1);

        if (result.enable_order_hint) {
            result.enable_jnt_comp = reader.read_bits<std::uint8_t>(1);
            result.enable_ref_frame_mvs = reader.read_bits<std::uint8_t>(1);
        }
        result.seq_choose_screen_content_tools = reader.read_bits<std::uint8_t>(1);
        if (result.seq_choose_screen_content_tools) {
            result.seq_force_screen_content_tools = reader.read_bits<std::uint8_t>(1);
        }

        if (result.seq_force_screen_content_tools > 0) {
            result.seq_choose_integer_mv = reader.read_bits<std::uint8_t>(1);
            if (result.seq_choose_integer_mv) {
                result.seq_force_integer_mv = reader.read_bits<std::uint8_t>(1);
            }
        }

        if (result.enable_order_hint) {
            result.order_hint_bits_minus_1 = reader.read_bits<std::uint8_t>(3);
        }
    }
    result.enable_superres = reader.read_bits<std::uint8_t>(1);
    result.enable_cdef = reader.read_bits<std::uint8_t>(1);
    result.enable_restoration = reader.read_bits<std::uint8_t>(1);

    // Color config
    auto& cc = result.color_config_data;
    cc.high_bitdepth = reader.read_bits<std::uint8_t>(1);
    if (result.seq_profile == 2 && cc.high_bitdepth) {
        cc.twelve_bit = reader.read_bits<std::uint8_t>(1);
    }

    if (result.seq_profile != 1) {
        cc.monochrome = reader.read_bits<std::uint8_t>(1);
    }

    cc.color_description_present_flag = reader.read_bits<std::uint8_t>(1);
    if (cc.color_description_present_flag) {
        cc.color_description_data.color_primaries = static_cast<av1::color_primaries>(
            reader.read_bits<std::uint8_t>(8)
        );
        cc.color_description_data.transfer_characteristics = static_cast<av1::transfer_characteristics>(
            reader.read_bits<std::uint8_t>(8)
        );
        cc.color_description_data.matrix_coefficients = static_cast<av1::matrix_coefficients>(
            reader.read_bits<std::uint8_t>(8)
        );
    }

    if (!cc.monochrome) {
        cc.color_range = reader.read_bits<std::uint8_t>(1);
        cc.subsampling_x = 1;
        cc.subsampling_y = 1;
        result.film_grain_params_present = reader.read_bits<std::uint8_t>(1);
        return result;
    }
    if (cc.color_description_data.color_primaries == av1::color_primaries::CP_BT_709
        && cc.color_description_data.transfer_characteristics == av1::transfer_characteristics::TC_SRGB
        && cc.color_description_data.matrix_coefficients == av1::matrix_coefficients::MC_IDENTITY) {
        cc.color_range = 1;
        cc.subsampling_x = 0;
        cc.subsampling_y = 0;
    } else {
        cc.color_range = reader.read_bits<std::uint8_t>(1);
        if (result.seq_profile == 0) {
            cc.subsampling_x = 1;
            cc.subsampling_y = 1;
        } else if (result.seq_profile == 1) {
            cc.subsampling_x = 0;
            cc.subsampling_y = 0;
        } else {
            if (cc.twelve_bit) {
                cc.subsampling_x = reader.read_bits<std::uint8_t>(1);
                if (cc.subsampling_x) {
                    cc.subsampling_y = reader.read_bits<std::uint8_t>(1);
                }
            } else {
                cc.subsampling_x = 1;
                cc.subsampling_y = 0;
            }
        }
        if (cc.subsampling_x && cc.subsampling_y) {
            cc.chroma_sample_position = static_cast<av1::chroma_sample_position>(reader.read_bits<std::uint8_t>(2));
        }
    }
    cc.separate_uv_delta_q = reader.read_bits<std::uint8_t>(1);
    // End color config

    result.film_grain_params_present = reader.read_bits<std::uint8_t>(1);
    return result;
}

//------------------------------------------------------------------------------------------------------------
// Metadata
constexpr auto basic_obu_view<obu_type::metadata>::header() const noexcept -> obu_metadata
{
    obu_metadata result{};
    auto [value, bytes] = leb128(payload);
    result.type = static_cast<metadata_type>(value);
    result.payload = payload.subspan(bytes);
    return result;
}

//------------------------------------------------------------------------------------------------------------
// Frame header
constexpr void mark_ref_frames(obu_context& context, std::size_t id_len) noexcept
{
    auto diff_len = std::uint32_t(context.sequence_header.delta_frame_id_length_minus_2) + 2;
    for (std::uint32_t i = 0; i < NUM_REF_FRAMES; i++) {
        if (context.current_frame_id > (1ull << diff_len)) {
            if (context.ref[i].frame_id > context.current_frame_id
                || context.ref[i].frame_id < context.current_frame_id - (1ull << diff_len)) {
                context.ref[i].valid = false;
            }
        } else {
            if (context.ref[i].frame_id > context.current_frame_id
                && context.ref[i].frame_id < ((1ull << id_len) + context.current_frame_id - (1ull << diff_len))) {
                context.ref[i].valid = false;
            }
        }
    }
}

constexpr void compute_image_size(obu_context& context) noexcept
{
    context.mi_cols = 2u * ((context.frame_width + 7) >> 3);
    context.mi_rows = 2u * ((context.frame_height + 7) >> 3);
}

constexpr void superres_params(obu_frame_header& frame_header, obu_context& context, bit_reader& reader) noexcept
{
    if (context.sequence_header.enable_superres) {
        frame_header.use_superres = reader.read_bits<std::uint8_t>(1);
    }

    std::uint32_t superres_denom = SUPERRES_NUM;
    if (frame_header.use_superres) {
        frame_header.coded_denom = reader.read_bits<std::uint8_t>(3);
        superres_denom = std::uint32_t(frame_header.coded_denom) + SUPERRES_DENOM_MIN;
    }

    context.upscaled_width = context.frame_width;
    context.frame_width = (context.upscaled_width * SUPERRES_NUM + (superres_denom / 2)) / superres_denom;
}

constexpr void frame_size(obu_frame_header& frame_header, obu_context& context, bit_reader& reader) noexcept
{
    if (frame_header.frame_size_override_flag) {
        frame_header.frame_width_minus_1 = reader.read_bits<std::uint16_t>(
            std::uint16_t(context.sequence_header.frame_width_bits_minus_1) + 1
        );
        frame_header.frame_height_minus_1 = reader.read_bits<std::uint16_t>(
            std::uint16_t(context.sequence_header.frame_height_bits_minus_1) + 1
        );
    } else {
        frame_header.frame_width_minus_1 = context.sequence_header.max_frame_width_minus_1;
        frame_header.frame_height_minus_1 = context.sequence_header.max_frame_height_minus_1;
    }

    context.frame_width = std::uint32_t(frame_header.frame_width_minus_1) + 1;
    context.frame_height = std::uint32_t(frame_header.frame_height_minus_1) + 1;

    superres_params(frame_header, context, reader);
    compute_image_size(context);
}

constexpr void render_size(obu_frame_header& frame_header, obu_context& context, bit_reader& reader) noexcept
{
    frame_header.render_and_frame_size_different = reader.read_bits<std::uint8_t>(1);
    if (frame_header.render_and_frame_size_different) {
        frame_header.render_width_minus_1 = reader.read_bits<std::uint16_t>(16);
        frame_header.render_height_minus_1 = reader.read_bits<std::uint16_t>(16);

        context.render_width = std::uint32_t(frame_header.render_width_minus_1) + 1;
        context.render_height = std::uint32_t(frame_header.render_height_minus_1) + 1;
    } else {
        context.render_width = context.upscaled_width;
        context.render_height = context.frame_height;
    }
}

// Converted from cbs_syntax_template.c of ffmpeg
constexpr auto get_relative_dist(const obu_sequence_header& seq, std::uint32_t a, std::uint32_t b) noexcept
    -> std::int32_t
{
    if (!seq.enable_order_hint) {
        return 0;
    }
    std::uint32_t diff = a - b;
    std::uint32_t m = 1 << seq.order_hint_bits_minus_1;
    diff = (diff & (m - 1)) - (diff & m);
    return diff;
}

constexpr void set_frame_refs(obu_frame_header& frame_header, obu_context& context) noexcept
{
    const obu_sequence_header& seq = context.sequence_header;
    constexpr ref_frame ref_frame_list[NUM_REF_FRAMES - 2] = {
        ref_frame::LAST2_FRAME,
        ref_frame::LAST3_FRAME,
        ref_frame::BWDREF_FRAME,
        ref_frame::ALTREF2_FRAME,
        ref_frame::ALTREF_FRAME,
    };

    std::int8_t ref_frame_idx[REFS_PER_FRAME]{};
    std::int8_t used_frame[NUM_REF_FRAMES]{};

    ref_frame_idx[+ref_frame::LAST_FRAME - +ref_frame::LAST_FRAME] = frame_header.last_frame_idx;
    ref_frame_idx[+ref_frame::GOLDEN_FRAME - +ref_frame::LAST_FRAME] = frame_header.gold_frame_idx;

    used_frame[frame_header.last_frame_idx] = 1;
    used_frame[frame_header.gold_frame_idx] = 1;

    std::uint32_t cur_frame_hint = 1u << (seq.order_hint_bits_minus_1);
    std::int16_t shifted_order_hints[NUM_REF_FRAMES];
    for (std::uint32_t i = 0; i < NUM_REF_FRAMES; i++) {
        shifted_order_hints[i] = cur_frame_hint
                               + get_relative_dist(seq, context.ref[i].order_hint, frame_header.order_hint);
    }

    std::int16_t latest_order_hint = shifted_order_hints[frame_header.last_frame_idx];
    std::int16_t earliest_order_hint = shifted_order_hints[frame_header.gold_frame_idx];

    std::int8_t ref = 0;
    for (std::uint32_t i = 0; i < NUM_REF_FRAMES; i++) {
        int hint = shifted_order_hints[i];
        if (!used_frame[i] && hint >= cur_frame_hint && (ref < 0 || hint >= latest_order_hint)) {
            ref = i;
            latest_order_hint = hint;
        }
    }
    if (ref >= 0) {
        ref_frame_idx[+ref_frame::ALTREF_FRAME - +ref_frame::LAST_FRAME] = ref;
        used_frame[ref] = 1;
    }

    ref = 0;
    for (std::uint32_t i = 0; i < NUM_REF_FRAMES; i++) {
        int hint = shifted_order_hints[i];
        if (!used_frame[i] && hint >= cur_frame_hint && (ref < 0 || hint < earliest_order_hint)) {
            ref = i;
            earliest_order_hint = hint;
        }
    }
    if (ref >= 0) {
        ref_frame_idx[+ref_frame::BWDREF_FRAME - +ref_frame::LAST_FRAME] = ref;
        used_frame[ref] = 1;
    }

    ref = 0;
    for (std::uint32_t i = 0; i < NUM_REF_FRAMES; i++) {
        int hint = shifted_order_hints[i];
        if (!used_frame[i] && hint >= cur_frame_hint && (ref < 0 || hint < earliest_order_hint)) {
            ref = i;
            earliest_order_hint = hint;
        }
    }
    if (ref >= 0) {
        ref_frame_idx[+ref_frame::ALTREF2_FRAME - +ref_frame::LAST_FRAME] = ref;
        used_frame[ref] = 1;
    }

    for (std::uint32_t i = 0; i < REFS_PER_FRAME - 2; i++) {
        auto ref_frame = ref_frame_list[i];
        if (ref_frame_idx[+ref_frame - +ref_frame::LAST_FRAME] < 0) {
            ref = 0;
            for (std::uint32_t j = 0; j < NUM_REF_FRAMES; j++) {
                int hint = shifted_order_hints[j];
                if (!used_frame[j] && hint < cur_frame_hint && (ref < 0 || hint >= latest_order_hint)) {
                    ref = j;
                    latest_order_hint = hint;
                }
            }
            if (ref >= 0) {
                ref_frame_idx[+ref_frame - +ref_frame::LAST_FRAME] = ref;
                used_frame[ref] = 1;
            }
        }
    }

    ref = 0;
    for (std::uint32_t i = 0; i < NUM_REF_FRAMES; i++) {
        int hint = shifted_order_hints[i];
        if (ref < 0 || hint < earliest_order_hint) {
            ref = i;
            earliest_order_hint = hint;
        }
    }
    for (std::uint32_t i = 0; i < REFS_PER_FRAME; i++) {
        if (ref_frame_idx[i] < 0) {
            ref_frame_idx[i] = ref;
        }
        frame_header.ref_frame_indices_data.set_ref_frame_index(i, ref_frame_idx[i]);
    }
}

constexpr void frame_size_with_refs(obu_frame_header& frame_header, obu_context& context, bit_reader& reader) noexcept
{
    std::uint8_t found_ref = 0;
    for (std::uint32_t i = 0; i < REFS_PER_FRAME; i++) {
        if (found_ref = reader.read_bits<std::uint8_t>(1)) {
            auto& ref_frame = context.ref[frame_header.ref_frame_indices_data.ref_frame_index(i)];
            frame_header.frame_width_minus_1 = ref_frame.upscaled_width - 1;
            frame_header.frame_height_minus_1 = ref_frame.frame_height - 1;
            frame_header.render_width_minus_1 = ref_frame.render_width - 1;
            frame_header.render_height_minus_1 = ref_frame.render_height - 1;

            context.upscaled_width = ref_frame.upscaled_width;
            context.frame_width = ref_frame.upscaled_width;
            context.frame_height = ref_frame.frame_height;
            context.render_width = ref_frame.render_width;
            context.render_height = ref_frame.render_height;
            break;
        }
    }

    if (!found_ref) {
        frame_size(frame_header, context, reader);
        render_size(frame_header, context, reader);
    } else {
        superres_params(frame_header, context, reader);
        compute_image_size(context);
    }
}

constexpr auto basic_obu_view<obu_type::frame_header>::header(obu_context& context) const noexcept -> obu_frame_header
{
    obu_frame_header result{};
    bit_reader reader{payload};
    const auto& seq_view = context.sequence_obu_header;
    const auto& sequence_header = context.sequence_header;

    std::uint32_t id_len = 0;
    if (sequence_header.frame_id_numbers_present_flag) {
        id_len = std::uint32_t(sequence_header.delta_frame_id_length_minus_2) + 2u
               + sequence_header.additional_frame_id_length_minus_1 + 1u;
    }
    std::uint32_t all_frames = (1u << NUM_REF_FRAMES) - 1;
    bool frame_is_intra = true;

    if (!sequence_header.reduced_still_picture_header) {
        result.show_existing_frame = reader.read_bits<std::uint8_t>(1);
        if (result.show_existing_frame) {
            result.frame_to_show_map_idx = reader.read_bits<std::uint8_t>(3);

            // Temporal point info
            if (sequence_header.decoder_model_info_present_flag && sequence_header.timing_info_present_flag
                && !sequence_header.timing_info_data.equal_picture_interval) {
                result.frame_presentation_time = reader.read_bits<std::uint64_t>(
                    sequence_header.decoder_model_info_data.frame_presentation_time_length_minus_1 + 1
                );
            }

            result.refresh_frame_flags = 0;
            if (sequence_header.frame_id_numbers_present_flag) {
                result.display_frame_id = reader.read_bits<std::uint64_t>(id_len);
            }

            result.frame_type = context.ref[result.frame_to_show_map_idx].frame_type;
            if (result.frame_type == frame_type::KEY_FRAME) {
                result.refresh_frame_flags = all_frames; // refresh all reference frames
            }

            if (sequence_header.film_grain_params_present) {
                // load_grain_params( frame_to_show_map_idx )
            }

            return result;
        }

        result.frame_type = static_cast<frame_type>(reader.read_bits<std::uint8_t>(2));
        bool frame_is_intra = (result.frame_type == frame_type::KEY_FRAME)
                              || (result.frame_type == frame_type::INTRA_ONLY_FRAME);

        result.show_frame = reader.read_bits<std::uint8_t>(1);

        // Temporal point info
        if (result.show_frame && sequence_header.decoder_model_info_present_flag
            && sequence_header.timing_info_present_flag && !sequence_header.timing_info_data.equal_picture_interval) {
            result.frame_presentation_time = reader.read_bits<std::uint64_t>(
                sequence_header.decoder_model_info_data.frame_presentation_time_length_minus_1 + 1
            );
        }

        if (result.show_frame) {
            result.showable_frame = result.frame_type != frame_type::KEY_FRAME;
        } else {
            result.showable_frame = reader.read_bits<std::uint8_t>(1);
        }

        if (result.frame_type == frame_type::SWITCH_FRAME
            || (result.frame_type == frame_type::KEY_FRAME && result.show_frame)) {
            result.error_resilient_mode = 1;
        } else {
            result.error_resilient_mode = reader.read_bits<std::uint8_t>(1);
        }
    }

    if (result.frame_type == frame_type::KEY_FRAME && result.show_frame) {
        for (std::uint32_t i = 0; i < NUM_REF_FRAMES; i++) {
            context.ref[i].valid = false;
            context.ref[i].order_hint = 0;
        }
        for (std::uint32_t i = 0; i < REFS_PER_FRAME; i++) {
            context.order_hints[+ref_frame::LAST_FRAME + i] = 0;
        }
    }

    result.disable_cdf_update = reader.read_bits<std::uint8_t>(1);
    if (sequence_header.seq_force_screen_content_tools == SELECT_SCREEN_CONTENT_TOOLS) {
        result.allow_screen_content_tools = reader.read_bits<std::uint8_t>(1);
    } else {
        result.allow_screen_content_tools = sequence_header.seq_force_screen_content_tools;
    }

    if (result.allow_screen_content_tools) {
        if (sequence_header.seq_force_integer_mv == SELECT_INTEGER_MV) {
            result.force_integer_mv = reader.read_bits<std::uint8_t>(1);
        } else {
            result.force_integer_mv = sequence_header.seq_force_integer_mv;
        }
    }

    if (frame_is_intra) {
        result.force_integer_mv = 1; // force_integer_mv must be 1 for intra frames
    }

    if (sequence_header.frame_id_numbers_present_flag) {
        context.prev_frame_id = context.current_frame_id;
        context.current_frame_id = result.current_frame_id = reader.read_bits<std::uint64_t>(id_len);
        mark_ref_frames(context, id_len);
    }

    if (result.frame_type == frame_type::SWITCH_FRAME) {
        result.frame_size_override_flag = reader.read_bits<std::uint8_t>(1);
    } else if (sequence_header.reduced_still_picture_header) {
        result.frame_size_override_flag = 0;
    } else {
        result.frame_size_override_flag = reader.read_bits<std::uint8_t>(1);
    }

    std::uint8_t order_hint_bits = sequence_header.order_hint_bits();
    if (order_hint_bits > 0) {
        result.order_hint = reader.read_bits<std::uint8_t>(order_hint_bits);
    }

    if (frame_is_intra || result.error_resilient_mode) {
        result.primary_ref_frame = PRIMARY_REF_NONE;
    } else {
        result.primary_ref_frame = reader.read_bits<std::uint8_t>(3);
    }

    // Buffer removal time
    if (sequence_header.decoder_model_info_present_flag) {
        if (result.buffer_removal_time_present_flag = reader.read_bits<std::uint8_t>(1)) {
            for (std::size_t i = 0; i <= sequence_header.operating_points_cnt_minus_1; i++) {
                if (sequence_header.operating_points[i].decoder_model_present_for_this_op) {
                    auto op_idc = sequence_header.operating_points[i].operating_point_idc;
                    auto in_temporal_layer = (op_idc >> seq_view.temporal_id) & 1;
                    auto in_spatial_layer = (op_idc >> (8 + seq_view.spatial_id)) & 1;

                    if (op_idc == 0 || (in_temporal_layer && in_spatial_layer)) {
                        result.buffer_removal_time[i] = reader.read_bits<std::uint64_t>(
                            sequence_header.decoder_model_info_data.buffer_removal_time_length_minus_1 + 1
                        );
                    }
                }
            }
        }
    }

    if (result.frame_type == frame_type::SWITCH_FRAME
        || (result.frame_type == frame_type::KEY_FRAME && result.show_frame)) {
        result.refresh_frame_flags = all_frames; // refresh all reference frames
    } else {
        result.refresh_frame_flags = reader.read_bits<std::uint8_t>(8);
    }

    if (!frame_is_intra || result.refresh_frame_flags != all_frames) {
        if (result.error_resilient_mode && sequence_header.enable_order_hint) {
            for (std::size_t i = 0; i < NUM_REF_FRAMES; i++) {
                result.ref_order_hint[i] = reader.read_bits<std::uint8_t>(order_hint_bits);
                if (result.ref_order_hint[i] != context.ref[i].order_hint) {
                    context.ref[i].valid = false;
                }
            }
        }
    }

    if (frame_is_intra) {
        frame_size(result, context, reader);
        render_size(result, context, reader);

        if (result.allow_screen_content_tools && context.upscaled_width == context.frame_width) {
            result.allow_intrabc = reader.read_bits<std::uint8_t>(1);
        }
    } else {
        if (sequence_header.enable_order_hint) {
            if (result.frame_refs_short_signaling = reader.read_bits<std::uint8_t>(1)) {
                result.last_frame_idx = reader.read_bits<std::uint8_t>(3);
                result.gold_frame_idx = reader.read_bits<std::uint8_t>(3);
                set_frame_refs(result, context);
            }
        }

        for (std::size_t i = 0; i < REFS_PER_FRAME; i++) {
            if (!result.frame_refs_short_signaling) {
                result.ref_frame_indices_data.set_ref_frame_index(i, reader.read_bits<std::uint8_t>(3));
            }
            if (sequence_header.frame_id_numbers_present_flag) {
                std::uint32_t delta_frame_id = reader.read_bits<std::uint32_t>(
                                                   std::uint32_t(sequence_header.delta_frame_id_length_minus_2) + 2
                                               )
                                             + 1;
                result
                    .expectedFrameId[i] = ((result.current_frame_id + (1ull << id_len) - delta_frame_id) % (1ull << id_len));
            }
        }

        if (result.frame_size_override_flag && !result.error_resilient_mode) {
            frame_size_with_refs(result, context, reader);
        } else {
            frame_size(result, context, reader);
            render_size(result, context, reader);
        }

        if (!result.force_integer_mv) {
            result.allow_high_precision_mv = reader.read_bits<std::uint8_t>(1);
        }

        // read interpolation filter
    }
}

//------------------------------------------------------------------------------------------------------------
// iterator

struct obu_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::expected<any_obu_view, unexpected>;
    using difference_type = std::ptrdiff_t;

    std::span<const std::byte> data{};

public:
    constexpr obu_iterator() noexcept = default;
    constexpr explicit obu_iterator(std::span<const std::byte> data) noexcept
        : data(data)
    {}
    constexpr explicit obu_iterator(const av1C_header& header) noexcept
        : obu_iterator(header.config_obus)
    {}

public:
    constexpr auto begin() const noexcept -> obu_iterator
    {
        return *this;
    }
    constexpr auto end() const noexcept -> obu_iterator
    {
        return {};
    }
    constexpr auto try_get() const noexcept -> std::expected<any_obu_view, unexpected>
    {
        return parse_obu(data);
    }
    constexpr auto operator*() const noexcept -> std::expected<any_obu_view, unexpected>
    {
        return try_get();
    }
    constexpr auto operator++() noexcept -> obu_iterator&
    {
        if (data.empty()) {
            data = {};
            return *this;
        }
        auto obu = try_get();
        if (!obu) {
            // If parsing fails mid-stream, terminate the iteration
            data = {};
            return *this;
        }

        // Calculate total bytes consumed by this OBU (Header + LEB128 size + Payload)
        // By taking the end of the payload and subtracting the start of our data block,
        // we get the exact number of bytes this specific OBU occupied.
        std::size_t bytes_consumed = (obu->payload.data() + obu->payload.size()) - data.data();

        if (bytes_consumed >= data.size()) {
            data = {}; // We reached the end
        } else {
            // Shrink the span from the front, moving to the next OBU
            data = data.subspan(bytes_consumed);
        }

        return *this;
    }
    constexpr auto operator++(int) noexcept -> obu_iterator
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }
    constexpr bool operator==(const obu_iterator& other) const noexcept
    {
        // special case: both iterators are at the end (empty)
        if (data.empty() && other.data.empty()) {
            return true;
        }
        return data.data() == other.data.data() && data.size() == other.data.size();
    }
};

} // namespace mbmff::av1
