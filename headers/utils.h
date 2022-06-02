#pragma once

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "./date/include/date/date.h"

namespace utils {

void sleep(long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

std::string to_lowercase(std::string str) {
    for (char &c : str) c = tolower(c);
    return str;
}

std::string pretty_space(std::string str, int n) {
    int spaces = n - str.length();
    for (int i = 0; i < spaces; i++) str += " ";
    return str;
}
std::string pretty_space_inv(std::string str, int n) {
    std::string res = "";
    int spaces = n - str.length();
    for (int i = 0; i < spaces; i++) res += " ";
    return res + str;
}

std::vector<std::string> split(std::string str, char delimiter) {
    std::vector<std::string> res;
    std::string last = "";
    for (char &c : str) {
        if (c == delimiter) {
            res.push_back(last);
            last = "";
        } else {
            last += c;
        }
    }
    res.push_back(last);
    return res;
}
bool filter_matches(std::string group, std::string user, std::string filter,
                    bool ignore_case) {
    for (std::string &f : split(filter, ' ')) {
        if (ignore_case) {
            if (to_lowercase(group).find(f) == std::string::npos &&
                to_lowercase(user).find(f) == std::string::npos)
                return false;
        } else {
            if (group.find(f) == std::string::npos &&
                user.find(f) == std::string::npos)
                return false;
        }
    }
    return true;
}

std::string now() {
    auto today = std::chrono::floor<date::days>(std::chrono::_V2::system_clock::now());
    return date::format("%F", today);
}

}  // namespace utils