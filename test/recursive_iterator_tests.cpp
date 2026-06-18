#include <mbmff/mbmff.hpp>
#include <array>
#include <cstdio>
#include <vector>

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
// Helpers: build raw ISOBMFF box bytes
// -----------------------------------------------------------------------

/// Append a 32-bit big-endian value.
static void append_u32be(std::vector<std::byte>& buf, std::uint32_t v)
{
    buf.push_back(static_cast<std::byte>(v >> 24));
    buf.push_back(static_cast<std::byte>(v >> 16));
    buf.push_back(static_cast<std::byte>(v >> 8));
    buf.push_back(static_cast<std::byte>(v));
}

/// Append a 4-byte fourcc.
static void append_fourcc(std::vector<std::byte>& buf, const char cc[4])
{
    buf.push_back(static_cast<std::byte>(cc[0]));
    buf.push_back(static_cast<std::byte>(cc[1]));
    buf.push_back(static_cast<std::byte>(cc[2]));
    buf.push_back(static_cast<std::byte>(cc[3]));
}

/// Build a single box (header + payload).
static std::vector<std::byte> box(const char fourcc[4], std::vector<std::byte> payload)
{
    std::vector<std::byte> b;
    append_u32be(b, 8 + static_cast<std::uint32_t>(payload.size()));
    append_fourcc(b, fourcc);
    b.insert(b.end(), payload.begin(), payload.end());
    return b;
}

/// Concatenate multiple byte vectors into one.
static std::vector<std::byte> join(std::vector<std::vector<std::byte>> parts)
{
    std::vector<std::byte> r;
    for (auto& p : parts) {
        r.insert(r.end(), p.begin(), p.end());
    }
    return r;
}

// -----------------------------------------------------------------------
// Helpers: run a recursive_box_iterator and check the type sequence
// -----------------------------------------------------------------------

static void check_seq(
    const char* name,
    std::span<const std::byte> data,
    std::initializer_list<mbmff::box_type> expected
)
{
    auto it = mbmff::recursive_box_iterator(data);
    auto end = it.end();
    for (auto exp : expected) {
        if (it == end) {
            TEST(name, false);
            return;
        }
        auto r = *it;
        if (!r) {
            std::fprintf(
                stderr,
                "  parse error at expected %s\n",
                mbmff::fourcc_string::from_uint32(mbmff::to_underlying(exp)).view().data()
            );
            TEST(name, false);
            return;
        }
        if (r->type_ != exp) {
            std::fprintf(
                stderr,
                "  expected %s, got %.*s\n",
                mbmff::fourcc_string::from_uint32(mbmff::to_underlying(exp)).view().data(),
                4,
                r->type_string().view().data()
            );
            TEST(name, false);
            return;
        }
        ++it;
    }
    TEST(name, it == end);
}

// -----------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------

static void test_empty()
{
    check_seq("empty", {}, {});
}

static void test_one_leaf()
{
    auto data = box("mdat", {std::byte{1}, std::byte{2}, std::byte{3}, std::byte{4}});
    check_seq("one_leaf", data, {mbmff::box_type::mdat});
}

static void test_two_leaves()
{
    auto data = join({box("mdat", {std::byte{1}}), box("mdat", {std::byte{2}})});
    check_seq("two_leaves", data, {mbmff::box_type::mdat, mbmff::box_type::mdat});
}

static void test_container_one_child()
{
    auto child = box("mdat", {std::byte{0}});
    auto data = box("moov", child);
    check_seq("container_one_child", data, {mbmff::box_type::moov, mbmff::box_type::mdat});
}

static void test_container_child_then_sibling()
{
    auto child = box("mdat", {std::byte{1}});
    auto container = box("moov", child);
    auto sibling = box("mdat", {std::byte{2}});
    auto data = join({container, sibling});
    check_seq(
        "container_child_then_sibling",
        data,
        {mbmff::box_type::moov, mbmff::box_type::mdat, mbmff::box_type::mdat}
    );
}

static void test_nested_containers()
{
    auto leaf = box("mdat", {std::byte{0}});
    auto inner = box("trak", leaf);
    auto data = box("moov", inner);
    check_seq("nested_containers", data, {mbmff::box_type::moov, mbmff::box_type::trak, mbmff::box_type::mdat});
}

static void test_container_two_children_then_sibling()
{
    auto c1 = box("mdat", {std::byte{1}});
    auto c2 = box("mdat", {std::byte{2}});
    auto container = box("moov", join({c1, c2}));
    auto sibling = box("mdat", {std::byte{3}});
    auto data = join({container, sibling});
    check_seq(
        "container_two_children_then_sibling",
        data,
        {mbmff::box_type::moov, mbmff::box_type::mdat, mbmff::box_type::mdat, mbmff::box_type::mdat}
    );
}

static void test_nested_with_siblings()
{
    auto leaf = box("mdat", {std::byte{0}});
    auto inner = box("trak", leaf);
    auto mid_child = box("mdat", {std::byte{1}});
    auto mid = box("moov", join({inner, mid_child}));
    auto top_sibling = box("mdat", {std::byte{2}});
    auto data = join({mid, top_sibling});
    check_seq(
        "nested_with_siblings",
        data,
        {mbmff::box_type::moov,
         mbmff::box_type::trak,
         mbmff::box_type::mdat,
         mbmff::box_type::mdat,
         mbmff::box_type::mdat}
    );
}

// -----------------------------------------------------------------------
int main()
{
    test_empty();
    test_one_leaf();
    test_two_leaves();
    test_container_one_child();
    test_container_child_then_sibling();
    test_nested_containers();
    test_container_two_children_then_sibling();
    test_nested_with_siblings();

    if (failed) {
        std::fprintf(stderr, "\n%d test(s) FAILED\n", failed);
        return 1;
    }
    std::fprintf(stdout, "\nAll tests passed.\n");
    return 0;
}
