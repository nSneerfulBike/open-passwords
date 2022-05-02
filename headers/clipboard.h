#pragma once

#include <iostream>

#ifndef __android__
#include "clip/clip.h"
#endif

namespace cb {

void copy_to_clipboard(std::string str) {
#ifndef __android__
    clip::set_text(str);
#endif
}

}  // namespace cb