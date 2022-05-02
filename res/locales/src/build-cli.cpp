#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "../../../headers/json/single_include/nlohmann/json.hpp"

#define EN_US_PATH "../en_us.json"

#define SEARCH_PATH "../"
#define DEST_PATH "../build/locales-cli.h"

#define EXTENSION_STRING ".json"
#define EXTENSION_STRING_LENGTH 5
#define SEPARATOR "/"
#define SEPARATOR_LENGTH 1

#define HELP_TEXT_PATH "../"
#define HELP_TEXT_SUFFIX "-help.txt"

struct {
    std::vector<std::string> keys;
    nlohmann::json values;
} en_us_data;

void err(std::string path);

std::string generate_namespace(std::string path, bool first = false);
std::string get_help_text(std::string locale);

std::string get_locale_from_path(std::string path);
std::string read_file(std::string path);
std::string hex(int n);
std::string generate_hex_values(std::string txt);

int main() {
    std::vector<std::string> paths;
    for (auto entry : std::filesystem::directory_iterator(SEARCH_PATH))
        paths.push_back(entry.path().u8string());

    std::vector<std::string> namespaces;
    namespaces.push_back(generate_namespace(EN_US_PATH, true));
    for (std::string& path : paths)
        if (path.find(EXTENSION_STRING) != std::string::npos &&
            path != EN_US_PATH) {
                try {
                    namespaces.push_back(generate_namespace(path));
                } catch (std::exception const& _) {
                    err(path);
                }
            }

    std::string res =
        "// auto-generated - do not touch\n\n#pragma once\n\n#include "
        "<iostream>\n\n";
    for (std::string& ns : namespaces) res += ns + "\n";

    std::ofstream end_file(DEST_PATH);
    end_file << res;
    end_file.close();
}
void err(std::string path) {
    std::string file = path.substr(3);
    std::cerr << "An error was found in the file " << file << "." << std::endl;
}

std::string generate_namespace(std::string path, bool first) {
    std::string res = "";
    std::string locale = get_locale_from_path(path);
    std::cout << "Building " << locale << "..." << std::endl;

    std::string data = read_file(path);
    nlohmann::json json_data = nlohmann::json::parse(data)["cli"];

    res += "struct {\n";
    int index = 0;
    std::string help_text = get_help_text(locale);
    if (help_text != "") {
        res += "const std::string _help_text = \"" +
               generate_hex_values(help_text) + "\";\n";
        res += "#ifndef help_text\n#define help_text " +
               (std::to_string(index++)) + "\n#endif\n";
    }
    if (first) {
        en_us_data.values = json_data;
        en_us_data.values["help_text"] = help_text;
        en_us_data.keys.push_back("help_text");
        for (auto& el : json_data.items()) {
            std::string key = std::string(el.key());
            en_us_data.keys.push_back(key);
            res += "const std::string _" + key + " = \"" +
                   generate_hex_values(el.value()) + "\";\n";
            res += "#ifndef " + key + "\n#define " + key + " " +
                   (std::to_string(index++)) + "\n#endif\n";
        }
    } else {
        for (std::string& key : en_us_data.keys) {
            try {
                res += "const std::string _" + key + " = \"" +
                       generate_hex_values(json_data[key]) + "\";\n";
            } catch (nlohmann::detail::type_error& err) {
                res += "const std::string _" + key + " = \"" +
                       generate_hex_values(en_us_data.values[key]) + "\";\n";
            }
        }
    }
    res += "} " + locale + ";";

    return res;
}
std::string get_help_text(std::string locale) {
    return read_file(HELP_TEXT_PATH + locale + HELP_TEXT_SUFFIX);
}

std::string get_locale_from_path(std::string path) {
    std::string locale = path;
    int pos0 = locale.find(EXTENSION_STRING);
    if (pos0 == std::string::npos) return "";
    locale = locale.replace(pos0, EXTENSION_STRING_LENGTH, "");
    int pos1 = locale.find_last_of(SEPARATOR);
    if (pos1 == std::string::npos) return "";
    locale = locale.substr(pos1 + SEPARATOR_LENGTH);
    return locale;
}
std::string read_file(std::string path) {
    std::ifstream file(path);
    std::string line, data = "";
    while (getline(file, line)) data += line + "\n";
    data = data.substr(0, data.length() - 1);
    file.close();
    return data;
}
std::string hex(int n) {
    std::stringstream ss;
    ss << std::hex << n;
    return ss.str();
}
std::string generate_hex_values(std::string txt) {
    std::string res = "";
    for (char& c : txt) res += "\\x" + hex(int(c));
    return res;
}