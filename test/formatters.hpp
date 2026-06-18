#pragma once
#include <mbmff/mbmff.hpp>
#include <format>
#include <string>

//------------------------------------------------------------------------------------------------------------
// fiel
template <>
struct std::formatter<mbmff::fiel_box> : std::formatter<std::string_view> {
    auto format(const mbmff::fiel_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" field_count=\"{}\" field_details=\"{}\"",
                box.size_,
                v.field_count,
                v.field_details
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// free
template <>
struct std::formatter<mbmff::free_box> : std::formatter<std::string_view> {
    auto format(const mbmff::free_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(std::format(" size=\"{}\"", box.size_), ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// wide
template <>
struct std::formatter<mbmff::wide_box> : std::formatter<std::string_view> {
    auto format(const mbmff::wide_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(std::format(" size=\"{}\"", box.size_), ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// ftyp
template <>
struct std::formatter<mbmff::ftyp_box> : std::formatter<std::string_view> {
    auto format(const mbmff::ftyp_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out;
        out += std::format(" size=\"{}\" major=\"{}\"", box.size_, v.major_brand.view());
        out += std::format(" minor=\"{}\" compatible=\"[", v.minor_version);
        for (std::size_t i = 0; i < v.compatible_brands.size(); ++i) {
            if (i) {
                out += ',';
            }
            out += v.compatible_brands[i].view();
        }
        out += "]\"";
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// gmin
template <>
struct std::formatter<mbmff::gmin_box> : std::formatter<std::string_view> {
    auto format(const mbmff::gmin_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" graphicsmode=\"{}\" opcolor=\"{} {} {}\" balance=\"{}\"",
                box.size_,
                v.graphicsmode,
                v.opcolor[0],
                v.opcolor[1],
                v.opcolor[2],
                v.balance
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// meta
template <>
struct std::formatter<mbmff::meta_box> : std::formatter<std::string_view> {
    auto format(const mbmff::meta_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(std::format(" size=\"{}\"", box.size_), ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// load
template <>
struct std::formatter<mbmff::load_box> : std::formatter<std::string_view> {
    auto format(const mbmff::load_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" preload_time=\"{}\" preload_size=\"{}\""
                " default_loading=\"{}\" duration_to_load=\"{}\"",
                box.size_,
                v.preload_time,
                v.preload_size,
                v.default_loading,
                v.duration_to_load
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// mdat
template <>
struct std::formatter<mbmff::mdat_box> : std::formatter<std::string_view> {
    auto format(const mbmff::mdat_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format(" size=\"{}\" data_size=\"{}\"", box.size_, box.payload.size()),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// hvcC
template <>
struct std::formatter<mbmff::hvcC_box> : std::formatter<std::string_view> {
    auto format(const mbmff::hvcC_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" config_version=\"{}\" profile_space=\"{}\""
                " tier_flag=\"{}\" profile_idc=\"{}\""
                " profile_compat=\"{:#010x}\""
                " constraint_indicator=\"{:#018x}\""
                " level_idc=\"{}\""
                " min_spatial_seg=\"{}\" parallelism=\"{}\""
                " chroma_format=\"{}\" bit_depth_luma=\"{}\""
                " bit_depth_chroma=\"{}\""
                " avg_framerate=\"{}\""
                " constant_framerate=\"{}\" temporal_layers=\"{}\""
                " temporal_nested=\"{}\" length_size=\"{}\""
                " num_arrays=\"{}\"",
                box.size_,
                v.configuration_version,
                v.general_profile_space,
                v.general_tier_flag,
                v.general_profile_idc,
                v.general_profile_compatibility_flags,
                v.general_constraint_indicator_flags,
                v.general_level_idc,
                v.min_spatial_segmentation_idc,
                v.parallelism_type,
                v.chroma_format,
                v.bit_depth_luma_minus8 + 8,
                v.bit_depth_chroma_minus8 + 8,
                v.avg_frame_rate,
                v.constant_frame_rate,
                v.num_temporal_layers,
                v.temporal_id_nested,
                v.length_size_minus_one + 1,
                v.num_arrays
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// iinf
template <>
struct std::formatter<mbmff::iinf_box> : std::formatter<std::string_view> {
    auto format(const mbmff::iinf_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(" size=\"{}\" version=\"{}\" entry_count=\"{}\"", box.size_, box.version(), v.entry_count),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// hev1
template <>
struct std::formatter<mbmff::hev1_box> : std::formatter<std::string_view> {
    auto format(const mbmff::hev1_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out = std::format(
            " size=\"{}\" width=\"{}\" height=\"{}\""
            " horizresolution=\"{:.2f}\" vertresolution=\"{:.2f}\""
            " frame_count=\"{}\" depth=\"{}\"",
            box.size_,
            v.width,
            v.height,
            static_cast<double>(v.horizresolution) / 65536.0,
            static_cast<double>(v.vertresolution) / 65536.0,
            v.frame_count,
            v.depth
        );
        if (!v.compressorname.empty()) {
            out += std::format(" name=\"{}\"", v.compressorname.string_view());
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// hdlr
template <>
struct std::formatter<mbmff::hdlr_box> : std::formatter<std::string_view> {
    auto format(const mbmff::hdlr_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out = std::format(" size=\"{}\" handler_type=\"{}\"", box.size_, v.handler_type.view());
        if (!v.name.empty()) {
            out += std::format(" name=\"{}\"", v.name.string_view());
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// pitm
template <>
struct std::formatter<mbmff::pitm_box> : std::formatter<std::string_view> {
    auto format(const mbmff::pitm_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(" size=\"{}\" item_id=\"{}\"", box.size_, v.item_id),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// ispe
template <>
struct std::formatter<mbmff::ispe_box> : std::formatter<std::string_view> {
    auto format(const mbmff::ispe_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" image_width=\"{}\" image_height=\"{}\"",
                box.size_,
                v.image_width,
                v.image_height
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// pixi
template <>
struct std::formatter<mbmff::pixi_box> : std::formatter<std::string_view> {
    auto format(const mbmff::pixi_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out = std::format(" size=\"{}\" bits_per_channel=\"[", box.size_);
        for (std::size_t i = 0; i < v.bits_per_channel.size(); ++i) {
            if (i) {
                out += ',';
            }
            out += std::format("{}", v.bits_per_channel[i]);
        }
        out += "]\"";
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// pasp
template <>
struct std::formatter<mbmff::pasp_box> : std::formatter<std::string_view> {
    auto format(const mbmff::pasp_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(" size=\"{}\" h_spacing=\"{}\" v_spacing=\"{}\"", box.size_, v.h_spacing, v.v_spacing),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// ipma
template <>
struct std::formatter<mbmff::ipma_box> : std::formatter<std::string_view> {
    auto format(const mbmff::ipma_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(" size=\"{}\" entry_count=\"{}\"", box.size_, v.entry_count),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// infe
template <>
struct std::formatter<mbmff::infe_box> : std::formatter<std::string_view> {
    auto format(const mbmff::infe_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out = std::format(
            " size=\"{}\" id=\"{}\" protection=\"{}\"",
            box.size_,
            v.item_id,
            v.item_protection_index
        );
        if (!v.item_name.empty()) {
            out += std::format(" name=\"{}\"", v.item_name.string_view());
        }
        if (v.item_type.view().size() == 4) {
            out += std::format(" type=\"{}\"", v.item_type.view());
        }
        if (!v.content_type.empty()) {
            out += std::format(" content_type=\"{}\"", v.content_type.string_view());
        }
        if (!v.content_encoding.empty()) {
            out += std::format(" content_encoding=\"{}\"", v.content_encoding.string_view());
        }
        if (!v.item_uri_type.empty()) {
            out += std::format(" uri=\"{}\"", v.item_uri_type.string_view());
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// alis
template <>
struct std::formatter<mbmff::alis_box> : std::formatter<std::string_view> {
    auto format(const mbmff::alis_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format(" size=\"{}\" version=\"{}\"", box.size_, box.version()),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// apcn
template <>
struct std::formatter<mbmff::apcn_box> : std::formatter<std::string_view> {
    auto format(const mbmff::apcn_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out = std::format(
            " size=\"{}\" width=\"{}\" height=\"{}\""
            " horizresolution=\"{:.2f}\" vertresolution=\"{:.2f}\""
            " frame_count=\"{}\" depth=\"{}\"",
            box.size_,
            v.width,
            v.height,
            static_cast<double>(v.horizresolution) / 65536.0,
            static_cast<double>(v.vertresolution) / 65536.0,
            v.frame_count,
            v.depth
        );
        if (!v.compressorname.empty()) {
            out += std::format(" name=\"{}\"", v.compressorname.string_view());
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// av1C
template <>
struct std::formatter<mbmff::av1C_box> : std::formatter<std::string_view> {
    auto format(const mbmff::av1C_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out = std::format(
            " size=\"{}\" seq_profile=\"{}\" seq_level_idx_0=\"{}\" seq_tier_0=\"{}\""
            " high_bitdepth=\"{}\" twelve_bit=\"{}\" monochrome=\"{}\""
            " chroma_subsampling_x=\"{}\" chroma_subsampling_y=\"{}\""
            " chroma_sample_position=\"{}\"",
            box.size_,
            static_cast<unsigned>(v.seq_profile),
            static_cast<unsigned>(v.seq_level_idx_0),
            static_cast<unsigned>(v.seq_tier_0),
            static_cast<unsigned>(v.high_bitdepth),
            static_cast<unsigned>(v.twelve_bit),
            static_cast<unsigned>(v.monochrome),
            static_cast<unsigned>(v.chroma_subsampling_x),
            static_cast<unsigned>(v.chroma_subsampling_y),
            static_cast<unsigned>(v.chroma_sample_position)
        );
        if (v.initial_presentation_delay_present) {
            out += std::format(
                " initial_presentation_delay_minus_one=\"{}\"",
                static_cast<unsigned>(v.initial_presentation_delay_minus_one)
            );
        }
        if (!v.config_obus.empty()) {
            out += std::format(" config_obus_size=\"{}\"", v.config_obus.size());
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// mdhd
template <>
struct std::formatter<mbmff::mdhd_box> : std::formatter<std::string_view> {
    auto format(const mbmff::mdhd_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" version=\"{}\" creation_time=\"{}\" modification_time=\"{}\""
                " timescale=\"{}\" duration=\"{}\" language=\"{}\" predefined=\"{}\"",
                box.size_,
                box.version(),
                v.creation_time,
                v.modification_time,
                v.timescale,
                v.duration,
                v.language.view(),
                v.predefined
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// co64
template <>
struct std::formatter<mbmff::co64_box> : std::formatter<std::string_view> {
    auto format(const mbmff::co64_box& box, std::format_context& ctx) const
    {
        auto d = box.value();
        std::string
            out = std::format(" size=\"{}\" version=\"{}\" entry_count=\"{}\"", box.size_, box.version(), d.size());
        for (std::uint32_t i = 0; i < d.size() && i < 3; ++i) {
            if (i) {
                out += " | ";
            }
            out += std::format(" chunk_offset=\"{}\"", d[i]);
        }
        if (d.size() > 3) {
            out += std::format(" ...");
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// colr
template <>
struct std::formatter<mbmff::colr_box> : std::formatter<std::string_view> {
    auto format(const mbmff::colr_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out = std::format(" size=\"{}\" colour_type=\"{}\"", box.size_, v.colour_type.view());
        if (v.colour_type == mbmff::fourcc_string::from_uint32(mbmff::fourcc("nclx"))) {
            out += std::format(
                " primaries=\"{}\" transfer=\"{}\" matrix=\"{}\" full_range=\"{}\"",
                v.colour_primaries,
                v.transfer_characteristics,
                v.matrix_coefficients,
                v.full_range_flag
            );
        } else if (
            v.colour_type == mbmff::fourcc_string::from_uint32(mbmff::fourcc("rICC"))
            || v.colour_type == mbmff::fourcc_string::from_uint32(mbmff::fourcc("prof"))
        ) {
            out += std::format(" icc_size=\"{}\"", v.icc_profile.size());
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// ctts
template <>
struct std::formatter<mbmff::ctts_box> : std::formatter<std::string_view> {
    auto format(const mbmff::ctts_box& box, std::format_context& ctx) const
    {
        auto d = box.value();
        std::string
            out = std::format(" size=\"{}\" version=\"{}\" entry_count=\"{}\"", box.size_, box.version(), d.size());
        for (std::uint32_t i = 0; i < d.size() && i < 3; ++i) {
            if (i) {
                out += " | ";
            }
            auto e = d[i];
            out += std::format(" sample_count=\"{}\" sample_offset=\"{}\"", e.sample_count, e.sample_offset);
        }
        if (d.size() > 3) {
            out += std::format(" ...");
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// dref
template <>
struct std::formatter<mbmff::dref_box> : std::formatter<std::string_view> {
    auto format(const mbmff::dref_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(" size=\"{}\" version=\"{}\" entry_count=\"{}\"", box.size_, box.version(), v.entry_count),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// tmcd
template <>
struct std::formatter<mbmff::tmcd_box> : std::formatter<std::string_view> {
    auto format(const mbmff::tmcd_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" data_ref=\"{}\" flags=\"{}\""
                " timescale=\"{}\" frame_dur=\"{}\" num_frames=\"{}\"",
                box.size_,
                v.data_reference_index,
                v.flags,
                v.timescale,
                v.frame_duration,
                v.number_of_frames
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// url
template <>
struct std::formatter<mbmff::url_box> : std::formatter<std::string_view> {
    auto format(const mbmff::url_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        auto out = std::format(" size=\"{}\" version=\"{}\"", box.size_, box.version());
        if (!v.location.empty()) {
            out += std::format(" location=\"{}\"", v.location.string_view());
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// avcC
template <>
struct std::formatter<mbmff::avcC_box> : std::formatter<std::string_view> {
    auto format(const mbmff::avcC_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" profile=\"{}\" compatibility=\"{}\" level=\"{}\""
                " length_size=\"{}\" nalu_size=\"{}\"",
                box.size_,
                v.AVCProfileIndication,
                v.profile_compatibility,
                v.AVCLevelIndication,
                v.lengthSizeMinusOne + 1,
                v.nalu_data.size()
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// avc1
template <>
struct std::formatter<mbmff::avc1_box> : std::formatter<std::string_view> {
    auto format(const mbmff::avc1_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out = std::format(
            " size=\"{}\" width=\"{}\" height=\"{}\""
            " horizresolution=\"{:.2f}\" vertresolution=\"{:.2f}\""
            " frame_count=\"{}\" depth=\"{}\"",
            box.size_,
            v.width,
            v.height,
            static_cast<double>(v.horizresolution) / 65536.0,
            static_cast<double>(v.vertresolution) / 65536.0,
            v.frame_count,
            v.depth
        );
        if (!v.compressorname.empty()) {
            out += std::format(" name=\"{}\"", v.compressorname.string_view());
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// mp4v
template <>
struct std::formatter<mbmff::mp4v_box> : std::formatter<std::string_view> {
    auto format(const mbmff::mp4v_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out = std::format(
            " size=\"{}\" width=\"{}\" height=\"{}\""
            " horizresolution=\"{:.2f}\" vertresolution=\"{:.2f}\""
            " frame_count=\"{}\" depth=\"{}\"",
            box.size_,
            v.width,
            v.height,
            static_cast<double>(v.horizresolution) / 65536.0,
            static_cast<double>(v.vertresolution) / 65536.0,
            v.frame_count,
            v.depth
        );
        if (!v.compressorname.empty()) {
            out += std::format(" name=\"{}\"", v.compressorname.string_view());
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// mp4a
template <>
struct std::formatter<mbmff::mp4a_box> : std::formatter<std::string_view> {
    auto format(const mbmff::mp4a_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" channels=\"{}\" sample_size=\"{}\" sample_rate=\"{:.2f}\"",
                box.size_,
                v.channelcount,
                v.samplesize,
                static_cast<double>(v.samplerate) / 65536.0
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// mvhd
template <>
struct std::formatter<mbmff::mvhd_box> : std::formatter<std::string_view> {
    auto format(const mbmff::mvhd_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out = std::format(
            " size=\"{}\" version=\"{}\" creation_time=\"{}\" modification_time=\"{}\""
            " timescale=\"{}\" duration=\"{}\" rate=\"{:.2f}\" volume=\"{:.2f}\" next_track_id=\"{}\"",
            box.size_,
            box.version(),
            v.creation_time,
            v.modification_time,
            v.timescale,
            v.duration,
            static_cast<double>(v.rate) / 65536.0,
            static_cast<double>(v.volume) / 256.0,
            v.next_track_id
        );
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// tkhd
template <>
struct std::formatter<mbmff::tkhd_box> : std::formatter<std::string_view> {
    auto format(const mbmff::tkhd_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out = std::format(
            " size=\"{}\" version=\"{}\" creation_time=\"{}\" modification_time=\"{}\""
            " track_id=\"{}\" duration=\"{}\" layer=\"{}\" alternate_group=\"{}\""
            " volume=\"{:.2f}\" width=\"{:.2f}\" height=\"{:.2f}\"",
            box.size_,
            box.version(),
            v.creation_time,
            v.modification_time,
            v.track_id,
            v.duration,
            v.layer,
            v.alternate_group,
            static_cast<double>(v.volume) / 256.0,
            static_cast<double>(v.width) / 65536.0,
            static_cast<double>(v.height) / 65536.0
        );
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// elst
template <>
struct std::formatter<mbmff::elst_box> : std::formatter<std::string_view> {
    auto format(const mbmff::elst_box& box, std::format_context& ctx) const
    {
        auto d = box.value();
        std::string
            out = std::format(" size=\"{}\" version=\"{}\" entry_count=\"{}\"", box.size_, box.version(), d.size());
        for (std::uint32_t i = 0; i < d.size(); ++i) {
            if (i) {
                out += " | ";
            }
            auto e = d[i];
            out += std::format(
                " entry=\"{}\" segment_duration=\"{}\" media_time=\"{}\""
                " media_rate_integer=\"{}\" media_rate_fraction=\"{}\"",
                i,
                e.segment_duration,
                e.media_time,
                e.media_rate_integer,
                e.media_rate_fraction
            );
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// sdtp
template <>
struct std::formatter<mbmff::sdtp_box> : std::formatter<std::string_view> {
    auto format(const mbmff::sdtp_box& box, std::format_context& ctx) const
    {
        auto d = box.value();
        std::string out = std::format(" size=\"{}\" entry_count=\"{}\"", box.size_, d.size());
        for (std::uint32_t i = 0; i < d.size() && i < 3; ++i) {
            if (i) {
                out += " | ";
            }
            auto e = d[i];
            out += std::format(
                " is_leading={} depends_on={} depended_on={} redundancy={}",
                e.is_leading,
                e.sample_depends_on,
                e.sample_is_depended_on,
                e.sample_has_redundancy
            );
        }
        if (d.size() > 3) {
            out += std::format(" ...");
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// smhd
template <>
struct std::formatter<mbmff::smhd_box> : std::formatter<std::string_view> {
    auto format(const mbmff::smhd_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" version=\"{}\" balance=\"{:.2f}\"",
                box.size_,
                box.version(),
                static_cast<double>(v.balance) / 256.0
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// stss
template <>
struct std::formatter<mbmff::stss_box> : std::formatter<std::string_view> {
    auto format(const mbmff::stss_box& box, std::format_context& ctx) const
    {
        auto d = box.value();
        std::string
            out = std::format(" size=\"{}\" version=\"{}\" entry_count=\"{}\"", box.size_, box.version(), d.size());
        for (std::uint32_t i = 0; i < d.size() && i < 3; ++i) {
            if (i) {
                out += " | ";
            }
            out += std::format(" sample_number=\"{}\"", d[i]);
        }
        if (d.size() > 3) {
            out += std::format(" ...");
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// stco
template <>
struct std::formatter<mbmff::stco_box> : std::formatter<std::string_view> {
    auto format(const mbmff::stco_box& box, std::format_context& ctx) const
    {
        auto d = box.value();
        std::string
            out = std::format(" size=\"{}\" version=\"{}\" entry_count=\"{}\"", box.size_, box.version(), d.size());
        for (std::uint32_t i = 0; i < d.size() && i < 3; ++i) {
            if (i) {
                out += " | ";
            }
            out += std::format(" chunk_offset=\"{}\"", d[i]);
        }
        if (d.size() > 3) {
            out += std::format(" ...");
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// stsc
template <>
struct std::formatter<mbmff::stsc_box> : std::formatter<std::string_view> {
    auto format(const mbmff::stsc_box& box, std::format_context& ctx) const
    {
        auto d = box.value();
        std::string
            out = std::format(" size=\"{}\" version=\"{}\" entry_count=\"{}\"", box.size_, box.version(), d.size());
        for (std::uint32_t i = 0; i < d.size() && i < 3; ++i) {
            if (i) {
                out += " | ";
            }
            auto e = d[i];
            out += std::format(
                " first_chunk=\"{}\" samples_per_chunk=\"{}\" description_index=\"{}\"",
                e.first_chunk,
                e.samples_per_chunk,
                e.sample_description_index
            );
        }
        if (d.size() > 3) {
            out += std::format(" ...");
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// stsz
template <>
struct std::formatter<mbmff::stsz_box> : std::formatter<std::string_view> {
    auto format(const mbmff::stsz_box& box, std::format_context& ctx) const
    {
        auto d = box.value();
        std::string out = std::format(
            " size=\"{}\" version=\"{}\" sample_size=\"{}\" sample_count=\"{}\"",
            box.size_,
            box.version(),
            d.sample_size,
            d.size()
        );
        if (d.sample_size == 0) {
            for (std::uint32_t i = 0; i < d.size() && i < 3; ++i) {
                if (i) {
                    out += " | ";
                }
                out += std::format(" entry_size=\"{}\"", d[i]);
            }
            if (d.size() > 3) {
                out += std::format(" ...");
            }
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// stts
template <>
struct std::formatter<mbmff::stts_box> : std::formatter<std::string_view> {
    auto format(const mbmff::stts_box& box, std::format_context& ctx) const
    {
        auto d = box.value();
        std::string
            out = std::format(" size=\"{}\" version=\"{}\" entry_count=\"{}\"", box.size_, box.version(), d.size());
        for (std::uint32_t i = 0; i < d.size(); ++i) {
            if (i) {
                out += " | ";
            }
            auto e = d[i];
            out += std::format(" sample_count=\"{}\" sample_delta=\"{}\"", e.sample_count, e.sample_delta);
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// stsd
template <>
struct std::formatter<mbmff::stsd_box> : std::formatter<std::string_view> {
    auto format(const mbmff::stsd_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(" size=\"{}\" version=\"{}\" entry_count=\"{}\"", box.size_, box.version(), v.entry_count),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// vmhd
template <>
struct std::formatter<mbmff::vmhd_box> : std::formatter<std::string_view> {
    auto format(const mbmff::vmhd_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" graphicsmode=\"{}\" opcolor=\"{} {} {}\"",
                box.size_,
                v.graphicsmode,
                v.opcolor[0],
                v.opcolor[1],
                v.opcolor[2]
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// esds
template <>
struct std::formatter<mbmff::esds_box> : std::formatter<std::string_view> {
    auto format(const mbmff::esds_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" version=\"{}\" object_type=\"{:#04x}\""
                " stream_type=\"{}\" buffer_size=\"{}\""
                " max_bitrate=\"{}\" avg_bitrate=\"{}\""
                " asc_size=\"{}\"",
                box.size_,
                box.version(),
                v.object_type,
                v.stream_type,
                v.buffer_size,
                v.max_bitrate,
                v.avg_bitrate,
                v.audio_specific_config.size()
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// iloc
template <>
struct std::formatter<mbmff::iloc_box> : std::formatter<std::string_view> {
    auto format(const mbmff::iloc_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" version=\"{}\" offset_size=\"{}\" length_size=\"{}\""
                " base_offset_size=\"{}\" index_size=\"{}\" item_count=\"{}\"",
                box.size_,
                box.version(),
                v.offset_size,
                v.length_size,
                v.base_offset_size,
                v.index_size,
                v.item_count
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// btrt
template <>
struct std::formatter<mbmff::btrt_box> : std::formatter<std::string_view> {
    auto format(const mbmff::btrt_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" buffer_size=\"{}\" max_bitrate=\"{}\" avg_bitrate=\"{}\"",
                box.size_,
                v.buffer_size_db,
                v.max_bitrate,
                v.avg_bitrate
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// sbgp
template <>
struct std::formatter<mbmff::sbgp_box> : std::formatter<std::string_view> {
    auto format(const mbmff::sbgp_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        auto type = mbmff::fourcc_string::from_uint32(v.grouping_type);
        auto entries = std::string{};
        auto n = (std::min)(std::size_t{3}, static_cast<std::size_t>(v.entry_count));
        for (std::size_t i = 0; i < n; ++i) {
            auto e = v[i];
            entries += std::format(" [{}=\"{}\" idx=\"{}\"]", i, e.sample_count, e.group_description_index);
        }
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" version=\"{}\" grouping_type=\"{}\""
                " grouping_type_parameter=\"{}\" entry_count=\"{}\"{}",
                box.size_,
                box.version(),
                type.view(),
                v.grouping_type_parameter,
                v.entry_count,
                entries
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// sgpd
template <>
struct std::formatter<mbmff::sgpd_box> : std::formatter<std::string_view> {
    auto format(const mbmff::sgpd_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        auto type = mbmff::fourcc_string::from_uint32(v.grouping_type);
        auto entries = std::string{};
        auto n = (std::min)(std::size_t{3}, static_cast<std::size_t>(v.entry_count));
        for (std::size_t i = 0; i < n; ++i) {
            auto d = v.entry_data(i);
            auto hex = std::string{};
            for (auto b : d) {
                hex += std::format("{:02x}", static_cast<std::uint8_t>(b));
            }
            entries += std::format(" [{}={}]", i, hex);
        }
        return std::formatter<std::string_view>::format(
            std::format(
                " size=\"{}\" version=\"{}\" grouping_type=\"{}\""
                " default_length=\"{}\""
                " default_sample_description_index=\"{}\" entry_count=\"{}\"{}",
                box.size_,
                box.version(),
                type.view(),
                v.default_length,
                v.default_sample_description_index,
                v.entry_count,
                entries
            ),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// avc3 (same layout as avc1)
template <>
struct std::formatter<mbmff::avc3_box> : std::formatter<std::string_view> {
    auto format(const mbmff::avc3_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out = std::format(
            " size=\"{}\" width=\"{}\" height=\"{}\""
            " horizresolution=\"{:.2f}\" vertresolution=\"{:.2f}\""
            " frame_count=\"{}\" depth=\"{}\"",
            box.size_,
            v.width,
            v.height,
            static_cast<double>(v.horizresolution) / 65536.0,
            static_cast<double>(v.vertresolution) / 65536.0,
            v.frame_count,
            v.depth
        );
        if (!v.compressorname.empty()) {
            out += std::format(" name=\"{}\"", v.compressorname.string_view());
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// cdsc
template <>
struct std::formatter<mbmff::cdsc_box> : std::formatter<std::string_view> {
    auto format(const mbmff::cdsc_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(std::format(" size=\"{}\"", box.size_), ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// dimg
template <>
struct std::formatter<mbmff::dimg_box> : std::formatter<std::string_view> {
    auto format(const mbmff::dimg_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(std::format(" size=\"{}\"", box.size_), ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// frma
template <>
struct std::formatter<mbmff::frma_box> : std::formatter<std::string_view> {
    auto format(const mbmff::frma_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        auto fc = mbmff::fourcc_string::from_uint32(v.data_format);
        return std::formatter<std::string_view>::format(
            std::format(" size=\"{}\" data_format=\"{}\"", box.size_, fc.view()),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// irot
template <>
struct std::formatter<mbmff::irot_box> : std::formatter<std::string_view> {
    auto format(const mbmff::irot_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        return std::formatter<std::string_view>::format(
            std::format(" size=\"{}\" angle=\"{}\"", box.size_, v.angle),
            ctx
        );
    }
};

//------------------------------------------------------------------------------------------------------------
// schm
template <>
struct std::formatter<mbmff::schm_box> : std::formatter<std::string_view> {
    auto format(const mbmff::schm_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        auto fc = mbmff::fourcc_string::from_uint32(v.scheme_type);
        auto out = std::format(
            " size=\"{}\" version=\"{}\" scheme_type=\"{}\" scheme_version=\"{}\"",
            box.size_,
            box.version(),
            fc.view(),
            v.scheme_version
        );
        if (!v.scheme_uri.empty()) {
            out += std::format(" scheme_uri=\"{}\"", v.scheme_uri.string_view());
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// thmb
template <>
struct std::formatter<mbmff::thmb_box> : std::formatter<std::string_view> {
    auto format(const mbmff::thmb_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(std::format(" size=\"{}\"", box.size_), ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// urn
template <>
struct std::formatter<mbmff::urn_box> : std::formatter<std::string_view> {
    auto format(const mbmff::urn_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        auto out = std::format(" size=\"{}\"", box.size_);
        if (!v.name.empty()) {
            out += std::format(" name=\"{}\"", v.name.string_view());
        }
        if (!v.location.empty()) {
            out += std::format(" location=\"{}\"", v.location.string_view());
        }
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// Display fourcc as string if printable, otherwise as hex
static auto display_fourcc(const mbmff::fourcc_string& fc) -> std::string
{
    std::string out;
    out.reserve(12);
    bool all_printable = true;
    for (std::size_t i = 0; i < 4; ++i) {
        auto c = fc[i];
        auto uc = static_cast<unsigned char>(c);
        if (uc < 0x20 || (uc > 0x7e && uc != 0xa9)) {
            all_printable = false;
            break;
        }
    }
    if (all_printable) {
        out.append(fc.view());
    } else {
        out += '[';
        for (std::size_t i = 0; i < 4; ++i) {
            if (i) {
                out += ' ';
            }
            auto uc = static_cast<unsigned char>(fc[i]);
            auto hex = "0123456789abcdef";
            out += "0x";
            out += hex[uc >> 4];
            out += hex[uc & 0xf];
        }
        out += ']';
    }
    return out;
}

//------------------------------------------------------------------------------------------------------------
// Print box information with indentation based on depth
static void print_box(const mbmff::any_box_view& box, std::size_t depth)
{
    for (std::size_t i = 0; i < depth; ++i) {
        std::cout << "  ";
    }

    if (!mbmff::is_box_implemented(box.type_)) {
        std::cout << "\x1b[33m[!]\x1b[0m ";
    }

    std::cout << '<' << display_fourcc(box.type_string());

    switch (box.type_) {
    case mbmff::box_type::fiel:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::fiel>(box));
        break;
    case mbmff::box_type::free:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::free>(box));
        break;
    case mbmff::box_type::wide:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::wide>(box));
        break;
    case mbmff::box_type::gmin:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::gmin>(box));
        break;
    case mbmff::box_type::ftyp:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::ftyp>(box));
        break;
    case mbmff::box_type::avcC:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::avcC>(box));
        break;
    case mbmff::box_type::btrt:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::btrt>(box));
        break;
    case mbmff::box_type::avc1:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::avc1>(box));
        break;
    case mbmff::box_type::hev1:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::hev1>(box));
        break;
    case mbmff::box_type::meta:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::meta>(box));
        break;
    case mbmff::box_type::load:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::load>(box));
        break;
    case mbmff::box_type::mdat:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::mdat>(box));
        break;
    case mbmff::box_type::hvcC:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::hvcC>(box));
        break;
    case mbmff::box_type::iinf:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::iinf>(box));
        break;
    case mbmff::box_type::iloc:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::iloc>(box));
        break;
    case mbmff::box_type::hdlr:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::hdlr>(box));
        break;
    case mbmff::box_type::pitm:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::pitm>(box));
        break;
    case mbmff::box_type::ispe:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::ispe>(box));
        break;
    case mbmff::box_type::pixi:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::pixi>(box));
        break;
    case mbmff::box_type::pasp:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::pasp>(box));
        break;
    case mbmff::box_type::ipma:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::ipma>(box));
        break;
    case mbmff::box_type::infe:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::infe>(box));
        break;
    case mbmff::box_type::alis:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::alis>(box));
        break;
    case mbmff::box_type::apcn:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::apcn>(box));
        break;
    case mbmff::box_type::av1C:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::av1C>(box));
        break;
    case mbmff::box_type::mvhd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::mvhd>(box));
        break;
    case mbmff::box_type::tmcd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::tmcd>(box));
        break;
    case mbmff::box_type::tkhd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::tkhd>(box));
        break;
    case mbmff::box_type::mp4v:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::mp4v>(box));
        break;
    case mbmff::box_type::mp4a:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::mp4a>(box));
        break;
    case mbmff::box_type::esds:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::esds>(box));
        break;
    case mbmff::box_type::elst:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::elst>(box));
        break;
    case mbmff::box_type::mdhd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::mdhd>(box));
        break;
    case mbmff::box_type::stco:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::stco>(box));
        break;
    case mbmff::box_type::stsc:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::stsc>(box));
        break;
    case mbmff::box_type::stsd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::stsd>(box));
        break;
    case mbmff::box_type::stss:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::stss>(box));
        break;
    case mbmff::box_type::stsz:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::stsz>(box));
        break;
    case mbmff::box_type::stts:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::stts>(box));
        break;
    case mbmff::box_type::sbgp:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::sbgp>(box));
        break;
    case mbmff::box_type::sgpd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::sgpd>(box));
        break;
    case mbmff::box_type::sdtp:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::sdtp>(box));
        break;
    case mbmff::box_type::smhd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::smhd>(box));
        break;
    case mbmff::box_type::vmhd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::vmhd>(box));
        break;
    case mbmff::box_type::co64:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::co64>(box));
        break;
    case mbmff::box_type::colr:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::colr>(box));
        break;
    case mbmff::box_type::ctts:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::ctts>(box));
        break;
    case mbmff::box_type::dref:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::dref>(box));
        break;
    case mbmff::box_type::url:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::url>(box));
        break;
    case mbmff::box_type::avc3:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::avc3>(box));
        break;
    case mbmff::box_type::cdsc:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::cdsc>(box));
        break;
    case mbmff::box_type::dimg:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::dimg>(box));
        break;
    case mbmff::box_type::frma:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::frma>(box));
        break;
    case mbmff::box_type::irot:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::irot>(box));
        break;
    case mbmff::box_type::schm:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::schm>(box));
        break;
    case mbmff::box_type::thmb:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::thmb>(box));
        break;
    case mbmff::box_type::urn:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::urn>(box));
        break;
    default:
        std::cout << " size=\"" << box.size_ << '"';
        break;
    }

    bool is_container = mbmff::is_container(box);
    std::cout << (is_container ? ">" : " />") << '\n';

    if (box.type_ == mbmff::box_type::iloc) {
        auto iloc = mbmff::box_cast<mbmff::box_type::iloc>(box);
        auto data = iloc.value();
        auto iter = mbmff::iloc_item_iterator(data, iloc.version());
        for (; iter != mbmff::iloc_item_iterator{}; ++iter) {
            auto item = *iter;
            for (std::size_t i = 0; i < depth + 1; ++i) {
                std::cout << "  ";
            }
            std::cout << std::format(
                "<item id=\"{}\" base_offset=\"{}\" construction_method=\"{}\""
                " data_reference_index=\"{}\" extents=\"{}\">\n",
                item.item_id,
                item.base_offset,
                item.construction_method,
                item.data_reference_index,
                item.size()
            );
            for (std::size_t e = 0; e < item.size(); ++e) {
                auto ext = item[e];
                for (std::size_t i = 0; i < depth + 2; ++i) {
                    std::cout << "  ";
                }
                std::cout << std::format("<extent offset=\"{}\" length=\"{}\"", ext.offset, ext.length);
                if (ext.index) {
                    std::cout << std::format(" index=\"{}\"", ext.index);
                }
                std::cout << " />\n";
            }
            for (std::size_t i = 0; i < depth + 1; ++i) {
                std::cout << "  ";
            }
            std::cout << "</item>\n";
        }
    }
}
