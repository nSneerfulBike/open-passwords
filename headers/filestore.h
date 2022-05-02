#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "passwords.h"
#include "username.h"

namespace fs {

class filestore {
   private:
    std::string file_path;

    std::string get_file_path() {
#ifdef __android__
        return ("/data/data/com.termux/files/home/.passwords");
#endif
#ifdef __linux__
        return ("/home/" + usr::get_username() + "/.passwords");
#endif
#ifdef _WIN32
        return ("C:\\Users\\" + usr::get_username() + "\\.passwords");
#endif
        return "";
    }

   public:
    filestore() { file_path = get_file_path(); }

    void write_file_raw(std::string text) {
        std::ofstream file(file_path);
        file << text;
        file.close();
    }
    std::string read_file_raw() {
        std::ifstream file(file_path);
        if (!file) return "";

        std::string line, cypher_str = "";
        while (getline(file, line)) cypher_str += line + "\n";
        cypher_str = cypher_str.substr(0, cypher_str.length() - 1);
        file.close();

        return cypher_str;
    }

    void write_file(std::vector<pass::password>& passwords, std::string key) {
        write_file_raw(pass::passwords_to_file_data(passwords, key));
    }

    std::vector<pass::password> read_file(std::string key) {
        std::string cypher_str;
        try {
            cypher_str = read_file_raw();
        } catch (std::exception const& err) {
            return {};
        }

        if (cypher_str == "") return {};
        return pass::file_data_to_passwords(cypher_str, key);
    }
};

}  // namespace fs