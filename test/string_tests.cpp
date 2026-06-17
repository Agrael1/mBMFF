#include <mbmff/mbmff.hpp>
#include <cstdio>
#include <string_view>
#include <array>

// -----------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------
static int failed = 0;

static void check(bool cond, const char* name, const char* file, int line)
{
    if (!cond) {
        std::fprintf(stderr, "FAIL: %s (%s:%d)\n", name, file, line);
        ++failed;
    } else {
        std::fprintf(stdout, "ok:  %s\n", name);
    }
}

#define TEST(name, expr) check(expr, name, __FILE__, __LINE__)

// -----------------------------------------------------------------------
// URL box tests
// -----------------------------------------------------------------------
static void test_url()
{
    // URL with location string
    {
        constexpr std::array<std::byte, 14> data{
            std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
            std::byte{'t'},  std::byte{'e'},  std::byte{'s'},  std::byte{'t'},
            std::byte{'_'},  std::byte{'u'},  std::byte{'r'},  std::byte{'l'},
            std::byte{0x00},
        };
        mbmff::basic_box_view<mbmff::box_type::url> box;
        box.version_ = 0;
        box.payload = std::span(data).subspan(4);
        auto v = box.value();
        TEST("url location", v.location == "test_url");
    }

    // URL with flag 0x000001 — no location
    {
        mbmff::basic_box_view<mbmff::box_type::url> box;
        box.version_ = 0;
        box.flags_[2] = std::uint8_t{1};
        box.payload = {};
        auto v = box.value();
        TEST("url flag_no_location", v.location.empty());
    }

    // URL empty payload (no flag)
    {
        mbmff::basic_box_view<mbmff::box_type::url> box;
        box.version_ = 0;
        box.payload = {};
        auto v = box.value();
        TEST("url empty", v.location.empty());
    }
}

// -----------------------------------------------------------------------
// URN box tests
// -----------------------------------------------------------------------
static void test_urn()
{
    // URN with name and location
    {
        constexpr std::array<std::byte, 16> data{
            std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
            std::byte{'i'},  std::byte{'s'},  std::byte{'o'},  std::byte{0x00},
            std::byte{'h'},  std::byte{'t'},  std::byte{'t'},  std::byte{'p'},
            std::byte{':'},  std::byte{'/'},  std::byte{'/'},  std::byte{0x00},
        };
        mbmff::basic_box_view<mbmff::box_type::urn> box;
        box.version_ = 0;
        box.payload = std::span(data).subspan(4);
        auto v = box.value();
        TEST("urn name_location", v.name == "iso" && v.location == "http://");
    }

    // URN with name only
    {
        constexpr std::array<std::byte, 9> data{
            std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
            std::byte{'i'},  std::byte{'s'},  std::byte{'o'},  std::byte{0x00},
            std::byte{0x00},
        };
        mbmff::basic_box_view<mbmff::box_type::urn> box;
        box.version_ = 0;
        box.payload = std::span(data).subspan(4);
        auto v = box.value();
        TEST("urn name_only", v.name == "iso" && v.location.empty());
    }

    // URN empty payload
    {
        mbmff::basic_box_view<mbmff::box_type::urn> box;
        box.version_ = 0;
        box.payload = {};
        auto v = box.value();
        TEST("urn empty", v.name.empty() && v.location.empty());
    }
}

// -----------------------------------------------------------------------
// SCHM box tests
// -----------------------------------------------------------------------
static void test_schm()
{
    // SCHM without scheme_uri
    {
        constexpr std::array<std::byte, 12> data{
            std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
            std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01},
            std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x02},
        };
        mbmff::basic_box_view<mbmff::box_type::schm> box;
        box.version_ = 0;
        box.payload = std::span(data);
        auto v = box.value();
        TEST("schm no_uri", v.scheme_type == 1 && v.scheme_version == 2 && v.scheme_uri.empty());
    }

    // SCHM with scheme_uri
    {
        constexpr std::array<std::byte, 19> data{
            std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
            std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01},
            std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x02},
            std::byte{'m'},  std::byte{'y'},  std::byte{'_'},  std::byte{'u'},
            std::byte{'r'},  std::byte{'i'},  std::byte{0x00},
        };
        mbmff::basic_box_view<mbmff::box_type::schm> box;
        box.version_ = 0;
        box.flags_[2] = std::uint8_t{1};
        box.payload = std::span(data);
        auto v = box.value();
        TEST("schm with_uri", v.scheme_type == 1 && v.scheme_version == 2 && v.scheme_uri == "my_uri");
    }
}

// -----------------------------------------------------------------------
// CDSC box tests
// -----------------------------------------------------------------------
static void test_cdsc()
{
    // CDSC with data
    {
        constexpr std::array<std::byte, 8> data{
            std::byte{'h'}, std::byte{'e'}, std::byte{'l'}, std::byte{'l'},
            std::byte{'o'}, std::byte{0x00}, std::byte{'x'}, std::byte{'y'},
        };
        mbmff::basic_box_view<mbmff::box_type::cdsc> box;
        box.payload = std::span(data);
        auto v = box.value();
        TEST("cdsc value", v.value.size() == 8 && v.value[0] == 'h' && v.value[5] == '\0');
    }

    // CDSC empty payload
    {
        mbmff::basic_box_view<mbmff::box_type::cdsc> box;
        box.payload = {};
        auto v = box.value();
        TEST("cdsc empty", v.value.empty());
    }
}

// -----------------------------------------------------------------------
int main()
{
    test_url();
    test_urn();
    test_schm();
    test_cdsc();

    if (failed) {
        std::fprintf(stderr, "\n%d test(s) FAILED\n", failed);
        return 1;
    }
    std::fprintf(stdout, "\nAll tests passed.\n");
    return 0;
}
