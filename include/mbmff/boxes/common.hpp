#pragma once
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <string_view>

namespace mbmff {
/// @brief enumeration describing the error condition
enum class error_code {
    /// @brief Operation succeeded.
    success = 0,

    /// @brief Format of the box is invalid.
    invalid_format,

    /// @brief The box is not loaded fully or at least the header is not fully readable.
    need_more_data,

    /// @brief Special condition, where the box is not loaded fully,
    /// but it is enough to continue recursive operation.
    truncated,
};

//------------------------------------------------------------------------------------------------------------
/// @brief Returns a human-readable string for an error code.
/// @param[in] code error code
/// @returns string representation of the error
inline constexpr auto to_string(mbmff::error_code code) noexcept -> std::string_view
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

/// @brief Holds the result of a parsing operation: the parsed value and the number of bytes consumed.
/// @tparam Type The type of the parsed value.
template <typename Type>
struct parsed {
    /// @brief The parsed value.
    Type value;

    /// @brief Number of bytes consumed from the input span to produce the value.
    std::size_t consumed = 0;
};

/// @brief Result type for operations that can succeed, fail, or partially succeed.
/// @tparam Type The type of the result value on success.
///
/// Check success via `operator bool()` or compare `code` against `error_code::success`.
template <typename Type>
struct result {
    /// @brief The result value. Valid only when `code == error_code::success`.
    Type value{};

    /// @brief Error code. `error_code::success` means the operation succeeded.
    mbmff::error_code code = mbmff::error_code::success;

    /// @brief For `error_code::need_more_data`: the minimum additional bytes required.
    std::size_t needed = 0;

public:
    /// @brief Returns `true` if the operation succeeded (`code == error_code::success`).
    constexpr explicit operator bool() const noexcept
    {
        return code == mbmff::error_code::success;
    }
    /// @brief Accesses the result value via pointer.
    constexpr auto operator->() noexcept -> Type*
    {
        return &value;
    }
    /// @brief Accesses the result value via reference.
    constexpr auto operator*() noexcept -> Type&
    {
        return value;
    }
    /// @brief Accesses the result value via const reference.
    constexpr auto operator*() const noexcept -> const Type&
    {
        return value;
    }
};

/// @brief Creates an error result. Type must be specified explicitly (e.g. `make_result<any_box_view>(code, n)`).
/// @tparam Type The result value type.
/// @param[in] code The error code.
/// @param[in] needed Minimum additional bytes required (for `need_more_data`).
/// @returns A result with the given error code.
template <typename Type>
constexpr auto make_result(mbmff::error_code code, std::size_t needed = 0) noexcept -> mbmff::result<Type>
{
    return {{}, code, needed};
}

/// @brief Creates a success result from a value.
/// @tparam Type The result value type (deduced from the argument).
/// @param[in] value The success value.
/// @returns A result with `error_code::success`.
template <typename Type>
    requires(!std::same_as<Type, mbmff::error_code>)
constexpr auto make_result(Type value) noexcept -> mbmff::result<Type>
{
    return {value};
}

/// @brief Creates a result with an explicit value and error code (for truncated results).
/// @tparam Type The result value type.
/// @param[in] value The result value.
/// @param[in] code The error code.
/// @returns A result with the given value and error code.
template <typename Type>
constexpr auto make_result(Type value, mbmff::error_code code) noexcept -> mbmff::result<Type>
{
    return {value, code};
}

//------------------------------------------------------------------------------------------------------------
/// @brief Converts a 4-character code to a uint32 fourcc value.
///
/// The input is read as little-endian bytes and stored in native uint32,
/// so `fourcc("abcd")` yields `0x64636261`. Round-trips through
/// `fourcc_string::from_uint32()`.
/// @param[in] str A 4-character C-string (only the first 4 chars are used).
/// @returns The fourcc as a uint32 value.
constexpr auto fourcc(const char* str) noexcept -> std::uint32_t
{
    return (static_cast<uint32_t>(static_cast<unsigned char>(str[3])) << 24)
         | (static_cast<uint32_t>(static_cast<unsigned char>(str[2])) << 16)
         | (static_cast<uint32_t>(static_cast<unsigned char>(str[1])) << 8)
         | static_cast<uint32_t>(static_cast<unsigned char>(str[0]));
}

/// @brief A 4-character code string, representing an ISOBMFF box type or similar identifier.
struct fourcc_string {
    /// @brief Raw 4-byte storage.
    std::array<char, 4> data_{};

public:
    /// @brief Returns the fourcc as a string view, trimmed at the first null byte.
    constexpr auto view() const noexcept -> std::string_view
    {
        auto len = std::size_t{0};
        while (len < 4 && data_[len] != 0) {
            ++len;
        }
        return std::string_view(data_.data(), len);
    }
    /// @brief Returns a const span over the 4 raw bytes.
    constexpr auto data() const noexcept -> const std::span<const char, 4>
    {
        return data_;
    }
    /// @brief Returns a mutable span over the 4 raw bytes.
    constexpr auto data() noexcept -> std::span<char, 4>
    {
        return data_;
    }
    /// @brief Converts the fourcc back to a uint32 value via `fourcc()`.
    constexpr auto to_uint32() const noexcept -> std::uint32_t
    {
        return mbmff::fourcc(data_.data());
    }

    /// @brief Equality comparison.
    constexpr auto operator==(const mbmff::fourcc_string& other) const noexcept -> bool
    {
        return data_ == other.data_;
    }
    /// @brief Accesses the i-th character of the fourcc.
    constexpr auto operator[](std::size_t index) const noexcept -> char
    {
        return data_[index];
    }
    /// @brief Implicit conversion to string view.
    constexpr operator std::string_view() const noexcept
    {
        return view();
    }

public:
    /// @brief Constructs a fourcc from the first 4 bytes of a byte span.
    /// @param[in] data Byte span (must have at least 4 bytes).
    /// @returns A fourcc_string.
    constexpr static auto from_data(std::span<const std::byte> data) noexcept -> mbmff::fourcc_string
    {
        return mbmff::fourcc_string{std::array<char, 4>{
            static_cast<char>(data[0]),
            static_cast<char>(data[1]),
            static_cast<char>(data[2]),
            static_cast<char>(data[3]),
        }};
    }
    /// @brief Constructs a fourcc from a uint32 (little-endian byte order).
    /// @param[in] value uint32 fourcc value.
    /// @returns A fourcc_string.
    constexpr static auto from_uint32(std::uint32_t value) noexcept -> mbmff::fourcc_string
    {
        return mbmff::fourcc_string{std::array<char, 4>{
            static_cast<char>(value & 0xFF),
            static_cast<char>((value >> 8) & 0xFF),
            static_cast<char>((value >> 16) & 0xFF),
            static_cast<char>((value >> 24) & 0xFF),
        }};
    }
    /// @brief Decodes an ISO-639-2 language code packed in 15 bits (per MP4 format) into a three-letter code + null.
    /// @param[in] lang 16-bit language code as stored in the MP4 format.
    /// @returns A fourcc_string with the decoded language (e.g. "eng", "jpn").
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
/// @brief Converts an enum value to its underlying integer type.
/// @tparam Type The enum type.
/// @param[in] value The enum value.
/// @returns The underlying integer value.
template <class Type>
[[nodiscard]] constexpr auto to_underlying(Type value) noexcept -> std::underlying_type_t<Type>
{
    return static_cast<std::underlying_type_t<Type>>(value);
}

/// @brief Reverses byte order of an integral value (supports 1, 2, 4, and 8 byte types).
/// @tparam T The integral type.
/// @param[in] value The value to byte-swap.
/// @returns The byte-swapped value.
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

/// @brief Reads an integral value from a byte span in big-endian order.
/// @tparam T The integral type to read (1, 2, 4, or 8 bytes).
/// @param[in] data Byte span (must have at least `sizeof(T)` bytes).
/// @returns The big-endian decoded value.
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
/// @brief A span of bytes with runtime-only string accessors.
///        Aggregate-initializable from a std::span<const std::byte>.
struct byte_view {
    /// @brief The underlying byte span (public for aggregate init).
    std::span<const std::byte> data_{};

    /// @brief Returns true if the span is empty.
    constexpr auto empty() const noexcept -> bool
    {
        return data_.empty();
    }
    /// @brief Returns the number of bytes.
    constexpr auto size() const noexcept -> std::size_t
    {
        return data_.size();
    }
    /// @brief Returns a pointer to the first byte.
    constexpr auto data() const noexcept -> const std::byte*
    {
        return data_.data();
    }
    /// @brief Accesses the i-th byte.
    constexpr auto operator[](std::size_t i) const noexcept -> const std::byte&
    {
        return data_[i];
    }
    /// @brief Returns a subspan.
    constexpr auto subspan(std::size_t offset, std::size_t count = std::dynamic_extent) const noexcept
        -> std::span<const std::byte>
    {
        return data_.subspan(offset, count);
    }
    /// @brief Constexpr byte-wise comparison with a string view.
    constexpr auto operator==(std::string_view sv) const noexcept -> bool
    {
        if (size() != sv.size()) {
            return false;
        }
        for (std::size_t i = 0; i < size(); ++i) {
            if (static_cast<char>(data_[i]) != sv[i]) {
                return false;
            }
        }
        return true;
    }
    /// @brief Extracts a C string from byte data starting at offset, bounded by the data size.
    static constexpr auto from_c_str(std::span<const std::byte> data, std::size_t offset) noexcept -> byte_view
    {
        if (offset >= data.size()) {
            return {};
        }
        std::size_t len = 0;
        while (offset + len < data.size() && data[offset + len] != std::byte{0}) {
            ++len;
        }
        return {data.subspan(offset, len)};
    }
    /// @brief Extracts the content portion of a Pascal string (length-prefixed, max 31 chars).
    static constexpr auto from_pascal_str(byte_view bv) noexcept -> byte_view
    {
        if (bv.empty()) {
            return {};
        }
        auto len = static_cast<std::size_t>(bv[0]);
        if (len > 31) {
            len = 31;
        }
        if (len + 1 > bv.size()) {
            len = bv.size() - 1;
        }
        return {bv.subspan(1, len)};
    }

    /// @brief Runtime-only: interprets the bytes as a string view.
    auto string_view() const noexcept -> std::string_view
    {
        return {reinterpret_cast<const char*>(data_.data()), data_.size()};
    }

    auto view() const noexcept -> std::string_view
    {
        return string_view();
    }
};

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

// byte_view tests
static_assert(mbmff::byte_view{}.empty(), "default byte_view should be empty");
static_assert(mbmff::byte_view{}.size() == 0, "default byte_view size should be 0");
static_assert(
    []() {
        constexpr auto bv = mbmff::byte_view{std::span<const std::byte>()};
        return bv.empty();
    }(),
    "byte_view from empty span should be empty"
);
static_assert(
    []() {
        constexpr std::array<std::byte, 3> arr{std::byte{'h'}, std::byte{'o'}, std::byte{'i'}};
        auto bv = mbmff::byte_view::from_c_str(arr, 0);
        return bv == "hoi";
    }(),
    "byte_view from span should have correct size and contents"
);

// byteswap tests
static_assert(mbmff::byteswap(std::uint16_t{0x1234}) == 0x3412, "byteswap(0x1234) should be 0x3412");
static_assert(mbmff::byteswap(std::uint32_t{0x12345678}) == 0x78563412, "byteswap(0x12345678) should be 0x78563412");
static_assert(
    mbmff::byteswap(std::uint64_t{0x0123456789ABCDEF}) == 0xEFCDAB8967452301,
    "byteswap(0x0123456789ABCDEF) should be 0xEFCDAB8967452301"
);
#endif
