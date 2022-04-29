#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../../../headers/json/single_include/nlohmann/json.hpp"

#define EN_US_PATH "../en_us.json"
#define SEARCH_PATH "../"

#define EXTENSION_STRING ".json"
#define EXTENSION_STRING_LENGTH 5
#define SEPARATOR "/"
#define SEPARATOR_LENGTH 1

#define HEADER_FILE_PATH "../../../headers/locales.h"
#define HEADER_SEARCH_STRING_0 "void check_locales(std::string locale) {"
#define HEADER_SEARCH_STRING_1 "}"
#define HEADER_SEARCH_STRING_1_LENGTH 1

std::string read_file(std::string path);

int main() {
    std::vector<std::string> locales;
    for (auto entry : std::filesystem::directory_iterator(SEARCH_PATH)) {
        std::string locale = entry.path();
        if (locale.find(EXTENSION_STRING) == std::string::npos) continue;

        int pos0 = locale.find(EXTENSION_STRING);
        if (pos0 == std::string::npos) return 1;
        locale = locale.replace(pos0, EXTENSION_STRING_LENGTH, "");
        int pos1 = locale.find_last_of(SEPARATOR);
        if (pos1 == std::string::npos) return 1;
        locale = locale.substr(pos1 + SEPARATOR_LENGTH);

        locales.push_back(locale);
    }

    std::string header_file_data = read_file(HEADER_FILE_PATH);
    int pos0 = header_file_data.find(HEADER_SEARCH_STRING_0);
    if (pos0 == std::string::npos) return 1;
    int pos1 = header_file_data.find(HEADER_SEARCH_STRING_1, pos0);
    if (pos1 == std::string::npos) return 1;
    int len = pos1 - pos0;

    std::string new_text = "void check_locales(std::string locale) {\n";
    for (std::string& locale : locales) {
        new_text += "    if (find_locale(locale, \"" + locale +
                    "\")) strings = (void*)(&" + locale + ");\n";
    }
    new_text += "}";

    header_file_data.replace(pos0, len + HEADER_SEARCH_STRING_1_LENGTH,
                             new_text);
    std::ofstream new_header_file(HEADER_FILE_PATH);
    new_header_file << header_file_data;
    new_header_file.close();
}

std::string read_file(std::string path) {
    std::ifstream file(path);
    std::string line, data = "";
    while (getline(file, line)) data += line + "\n";
    data = data.substr(0, data.length() - 1);
    file.close();
    return data;
}