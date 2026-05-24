#pragma once
#include <string>
#include <mbmff/mbmff.hpp>
#include <mbmff/av1.hpp>

template<>
struct std::formatter<mbmff::obu_sequence_header_view> : std::formatter<std::string_view> {
    auto format(const mbmff::obu_sequence_header_view& view, std::format_context& ctx) const
    {
        auto header = view.header();
        std::string output = std::format(
            "obu_sequence_header: seq_profile={}, still_picture={}, reduced_still_picture_header={}, "
            "timing_info_present_flag={}, decoder_model_info_present_flag={}, "
            "initial_display_delay_present_flag={}, operating_points_cnt_minus_1={}",
            uint8_t(header.seq_profile),
            uint8_t(header.still_picture),
            uint8_t(header.reduced_still_picture_header),
            uint8_t(header.timing_info_present_flag),
            uint8_t(header.decoder_model_info_present_flag),
            uint8_t(header.initial_display_delay_present_flag),
            uint8_t(header.operating_points_cnt_minus_1)
        );

        if (header.operating_points_cnt_minus_1 > 0 || true) {
            output += "\n  Operating Points:";
            for (std::uint8_t i = 0; i <= header.operating_points_cnt_minus_1; ++i) {
                const auto& op = header.operating_points[i];
                output += std::format(
                    "\n    [{}] idc={}, level_idx={}, tier={}, decoder_model_present={}, initial_display_delay_present={}",
                    i, op.operating_point_idc, static_cast<uint32_t>(op.seq_level_idx),
                    op.seq_tier ? "high" : "main",
                    static_cast<uint32_t>(op.decoder_model_present_for_this_op),
                    static_cast<uint32_t>(op.initial_display_delay_present_for_this_op)
                );
            }
        }

        output += std::format("\n  Color Config:");
        output += std::format("\n    high_bitdepth={}, twelve_bit={}, monochrome={}, color_description_present={}", 
            static_cast<uint32_t>(header.color_config_data.high_bitdepth),
            static_cast<uint32_t>(header.color_config_data.twelve_bit),
            static_cast<uint32_t>(header.color_config_data.monochrome),
            static_cast<uint32_t>(header.color_config_data.color_description_present_flag)
        );

        if (header.color_config_data.color_description_present_flag) {
            const auto& cd = header.color_config_data.color_description_data;
            output += std::format("\n    color_primaries={}, transfer_characteristics={}, matrix_coefficients={}", 
                mbmff::to_string(cd.color_primaries),
                mbmff::to_string(cd.transfer_characteristics),
                mbmff::to_string(cd.matrix_coefficients)
            );
        }

        output += std::format("\n  Frame Max Dimensions: {}x{}",
            header.max_frame_width_minus_1 + 1,
            header.max_frame_height_minus_1 + 1
        );

        return std::formatter<std::string_view>::format(output, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
template <>
struct std::formatter<mbmff::any_obu_view> : std::formatter<std::string_view> {
    auto format(const mbmff::any_obu_view& box, format_context& ctx) const -> format_context::iterator
    {
        std::string output = std::format(
            "OBU: type={}",
            mbmff::to_string(static_cast<mbmff::obu_type>(box.type))
        );

        // Add size field if present
        if (box.has_size_field) {
            output += std::format(", size_field=1");
        }
        
        // Add extension fields
        if (box.extension_flag) {
            output += std::format(
                ", temporal_id={}, spatial_id={}", 
                box.temporal_id, 
                box.spatial_id
            );
        }

        output += "}";
        return std::formatter<std::string_view>::format(output, ctx);
    }
};

//------------------------------------------------------------------------------------------------------------
template <>
struct std::formatter<mbmff::error_code> : std::formatter<std::string_view> {
    auto format(mbmff::error_code code, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(mbmff::get_error_message(code), ctx);
    }
};

template <>
struct std::formatter<mbmff::box_header> : std::formatter<std::string_view> {
    auto format(const mbmff::box_header& header, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format("{} ({} bytes)", header.type_string().view(), header.size),
            ctx
        );
    }
};

template <>
struct std::formatter<mbmff::ftyp_box> : std::formatter<std::string_view> {
    auto format(const mbmff::ftyp_box& box, std::format_context& ctx) const
    {
        std::string output = std::format(
            "FTYP [{} bytes]: {} minor={} compatible=[",
            box.box_header.size,
            box.major_brand().view(),
            box.minor_version()
        );

        auto compatible_brands = box.compatible_brands();

        for (std::size_t i = 0; i < compatible_brands.size(); ++i) {
            if (i != 0) {
                output.append(", ");
            }
            output.append(compatible_brands[i].view());
        }
        output.push_back(']');
        return std::formatter<std::string_view>::format(output, ctx);
    }
};

template <>
struct std::formatter<mbmff::meta_box> : std::formatter<std::string_view> {
    auto format(const mbmff::meta_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format(
                "META [{} bytes]: version={} flags=0x{:06X}",
                box.box_header.size,
                box.box_header.version,
                box.box_header.flags_value()
            ),
            ctx
        );
    }
};

template <>
struct std::formatter<mbmff::mdat_box> : std::formatter<std::string_view> {
    auto format(const mbmff::mdat_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format("MDAT [{} bytes]: payload={} bytes", box.box_header.size, box.data_size()),
            ctx
        );
    }
};

template <>
struct std::formatter<mbmff::iinf_box> : std::formatter<std::string_view> {
    auto format(const mbmff::iinf_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format(
                "IINF [{} bytes]: version={} flags=0x{:06X}, entries={}",
                box.box_header.size,
                box.box_header.version,
                box.box_header.flags_value(),
                box.entry_count()
            ),
            ctx
        );
    }
};

template <>
struct std::formatter<mbmff::infe_box> : std::formatter<std::string_view> {
    auto format(const mbmff::infe_box& box, std::format_context& ctx) const
    {
        auto header = box.header();
        auto version = box.version();
        std::string output = std::format(
            "INFE [{} bytes]: version={} id={} protection={}",
            box.box_header.size,
            version,
            header.item_id,
            header.item_protection_index
        );

        if (version < 2) {
            output.append(std::format(" name={}", header.item_name));
        }

        if (version >= 2) {
            output.append(std::format(" type={}", header.item_type.view()));
            if (header.item_type.view() == "mime") {
                output.append(std::format(" content_type={}", header.content_type));
                if (!header.content_encoding.empty()) {
                    output.append(std::format(" content_encoding={}", header.content_encoding));
                }
            } else if (header.item_type.view() == "uri ") {
                output.append(std::format(" uri={}", header.item_uri_type));
            }
        }

        return std::formatter<std::string_view>::format(output, ctx);
    }
};

template <>
struct std::formatter<mbmff::hdlr_box> : std::formatter<std::string_view> {
    auto format(const mbmff::hdlr_box& box, std::format_context& ctx) const
    {
        auto header = box.header();
        std::string output = std::format(
            "HDLR [{} bytes]: handler_type={}",
            box.box_header.size,
            header.handler_type.view()
        );
        if (!header.name.empty()) {
            output.append(std::format(" name={}", header.name));
        }
        return std::formatter<std::string_view>::format(output, ctx);
    }
};

template <>
struct std::formatter<mbmff::pitm_box> : std::formatter<std::string_view> {
    auto format(const mbmff::pitm_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format("PITM [{} bytes]: item_id={}", box.box_header.size, box.item_id()),
            ctx
        );
    }
};

template <>
struct std::formatter<mbmff::iloc_box> : std::formatter<std::string_view> {
    auto format(const mbmff::iloc_box& box, std::format_context& ctx) const
    {
        auto header = box.header();
        std::string output = std::format(
            "ILOC [{} bytes]: version={} offset_size={} length_size={} base_offset_size={} index_size={} item_count={}",
            box.box_header.size,
            box.version(),
            header.offset_size,
            header.length_size,
            header.base_offset_size,
            header.index_size,
            header.item_count
        );
        return std::formatter<std::string_view>::format(output, ctx);
    }
};

template <>
struct std::formatter<mbmff::iprp_box> : std::formatter<std::string_view> {
    auto format(const mbmff::iprp_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format("IPRP [{} bytes]: Container", box.box_header.size),
            ctx
        );
    }
};

template <>
struct std::formatter<mbmff::ipco_box> : std::formatter<std::string_view> {
    auto format(const mbmff::ipco_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format("IPCO [{} bytes]: Container", box.box_header.size),
            ctx
        );
    }
};

template <>
struct std::formatter<mbmff::ispe_box> : std::formatter<std::string_view> {
    auto format(const mbmff::ispe_box& box, std::format_context& ctx) const
    {
        auto header = box.header();
        return std::formatter<std::string_view>::format(
            std::format(
                "ISPE [{} bytes]: image_width={} image_height={}",
                box.box_header.size,
                header.image_width,
                header.image_height
            ),
            ctx
        );
    }
};

template <>
struct std::formatter<mbmff::av1C_box> : std::formatter<std::string_view> {
    auto format(const mbmff::av1C_box& box, std::format_context& ctx) const
    {
        auto header = box.header();
        std::string output = std::format(
            "AV1C [{} bytes]: profile={} level={} tier={} bitdepth={} monochrome={} chroma_subsampling=({}, {})",
            box.box_header.size,
            std::uint8_t(header.seq_profile),
            std::uint8_t(header.seq_level_idx_0),
            header.seq_tier_0 ? "high" : "main",
            header.high_bitdepth ? (header.twelve_bit ? 12 : 10) : 8,
            header.monochrome ? "yes" : "no",
            header.chroma_subsampling_x ? "1" : "0",
            header.chroma_subsampling_y ? "1" : "0"
        );
        return std::formatter<std::string_view>::format(output, ctx);
    }
};

template <>
struct std::formatter<mbmff::pixi_box> : std::formatter<std::string_view> {
    auto format(const mbmff::pixi_box& box, std::format_context& ctx) const
    {
        auto bits = box.bits_per_channel();
        std::string output = std::format("PIXI [{} bytes]: bits_per_channel=[", box.box_header.size);
        for (std::size_t i = 0; i < bits.size(); ++i) {
            if (i != 0) {
                output.append(", ");
            }
            output.append(std::format("{}", bits[i]));
        }
        return std::formatter<std::string_view>::format(output.append("]"), ctx);
    }
};

template <>
struct std::formatter<mbmff::ipma_association> : std::formatter<std::string_view> {
    auto format(const mbmff::ipma_association& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format("ipco_property={} essential={}", box.property_index, box.essential ? "yes" : "no"),
            ctx
        );
    }
};

template <>
struct std::formatter<mbmff::ipma_entry> : std::formatter<std::string_view> {
    auto format(const mbmff::ipma_entry& entry, std::format_context& ctx) const
    {
        std::string output = std::format("IPMA Entry: item_id={} associations=[", entry.item_id);
        for (std::size_t i = 0; i < entry.size(); ++i) {
            if (i != 0) {
                output.append(", ");
            }
            output += std::format("{{ {} }}", entry[i]);
        }
        return std::formatter<std::string_view>::format(output.append("]"), ctx);
    }
};

template <>
struct std::formatter<mbmff::pasp_box> : std::formatter<std::string_view> {
    auto format(const mbmff::pasp_box& box, std::format_context& ctx) const
    {
        auto [x, y] = box.aspect_ratio();
        return std::formatter<std::string_view>::format(
            std::format("PASP [{} bytes]: aspect_ratio=({}x{})", box.box_header.size, x, y),
            ctx
        );
    }
};
