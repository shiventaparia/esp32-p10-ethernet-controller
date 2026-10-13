// Independently written example code for the public case study.
#pragma once

#include <string>

#include "command_parser.hpp"
#include "display_state.hpp"

namespace flc {

// Applies an already validated command to the display state.
//
// Parsing and applying are kept separate on purpose: reaching this layer means
// the payload was structurally accepted, but the state layer still refuses
// anything it cannot honour, so a bug in validation cannot corrupt the display.
bool apply_command(const Command& command, DisplayState& display, std::string& error);

}  // namespace flc
