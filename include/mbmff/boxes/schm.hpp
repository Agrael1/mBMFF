#pragma once
#include "box_view.hpp"

namespace mbmff {

struct schm_data {
    std::uint32_t scheme_type;
    std::uint32_t scheme_version;
    mbmff::byte_view scheme_uri{};
};

template <>
struct basic_box_view<mbmff::box_type::schm> : public mbmff::box_view_base {
    constexpr static mbmff::box_properties properties = mbmff::box_properties::full_box;
    constexpr static auto validate(mbmff::any_box_view box) noexcept -> mbmff::result<mbmff::any_box_view>;
    constexpr auto value() const noexcept -> mbmff::schm_data;
};

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::schm>::validate(mbmff::any_box_view box) noexcept
    -> mbmff::result<mbmff::any_box_view>
{
    if (box.payload.size() < 4) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 4);
    }
    box.fill_full_header(box.payload);
    auto data = box.payload.subspan(4);
    if (data.size() < 8) {
        return mbmff::make_result<mbmff::any_box_view>(mbmff::error_code::need_more_data, 8);
    }
    return {box};
}

inline constexpr auto mbmff::basic_box_view<mbmff::box_type::schm>::value() const noexcept -> mbmff::schm_data
{
    if (payload.empty() && payload.data() == nullptr) {
        return {};
    }
    auto data = payload.subspan(4);
    auto scheme_type = mbmff::read_be<std::uint32_t>(data);
    auto scheme_version = mbmff::read_be<std::uint32_t>(data.subspan(4));
    mbmff::byte_view scheme_uri{};
    if (flags() & 0x000001) {
        scheme_uri = mbmff::byte_view::from_c_str(payload, 12);
    }
    return {scheme_type, scheme_version, scheme_uri};
}

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    !mbmff::basic_box_view<mbmff::box_type::schm>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);

static_assert(static_cast<bool>(mbmff::basic_box_view<mbmff::box_type::schm>::validate(
    {mbmff::box_header{}, std::span<const std::byte>(std::array<std::byte, 12>{}.data(), 12)}
)));

// schm without scheme_uri (flag not set)
static_assert([] {
    constexpr std::array<std::byte, 12> data{
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x01},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x00},
        std::byte{0x02},
    };
    mbmff::basic_box_view<mbmff::box_type::schm> schm_box;
    schm_box.version_ = 0;
    schm_box.payload = std::span(data);
    auto v = schm_box.value();
    return v.scheme_type == 1 && v.scheme_version == 2 && v.scheme_uri.empty();
}());

// schm with scheme_uri (flag 0x000001 set)
static_assert([] {
    constexpr std::array<std::byte, 19> data{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x01}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x02}, std::byte{'m'},  std::byte{'y'},  std::byte{'_'},
        std::byte{'u'},  std::byte{'r'},  std::byte{'i'},  std::byte{0x00},
    };
    mbmff::basic_box_view<mbmff::box_type::schm> schm_box;
    schm_box.version_ = 0;
    schm_box.flags_[2] = std::uint8_t{1};
    schm_box.payload = std::span(data);
    auto v = schm_box.value();
    return v.scheme_type == 1 && v.scheme_version == 2 && v.scheme_uri == "my_uri";
}());

#endif

} // namespace mbmff
