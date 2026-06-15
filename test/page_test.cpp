#include <mbmff/mbmff.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace {
constexpr std::size_t page_size = 4096;

static constexpr bool is_box_implemented(mbmff::box_type type) noexcept
{
    switch (type) {
    case mbmff::box_type::ftyp:
    case mbmff::box_type::meta:
    case mbmff::box_type::mdat:
    case mbmff::box_type::av1C:
    case mbmff::box_type::moov:
    case mbmff::box_type::trak:
    case mbmff::box_type::mdia:
    case mbmff::box_type::minf:
    case mbmff::box_type::stbl:
    case mbmff::box_type::dinf:
    case mbmff::box_type::edts:
    case mbmff::box_type::udta:
    case mbmff::box_type::mvex:
    case mbmff::box_type::moof:
    case mbmff::box_type::traf:
    case mbmff::box_type::mfra:
    case mbmff::box_type::ipco:
    case mbmff::box_type::iprp:
    case mbmff::box_type::iinf:
    case mbmff::box_type::iref:
    case mbmff::box_type::iloc:
    case mbmff::box_type::hdlr:
    case mbmff::box_type::pitm:
    case mbmff::box_type::ispe:
    case mbmff::box_type::pixi:
    case mbmff::box_type::pasp:
    case mbmff::box_type::ipma:
    case mbmff::box_type::infe:
        return true;
    default:
        return false;
    }
}

static bool is_container(const mbmff::any_box_view& box) noexcept
{
    return mbmff::has(mbmff::get_box_properties(box.type()), mbmff::box_properties::container);
}

// ---------------------------------------------------------------------------
// Box tree printer
// ---------------------------------------------------------------------------
static void print_box_info(const mbmff::any_box_view& box, std::size_t depth)
{
    for (std::size_t i = 0; i < depth; ++i) {
        std::cout << "  ";
    }

    if (!is_box_implemented(box.type())) {
        std::cout << "\x1b[33m[!]\x1b[0m ";
    }

    std::cout << '<' << box.type_string().view();
    std::cout << " size=\"" << box.size() << '\"';
    std::cout << (is_container(box) ? ">" : " />") << '\n';
}

static void walk_children(std::span<const std::byte> data, std::size_t depth)
{
    for (const auto& result : mbmff::box_iterator(data, mbmff::iterator_flags::allow_partial)) {
        if (!result) {
            if (result.code == mbmff::error_code::need_more_data) {
                return;
            }
            if (result.code == mbmff::error_code::truncated) {
                print_box_info(*result, depth);
                return;
            }
            return;
        }
        auto& box = *result;
        print_box_info(box, depth);
        if (is_container(box) && !box.payload.empty()) {
            walk_children(box.payload, depth + 1);
        }
    }
}

static void walk_box(const mbmff::any_box_view& box, std::size_t depth)
{
    print_box_info(box, depth);
    if (is_container(box) && !box.payload.empty()) {
        walk_children(box.payload, depth + 1);
    }
}

// ---------------------------------------------------------------------------
// Growing reader — buffer grows by appending pages; existing spans stay valid.
// Tracks the absolute file position of buf_[0].
// ---------------------------------------------------------------------------
struct growing_reader {
    std::ifstream& file_;
    std::vector<std::byte> buf_;
    std::size_t file_offset_ = 0; // absolute file position of buf_[0]

    explicit growing_reader(std::ifstream& file)
        : file_(file)
    {}

    void init()
    {
        file_offset_ = static_cast<std::size_t>(file_.tellg());
        buf_.resize(page_size);
        file_.read(reinterpret_cast<char*>(buf_.data()), page_size);
        buf_.resize(static_cast<std::size_t>(file_.gcount()));
    }

    bool ensure(std::size_t needed)
    {
        while (buf_.size() < needed) {
            std::size_t old = buf_.size();
            buf_.resize(old + page_size);
            file_.read(reinterpret_cast<char*>(buf_.data() + old), page_size);
            buf_.resize(old + static_cast<std::size_t>(file_.gcount()));
            if (file_.gcount() == 0) {
                return false;
            }
        }
        return true;
    }

    void seek_to(std::size_t abs_pos)
    {
        file_.seekg(static_cast<std::streamoff>(abs_pos));
        file_offset_ = abs_pos;
        buf_.clear();
        init();
    }

    std::span<const std::byte> span() const
    {
        return std::span(buf_);
    }
};

// ---------------------------------------------------------------------------
// Top-level loop
// ---------------------------------------------------------------------------
static void walk_file(growing_reader& reader)
{
    std::size_t offset = 0;

    while (true) {
        if (!reader.ensure(offset + 8)) {
            break;
        }

        auto data = reader.span().subspan(offset);
        auto result = mbmff::parse(data, mbmff::iterator_flags::allow_partial);

        if (!result) {
            if (result.code == mbmff::error_code::need_more_data) {
                reader.ensure(offset + result.needed);
                continue;
            }
            if (result.code == mbmff::error_code::truncated) {
                auto& box = *result;

                if (box.type_ == mbmff::box_type::mdat) {
                    // Skip mdat content: seek file past it
                    const auto abs_file_pos = reader.file_offset_ + offset;
                    reader.seek_to(abs_file_pos + static_cast<std::size_t>(box.size_));
                    offset = 0;
                    continue;
                }

                // Non-md: load the full box so the next parse succeeds
                if (box.size_ != 0) {
                    if (!reader.ensure(offset + static_cast<std::size_t>(box.size_))) {
                        break;
                    }
                }
                continue;
            }
            break;
        }

        auto& box = *result;

        // mdat that was small enough to fit in the buffer
        if (box.type_ == mbmff::box_type::mdat) {
            if (box.size_ == 0) {
                break;
            }
            offset += static_cast<std::size_t>(box.size_);
            continue;
        }

        walk_box(box, 0);

        if (box.size_ == 0) {
            break;
        }
        offset += static_cast<std::size_t>(box.size_);
    }
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: mbmff-page-test <file>\n";
        return 1;
    }

    std::filesystem::path path{argv[1]};
    if (!std::filesystem::exists(path)) {
        std::cerr << "File does not exist: " << argv[1] << '\n';
        return 1;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open: " << argv[1] << '\n';
        return 1;
    }

    growing_reader reader(file);
    reader.init();
    if (reader.span().empty()) {
        return 0;
    }

    walk_file(reader);
    return 0;
}
