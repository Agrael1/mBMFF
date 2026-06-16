#pragma once
#include <mbmff/mbmff.hpp>
#include <format>
#include <string>

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
// meta
template <>
struct std::formatter<mbmff::meta_box> : std::formatter<std::string_view> {
    auto format(const mbmff::meta_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(std::format(" size=\"{}\"", box.size_), ctx);
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
// url
template <>
struct std::formatter<mbmff::url_box> : std::formatter<std::string_view> {
    auto format(const mbmff::url_box& box, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(
            std::format(" size=\"{}\" version=\"{}\"", box.size_, box.version()),
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
// stts
template <>
struct std::formatter<mbmff::stts_box> : std::formatter<std::string_view> {
    auto format(const mbmff::stts_box& box, std::format_context& ctx) const
    {
        auto d = box.value();
        std::string
            out = std::format(" size=\"{}\" version=\"{}\" entry_count=\"{}\"", box.size_, box.version(), d.size());
        for (std::uint32_t i = 0; i < d.size(); ++i) {
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
// Display fourcc as string if printable, otherwise as hex
static auto display_fourcc(const mbmff::fourcc_string& fc) -> std::string
{
    std::string out;
    out.reserve(12);
    bool all_printable = true;
    for (std::size_t i = 0; i < 4; ++i) {
        auto c = fc[i];
        if (static_cast<unsigned char>(c) < 0x20 || static_cast<unsigned char>(c) > 0x7e) {
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
    case mbmff::box_type::ftyp:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::ftyp>(box));
        break;
    case mbmff::box_type::meta:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::meta>(box));
        break;
    case mbmff::box_type::mdat:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::mdat>(box));
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
    case mbmff::box_type::av1C:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::av1C>(box));
        break;
    case mbmff::box_type::mvhd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::mvhd>(box));
        break;
    case mbmff::box_type::tkhd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::tkhd>(box));
        break;
    case mbmff::box_type::elst:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::elst>(box));
        break;
    case mbmff::box_type::mdhd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::mdhd>(box));
        break;
    case mbmff::box_type::stsd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::stsd>(box));
        break;
    case mbmff::box_type::stts:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::stts>(box));
        break;
    case mbmff::box_type::smhd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::smhd>(box));
        break;
    case mbmff::box_type::vmhd:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::vmhd>(box));
        break;
    case mbmff::box_type::dref:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::dref>(box));
        break;
    case mbmff::box_type::url:
        std::cout << std::format("{}", mbmff::box_cast<mbmff::box_type::url>(box));
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
