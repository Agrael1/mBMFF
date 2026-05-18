#pragma once
#include <cstddef>

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

} // namespace mbmff
