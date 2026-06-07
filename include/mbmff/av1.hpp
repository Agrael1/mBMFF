#pragma once
#include <bitset>
#include <expected>
#include <format>
#include <span>
#include "av1/av1_funcs.hpp"

namespace mbmff::av1 {
//------------------------------------------------------------------------------------------------------------
using any_obu_view = struct obu_view_base;

template <av1::obu_type Type>
struct basic_obu_view : public obu_view_base {};

template <>
struct basic_obu_view<av1::obu_type::sequence_header> : public av1::obu_view_base {
    constexpr auto header() const noexcept -> av1::obu_sequence_header;
};
template <>
struct basic_obu_view<av1::obu_type::metadata> : public av1::obu_view_base {
    constexpr auto header() const noexcept -> av1::obu_metadata;
};
template <>
struct basic_obu_view<av1::obu_type::frame_header> : public av1::obu_view_base {
    constexpr auto header(obu_context& context) const noexcept -> av1::obu_frame_header;
};
//------------------------------------------------------------------------------------------------------------
using obu_sequence_header_view = av1::basic_obu_view<av1::obu_type::sequence_header>;

//------------------------------------------------------------------------------------------------------------
template <av1::obu_type Obu>
constexpr auto obu_cast(const av1::any_obu_view& obu) noexcept -> av1::basic_obu_view<Obu>
{
    if (static_cast<av1::obu_type>(obu.type) != Obu) {
        return {};
    }
    return static_cast<av1::basic_obu_view<Obu>>(obu);
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
                result.expectedFrameId
                    [i] = ((result.current_frame_id + (1ull << id_len) - delta_frame_id) % (1ull << id_len));
            }
        }

        if (result.frame_size_override_flag && !result.error_resilient_mode) {
            frame_size_with_refs(result, context, reader);
        } else {
            frame_size(result, context, reader);
            render_size(result, context, reader);
        }

        if (!result.force_integer_mv) {
            result.allow_high_precision_mv = reader.read_flag();
        }

        read_interpolation_filter(result, context, reader);
        result.is_motion_mode_switchable = reader.read_flag();

        if (!result.error_resilient_mode && sequence_header.enable_ref_frame_mvs) {
            result.use_ref_frame_mvs = reader.read_flag();
        }

        for (std::uint32_t i = 0; i < REFS_PER_FRAME; i++) {
            std::uint32_t ref_frame = +ref_frame::LAST_FRAME + i;
            std::uint32_t hint = context.ref[result.ref_frame_indices_data.ref_frame_index(i)].order_hint;
            context.order_hints[ref_frame] = hint;

            if (!sequence_header.enable_order_hint) {
                context.ref_frame_sign_bias[ref_frame] = false;
            } else {
                context.ref_frame_sign_bias[ref_frame] = get_relative_dist(sequence_header, hint, result.order_hint)
                                                       > 0;
            }
        }
    }

    if (!sequence_header.reduced_still_picture_header && !result.disable_cdf_update) {
        result.disable_frame_end_update_cdf = reader.read_flag();
    }

    // We don't care
    if (result.primary_ref_frame == PRIMARY_REF_NONE) {
        // init_non_coeff_cdfs
        // setup_past_independence
    } else {
        // load_cdfs(ref_frame_idx[primary_ref_frame])
        // load_previous( )
    }

    // Also don't care
    if (result.use_ref_frame_mvs) {
        // motion_field_estimation()
    }

    tile_info(result, context, reader);
    quantization_params(result, context, reader);
    segmentation_params(result, context, reader);
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
