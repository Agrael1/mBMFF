#include <mbmff/mbmff.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include "formatters.hpp"

constexpr std::size_t page_size = 4096;

static void walk_box(const mbmff::any_box_view& box, std::size_t depth);
static void walk_children(std::span<const std::byte> data, std::size_t depth)
{
    for (const auto& result : mbmff::partial_box_iterator(data)) {
        if (!result) {
            if (result.code == mbmff::error_code::need_more_data) {
                return;
            }
            if (result.code == mbmff::error_code::truncated) {
                print_box(*result, depth);
                return;
            }
            return;
        }
        walk_box(*result, depth);
    }
}

static void walk_box(const mbmff::any_box_view& box, std::size_t depth)
{
    print_box(box, depth);
    if (mbmff::is_container(box) && !box.payload.empty()) {
        walk_children(box.payload, depth + 1);
    }
}

// ---------------------------------------------------------------------------
// Growing reader — buffer grows by appending pages; existing spans stay valid.
// Tracks the absolute file position of buf_[0].
// ---------------------------------------------------------------------------
struct growing_reader {
    std::ifstream file_;
    std::vector<std::byte> buf_;
    std::size_t file_offset_ = 0; // absolute file position of buf_[0]
public:
    explicit growing_reader(std::ifstream&& file)
        : file_(std::move(file))
    {
        reset();
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
        reset();
    }

    std::span<const std::byte> span() const
    {
        return std::span(buf_);
    }

    bool is_empty() const
    {
        return buf_.empty();
    }

    void reset()
    {
        file_offset_ = static_cast<std::size_t>(file_.tellg());
        buf_.resize(page_size);
        file_.read(reinterpret_cast<char*>(buf_.data()), page_size);
        buf_.resize(static_cast<std::size_t>(file_.gcount()));
    }

public:
    int walk_file()
    {
        std::size_t offset = 0;

        while (true) {
            if (!ensure(offset + 8)) {
                break;
            }

            auto data = span().subspan(offset);
            auto result = mbmff::parse<mbmff::iterator_flags::allow_partial>(data);

            if (!result) {
                if (result.code == mbmff::error_code::need_more_data) {
                    ensure(offset + result.needed);
                    continue;
                }
                if (result.code == mbmff::error_code::truncated) {
                    auto& box = *result;

                    if (box.type_ == mbmff::box_type::mdat) {
                        // Skip mdat content: seek file past it
                        const auto abs_file_pos = file_offset_ + offset;
                        seek_to(abs_file_pos + static_cast<std::size_t>(box.size_));
                        offset = 0;
                        continue;
                    }

                    // Non-md: load the full box so the next parse succeeds
                    if (box.size_ != 0) {
                        if (!ensure(offset + static_cast<std::size_t>(box.size_))) {
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
        return 0;
    }
};

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

    growing_reader reader(std::move(file));
    if (reader.is_empty()) {
        return 0;
    }

    return reader.walk_file();
}
