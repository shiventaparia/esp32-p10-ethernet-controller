// Independently written example code for the public case study.
// It is not derived from, and does not implement, any production protocol.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>

namespace flc {

inline constexpr std::size_t kLineCount = 4;

struct LineState {
    std::string text;
    int scroll_offset = 0;
    // Bumped only for the line that actually changed. This is what lets a test
    // assert the property the case study claims: updating one line leaves the
    // other three untouched.
    std::uint32_t revision = 0;
};

// Four individually addressable lines.
class DisplayState {
public:
    // Returns false when the index is out of range or the text is too long,
    // leaving the state unchanged.
    bool set_line(std::size_t index, const std::string& text);
    bool clear_line(std::size_t index);
    // Scroll offsets must be in the inclusive range -64..64. Invalid indices
    // and offsets leave the state unchanged.
    bool set_scroll(std::size_t index, int offset);

    LineState line(std::size_t index) const;
    std::array<LineState, kLineCount> snapshot() const;

private:
    std::array<LineState, kLineCount> lines_{};
};

// The same state behind a mutex, mirroring the shared-state boundary described
// in docs/concurrency.md: one side writes, the other reads a consistent
// snapshot. A reader never observes a half-applied update.
class SharedDisplay {
public:
    bool set_line(std::size_t index, const std::string& text);
    bool clear_line(std::size_t index);
    bool set_scroll(std::size_t index, int offset);

    std::array<LineState, kLineCount> snapshot() const;

private:
    mutable std::mutex mutex_;
    DisplayState state_;
};

}  // namespace flc
