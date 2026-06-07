#pragma once
#include "av1_enums.hpp"

namespace mbmff::av1 {
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
    constexpr auto order_hint_bits() const noexcept -> std::uint8_t
    {
        if (reduced_still_picture_header) {
            return 0;
        }

        if (enable_order_hint) {
            return order_hint_bits_minus_1 + 1;
        }
        return 0;
    }

    constexpr auto num_planes() const noexcept -> std::uint8_t
    {
        return color_config_data.monochrome ? 1 : 3;
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

    std::uint8_t use_ref_frame_mvs                 : 1 = 0;
    std::uint8_t allow_intrabc                     : 1 = 0;
    std::uint8_t frame_refs_short_signaling        : 1 = 0;
    std::uint8_t last_frame_idx                    : 3 = 0;
    std::uint8_t gold_frame_idx                    : 3 = 0;
    av1::interpolation_filter interpolation_filter : 2 = av1::interpolation_filter::SWITCHABLE;
    std::uint8_t is_motion_mode_switchable         : 1 = 0;
    std::uint8_t disable_frame_end_update_cdf      : 1 = 1;
    std::uint8_t uniform_tile_spacing_flag         : 1 = 0;
    std::uint8_t tile_size_bytes_minus_1           : 2 = 0;
    std::uint8_t using_qmatrix                     : 1 = 0;
    std::uint8_t diff_uv_delta                     : 1 = 0;

    std::uint8_t refresh_frame_flags = 0;
    av1::frame_type frame_type = av1::frame_type::KEY_FRAME;
    std::uint8_t order_hint = 0;

    // tile info
    std::uint8_t tile_cols_log2 = 0;
    std::uint8_t tile_rows_log2 = 0;
    std::uint8_t mi_col_starts[MAX_TILE_COLS]{};
    std::uint8_t mi_row_starts[MAX_TILE_ROWS]{};
    std::uint8_t width_in_sbs_minus_1[MAX_TILE_COLS]{};
    std::uint8_t height_in_sbs_minus_1[MAX_TILE_ROWS]{};

    std::uint16_t tile_cols = 0;
    std::uint16_t tile_rows = 0;
    std::uint16_t context_update_tile_id = 0;

    // quantization params
    std::uint8_t base_q_idx = 0;
    std::int8_t delta_q_y_dc = 0;
    std::int8_t delta_q_u_dc = 0;
    std::int8_t delta_q_u_ac = 0;
    std::int8_t delta_q_v_dc = 0;
    std::int8_t delta_q_v_ac = 0;
    std::uint8_t qm_y : 4 = 0;
    std::uint8_t qm_u : 4 = 0;
    std::uint8_t qm_v : 4 = 0;

    // segmrentation params
    std::uint8_t segmentation_enabled         : 1 = 0;
    std::uint8_t segmentation_update_map      : 1 = 0;
    std::uint8_t segmentation_temporal_update : 1 = 0;
    std::uint8_t segmentation_update_data     : 1 = 0;
    std::bitset<MAX_SEGMENTS> feature_enabled[SEG_LVL_MAX];
    std::int16_t feature_value[MAX_SEGMENTS][SEG_LVL_MAX];

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
    std::bitset<REFS_PER_FRAME> ref_frame_sign_bias{};
    av1::ref_frame_state ref[NUM_REF_FRAMES]{};
};
} // namespace mbmff::av1
