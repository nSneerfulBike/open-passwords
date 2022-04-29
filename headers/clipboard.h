#pragma once

#include <iostream>

#include "clip/clip.h"

namespace cb {

void copy_to_clipboard(std::string str) { clip::set_text(str); }

}  // namespace cb