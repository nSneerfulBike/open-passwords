#pragma once

#include <iostream>

namespace color {

#define RESET "\x1b[0m"

std::string bold(std::string str) { return "\x1b[1m" + str + RESET; }

std::string yellow(std::string str) { return "\x1b[33m" + str + RESET; }
std::string cyan(std::string str) { return "\x1b[36m" + str + RESET; }

}  // namespace color