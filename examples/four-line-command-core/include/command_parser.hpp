// Independently written example code for the public case study.
// It is not derived from, and does not implement, any production protocol.
#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace flc {

enum class CommandKind {
    SetLine,
    ClearLine,
    ScrollLine,
    Unknown,
};

// A validated command. Fields that do not apply to the kind are left at their
// default values, so an invalid combination cannot be represented.
struct Command {
    CommandKind kind = CommandKind::Unknown;
    std::size_t line = 0;  // zero-based index into the four lines
    std::string text;      // SetLine only
    int speed = 0;         // ScrollLine only, steps per refresh tick
};

struct ParseResult {
    bool ok = false;
    Command command{};
    std::string error;  // stable, human-readable reason when ok == false
};

// Parses one command from a small, deliberately limited JSON subset: a single
// flat object whose values are strings, integers, or booleans. Nested objects,
// arrays, floats, null, duplicate keys, and trailing bytes are rejected rather
// than guessed at, because a command boundary is not a place to be lenient.
ParseResult parse_command(const std::string& payload);

// Number of lines the command vocabulary can address, the maximum text length
// accepted for a single line, and the inclusive scroll speed/offset range.
inline constexpr std::size_t kAddressableLines = 4;
inline constexpr std::size_t kMaxTextLength = 32;
inline constexpr int kMinScrollOffset = -64;
inline constexpr int kMaxScrollOffset = 64;

}  // namespace flc
