#pragma once

#include <iostream>
#include <random>
#include <vector>

#include "base64.h"
#include "encryption.h"
#include "json/single_include/nlohmann/json.hpp"

namespace pass {

class settings {
   private:
    // starting from the right:
    // - 1st bit: use_lower
    // - 2nd bit: use_upper
    // - 3rd bit: use_numbers
    // - 4th bit: use_special
    int options = 0b1111;
    int length = 16;

   public:
    void set_lower(bool val) {
        bool is_val = does_use_lower();
        if (val && !is_val) {
            options += 0b0001;
        } else if (!val && is_val) {
            options -= 0b0001;
        }
    }
    void set_upper(bool val) {
        bool is_val = does_use_upper();
        if (val && !is_val) {
            options += 0b0010;
        } else if (!val && is_val) {
            options -= 0b0010;
        }
    }
    void set_numbers(bool val) {
        bool is_val = does_use_numbers();
        if (val && !is_val) {
            options += 0b0100;
        } else if (!val && is_val) {
            options -= 0b0100;
        }
    }
    void set_special(bool val) {
        bool is_val = does_use_special();
        if (val && !is_val) {
            options += 0b1000;
        } else if (!val && is_val) {
            options -= 0b1000;
        }
    }

    bool does_use_lower() { return options & 0b0001; }
    bool does_use_upper() { return options & 0b0010; }
    bool does_use_numbers() { return options & 0b0100; }
    bool does_use_special() { return options & 0b1000; }

   public:
    void set_length(int new_length) { length = new_length; }

    int get_length() { return length; }
};

const std::string LOWERCASE = "abcdefghijklmnopqrstuvwxyz";
const std::string UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string NUMBERS = "0123456789";
const std::string SPECIAL = "!?%&^@";
std::string generate_password(settings& s) {
    std::random_device rd;
    std::mt19937 generator(rd());

    std::uniform_int_distribution<> distr_lower(0, LOWERCASE.length() - 1);
    std::uniform_int_distribution<> distr_upper(0, UPPERCASE.length() - 1);
    std::uniform_int_distribution<> distr_numbers(0, NUMBERS.length() - 1);
    std::uniform_int_distribution<> distr_special(0, SPECIAL.length() - 1);

    std::vector<std::pair<std::string, std::uniform_int_distribution<>>>
        distributions;
    if (s.does_use_lower()) distributions.push_back({LOWERCASE, distr_lower});
    if (s.does_use_upper()) distributions.push_back({UPPERCASE, distr_upper});
    if (s.does_use_numbers()) distributions.push_back({NUMBERS, distr_numbers});
    if (s.does_use_special()) distributions.push_back({SPECIAL, distr_special});
    std::uniform_int_distribution<> distr(0, distributions.size() - 1);

    std::string result = "";
    char last_char = '\0';
    for (int i = 0; i < s.get_length(); i++) {
        int index = distr(generator);
        std::pair<std::string, std::uniform_int_distribution<>> p =
            distributions[index];

        char new_char = p.first[p.second(generator)];
        if (new_char == last_char) {
            i--;
            continue;
        }

        result += last_char = new_char;
    }

    return result;
}

class password {
   private:
    std::string group;
    std::string user;
    std::string pass;
    int time = 0;

   public:
    password(std::string new_group, std::string new_user, std::string new_pass,
             int new_time) {
        group = new_group;
        user = new_user;
        pass = new_pass;
        time = new_time;
    }

    void set_group(std::string new_group) { group = new_group; }
    void set_user(std::string new_user) { user = new_user; }
    void set_pass(std::string new_pass) { pass = new_pass; }
    void set_time(int new_time) { time = new_time; }

    std::string get_group() { return group; }
    std::string get_user() { return user; }
    std::string get_pass() { return pass; }
    int get_time() { return time; }
};

std::vector<pass::password> file_data_to_passwords(std::string data,
                                                   std::string key) {
    std::string decrypted_str = b64::decode(enc::decrypt(data, key));
    nlohmann::json json_data = nlohmann::json::parse(decrypted_str);

    std::vector<pass::password> arr;
    for (int i = 0; i < json_data.size(); i++)
        arr.push_back(pass::password(json_data[i]["group"],
                                     json_data[i]["user"], json_data[i]["pass"],
                                     json_data[i]["time"]));
    return arr;
}
std::string passwords_to_file_data(std::vector<pass::password>& passwords,
                                   std::string key) {
    nlohmann::json json_data;
    for (int i = 0; i < passwords.size(); i++) {
        json_data.push_back({{"group", passwords[i].get_group()},
                             {"user", passwords[i].get_user()},
                             {"pass", passwords[i].get_pass()},
                             {"time", passwords[i].get_time()}});
    }
    return enc::encrypt(b64::encode(json_data.dump()), key);
}

}  // namespace pass