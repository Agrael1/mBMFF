#pragma once
#include <expected>
#include <format>
#include <span>
#include "common.hpp"

namespace mbmff {
enum class obu_type : uint8_t {
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

enum class av1_color_primaries : uint8_t {
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

constexpr auto to_string(av1_color_primaries primaries) noexcept -> std::string_view
{
    switch (primaries) {
    case mbmff::av1_color_primaries::CP_RESERVED_0:
        return "Reserved (0)";
    case mbmff::av1_color_primaries::CP_BT_709:
        return "BT.709";
    case mbmff::av1_color_primaries::CP_UNSPECIFIED:
        return "Unspecified";
    case mbmff::av1_color_primaries::CP_RESERVED_3:
        return "Reserved (3)";
    case mbmff::av1_color_primaries::CP_BT_470_M:
        return "BT.470 System M (historical)";
    case mbmff::av1_color_primaries::CP_BT_470_B_G:
        return "BT.470 System B/G (historical)";
    case mbmff::av1_color_primaries::CP_BT_601:
        return "BT.601";
    case mbmff::av1_color_primaries::CP_SMPTE_240:
        return "SMPTE 240M";
    case mbmff::av1_color_primaries::CP_GENERIC_FILM:
        return "Generic film (color filters using illuminant C)";
    case mbmff::av1_color_primaries::CP_BT_2020:
        return "BT.2020, BT.2100";
    case mbmff::av1_color_primaries::CP_XYZ:
        return "SMPTE 428 (CIE 1921 XYZ)";
    case mbmff::av1_color_primaries::CP_SMPTE_431:
        return "SMPTE RP 431-2";
    case mbmff::av1_color_primaries::CP_SMPTE_432:
        return "SMPTE EG 432-1";
    case mbmff::av1_color_primaries::CP_EBU_3213:
        return "EBU Tech 3213-E";
    default:
        return "Unknown color primaries";
    }
}

enum class av1_transfer_characteristics : uint8_t {
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

constexpr auto to_string(av1_transfer_characteristics tc) noexcept -> std::string_view
{
    switch (tc) {
    case mbmff::av1_transfer_characteristics::TC_RESERVED_0:
        return "Reserved (0)";
    case mbmff::av1_transfer_characteristics::TC_BT_709:
        return "BT.709";
    case mbmff::av1_transfer_characteristics::TC_UNSPECIFIED:
        return "Unspecified";
    case mbmff::av1_transfer_characteristics::TC_RESERVED_3:
        return "Reserved (3)";
    case mbmff::av1_transfer_characteristics::TC_BT_470_M:
        return "BT.470 System M (historical)";
    case mbmff::av1_transfer_characteristics::TC_BT_470_B_G:
        return "BT.470 System B/G (historical)";
    case mbmff::av1_transfer_characteristics::TC_BT_601:
        return "BT.601";
    case mbmff::av1_transfer_characteristics::TC_SMPTE_240:
        return "SMPTE 240M";
    case mbmff::av1_transfer_characteristics::TC_LINEAR:
        return "Linear";
    case mbmff::av1_transfer_characteristics::TC_LOG_100:
        return "Logarithmic (100:1 range)";
    case mbmff::av1_transfer_characteristics::TC_LOG_100_SQRT10:
        return "Logarithmic (100 * sqrt(10):1 range)";
    case mbmff::av1_transfer_characteristics::TC_IEC_61966:
        return "IEC 61966-2-4";
    case mbmff::av1_transfer_characteristics::TC_BT_1361:
        return "BT.1361";
    case mbmff::av1_transfer_characteristics::TC_SRGB:
        return "sRGB or sYCC";
    case mbmff::av1_transfer_characteristics::TC_BT_2020_10_BIT:
        return "BT.2020 10-bit system";
    case mbmff::av1_transfer_characteristics::TC_BT_2020_12_BIT:
        return "BT.2020 12-bit system";
    case mbmff::av1_transfer_characteristics::TC_SMPTE_2084:
        return "SMPTE ST 2084, ITU BT.2100 PQ";
    case mbmff::av1_transfer_characteristics::TC_SMPTE_428:
        return "SMPTE ST 428";
    case mbmff::av1_transfer_characteristics::TC_HLG:
        return "BT.2100 HLG, ARIB STD-B67";
    default:
        return "Unknown transfer characteristics";
    }
}

enum class av1_matrix_coefficients : uint8_t {
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

constexpr auto to_string(av1_matrix_coefficients mc) noexcept -> std::string_view
{
    switch (mc) {
    case mbmff::av1_matrix_coefficients::MC_IDENTITY:
        return "Identity";
    case mbmff::av1_matrix_coefficients::MC_BT_709:
        return "BT.709";
    case mbmff::av1_matrix_coefficients::MC_UNSPECIFIED:
        return "Unspecified";
    case mbmff::av1_matrix_coefficients::MC_RESERVED_3:
        return "Reserved (3)";
    case mbmff::av1_matrix_coefficients::MC_FCC:
        return "US FCC 73.628";
    case mbmff::av1_matrix_coefficients::MC_BT_470_B_G:
        return "BT.470 System B/G (historical)";
    case mbmff::av1_matrix_coefficients::MC_BT_601:
        return "BT.601";
    case mbmff::av1_matrix_coefficients::MC_SMPTE_240:
        return "SMPTE 240M";
    case mbmff::av1_matrix_coefficients::MC_SMPTE_YCGCO:
        return "YCgCo";
    case mbmff::av1_matrix_coefficients::MC_BT_2020_NCL:
        return "BT.2020 non-constant luminance system";
    case mbmff::av1_matrix_coefficients::MC_BT_2020_CL:
        return "BT.2020 constant luminance system";
    case mbmff::av1_matrix_coefficients::MC_SMPTE_2085:
        return "SMPTE ST 2085 YDzDx";
    case mbmff::av1_matrix_coefficients::MC_CHROMAT_NCL:
        return "Chromaticity-derived non-constant luminance system";
    case mbmff::av1_matrix_coefficients::MC_CHROMAT_CL:
        return "Chromaticity-derived constant luminance system";
    case mbmff::av1_matrix_coefficients::MC_ICTCP:
        return "ICTCP (BT.2100)";
    default:
        return "Unknown matrix coefficients";
    }
}

enum class av1_chroma_sample_position : uint8_t {
    CSP_UNKNOWN = 0,
    CSP_VERTICAL = 1,
    CSP_COLOCATED = 2,
    CSP_RESERVED = 3,
};

constexpr auto to_string(av1_chroma_sample_position csp) noexcept -> std::string_view
{
    switch (csp) {
    case mbmff::av1_chroma_sample_position::CSP_UNKNOWN:
        return "Unknown";
    case mbmff::av1_chroma_sample_position::CSP_VERTICAL:
        return "Vertical";
    case mbmff::av1_chroma_sample_position::CSP_COLOCATED:
        return "Colocated";
    case mbmff::av1_chroma_sample_position::CSP_RESERVED:
        return "Reserved";
    default:
        return "Invalid chroma sample position";
    }
}

// Value that indicates the allow_screen_content_tools syntax element is coded
static constexpr std::uint8_t SELECT_SCREEN_CONTENT_TOOLS = 2;

// Value that indicates the force_integer_mv syntax element is coded
static constexpr std::uint8_t SELECT_INTEGER_MV = 2;

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
    mbmff::av1_color_primaries color_primaries = mbmff::av1_color_primaries::CP_UNSPECIFIED;
    mbmff::av1_transfer_characteristics transfer_characteristics = mbmff::av1_transfer_characteristics::TC_UNSPECIFIED;
    mbmff::av1_matrix_coefficients matrix_coefficients = mbmff::av1_matrix_coefficients::MC_UNSPECIFIED;
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

    mbmff::av1_chroma_sample_position chroma_sample_position = mbmff::av1_chroma_sample_position::CSP_UNKNOWN;
    mbmff::color_description color_description_data{};
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

    std::uint8_t order_hint_bits_minus_1_present : 1 = 0;
    std::uint8_t enable_superres                 : 1 = 0;
    std::uint8_t enable_cdef                     : 1 = 0;
    std::uint8_t enable_restoration              : 1 = 0;
    std::uint8_t film_grain_params_present       : 1 = 0;

    std::uint8_t operating_points_cnt_minus_1 : 5 = 0;
    std::array<operating_point, 32> operating_points{}; // max 32 operating points (as per spec)

    timing_info timing_info_data{};
    decoder_model_info decoder_model_info_data{};
    color_config color_config_data{};

    uint8_t frame_width_bits_minus_1  : 4 = 0;
    uint8_t frame_height_bits_minus_1 : 4 = 0;
    uint16_t max_frame_width_minus_1 = 0;
    uint16_t max_frame_height_minus_1 = 0;
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

using any_obu_view = struct obu_view_base;

template <obu_type Type>
struct basic_obu_view : public obu_view_base {};

template <>
struct basic_obu_view<obu_type::sequence_header> : public obu_view_base {
    constexpr auto header() const noexcept -> obu_sequence_header;
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
        result.operating_points[0].operating_point_idc = reader.read_bits<std::uint8_t>(5);
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
        cc.color_description_data.color_primaries = static_cast<mbmff::av1_color_primaries>(
            reader.read_bits<std::uint8_t>(8)
        );
        cc.color_description_data.transfer_characteristics = static_cast<mbmff::av1_transfer_characteristics>(
            reader.read_bits<std::uint8_t>(8)
        );
        cc.color_description_data.matrix_coefficients = static_cast<mbmff::av1_matrix_coefficients>(
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
    if (cc.color_description_data.color_primaries == mbmff::av1_color_primaries::CP_BT_709
        && cc.color_description_data.transfer_characteristics == mbmff::av1_transfer_characteristics::TC_SRGB
        && cc.color_description_data.matrix_coefficients == mbmff::av1_matrix_coefficients::MC_IDENTITY) {
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
            cc.chroma_sample_position = static_cast<mbmff::av1_chroma_sample_position>(
                reader.read_bits<std::uint8_t>(2)
            );
        }
    }
    cc.separate_uv_delta_q = reader.read_bits<std::uint8_t>(1);
    // End color config

    result.film_grain_params_present = reader.read_bits<std::uint8_t>(1);
    return result;
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
        : data(header.config_obus)
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

} // namespace mbmff
