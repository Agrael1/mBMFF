#pragma once
#include <concepts>
#include <cstddef>
#include <expected>

namespace mbmff {
enum class error_code {
    success = 0,
    invalid_format,
    need_more_data,
};

//------------------------------------------------------------------------------------------------------------
static constexpr auto get_error_message(error_code code) noexcept -> std::string_view
{
    switch (code) {
    case error_code::success:
        return "Success";
    case error_code::invalid_format:
        return "Invalid format";
    case error_code::need_more_data:
        return "Need more data";
    default:
        return "Unknown error code";
    }
}
//------------------------------------------------------------------------------------------------------------
template <typename Type>
struct parsed {
    Type value;
    std::size_t consumed;
};
struct unexpected {
    mbmff::error_code code;
    std::size_t needed;
};

//------------------------------------------------------------------------------------------------------------
struct bit_reader {
    std::span<const std::byte> data{};
    std::size_t bit_offset = 0;

public:
    template <std::unsigned_integral T = std::size_t>
    constexpr auto read_bits(std::size_t num_bits) noexcept -> T
    {
        T value = 0;
        while (num_bits > 0) {
            std::size_t byte_index = bit_offset / 8;
            std::size_t bit_index = bit_offset % 8;

            std::size_t bits_avail = 8 - bit_index;
            std::size_t bits_to_read = std::min(num_bits, bits_avail);

            std::uint8_t byte_val = static_cast<std::uint8_t>(data[byte_index]);

            std::size_t shift = bits_avail - bits_to_read;
            std::size_t mask = (1ULL << bits_to_read) - 1;

            value <<= bits_to_read;
            value |= static_cast<T>((byte_val >> shift) & mask);

            bit_offset += bits_to_read;
            num_bits -= bits_to_read;
        }
        return value;
    }

    constexpr auto uvlc() noexcept -> std::uint32_t
    {
        std::size_t leading_zero_bits = 0;
        while (!read_bits(1)) {
            if (++leading_zero_bits >= 32) {
                return (1ull << 32) - 1;
            }
        }
        std::size_t value = read_bits(leading_zero_bits);
        return value + ((1u << leading_zero_bits) - 1);
    }
};
} // namespace mbmff
