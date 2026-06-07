#pragma once
#include "../common.hpp"
#include "av1_structs.hpp"

namespace mbmff::av1 {
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

static std::uint32_t tile_log2(std::uint32_t blksize, std::uint32_t target)
{
    std::uint32_t k = 0;
    for (; (blksize << k) < target; k++)
        ;
    return k;
}

constexpr void mark_ref_frames(av1::obu_context& context, std::size_t id_len) noexcept
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

constexpr void compute_image_size(av1::obu_context& context) noexcept
{
    context.mi_cols = 2u * ((context.frame_width + 7) >> 3);
    context.mi_rows = 2u * ((context.frame_height + 7) >> 3);
}

constexpr void superres_params(
    av1::obu_frame_header& frame_header,
    av1::obu_context& context,
    mbmff::bit_reader& reader
) noexcept
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

constexpr void frame_size(
    av1::obu_frame_header& frame_header,
    av1::obu_context& context,
    mbmff::bit_reader& reader
) noexcept
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

constexpr void render_size(
    av1::obu_frame_header& frame_header,
    av1::obu_context& context,
    mbmff::bit_reader& reader
) noexcept
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
constexpr auto get_relative_dist(const av1::obu_sequence_header& seq, std::uint32_t a, std::uint32_t b) noexcept
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

constexpr void set_frame_refs(av1::obu_frame_header& frame_header, av1::obu_context& context) noexcept
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

constexpr void frame_size_with_refs(
    av1::obu_frame_header& frame_header,
    av1::obu_context& context,
    mbmff::bit_reader& reader
) noexcept
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

constexpr void read_interpolation_filter(
    av1::obu_frame_header& frame_header,
    av1::obu_context& context,
    mbmff::bit_reader& reader
) noexcept
{
    std::uint8_t is_filter_switchable = reader.read_flag();
    if (!is_filter_switchable) {
        frame_header.interpolation_filter = static_cast<av1::interpolation_filter>(reader.read_bits<std::uint8_t>(2));
    }
}

constexpr void tile_info(
    av1::obu_frame_header& frame_header,
    av1::obu_context& context,
    mbmff::bit_reader& reader
) noexcept
{
    const auto& seq = context.sequence_header;
    std::uint32_t sb_cols = seq.use_128x128_superblock ? ((context.mi_cols + 31) >> 5) : ((context.mi_cols + 15) >> 4);
    std::uint32_t sb_rows = seq.use_128x128_superblock ? ((context.mi_rows + 31) >> 5) : ((context.mi_rows + 15) >> 4);
    std::uint32_t sb_shift = seq.use_128x128_superblock ? 5 : 4;

    std::uint32_t sb_size = sb_shift + 2;
    std::uint32_t max_tile_width_sb = MAX_TILE_WIDTH >> sb_size;
    std::uint32_t max_tile_area_sb = MAX_TILE_AREA >> (2 * sb_size);

    std::uint32_t min_log2_tile_cols = tile_log2(max_tile_width_sb, sb_cols);
    std::uint32_t max_log2_tile_cols = tile_log2(1, std::min(sb_cols, MAX_TILE_COLS));

    std::uint32_t max_log2_tile_rows = tile_log2(1, std::min(sb_rows, MAX_TILE_ROWS));
    std::uint32_t min_log2_tiles = std::max(min_log2_tile_cols, tile_log2(max_tile_area_sb, sb_rows * sb_cols));

    if (frame_header.uniform_tile_spacing_flag = reader.read_flag()) {
        frame_header.tile_cols_log2 = min_log2_tile_cols;
        while (frame_header.tile_cols_log2 < max_log2_tile_cols) {
            if (bool increment_tile_cols_log2 = reader.read_flag()) {
                frame_header.tile_cols_log2++;
            } else {
                break;
            }
        }

        std::uint32_t tile_width_sb = (sb_cols + (1u << frame_header.tile_cols_log2) - 1)
                                   >> frame_header.tile_cols_log2;
        std::uint32_t i = 0;
        for (std::uint32_t start_sb = 0; start_sb < sb_cols; start_sb += tile_width_sb) {
            frame_header.mi_col_starts[i++] = start_sb << sb_shift;
        }
        frame_header.mi_col_starts[i] = context.mi_cols;
        frame_header.tile_cols = i;

        std::uint32_t min_log2_tile_rows = std::max(min_log2_tiles - frame_header.tile_cols_log2, 0u);
        frame_header.tile_rows_log2 = min_log2_tile_rows;
        while (frame_header.tile_rows_log2 < max_log2_tile_rows) {
            if (bool increment_tile_cols_log2 = reader.read_flag()) {
                frame_header.tile_rows_log2++;
            } else {
                break;
            }
        }

        std::uint32_t tile_height_sb = (sb_rows + (1 << frame_header.tile_rows_log2) - 1)
                                    >> frame_header.tile_rows_log2;
        i = 0;
        for (std::uint32_t start_sb = 0; start_sb < sb_rows; start_sb += tile_height_sb) {
            frame_header.mi_row_starts[i++] = start_sb << sb_shift;
        }

        frame_header.mi_row_starts[i] = context.mi_rows;
        frame_header.tile_rows = i;
    } else {
        std::uint32_t widest_tile_sb = 0;
        std::uint32_t start_sb = 0;
        std::uint32_t i = 0;
        for (; start_sb < sb_cols; i++) {
            frame_header.mi_col_starts[i] = start_sb << sb_shift;
            std::uint32_t max_width = std::min(sb_cols - start_sb, max_tile_width_sb);

            frame_header.width_in_sbs_minus_1[i] = reader.ns(max_width);
            std::uint32_t size_sb = frame_header.width_in_sbs_minus_1[i] + 1;

            std::uint32_t widest_tile_sb = std::max(size_sb, widest_tile_sb);
            start_sb += size_sb;
        }

        frame_header.mi_col_starts[i] = context.mi_cols;
        frame_header.tile_cols = i;
        frame_header.tile_cols_log2 = tile_log2(1, frame_header.tile_cols);

        if (min_log2_tiles > 0) {
            max_tile_area_sb = (sb_rows * sb_cols) >> (min_log2_tiles + 1);
        } else {
            max_tile_area_sb = sb_rows * sb_cols;
        }
        std::uint32_t max_tile_height_sb = std::max(max_tile_area_sb / widest_tile_sb, 1u);


        start_sb = 0;
        i = 0;
        for (; start_sb < sb_rows; i++) {
            frame_header.mi_row_starts[i] = start_sb << sb_shift;
            std::uint32_t max_height = std::min(sb_rows - start_sb, max_tile_height_sb);
            frame_header.height_in_sbs_minus_1[i] = reader.ns(max_height);
            std::uint32_t size_sb = frame_header.height_in_sbs_minus_1[i] + 1;
            start_sb += size_sb;
        }

        frame_header.mi_row_starts[i] = context.mi_rows;
        frame_header.tile_rows = i;
        frame_header.tile_rows_log2 = tile_log2(1, frame_header.tile_rows);
    }

    if (frame_header.tile_cols_log2 > 0 || frame_header.tile_rows_log2 > 0) {
        frame_header.context_update_tile_id = reader.read_bits<std::uint16_t>(
            frame_header.tile_cols_log2 + frame_header.tile_rows_log2
        );
        frame_header.tile_size_bytes_minus_1 = reader.read_bits<std::uint8_t>(2);
    }
}

constexpr auto read_delta_q(mbmff::bit_reader& reader) -> std::int8_t
{
    std::int8_t delta_q = 0;
    if (bool delta_coded = reader.read_flag()) {
        delta_q = reader.su(7);
    }
    return delta_q;
}


constexpr void quantization_params(
    av1::obu_frame_header& frame_header,
    av1::obu_context& context,
    mbmff::bit_reader& reader
) noexcept
{
    const auto& seq = context.sequence_header;
    frame_header.base_q_idx = reader.read_bits<std::uint8_t>(8);
    frame_header.delta_q_y_dc = read_delta_q(reader);
    if (seq.num_planes() > 1) {
        if (seq.color_config_data.separate_uv_delta_q) {
            frame_header.diff_uv_delta = reader.read_flag();
        }

        frame_header.delta_q_u_dc = read_delta_q(reader);
        frame_header.delta_q_u_ac = read_delta_q(reader);
        if (frame_header.diff_uv_delta) {
            frame_header.delta_q_v_dc = read_delta_q(reader);
            frame_header.delta_q_v_ac = read_delta_q(reader);
        } else {
            frame_header.delta_q_v_dc = frame_header.delta_q_u_dc;
            frame_header.delta_q_v_ac = frame_header.delta_q_u_ac;
        }
    } 

    if (frame_header.using_qmatrix = reader.read_flag()) {
        frame_header.qm_y = reader.read_bits<std::uint8_t>(4);
        frame_header.qm_u = reader.read_bits<std::uint8_t>(4);
        if (!seq.color_config_data.separate_uv_delta_q) {
            frame_header.qm_v = frame_header.qm_u;
        } else {
            frame_header.qm_v = reader.read_bits<std::uint8_t>(4);
        }
    }
}

constexpr void segmentation_params(
    av1::obu_frame_header& frame_header,
    av1::obu_context& context,
    mbmff::bit_reader& reader
) noexcept
{
    frame_header.segmentation_enabled = reader.read_flag();
    if (frame_header.segmentation_enabled) {

    }
}


} // namespace mbmff::av1
