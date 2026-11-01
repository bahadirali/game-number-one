#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "match.h"

namespace game {

// Builds a match from the contents of a level file. The first line gives the
// number of moves in a round, the rest is the board as in parse_match:
//
//     moves 50
//     #######
//     #1.$.2#
//     #######
//
// Windows line endings and blank lines at the end are fine. Returns nothing
// if the header is missing or malformed, or if parse_match rejects the board.
std::optional<Match> parse_match_file(std::string_view contents);

// Reads the level file at `path` (UTF-8, as engine::base_path() returns it)
// and parses it with parse_match_file. Returns nothing if the file can't be
// read or isn't a valid level.
std::optional<Match> load_match_file(const std::string& path);

// Reads the list of levels to play in order: one level file name per line.
// Blank lines are skipped and spaces around a name are dropped, so the list
// is easy to edit by hand.
std::vector<std::string> parse_level_list(std::string_view contents);

// Reads the level list at `path` (UTF-8) with parse_level_list. Returns
// nothing if the file can't be read; an empty list is up to the caller.
std::optional<std::vector<std::string>> load_level_list(const std::string& path);

} // namespace game
