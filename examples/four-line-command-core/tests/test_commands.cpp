// Independently written example code for the public case study.
//
// A dependency-free test runner: no framework, no network, no hardware. It
// exits non-zero if any check fails, so it works as a local gate or as an
// automated one without any extra wiring.
//
// Payloads use the R"json(...)json" delimiter so that quotes, braces and
// parentheses inside the JSON can never be mistaken for the end of the literal.
#include <array>
#include <atomic>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "command_dispatch.hpp"
#include "command_parser.hpp"
#include "display_state.hpp"

namespace {

int g_checks = 0;
int g_failures = 0;

void report(bool condition, const char* expression, const char* file, int line) {
    ++g_checks;
    if (!condition) {
        ++g_failures;
        std::cout << "FAIL " << file << ":" << line << "  " << expression << '\n';
    }
}

#define CHECK(expression) report((expression), #expression, __FILE__, __LINE__)

// A payload is expected to be rejected, and for a documented reason. Error
// strings are part of the example's contract, so tests pin their prefixes.
void expect_error(const std::string& payload, const std::string& prefix, const char* file, int line) {
    const flc::ParseResult result = flc::parse_command(payload);
    ++g_checks;
    if (result.ok) {
        ++g_failures;
        std::cout << "FAIL " << file << ":" << line << "  expected rejection: " << payload << '\n';
        return;
    }
    if (result.error.rfind(prefix, 0) != 0) {
        ++g_failures;
        std::cout << "FAIL " << file << ":" << line << "  wrong reason for: " << payload
                  << "  got: " << result.error << "  expected prefix: " << prefix << '\n';
    }
}

#define CHECK_ERROR(payload, prefix) expect_error((payload), (prefix), __FILE__, __LINE__)

// Parses and applies in one step, the way a caller would.
bool run(const std::string& payload, flc::DisplayState& display, std::string& error) {
    const flc::ParseResult result = flc::parse_command(payload);
    if (!result.ok) {
        error = result.error;
        return false;
    }
    return flc::apply_command(result.command, display, error);
}

void test_single_line_update_leaves_others_untouched() {
    flc::DisplayState display;
    std::string error;

    CHECK(run(R"json({"cmd":"set_line","line":2,"text":"LINE TWO"})json", display, error));
    CHECK(error.empty());

    // The documented capability: one line changes, the other three do not.
    CHECK(display.line(2).text == "LINE TWO");
    CHECK(display.line(2).revision == 1);
    CHECK(display.line(0).revision == 0);
    CHECK(display.line(1).revision == 0);
    CHECK(display.line(3).revision == 0);
    CHECK(display.line(0).text.empty());
    CHECK(display.line(1).text.empty());
    CHECK(display.line(3).text.empty());

    // A second update to the same line bumps only that line again.
    CHECK(run(R"json({"cmd":"set_line","line":2,"text":"SECOND"})json", display, error));
    CHECK(display.line(2).revision == 2);
    CHECK(display.line(0).revision == 0);
    CHECK(display.line(1).revision == 0);
    CHECK(display.line(3).revision == 0);

    // All four lines are independently addressable.
    for (std::size_t index = 0; index < flc::kLineCount; ++index) {
        const std::string payload =
            R"json({"cmd":"set_line","line":)json" + std::to_string(index) + R"json(,"text":"ROW)json" +
            std::to_string(index) + R"json("})json";
        CHECK(run(payload, display, error));
    }
    CHECK(display.line(0).text == "ROW0");
    CHECK(display.line(1).text == "ROW1");
    CHECK(display.line(2).text == "ROW2");
    CHECK(display.line(3).text == "ROW3");
}

void test_clear_and_scroll_are_line_local() {
    flc::DisplayState display;
    std::string error;

    for (std::size_t index = 0; index < flc::kLineCount; ++index) {
        CHECK(run(R"json({"cmd":"set_line","line":)json" + std::to_string(index) + R"json(,"text":"X"})json",
                  display, error));
    }

    const std::uint32_t untouched_revision = display.line(3).revision;
    CHECK(run(R"json({"cmd":"clear_line","line":1})json", display, error));
    CHECK(display.line(1).text.empty());
    CHECK(display.line(0).text == "X");
    CHECK(display.line(2).text == "X");
    CHECK(display.line(3).revision == untouched_revision);

    // Clearing an already empty line is not an event.
    const std::uint32_t cleared_revision = display.line(1).revision;
    CHECK(run(R"json({"cmd":"clear_line","line":1})json", display, error));
    CHECK(display.line(1).revision == cleared_revision);

    CHECK(run(R"json({"cmd":"scroll_line","line":3,"speed":2})json", display, error));
    CHECK(display.line(3).scroll_offset == 2);
    CHECK(display.line(0).scroll_offset == 0);
    CHECK(display.line(2).scroll_offset == 0);

    // New text resets that line's scroll offset, and only that line's.
    CHECK(run(R"json({"cmd":"set_line","line":3,"text":"FRESH"})json", display, error));
    CHECK(display.line(3).scroll_offset == 0);
    CHECK(run(R"json({"cmd":"scroll_line","line":0,"speed":-1})json", display, error));
    CHECK(display.line(0).scroll_offset == -1);
    CHECK(display.line(3).scroll_offset == 0);
}

void test_rejects_malformed_payloads() {
    CHECK_ERROR("", "syntax: empty payload");
    CHECK_ERROR("   ", "syntax: expected '{'");
    CHECK_ERROR("not json", "syntax: expected '{'");
    CHECK_ERROR(R"json({"cmd":"set_line")json", "syntax: expected ',' or '}'");
    CHECK_ERROR(R"json({"cmd" "set_line"})json", "syntax: expected ':'");
    CHECK_ERROR(R"json({"cmd":set_line})json", "syntax: expected a value");
    CHECK_ERROR(R"json({"cmd":"set_line","line":0,"text":"unterminated})json", "syntax: unterminated string");
    CHECK_ERROR(R"json({"cmd":"set_line","line":0,"text":"bad\q"})json", "syntax: unsupported escape sequence");
    CHECK_ERROR(R"json({"cmd":"set_line","line":0,"text":"x"}) trailing)json", "syntax: trailing data after object");
    CHECK_ERROR(R"json({"cmd":"set_line","cmd":"clear_line","line":0})json", "value: duplicate key");
    CHECK_ERROR(R"json({"cmd":"set_line","line":0,"text":{"a":1}})json",
                "value: nested objects and arrays are not supported");
    CHECK_ERROR(R"json({"cmd":"set_line","line":0,"text":[1]})json",
                "value: nested objects and arrays are not supported");
    CHECK_ERROR(R"json({"cmd":"set_line","line":0,"text":null})json", "value: null is not supported");
    CHECK_ERROR(R"json({"cmd":"set_line","line":1.5,"text":"x"})json", "value: only integers are supported");
    CHECK_ERROR(R"json({"cmd":"set_line","line":99999999999999999999,"text":"x"})json", "value: integer out of range");

    // Oversized input is refused before parsing rather than truncated.
    std::string huge = R"json({"cmd":"set_line","line":0,"text":")json";
    huge.append(400, 'a');
    huge += R"json("})json";
    CHECK_ERROR(huge, "syntax: payload larger than the accepted maximum");
}

void test_rejects_wrong_types_and_ranges() {
    CHECK_ERROR(R"json({"cmd":7,"line":0})json", "type: 'cmd' must be a string");
    CHECK_ERROR(R"json({"line":0,"text":"x"})json", "command: missing 'cmd'");
    CHECK_ERROR(R"json({"cmd":"set_line","text":"x"})json", "command: missing 'line'");
    CHECK_ERROR(R"json({"cmd":"set_line","line":"0","text":"x"})json", "type: 'line' must be an integer");
    CHECK_ERROR(R"json({"cmd":"set_line","line":0})json", "command: missing 'text'");
    CHECK_ERROR(R"json({"cmd":"set_line","line":0,"text":5})json", "type: 'text' must be a string");
    CHECK_ERROR(R"json({"cmd":"clear_line"})json", "command: missing 'line'");
    CHECK_ERROR(R"json({"cmd":"scroll_line","line":0})json", "command: missing 'speed'");
    CHECK_ERROR(R"json({"cmd":"scroll_line","line":0,"speed":"fast"})json", "type: 'speed' must be an integer");

    // The vocabulary addresses four lines, and nothing else.
    CHECK_ERROR(R"json({"cmd":"set_line","line":4,"text":"x"})json", "line: out of range");
    CHECK_ERROR(R"json({"cmd":"set_line","line":-1,"text":"x"})json", "line: out of range");
    CHECK_ERROR(R"json({"cmd":"scroll_line","line":0,"speed":1000})json", "speed: out of range");

    const std::string too_long(flc::kMaxTextLength + 1, 'z');
    CHECK_ERROR(R"json({"cmd":"set_line","line":0,"text":")json" + too_long + R"json("})json",
                "text: longer than the accepted maximum");

    // Unsupported commands and unexpected keys are refused, not ignored.
    CHECK_ERROR(R"json({"cmd":"reboot"})json", "command: unsupported 'cmd' value");
    CHECK_ERROR(R"json({"cmd":"set_line","line":0,"text":"x","extra":1})json", "field: unsupported key present");

    // Untrusted values are never echoed back into error text.
    const flc::ParseResult echoed = flc::parse_command(R"json({"cmd":"<script>alert(1)</script>"})json");
    CHECK(!echoed.ok);
    CHECK(echoed.error.find("script") == std::string::npos);
}

void test_rejected_command_leaves_state_alone() {
    flc::DisplayState display;
    std::string error;
    CHECK(run(R"json({"cmd":"set_line","line":1,"text":"KEEP"})json", display, error));
    const std::uint32_t revision_before = display.line(1).revision;
    const std::string text_before = display.line(1).text;

    const std::vector<std::string> rejected = {
        R"json({"cmd":"set_line","line":9,"text":"x"})json",
        R"json({"cmd":"set_line","line":1})json",
        R"json({"cmd":"clear_line","line":99})json",
        R"json({"cmd":"set_line","line":1,"text":)json" + std::string(flc::kMaxTextLength + 1, 'q') + "}",
        "garbage",
        "",
    };
    for (const std::string& payload : rejected) {
        std::string apply_error;
        CHECK(!run(payload, display, apply_error));
        CHECK(!apply_error.empty());
    }
    CHECK(display.line(1).text == text_before);
    CHECK(display.line(1).revision == revision_before);

    // The direct state API refuses the same things the parser does, so a caller
    // that skips validation still cannot corrupt the display.
    CHECK(!display.set_line(flc::kLineCount, "x"));
    CHECK(!display.clear_line(flc::kLineCount));
    CHECK(!display.set_line(0, std::string(flc::kMaxTextLength + 1, 'q')));
}

void test_direct_scroll_rejects_out_of_range_without_mutation() {
    flc::DisplayState display;
    CHECK(display.set_scroll(1, 12));
    const flc::LineState before = display.line(1);

    CHECK(!display.set_scroll(1, flc::kMinScrollOffset - 1));
    CHECK(!display.set_scroll(1, flc::kMaxScrollOffset + 1));

    const flc::LineState after = display.line(1);
    CHECK(after.text == before.text);
    CHECK(after.scroll_offset == before.scroll_offset);
    CHECK(after.revision == before.revision);
}

// A stress check for the mutex boundary. Two threads run at once: one applies
// commands, the other reads snapshots. The reader must never observe a line
// longer than the limit, and revisions must never move backwards. The initial
// write/snapshot handshake guarantees at least one snapshot overlaps an active
// writer instead of relying on thread scheduling.
void test_concurrent_snapshot_stays_consistent() {
    flc::SharedDisplay shared;
    std::atomic<bool> reader_ready{false};
    std::atomic<bool> writer_ready{false};
    std::atomic<bool> start{false};
    std::atomic<bool> first_write_done{false};
    std::atomic<bool> first_snapshot_taken{false};
    std::atomic<bool> writer_done{false};
    std::atomic<long> snapshots{0};
    std::atomic<long> violations{0};
    std::atomic<long> distinct_observed{0};

    std::thread writer([&shared, &reader_ready, &writer_ready, &start, &first_write_done,
                        &first_snapshot_taken, &writer_done] {
        while (!reader_ready.load()) {
            std::this_thread::yield();
        }
        writer_ready.store(true);
        while (!start.load()) {
            std::this_thread::yield();
        }

        shared.set_line(0, "MSG0");
        first_write_done.store(true);
        while (!first_snapshot_taken.load()) {
            std::this_thread::yield();
        }

        for (int i = 1; i < 20000; ++i) {
            const std::size_t line = static_cast<std::size_t>(i % static_cast<int>(flc::kLineCount));
            if (i % 3 == 0) {
                shared.clear_line(line);
            } else {
                shared.set_line(line, "MSG" + std::to_string(i % 100));
            }
        }
        writer_done.store(true);
    });

    std::uint32_t last_revision[flc::kLineCount] = {0, 0, 0, 0};
    std::string last_signature;
    reader_ready.store(true);
    while (!writer_ready.load()) {
        std::this_thread::yield();
    }
    start.store(true);
    while (!first_write_done.load()) {
        std::this_thread::yield();
    }

    const auto inspect_snapshot = [&] {
        const std::array<flc::LineState, flc::kLineCount> taken = shared.snapshot();
        ++snapshots;
        std::string signature;
        for (std::size_t index = 0; index < flc::kLineCount; ++index) {
            if (taken[index].text.size() > flc::kMaxTextLength) {
                ++violations;
            }
            if (taken[index].revision < last_revision[index]) {
                ++violations;
            }
            last_revision[index] = taken[index].revision;
            signature += taken[index].text;
            signature += '|';
        }
        if (signature != last_signature) {
            ++distinct_observed;
            last_signature = signature;
        }
    };

    // The writer waits for this snapshot, so this observation is guaranteed to
    // happen after its first mutation and before it can finish.
    inspect_snapshot();
    first_snapshot_taken.store(true);
    while (!writer_done.load()) {
        inspect_snapshot();
    }
    writer.join();

    CHECK(snapshots.load() > 0);
    CHECK(violations.load() == 0);
    // Sanity: the reader really did run alongside the writer.
    CHECK(distinct_observed.load() > 1);
}

}  // namespace

int main() {
    test_single_line_update_leaves_others_untouched();
    test_clear_and_scroll_are_line_local();
    test_rejects_malformed_payloads();
    test_rejects_wrong_types_and_ranges();
    test_rejected_command_leaves_state_alone();
    test_direct_scroll_rejects_out_of_range_without_mutation();
    test_concurrent_snapshot_stays_consistent();

    std::cout << (g_failures == 0 ? "PASS" : "FAIL") << ": " << (g_checks - g_failures) << "/" << g_checks
              << " checks passed\n";
    return g_failures == 0 ? 0 : 1;
}
