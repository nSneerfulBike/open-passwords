#pragma once

#include <iostream>

#include "cpp-base64/base64.h"

namespace b64 {

std::string encode(std::string str) {
    return base64_encode(str);
}

std::string decode(std::string str) {
    return base64_decode(str);
}

}