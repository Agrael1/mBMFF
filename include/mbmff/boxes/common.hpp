#pragma once
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>

namespace mbmff {
enum class error_code {
    success = 0,
    invalid_format,
    need_more_data,
    truncated,
};

//------------------------------------------------------------------------------------------------------------
static constexpr auto to_string(mbmff::error_code code) noexcept -> std::string_view
{
    switch (code) {
    case mbmff::error_code::success:
        return "Success";
    case mbmff::error_code::invalid_format:
        return "Invalid format";
    case mbmff::error_code::need_more_data:
        return "Need more data";
    case mbmff::error_code::truncated:
        return "Truncated";
    default:
        return "Unknown error code";
    }
}
//------------------------------------------------------------------------------------------------------------
template <typename Type>
struct parsed {
    Type value;
    std::size_t consumed = 0;
};

template <typename Type>
struct result {
    Type value{};
    mbmff::error_code code = mbmff::error_code::success;
    std::size_t needed = 0;

public:
    constexpr explicit operator bool() const noexcept
    {
        return code == mbmff::error_code::success;
    }
    constexpr auto operator->() noexcept -> Type*
    {
        return &value;
    }
    constexpr auto operator*() noexcept -> Type&
    {
        return value;
    }
    constexpr auto operator*() const noexcept -> const Type&
    {
        return value;
    }
};

// Error result — Type must be specified explicitly (e.g. make_result<any_box_view>(code, n))
template <typename Type>
constexpr auto make_result(mbmff::error_code code, std::size_t needed = 0) noexcept -> mbmff::result<Type>
{
    return {{}, code, needed};
}

template <typename Type>
    requires(!std::same_as<Type, mbmff::error_code>)
constexpr auto make_result(Type value) noexcept -> mbmff::result<Type>
{
    return {value};
}

template <typename Type>
constexpr auto make_result(Type value, mbmff::error_code code) noexcept -> mbmff::result<Type>
{
    return {value, code};
}

//------------------------------------------------------------------------------------------------------------

constexpr auto fourcc(const char* str) noexcept -> std::uint32_t
{
    return (static_cast<uint32_t>(static_cast<unsigned char>(str[3])) << 24)
         | (static_cast<uint32_t>(static_cast<unsigned char>(str[2])) << 16)
         | (static_cast<uint32_t>(static_cast<unsigned char>(str[1])) << 8)
         | static_cast<uint32_t>(static_cast<unsigned char>(str[0]));
}

struct fourcc_string {
    std::array<char, 4> data_{};

public:
    constexpr auto view() const noexcept -> std::string_view
    {
        auto len = std::size_t{0};
        while (len < 4 && data_[len] != 0) {
            ++len;
        }
        return std::string_view(data_.data(), len);
    }
    constexpr auto data() const noexcept -> const std::span<const char, 4>
    {
        return data_;
    }
    constexpr auto data() noexcept -> std::span<char, 4>
    {
        return data_;
    }
    constexpr auto to_uint32() const noexcept -> std::uint32_t
    {
        return mbmff::fourcc(data_.data());
    }

    constexpr auto operator==(const mbmff::fourcc_string& other) const noexcept -> bool
    {
        return data_ == other.data_;
    }
    constexpr auto operator[](std::size_t index) const noexcept -> char
    {
        return data_[index];
    }
    constexpr operator std::string_view() const noexcept
    {
        return view();
    }

public:
    constexpr static auto from_data(std::span<const std::byte> data) noexcept -> mbmff::fourcc_string
    {
        return mbmff::fourcc_string{std::array<char, 4>{
            static_cast<char>(data[0]),
            static_cast<char>(data[1]),
            static_cast<char>(data[2]),
            static_cast<char>(data[3]),
        }};
    }
    constexpr static auto from_uint32(std::uint32_t value) noexcept -> mbmff::fourcc_string
    {
        return mbmff::fourcc_string{std::array<char, 4>{
            static_cast<char>(value & 0xFF),
            static_cast<char>((value >> 8) & 0xFF),
            static_cast<char>((value >> 16) & 0xFF),
            static_cast<char>((value >> 24) & 0xFF),
        }};
    }
    constexpr static auto from_language(std::uint16_t lang) noexcept -> mbmff::fourcc_string
    {
        return mbmff::fourcc_string{std::array<char, 4>{
            static_cast<char>('a' + ((lang >> 10) & 0x1F)),
            static_cast<char>('a' + ((lang >> 5) & 0x1F)),
            static_cast<char>('a' + ((lang >> 0) & 0x1F)),
            char{0},
        }};
    }
};

//------------------------------------------------------------------------------------------------------------
template <class Type>
[[nodiscard]] constexpr auto to_underlying(Type value) noexcept -> std::underlying_type_t<Type>
{
    return static_cast<std::underlying_type_t<Type>>(value);
}

template <std::integral T>
constexpr auto byteswap(T value) noexcept -> T
{
    if constexpr (sizeof(T) == 1) {
        return value;
    } else if constexpr (sizeof(T) == 2) {
        return static_cast<T>(((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8));
    } else if constexpr (sizeof(T) == 4) {
        return static_cast<T>(
            ((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8)
            | ((value & 0xFF000000) >> 24)
        );
    } else if constexpr (sizeof(T) == 8) {
        return static_cast<T>(
            ((value & 0x00000000000000FFULL) << 56) | ((value & 0x000000000000FF00ULL) << 40)
            | ((value & 0x0000000000FF0000ULL) << 24) | ((value & 0x00000000FF000000ULL) << 8)
            | ((value & 0x000000FF00000000ULL) >> 8) | ((value & 0x0000FF0000000000ULL) >> 24)
            | ((value & 0x00FF000000000000ULL) >> 40) | ((value & 0xFF00000000000000ULL) >> 56)
        );
    }
}

template <std::integral T>
constexpr auto read_be(std::span<const std::byte> data) noexcept -> T
{
    T value{};
    if (std::is_constant_evaluated()) {
        for (std::size_t i = 0; i < sizeof(T); ++i) {
            value <<= 8;
            value |= static_cast<T>(data[i]);
        }
        return value;
    }
    std::memcpy(&value, data.data(), sizeof(value));
    return mbmff::byteswap(value);
}

//------------------------------------------------------------------------------------------------------------
struct parsed_cstr {
    std::string_view value{};
    std::size_t next = 0;
};

constexpr auto read_cstr(std::span<const std::byte> data, std::size_t offset) noexcept -> parsed_cstr
{
    if (offset >= data.size()) {
        return {};
    }

    const auto* begin = reinterpret_cast<const char*>(data.data() + offset);
    std::size_t length = 0;
    for (std::size_t i = offset; i < data.size(); ++i) {
        if (data[i] == std::byte{0}) {
            break;
        }
        ++length;
    }

    std::size_t next = offset + length;
    if (next < data.size() && data[next] == std::byte{0}) {
        ++next;
    }

    return {std::string_view(begin, length), next};
}

} // namespace mbmff

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
// result tests
static_assert(mbmff::make_result(42), "make_result(42) should be sucessful (result<int>)");
static_assert(!mbmff::make_result<int>(mbmff::error_code::invalid_format), "make_result<int>(code) should be an error");

// fourcc tests
static_assert(mbmff::fourcc("abcd") == 0x64636261, "fourcc(\"abcd\") should be 0x64636261");
static_assert(
    []() {
        constexpr auto str = mbmff::fourcc_string::from_uint32(0x64636261);
        constexpr auto d = std::array<char, 4>{'a', 'b', 'c', 'd'};
        return str[0] == d[0] && str[1] == d[1] && str[2] == d[2] && str[3] == d[3];
    }(),
    "fourcc_string::from_uint32(0x64636261) should return 'abcd'"
);

// byteswap tests
static_assert(mbmff::byteswap(std::uint16_t{0x1234}) == 0x3412, "byteswap(0x1234) should be 0x3412");
static_assert(mbmff::byteswap(std::uint32_t{0x12345678}) == 0x78563412, "byteswap(0x12345678) should be 0x78563412");
static_assert(
    mbmff::byteswap(std::uint64_t{0x0123456789ABCDEF}) == 0xEFCDAB8967452301,
    "byteswap(0x0123456789ABCDEF) should be 0xEFCDAB8967452301"
);
#endif
