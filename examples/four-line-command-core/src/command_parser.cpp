// Independently written example code for the public case study.
#include "command_parser.hpp"

#include <cstdint>
#include <limits>

namespace flc {
namespace {

constexpr std::size_t kMaxPayloadBytes = 256;

struct JsonValue {
    enum class Type { String, Integer, Boolean };
    Type type = Type::String;
    std::string str;
    std::int64_t num = 0;
    bool boolean = false;
};

struct Field {
    std::string key;
    JsonValue value;
};

// A deliberately small scanner over the accepted subset. Every failure path
// records a stable reason and stops; nothing is repaired or guessed.
class SubsetParser {
public:
    explicit SubsetParser(const std::string& text) : text_(text) {}

    bool parse_object(std::vector<Field>& out) {
        skip_ws();
        if (!consume('{')) {
            return fail("syntax: expected '{'");
        }
        skip_ws();
        if (consume('}')) {
            return finish();
        }
        for (;;) {
            skip_ws();
            std::string key;
            if (!parse_string(key)) {
                return false;
            }
            skip_ws();
            if (!consume(':')) {
                return fail("syntax: expected ':'");
            }
            skip_ws();
            JsonValue value;
            if (!parse_value(value)) {
                return false;
            }
            for (const Field& existing : out) {
                if (existing.key == key) {
                    return fail("value: duplicate key");
                }
            }
            out.push_back(Field{key, value});
            skip_ws();
            if (consume(',')) {
                continue;
            }
            if (consume('}')) {
                return finish();
            }
            return fail("syntax: expected ',' or '}'");
        }
    }

    const std::string& error() const { return error_; }

private:
    bool finish() {
        skip_ws();
        if (index_ < text_.size()) {
            return fail("syntax: trailing data after object");
        }
        return true;
    }

    bool fail(const std::string& reason) {
        if (error_.empty()) {
            error_ = reason;
        }
        return false;
    }

    bool at_end() const { return index_ >= text_.size(); }
    bool peek_is(char c) const { return !at_end() && text_[index_] == c; }

    void skip_ws() {
        while (!at_end()) {
            const char c = text_[index_];
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                ++index_;
            } else {
                break;
            }
        }
    }

    bool consume(char expected) {
        if (peek_is(expected)) {
            ++index_;
            return true;
        }
        return false;
    }

    bool parse_string(std::string& out) {
        if (!consume('"')) {
            return fail("syntax: expected a quoted key or value");
        }
        while (!at_end()) {
            const unsigned char c = static_cast<unsigned char>(text_[index_]);
            if (c == '"') {
                ++index_;
                return true;
            }
            if (c == '\\') {
                ++index_;
                if (at_end()) {
                    return fail("syntax: unterminated escape");
                }
                const char escaped = text_[index_++];
                switch (escaped) {
                    case '"': out.push_back('"'); break;
                    case '\\': out.push_back('\\'); break;
                    case '/': out.push_back('/'); break;
                    default: return fail("syntax: unsupported escape sequence");
                }
                continue;
            }
            if (c < 0x20) {
                return fail("syntax: control character in string");
            }
            out.push_back(static_cast<char>(c));
            ++index_;
        }
        return fail("syntax: unterminated string");
    }

    bool parse_value(JsonValue& out) {
        if (peek_is('"')) {
            out.type = JsonValue::Type::String;
            return parse_string(out.str);
        }
        if (peek_is('t')) {
            out.type = JsonValue::Type::Boolean;
            out.boolean = true;
            return consume_literal("true");
        }
        if (peek_is('f')) {
            out.type = JsonValue::Type::Boolean;
            out.boolean = false;
            return consume_literal("false");
        }
        if (peek_is('-') || (peek_is_digit())) {
            out.type = JsonValue::Type::Integer;
            return parse_integer(out.num);
        }
        if (peek_is('{') || peek_is('[')) {
            return fail("value: nested objects and arrays are not supported");
        }
        if (peek_is('n')) {
            return fail("value: null is not supported");
        }
        return fail("syntax: expected a value");
    }

    bool peek_is_digit() const {
        return !at_end() && text_[index_] >= '0' && text_[index_] <= '9';
    }

    bool consume_literal(const char* literal) {
        for (const char* p = literal; *p != '\0'; ++p) {
            if (at_end() || text_[index_] != *p) {
                return fail("syntax: malformed literal");
            }
            ++index_;
        }
        return true;
    }

    bool parse_integer(std::int64_t& out) {
        const bool negative = consume('-');
        if (!peek_is_digit()) {
            return fail("syntax: expected digits in number");
        }
        std::int64_t value = 0;
        while (peek_is_digit()) {
            const int digit = text_[index_] - '0';
            if (value > (std::numeric_limits<std::int64_t>::max() - digit) / 10) {
                return fail("value: integer out of range");
            }
            value = value * 10 + digit;
            ++index_;
        }
        if (peek_is('.') || peek_is('e') || peek_is('E')) {
            return fail("value: only integers are supported");
        }
        out = negative ? -value : value;
        return true;
    }

    const std::string& text_;
    std::size_t index_ = 0;
    std::string error_;
};

const JsonValue* find_field(const std::vector<Field>& fields, const std::string& key) {
    for (const Field& field : fields) {
        if (field.key == key) {
            return &field.value;
        }
    }
    return nullptr;
}

bool has_only_fields(const std::vector<Field>& fields, const std::vector<std::string>& allowed) {
    for (const Field& field : fields) {
        bool known = false;
        for (const std::string& name : allowed) {
            if (field.key == name) {
                known = true;
                break;
            }
        }
        if (!known) {
            return false;
        }
    }
    return true;
}

ParseResult fail_result(const std::string& reason) {
    ParseResult result;
    result.ok = false;
    result.error = reason;
    return result;
}

// Reads a required integer field, checking type and range.
bool read_line_index(const std::vector<Field>& fields, std::size_t& out, std::string& error) {
    const JsonValue* value = find_field(fields, "line");
    if (value == nullptr) {
        error = "command: missing 'line'";
        return false;
    }
    if (value->type != JsonValue::Type::Integer) {
        error = "type: 'line' must be an integer";
        return false;
    }
    if (value->num < 0 || value->num >= static_cast<std::int64_t>(kAddressableLines)) {
        error = "line: out of range";
        return false;
    }
    out = static_cast<std::size_t>(value->num);
    return true;
}

}  // namespace

ParseResult parse_command(const std::string& payload) {
    if (payload.empty()) {
        return fail_result("syntax: empty payload");
    }
    if (payload.size() > kMaxPayloadBytes) {
        return fail_result("syntax: payload larger than the accepted maximum");
    }

    std::vector<Field> fields;
    SubsetParser parser(payload);
    if (!parser.parse_object(fields)) {
        return fail_result(parser.error().empty() ? std::string("syntax: malformed payload")
                                                 : parser.error());
    }

    const JsonValue* cmd = find_field(fields, "cmd");
    if (cmd == nullptr) {
        return fail_result("command: missing 'cmd'");
    }
    if (cmd->type != JsonValue::Type::String) {
        return fail_result("type: 'cmd' must be a string");
    }

    // Field names are compared, never echoed back. Untrusted input does not get
    // copied into error text, so it cannot be used to forge log lines.
    if (cmd->str == "set_line") {
        if (!has_only_fields(fields, {"cmd", "line", "text"})) {
            return fail_result("field: unsupported key present");
        }
        Command command;
        command.kind = CommandKind::SetLine;
        std::string error;
        if (!read_line_index(fields, command.line, error)) {
            return fail_result(error);
        }
        const JsonValue* text = find_field(fields, "text");
        if (text == nullptr) {
            return fail_result("command: missing 'text'");
        }
        if (text->type != JsonValue::Type::String) {
            return fail_result("type: 'text' must be a string");
        }
        if (text->str.size() > kMaxTextLength) {
            return fail_result("text: longer than the accepted maximum");
        }
        command.text = text->str;
        return ParseResult{true, command, std::string()};
    }

    if (cmd->str == "clear_line") {
        if (!has_only_fields(fields, {"cmd", "line"})) {
            return fail_result("field: unsupported key present");
        }
        Command command;
        command.kind = CommandKind::ClearLine;
        std::string error;
        if (!read_line_index(fields, command.line, error)) {
            return fail_result(error);
        }
        return ParseResult{true, command, std::string()};
    }

    if (cmd->str == "scroll_line") {
        if (!has_only_fields(fields, {"cmd", "line", "speed"})) {
            return fail_result("field: unsupported key present");
        }
        Command command;
        command.kind = CommandKind::ScrollLine;
        std::string error;
        if (!read_line_index(fields, command.line, error)) {
            return fail_result(error);
        }
        const JsonValue* speed = find_field(fields, "speed");
        if (speed == nullptr) {
            return fail_result("command: missing 'speed'");
        }
        if (speed->type != JsonValue::Type::Integer) {
            return fail_result("type: 'speed' must be an integer");
        }
        if (speed->num < kMinScrollOffset || speed->num > kMaxScrollOffset) {
            return fail_result("speed: out of range");
        }
        command.speed = static_cast<int>(speed->num);
        return ParseResult{true, command, std::string()};
    }

    return fail_result("command: unsupported 'cmd' value");
}

}  // namespace flc
