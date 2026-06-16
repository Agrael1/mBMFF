#pragma once
#include "box_view.hpp"

namespace mbmff {

struct esds_data {
    std::uint8_t object_type = 0;
    std::uint8_t stream_type = 0;
    std::uint32_t buffer_size = 0;
    std::uint32_t max_bitrate = 0;
    std::uint32_t avg_bitrate = 0;
    std::span<const std::byte> audio_specific_config{};
};

template <>
struct mbmff::basic_box_view<mbmff::box_type::esds> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::esds_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::esds>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    box.payload = box.payload.subspan(4);
    return {box};
}

constexpr auto mbmff::basic_box_view<mbmff::box_type::esds>::value() const noexcept -> mbmff::esds_data
{
    esds_data result{};
    if (payload.size() < 2) {
        return result;
    }

    std::size_t pos = 0;

    if (pos >= payload.size() || static_cast<std::uint8_t>(payload[pos++]) != 0x03) {
        return result;
    }

    for (int i = 0; i < 4 && pos < payload.size(); ++i) {
        auto b = static_cast<std::uint8_t>(payload[pos++]);
        if (!(b & 0x80)) {
            break;
        }
    }

    if (pos + 3 > payload.size()) {
        return result;
    }
    pos += 2;
    auto flags = static_cast<std::uint8_t>(payload[pos++]);

    if (flags & 0x80) {
        pos += 2;
    }
    if (flags & 0x40) {
        if (pos >= payload.size()) {
            return result;
        }
        pos += 1 + static_cast<std::uint8_t>(payload[pos]);
    }
    if (flags & 0x20) {
        pos += 2;
    }

    while (pos + 1 < payload.size()) {
        auto tag = static_cast<std::uint8_t>(payload[pos++]);
        auto len_start = pos;
        std::uint32_t dcd_len = 0;
        for (int i = 0; i < 4 && pos < payload.size(); ++i) {
            auto b = static_cast<std::uint8_t>(payload[pos++]);
            dcd_len = (dcd_len << 7) | (b & 0x7F);
            if (!(b & 0x80)) {
                break;
            }
        }

        if (tag == 0x04) {
            if (pos + 13 > payload.size()) {
                return result;
            }
            result.object_type = static_cast<std::uint8_t>(payload[pos]);
            result.stream_type = (static_cast<std::uint8_t>(payload[pos + 1]) >> 2) & 0x3F;
            result.buffer_size = (static_cast<std::uint32_t>(static_cast<std::uint8_t>(payload[pos + 2])) << 16)
                               | (static_cast<std::uint8_t>(payload[pos + 3]) << 8)
                               | static_cast<std::uint8_t>(payload[pos + 4]);
            result.max_bitrate = mbmff::read_be<std::uint32_t>(payload.subspan(pos + 5));
            result.avg_bitrate = mbmff::read_be<std::uint32_t>(payload.subspan(pos + 9));

            {
                auto inner = pos + 13;
                auto inner_end = pos + dcd_len;
                while (inner + 1 < inner_end && inner < payload.size()) {
                    auto t = static_cast<std::uint8_t>(payload[inner]);
                    if (t > 0x3F) {
                        ++inner;
                        continue;
                    }
                    ++inner;
                    std::uint32_t inner_len = 0;
                    for (int i = 0; i < 4 && inner < payload.size(); ++i) {
                        auto b = static_cast<std::uint8_t>(payload[inner++]);
                        inner_len = (inner_len << 7) | (b & 0x7F);
                        if (!(b & 0x80)) {
                            break;
                        }
                    }
                    if (t == 0x05 && inner + inner_len <= payload.size()) {
                        result.audio_specific_config = payload.subspan(inner, inner_len);
                        break;
                    }
                    inner += inner_len;
                }
            }
            break;
        }
        pos = len_start + dcd_len;
    }

    return result;
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::esds>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::esds>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 4>{}.data(), 4)}
)));

static_assert([] {
    constexpr std::array<std::byte, 26> data{
        std::byte{0x03}, std::byte{0x18}, std::byte{0x00}, std::byte{0x01}, std::byte{0x00}, std::byte{0x04},
        std::byte{0x13}, std::byte{0x40}, std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x10},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x05}, std::byte{0x04}, std::byte{0x11}, std::byte{0x90},
        std::byte{0x20}, std::byte{0x12},
    };
    mbmff::basic_box_view<mbmff::box_type::esds> esds;
    esds.version_ = 0;
    esds.payload = std::span(data);
    auto v = esds.value();
    return v.object_type == 0x40 && v.stream_type == 5 && v.buffer_size == 0x10 && v.max_bitrate == 0
        && v.avg_bitrate == 0 && v.audio_specific_config.size() == 4;
}());
#endif

} // namespace mbmff
