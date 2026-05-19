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
    metadata = 5, // e.g. color config
    frame = 6,
    redundant_frame_header = 7,
    tile_list = 8,
    // 9-14 are reserved
    padding = 15, // special type used for padding OBUs that may be present in the stream. These should be ignored by
                  // decoders.
};

constexpr static auto obu_type_to_string(obu_type type) noexcept -> std::string_view
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


struct obu_view {
    constexpr static std::uint64_t max_obu_size = (1ULL << 32) - 1; // 4GB - 1 byte

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
};

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

constexpr auto parse_obu(std::span<const std::byte> data) noexcept -> std::expected<obu_view, unexpected>
{
    obu_view header{};
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
        if (obu_size > obu_view::max_obu_size) {
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
    return std::expected<obu_view, unexpected>{header};
}

//------------------------------------------------------------------------------------------------------------
// iterator

struct obu_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::expected<obu_view, unexpected>;
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
    constexpr auto try_get() const noexcept -> std::expected<obu_view, unexpected>
    {
        return parse_obu(data);
    }
    constexpr auto operator*() const noexcept -> std::expected<obu_view, unexpected>
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

