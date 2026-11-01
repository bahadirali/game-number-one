#include "level_file.h"

#include <charconv>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

namespace game {

namespace {

// Splits `text` into lines, dropping the '\r' of Windows line endings.
std::vector<std::string_view> split_lines(std::string_view text) {
    std::vector<std::string_view> lines;
    while (!text.empty()) {
        const auto end = text.find('\n');
        std::string_view line = text.substr(0, end);
        if (line.ends_with('\r')) {
            line.remove_suffix(1);
        }
        lines.push_back(line);
        if (end == std::string_view::npos) {
            break;
        }
        text.remove_prefix(end + 1);
    }
    return lines;
}

// Reads "moves <number>", with nothing else on the line.
std::optional<int> parse_moves_header(std::string_view line) {
    constexpr std::string_view kPrefix = "moves ";
    if (!line.starts_with(kPrefix)) {
        return std::nullopt;
    }
    line.remove_prefix(kPrefix.size());
    int moves = 0;
    const auto [end, error] = std::from_chars(line.data(), line.data() + line.size(), moves);
    if (error != std::errc{} || end != line.data() + line.size()) {
        return std::nullopt;
    }
    return moves;
}

// Reads a whole file. `path` is UTF-8, as engine::base_path() returns it.
std::optional<std::string> read_text_file(const std::string& path) {
    // A plain std::string path would be read in the Windows code page, so a
    // folder with non-ASCII letters in its name wouldn't be found. Going
    // through char8_t tells std::filesystem the bytes are UTF-8.
    // (std::filesystem::u8path does the same, but is deprecated in C++20.)
    const std::u8string utf8(reinterpret_cast<const char8_t*>(path.data()), path.size());

    // Binary mode reads the bytes as they are; the parsers deal with either
    // kind of line ending themselves.
    std::ifstream file(std::filesystem::path(utf8), std::ios::binary);
    if (!file) {
        return std::nullopt;
    }
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

} // namespace

std::optional<Match> parse_match_file(std::string_view contents) {
    std::vector<std::string_view> lines = split_lines(contents);
    while (!lines.empty() && lines.back().empty()) {
        lines.pop_back();
    }
    if (lines.size() < 2) {
        return std::nullopt;
    }

    const auto moves = parse_moves_header(lines.front());
    if (!moves) {
        return std::nullopt;
    }
    return parse_match(std::span(lines).subspan(1), *moves);
}

std::optional<Match> load_match_file(const std::string& path) {
    const auto contents = read_text_file(path);
    if (!contents) {
        return std::nullopt;
    }
    return parse_match_file(*contents);
}

std::vector<std::string> parse_level_list(std::string_view contents) {
    std::vector<std::string> names;
    for (std::string_view line : split_lines(contents)) {
        constexpr std::string_view kSpaces = " \t";
        const auto first = line.find_first_not_of(kSpaces);
        if (first == std::string_view::npos) {
            continue;
        }
        const auto last = line.find_last_not_of(kSpaces);
        names.emplace_back(line.substr(first, last - first + 1));
    }
    return names;
}

std::optional<std::vector<std::string>> load_level_list(const std::string& path) {
    const auto contents = read_text_file(path);
    if (!contents) {
        return std::nullopt;
    }
    return parse_level_list(*contents);
}

} // namespace game
