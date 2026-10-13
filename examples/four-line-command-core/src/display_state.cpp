// Independently written example code for the public case study.
#include "display_state.hpp"

#include "command_parser.hpp"  // shared text and scroll limits

namespace flc {

bool DisplayState::set_line(std::size_t index, const std::string& text) {
    if (index >= kLineCount || text.size() > kMaxTextLength) {
        return false;
    }
    LineState& target = lines_[index];
    target.text = text;
    target.scroll_offset = 0;
    ++target.revision;
    return true;
}

bool DisplayState::clear_line(std::size_t index) {
    if (index >= kLineCount) {
        return false;
    }
    LineState& target = lines_[index];
    // Clearing an already-empty line is a no-op rather than an event, so a
    // repeated clear does not keep bumping the revision.
    if (target.text.empty() && target.scroll_offset == 0) {
        return true;
    }
    target.text.clear();
    target.scroll_offset = 0;
    ++target.revision;
    return true;
}

bool DisplayState::set_scroll(std::size_t index, int offset) {
    if (index >= kLineCount || offset < kMinScrollOffset || offset > kMaxScrollOffset) {
        return false;
    }
    LineState& target = lines_[index];
    if (target.scroll_offset == offset) {
        return true;
    }
    target.scroll_offset = offset;
    ++target.revision;
    return true;
}

LineState DisplayState::line(std::size_t index) const {
    if (index >= kLineCount) {
        return LineState{};
    }
    return lines_[index];
}

std::array<LineState, kLineCount> DisplayState::snapshot() const {
    return lines_;
}

bool SharedDisplay::set_line(std::size_t index, const std::string& text) {
    const std::lock_guard<std::mutex> guard(mutex_);
    return state_.set_line(index, text);
}

bool SharedDisplay::clear_line(std::size_t index) {
    const std::lock_guard<std::mutex> guard(mutex_);
    return state_.clear_line(index);
}

bool SharedDisplay::set_scroll(std::size_t index, int offset) {
    const std::lock_guard<std::mutex> guard(mutex_);
    return state_.set_scroll(index, offset);
}

std::array<LineState, kLineCount> SharedDisplay::snapshot() const {
    const std::lock_guard<std::mutex> guard(mutex_);
    return state_.snapshot();
}

}  // namespace flc
