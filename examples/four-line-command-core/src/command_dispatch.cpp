// Independently written example code for the public case study.
#include "command_dispatch.hpp"

namespace flc {

bool apply_command(const Command& command, DisplayState& display, std::string& error) {
    switch (command.kind) {
        case CommandKind::SetLine:
            if (!display.set_line(command.line, command.text)) {
                error = "state: set_line refused";
                return false;
            }
            return true;

        case CommandKind::ClearLine:
            if (!display.clear_line(command.line)) {
                error = "state: clear_line refused";
                return false;
            }
            return true;

        case CommandKind::ScrollLine:
            if (!display.set_scroll(command.line, command.speed)) {
                error = "state: scroll_line refused";
                return false;
            }
            return true;

        case CommandKind::Unknown:
            break;
    }
    error = "state: command is not applicable";
    return false;
}

}  // namespace flc
