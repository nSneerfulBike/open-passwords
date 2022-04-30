#pragma once

#include <iostream>

#ifdef __linux__
#include <pwd.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

namespace usr {

std::string get_username() {
#ifdef __linux__
    uid_t userid;
    struct passwd* pass;
    userid = getuid();
    pass = getpwuid(userid);
    return pass->pw_name;
#endif

#ifdef _WIN32
    const int max_buffer = 100;
    char sz_buffer[max_buffer];
    DWORD length = max_buffer;
    if (GetUserName(sz_buffer, &length)) return sz_buffer;
#endif

    return "";
}

}  // namespace usr