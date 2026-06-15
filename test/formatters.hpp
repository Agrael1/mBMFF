#pragma once
#include <format>
#include <string>
#include <mbmff/mbmff.hpp>

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
            if (i) out += ',';
            out += v.compatible_brands[i].view();
        }
        out += "]\"";
        return std::formatter<std::string_view>::format(out, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
// meta
template <>
struct std::formatter<mbmff::meta_box> : std::formatter<std::string_view> {
    auto format(const mbmff::meta_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format(" size=\"{}\"", box.size_),
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
// hdlr
template <>
struct std::formatter<mbmff::hdlr_box> : std::formatter<std::string_view> {
    auto format(const mbmff::hdlr_box& box, std::format_context& ctx) const
    {
        auto v = box.value();
        std::string out = std::format(" size=\"{}\" handler_type=\"{}\"", box.size_, v.handler_type.view());
        if (!v.name.empty()) {
            out += std::format(" name=\"{}\"", v.name);
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
            std::format(" size=\"{}\" image_width=\"{}\" image_height=\"{}\"", box.size_, v.image_width, v.image_height),
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
            if (i) out += ',';
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
        std::string out = std::format(" size=\"{}\" id=\"{}\" protection=\"{}\"",
            box.size_, v.item_id, v.item_protection_index);
        if (!v.item_name.empty()) {
            out += std::format(" name=\"{}\"", v.item_name);
        }
        if (v.item_type.view().size() == 4) {
            out += std::format(" type=\"{}\"", v.item_type.view());
        }
        if (!v.content_type.empty()) {
            out += std::format(" content_type=\"{}\"", v.content_type);
        }
        if (!v.content_encoding.empty()) {
            out += std::format(" content_encoding=\"{}\"", v.content_encoding);
        }
        if (!v.item_uri_type.empty()) {
            out += std::format(" uri=\"{}\"", v.item_uri_type);
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
            static_cast<unsigned>(v.seq_profile), static_cast<unsigned>(v.seq_level_idx_0),
            static_cast<unsigned>(v.seq_tier_0),
            static_cast<unsigned>(v.high_bitdepth), static_cast<unsigned>(v.twelve_bit),
            static_cast<unsigned>(v.monochrome),
            static_cast<unsigned>(v.chroma_subsampling_x), static_cast<unsigned>(v.chroma_subsampling_y),
            static_cast<unsigned>(v.chroma_sample_position));
        if (v.initial_presentation_delay_present) {
            out += std::format(" initial_presentation_delay_minus_one=\"{}\"",
                static_cast<unsigned>(v.initial_presentation_delay_minus_one));
        }
        if (!v.config_obus.empty()) {
            out += std::format(" config_obus_size=\"{}\"", v.config_obus.size());
        }
        return std::formatter<std::string_view>::format(out, ctx);
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
            " timescale=\"{}\" duration=\"{}\" rate=\"{}\" volume=\"{}\" next_track_id=\"{}\"",
            box.size_, box.version(), v.creation_time, v.modification_time,
            v.timescale, v.duration, v.rate, v.volume, v.next_track_id);
        return std::formatter<std::string_view>::format(out, ctx);
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
            std::format(" size=\"{}\" version=\"{}\" offset_size=\"{}\" length_size=\"{}\""
                        " base_offset_size=\"{}\" index_size=\"{}\" item_count=\"{}\"",
                        box.size_, box.version(),
                        v.offset_size, v.length_size, v.base_offset_size, v.index_size, v.item_count),
            ctx
        );
    }
};
