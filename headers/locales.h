#pragma once

#include <iostream>

#include "../res/locales/build/locales-cli.h"

namespace locale {

#define SEARCH_PREFIX "LC_MESSAGES="
#define SEARCH_PREFIX_LENGTH 12
#define SEARCH_SUFFIX "."
#define SEARCH_SUFFIX_LENGTH 1

#define STRING(strcode) ((std::string*)(locale::strings))[strcode]

void* strings = (void*)(&en_us);

bool find_locale(std::string locale, std::string search) {
    return locale.find(search) == 0;
}
// auto-generated - do not touch
void check_locales(std::string locale) {
    if (find_locale(locale, "en_us")) strings = (void*)(&en_us);
}

void intialize_locales() {
    std::string locale = std::locale("").name();
    int pos0 = locale.find(SEARCH_PREFIX);
    if (pos0 == std::string::npos) return;
    locale = locale.substr(pos0 + SEARCH_PREFIX_LENGTH);
    int pos1 = locale.find(SEARCH_SUFFIX);
    if (pos1 == std::string::npos) return;
    locale = locale.substr(0, pos1);
    for (char& c : locale) c = tolower(c);

    check_locales(locale);
}
void intialize_locales(std::string locale) {
    check_locales(locale);
}

}  // namespace locale