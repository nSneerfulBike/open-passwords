#include <ctime>
#include <iostream>
#include <string>

#include "../headers/clipboard.h"
#include "../headers/colors.h"
#include "../headers/filestore.h"
#include "../headers/locales.h"
#include "../headers/passwords.h"
#include "../headers/socket.h"
#include "../headers/utils.h"

#define LIST_COMMAND "list"
#define LIST_COMMAND_ALIAS "ls"
#define LIST_COMMAND_ALIAS_ALIAS "l"
#define GENERATE_COMMAND "generate"
#define SHOW_COMMAND "show"
#define SHOW_COMMAND_ALIAS "print"
#define SHOW_COMMAND_ALIAS_ALIAS "p"
#define COPY_COMMAND "copy"
#define COPY_COMMAND_ALIAS "cp"
#define ADD_COMMAND "add"
#define ADD_COMMAND_ALIAS "a"
#define REMOVE_COMMAND "remove"
#define REMOVE_COMMAND_ALIAS "rm"
#define EDIT_COMMAND "edit"
#define BACKUP_COMMAND "backup"
#define BACKUP_LIST_COMMAND "backup-list"
#define REMOVE_BACKUP_COMMAND "remove-backup"
#define RESTORE_COMMAND "restore"
#define SHARE_COMMAND "share"
#define RETRIEVE_COMMAND "retrieve"
#define SYNC_COMMAND "sync"
#define CHANGE_KEY_COMMAND "change-key"

#define GROUP_OPT "--group"
#define GROUP_OPT_ALIAS "-g"

#define USER_OPT "--user"
#define USER_OPT_ALIAS "-u"

#define PASSWORD_OPT "--password"
#define PASSWORD_OPT_ALIAS "-p"

#define REMOTE_KEY_OPT "--remote-key"
#define KEY_OPT "--key"
#define KEY_OPT_ALIAS "-k"

#define FILTER_OPT "--filter"
#define FILTER_OPT_ALIAS "-f"

#define IGNORE_CASE_OPT "--ignore-case"
#define IGNORE_CASE_OPT_ALIAS "-i"

#define LENGTH_OPT "--length"
#define LENGTH_OPT_ALIAS "-l"

#define GENERATE_OPT "--generate"
#define PRINT_OPT "--print"

#define EXCLUDE_LOWER_OPT "--no-lower"
#define EXCLUDE_UPPER_OPT "--no-upper"
#define EXCLUDE_NUMBERS_OPT "--no-numbers"
#define EXCLUDE_SPECIAL_OPT "--no-special"

#define NAME_OPT "--name"
#define NAME_OPT_ALIAS "-n"

#define OLD_OPT "--old"
#define NEW_OPT "--new"

#define PORT_OPT "--port"

#define IP_ADDRESS_OPT "--ip-address"

#define DEFAULT_PORT 8080

struct arg_handler {
    std::vector<std::string> args;
    std::function<void(std::string *current, std::string *next)> handler;
};

fs::filestore filestore;

void exit_err(std::string error);
void exit_err(std::string error, int errcode);
void wrong_key();
void not_enough_arguments();

void check_args(int argc, char **argv, int start_pos,
                std::vector<struct arg_handler> args_handlers);
void pretty_print_list(std::vector<pass::password> &passwords,
                       std::vector<int> &indexes);
void pretty_print_password(std::string group, std::string user,
                           std::string pass);

void list_passwords(int argc, char **argv);
void generate_password(int argc, char **argv);
void show_password(int argc, char **argv);
void copy_password(int argc, char **argv);
void add_password(int argc, char **argv);
void remove_password(int argc, char **argv);
void edit_password(int argc, char **argv);
void backup_passwords(int argc, char **argv);
void list_backups(int argc, char **argv);
void remove_backup(int argc, char **argv);
void restore_backup(int argc, char **argv);
void share_passwords(int argc, char **argv);
void retrieve_passwords(int argc, char **argv);
void sync_passwords(int argc, char **argv);
void change_key(int argc, char **argv);

int main(int argc, char **argv) {
    locale::intialize_locales();

    if (argc <= 1) exit_err(STRING(help_text), 1);
    std::string arg = argv[1];

    if (arg == LIST_COMMAND || arg == LIST_COMMAND_ALIAS ||
        arg == LIST_COMMAND_ALIAS_ALIAS) {
        list_passwords(argc, argv);
    } else if (arg == GENERATE_COMMAND) {
        generate_password(argc, argv);
    } else if (arg == SHOW_COMMAND || arg == SHOW_COMMAND_ALIAS ||
               arg == SHOW_COMMAND_ALIAS_ALIAS) {
        show_password(argc, argv);
    } else if (arg == COPY_COMMAND || arg == COPY_COMMAND_ALIAS) {
        copy_password(argc, argv);
    } else if (arg == ADD_COMMAND || arg == ADD_COMMAND_ALIAS) {
        add_password(argc, argv);
    } else if (arg == REMOVE_COMMAND || arg == REMOVE_COMMAND_ALIAS) {
        remove_password(argc, argv);
    } else if (arg == EDIT_COMMAND) {
        edit_password(argc, argv);
    } else if (arg == BACKUP_COMMAND) {
        backup_passwords(argc, argv);
    } else if (arg == BACKUP_LIST_COMMAND) {
        list_backups(argc, argv);
    } else if (arg == REMOVE_BACKUP_COMMAND) {
        remove_backup(argc, argv);
    } else if (arg == RESTORE_COMMAND) {
        restore_backup(argc, argv);
    } else if (arg == SHARE_COMMAND) {
        share_passwords(argc, argv);
    } else if (arg == RETRIEVE_COMMAND) {
        retrieve_passwords(argc, argv);
    } else if (arg == SYNC_COMMAND) {
        sync_passwords(argc, argv);
    } else if (arg == CHANGE_KEY_COMMAND) {
        change_key(argc, argv);
    } else {
        std::cout << STRING(help_text) << std::endl;
    }
}
void exit_err(std::string error) { exit_err(error, 1); }
void exit_err(std::string error, int errcode) {
    std::cerr << error << std::endl;
    exit(errcode);
}
void wrong_key() {
    utils::sleep(1500);
    exit_err(STRING(_s_wrong_key), 1);
}
void not_enough_arguments() { exit_err(STRING(_s_not_enough_arguments), 1); }

void check_args(int argc, char **argv, int start_pos,
                std::vector<struct arg_handler> args_handlers) {
    for (int i = start_pos; i < argc; i++) {
        std::string arg = argv[i];
        std::string arg_0 = "", arg_1 = "";
        int pos = arg.find("=");
        if (pos != std::string::npos) {
            arg_0 = arg.substr(0, pos);
            arg_1 = arg.substr(pos + 1);
        }

        for (arg_handler &h : args_handlers) {
            bool equals = false;
            bool equals_01 = false;
            for (std::string &s : h.args)
                if (arg_0 == s)
                    equals_01 = true;
                else if (arg == s)
                    equals = true;

            if (!equals && !equals_01) continue;
            std::string *current;
            std::string *next;
            if (equals_01) {
                current = new std::string(arg_0);
                next = new std::string(arg_1);
            } else if (equals) {
                current = new std::string(argv[i]);
                next = nullptr;
                if (i < (argc - 1)) next = new std::string(argv[i + 1]);
            }
            h.handler(current, next);
            delete current, next;
        }
    }
}
void pretty_print_list(std::vector<pass::password> &passwords,
                       std::vector<int> &indexes) {
    int indexes_spaces = 0;
    int usernames_spaces = 0;
    std::vector<std::string> indexes_str;
    for (int &i : indexes) {
        std::string index_str = std::to_string(i);
        indexes_str.push_back(index_str);
        int new_index_length = index_str.length();
        if (indexes_spaces < new_index_length)
            indexes_spaces = new_index_length;

        int new_user_length = passwords[i].get_group().length();
        if (usernames_spaces < new_user_length)
            usernames_spaces = new_user_length;
    }
    int j = 0;
    for (int &i : indexes) {
        std::cout << color::bold(color::yellow(utils::pretty_space_inv(
                         indexes_str[j++], indexes_spaces)))
                  << color::bold(color::yellow(": "))
                  << color::bold(color::cyan(utils::pretty_space(
                         passwords[i].get_group(), usernames_spaces)))
                  << color::cyan(" ") << passwords[i].get_user() << std::endl;
    }
}
void pretty_print_password(std::string group, std::string user,
                           std::string pass) {
    std::cout << color::bold(color::yellow(STRING(_s_pretty_p_group))) << group
              << std::endl;
    std::cout << color::bold(color::yellow(STRING(_s_pretty_p_usern))) << user
              << std::endl;
    std::cout << color::bold(color::yellow(STRING(_s_pretty_p_passw))) << pass
              << std::endl;
}
void pretty_print_backups(
    std::map<std::string, std::vector<pass::password>> backups) {
    for (std::map<std::string, std::vector<pass::password>>::iterator it =
             backups.begin();
         it != backups.end(); it++) {
        std::cout << it->first << std::endl;
    }
}

// argc [0, 1] used in all the following functions
void list_passwords(int argc, char **argv) {
    std::string key = "";
    std::string filter = "";
    bool ignore_case = false;
    check_args(
        argc, argv, 2,
        {{{IGNORE_CASE_OPT, IGNORE_CASE_OPT_ALIAS},
          [&](std::string *current, std::string *next) { ignore_case = true; }},
         {{KEY_OPT, KEY_OPT_ALIAS},
          [&](std::string *current, std::string *next) {
              if (next != nullptr) key = *next;
          }},
         {{FILTER_OPT, FILTER_OPT_ALIAS, IGNORE_CASE_OPT,
           IGNORE_CASE_OPT_ALIAS},
          [&](std::string *current, std::string *next) {
              if (next != nullptr) filter = *next;
          }}});

    struct pass::passwords_data pd;
    try {
        pd = filestore.read_file(key);
    } catch (std::exception const &err) {
        wrong_key();
    }
    std::vector<int> indexes;
    for (int i = 0; i < pd.passwords.size(); i++) {
        if (filter == "") {
            indexes.push_back(i);
        } else if (utils::filter_matches(pd.passwords[i].get_group(),
                                         pd.passwords[i].get_user(), filter,
                                         ignore_case)) {
            indexes.push_back(i);
        }
    }
    pretty_print_list(pd.passwords, indexes);
}
void generate_password(int argc, char **argv) {
    pass::settings settings;
    check_args(argc, argv, 2,
               {{{EXCLUDE_LOWER_OPT},
                 [&](std::string *current, std::string *next) {
                     settings.set_lower(false);
                 }},
                {{EXCLUDE_UPPER_OPT},
                 [&](std::string *current, std::string *next) {
                     settings.set_upper(false);
                 }},
                {{EXCLUDE_NUMBERS_OPT},
                 [&](std::string *current, std::string *next) {
                     settings.set_numbers(false);
                 }},
                {{EXCLUDE_SPECIAL_OPT},
                 [&](std::string *current, std::string *next) {
                     settings.set_special(false);
                 }},
                {{LENGTH_OPT, LENGTH_OPT_ALIAS},
                 [&](std::string *current, std::string *next) {
                     if (next != nullptr)
                         settings.set_length(atoi(next->c_str()));
                 }}});

    std::string new_password = pass::generate_password(settings);
    std::cout << new_password << std::endl;
}
void show_password(int argc, char **argv) {
    if (argc <= 2) not_enough_arguments();

    int index_to_show = atoi(argv[2]);
    std::string key = "";
    check_args(argc, argv, 3,
               {{{KEY_OPT, KEY_OPT_ALIAS},
                 [&](std::string *current, std::string *next) {
                     if (next != nullptr) key = *next;
                 }}});

    struct pass::passwords_data pd;
    try {
        pd = filestore.read_file(key);
    } catch (std::exception const &err) {
        wrong_key();
    }
    if (index_to_show >= pd.passwords.size())
        exit_err(STRING(_s_index_out_of_range), 1);
    pass::password *password = &pd.passwords[index_to_show];
    pretty_print_password(password->get_group(), password->get_user(),
                          password->get_pass());
}
void copy_password(int argc, char **argv) {
    if (argc <= 2) not_enough_arguments();

    int index_to_copy = atoi(argv[2]);
    std::string key = "";
    check_args(argc, argv, 3,
               {{{KEY_OPT, KEY_OPT_ALIAS},
                 [&](std::string *current, std::string *next) {
                     if (next != nullptr) key = *next;
                 }}});

    struct pass::passwords_data pd;
    try {
        pd = filestore.read_file(key);
    } catch (std::exception const &err) {
        wrong_key();
    }
    if (index_to_copy >= pd.passwords.size())
        exit_err(STRING(_s_index_out_of_range), 1);
    pass::password *password = &pd.passwords[index_to_copy];
    cb::copy_to_clipboard(password->get_pass());
    pretty_print_password(password->get_group(), password->get_user(),
                          STRING(_s_copied_to_clipboard));
}
void add_password(int argc, char **argv) {
    std::string key = "";
    std::string *group = nullptr;
    std::string *user = nullptr;
    std::string *pass = nullptr;
    bool generate = false;
    bool print = false;
    pass::settings settings;
    check_args(
        argc, argv, 2,
        {{{EXCLUDE_LOWER_OPT},
          [&](std::string *current, std::string *next) {
              settings.set_lower(false);
          }},
         {{EXCLUDE_UPPER_OPT},
          [&](std::string *current, std::string *next) {
              settings.set_upper(false);
          }},
         {{EXCLUDE_NUMBERS_OPT},
          [&](std::string *current, std::string *next) {
              settings.set_numbers(false);
          }},
         {{EXCLUDE_SPECIAL_OPT},
          [&](std::string *current, std::string *next) {
              settings.set_special(false);
          }},
         {{GENERATE_OPT},
          [&](std::string *current, std::string *next) { generate = true; }},
         {{PRINT_OPT},
          [&](std::string *current, std::string *next) { print = true; }},
         {{LENGTH_OPT, LENGTH_OPT_ALIAS},
          [&](std::string *current, std::string *next) {
              if (next != nullptr) settings.set_length(atoi(next->c_str()));
          }},
         {{GROUP_OPT, GROUP_OPT_ALIAS},
          [&](std::string *current, std::string *next) {
              if (next == nullptr) return;
              if (group != nullptr) delete group;
              group = new std::string(next->c_str());
          }},
         {{USER_OPT, USER_OPT_ALIAS},
          [&](std::string *current, std::string *next) {
              if (next == nullptr) return;
              if (user != nullptr) delete user;
              user = new std::string(next->c_str());
          }},
         {{PASSWORD_OPT, PASSWORD_OPT_ALIAS},
          [&](std::string *current, std::string *next) {
              if (next == nullptr) return;
              if (pass != nullptr) delete pass;
              pass = new std::string(next->c_str());
          }},
         {{KEY_OPT, KEY_OPT_ALIAS},
          [&](std::string *current, std::string *next) {
              if (next != nullptr) key = *next;
          }}});

    if (generate) {
        if (pass != nullptr) delete pass;
        pass = new std::string(pass::generate_password(settings));
    }
    if (group == nullptr) exit_err(STRING(_s_group_mandatory), 1);
    if (user == nullptr) exit_err(STRING(_s_username_mandatory), 1);
    if (pass == nullptr) exit_err(STRING(_s_password_mandatory), 1);
    pass::password new_password(*group, *user, *pass, time(0));
    struct pass::passwords_data pd;
    try {
        pd = filestore.read_file(key);
    } catch (std::exception const &err) {
        wrong_key();
    }
    pd.passwords.push_back(new_password);
    filestore.write_file(pd, key);

    if (print)
        std::cout << STRING(_s_password_added_) << *pass << std::endl;
    else
        std::cout << STRING(_s_password_added) << std::endl;
}
void remove_password(int argc, char **argv) {
    if (argc <= 2) not_enough_arguments();

    int index_to_remove = atoi(argv[2]);
    std::string key = "";

    check_args(argc, argv, 3,
               {{{KEY_OPT, KEY_OPT_ALIAS},
                 [&](std::string *current, std::string *next) {
                     if (next != nullptr) key = *next;
                 }}});

    struct pass::passwords_data pd;
    try {
        pd = filestore.read_file(key);
    } catch (std::exception const &err) {
        wrong_key();
    }
    if (index_to_remove >= pd.passwords.size())
        exit_err(STRING(_s_index_out_of_range), 1);
    pd.passwords.erase(pd.passwords.begin() + index_to_remove);
    filestore.write_file(pd, key);

    std::cout << STRING(_s_password_removed) << std::endl;
}
void edit_password(int argc, char **argv) {
    if (argc <= 2) not_enough_arguments();

    int index_to_edit = atoi(argv[2]);
    std::string key = "";
    std::string *group = nullptr;
    std::string *user = nullptr;
    std::string *pass = nullptr;
    bool generate = false;
    bool print = false;
    pass::settings settings;
    check_args(
        argc, argv, 3,
        {{{EXCLUDE_LOWER_OPT},
          [&](std::string *current, std::string *next) {
              settings.set_lower(false);
          }},
         {{EXCLUDE_UPPER_OPT},
          [&](std::string *current, std::string *next) {
              settings.set_upper(false);
          }},
         {{EXCLUDE_NUMBERS_OPT},
          [&](std::string *current, std::string *next) {
              settings.set_numbers(false);
          }},
         {{EXCLUDE_SPECIAL_OPT},
          [&](std::string *current, std::string *next) {
              settings.set_special(false);
          }},
         {{GENERATE_OPT},
          [&](std::string *current, std::string *next) { generate = true; }},
         {{PRINT_OPT},
          [&](std::string *current, std::string *next) { print = true; }},
         {{LENGTH_OPT, LENGTH_OPT_ALIAS},
          [&](std::string *current, std::string *next) {
              if (next != nullptr) settings.set_length(atoi(next->c_str()));
          }},
         {{GROUP_OPT, GROUP_OPT_ALIAS},
          [&](std::string *current, std::string *next) {
              if (next == nullptr) return;
              if (group != nullptr) delete group;
              group = new std::string(next->c_str());
          }},
         {{USER_OPT, USER_OPT_ALIAS},
          [&](std::string *current, std::string *next) {
              if (next == nullptr) return;
              if (user != nullptr) delete user;
              user = new std::string(next->c_str());
          }},
         {{PASSWORD_OPT, PASSWORD_OPT_ALIAS},
          [&](std::string *current, std::string *next) {
              if (next == nullptr) return;
              if (pass != nullptr) delete pass;
              pass = new std::string(next->c_str());
          }},
         {{KEY_OPT, KEY_OPT_ALIAS},
          [&](std::string *current, std::string *next) {
              if (next != nullptr) key = *next;
          }}});

    if (generate) {
        if (pass != nullptr) delete pass;
        pass = new std::string(pass::generate_password(settings));
    }
    struct pass::passwords_data pd;
    try {
        pd = filestore.read_file(key);
    } catch (std::exception const &err) {
        wrong_key();
    }
    pass::password *new_password = &pd.passwords[index_to_edit];
    if (group != nullptr) new_password->set_group(*group);
    if (user != nullptr) new_password->set_user(*user);
    if (pass != nullptr) new_password->set_pass(*pass);
    new_password->set_time(time(0));
    filestore.write_file(pd, key);

    std::cout << STRING(_s_password_edited);
    if (print)
        std::cout << ": " << *pass;
    else
        std::cout << ".";
    std::cout << std::endl;
}
void backup_passwords(int argc, char **argv) {
    std::string key = "";
    std::string *name = nullptr;

    check_args(argc, argv, 2,
               {{{NAME_OPT, NAME_OPT_ALIAS},
                 [&](std::string *current, std::string *next) {
                     if (next == nullptr) return;
                     delete name;
                     name = new std::string(*next);
                 }},
                {{KEY_OPT, KEY_OPT_ALIAS},
                 [&](std::string *current, std::string *next) {
                     if (next != nullptr) key = *next;
                 }}});

    if (name == nullptr) {
        name = new std::string("backup-");
        *name += utils::now();
    }

    struct pass::passwords_data pd;
    try {
        pd = filestore.read_file(key);
    } catch (std::exception const &err) {
        wrong_key();
    }

    std::map<std::string, std::vector<pass::password>>::iterator i =
        pd.backups.find(*name);
    if (i != pd.backups.end())
        exit_err(STRING(_s_error_backup_name_already_exists));

    pd.backups[*name] = pd.passwords;
    filestore.write_file(pd, key);

    std::cout << STRING(_s_backup_created) << std::endl;
}
void list_backups(int argc, char **argv) {
    std::string key = "";

    check_args(argc, argv, 2,
               {{{KEY_OPT, KEY_OPT_ALIAS},
                 [&](std::string *current, std::string *next) {
                     if (next != nullptr) key = *next;
                 }}});

    struct pass::passwords_data pd;
    try {
        pd = filestore.read_file(key);
    } catch (std::exception const &err) {
        wrong_key();
    }

    pretty_print_backups(pd.backups);
}
void remove_backup(int argc, char **argv) {
    if (argc <= 2) not_enough_arguments();

    std::string name = argv[2];
    std::string key = "";

    check_args(argc, argv, 3,
               {{{KEY_OPT, KEY_OPT_ALIAS},
                 [&](std::string *current, std::string *next) {
                     if (next != nullptr) key = *next;
                 }}});

    struct pass::passwords_data pd;
    try {
        pd = filestore.read_file(key);
    } catch (std::exception const &err) {
        wrong_key();
    }

    std::map<std::string, std::vector<pass::password>>::iterator i =
        pd.backups.find(name);
    if (i == pd.backups.end())
        exit_err(STRING(_s_error_backup_name_does_not_exist));

    pd.backups.erase(i);
    filestore.write_file(pd, key);

    std::cout << STRING(_s_backup_removed) << std::endl;
}
void restore_backup(int argc, char **argv) {
    if (argc <= 2) not_enough_arguments();

    std::string name = argv[2];
    std::string key = "";

    check_args(argc, argv, 3,
               {{{KEY_OPT, KEY_OPT_ALIAS},
                 [&](std::string *current, std::string *next) {
                     if (next != nullptr) key = *next;
                 }}});

    struct pass::passwords_data pd;
    try {
        pd = filestore.read_file(key);
    } catch (std::exception const &err) {
        wrong_key();
    }

    std::map<std::string, std::vector<pass::password>>::iterator i =
        pd.backups.find(name);
    if (i == pd.backups.end())
        exit_err(STRING(_s_error_backup_name_does_not_exist));

    pd.passwords = i->second;
    filestore.write_file(pd, key);

    std::cout << STRING(_s_backup_restored) << std::endl;
}
void share_passwords(int argc, char **argv) {
    std::string *port = nullptr;
    check_args(argc, argv, 2,
               {{{PORT_OPT}, [&](std::string *current, std::string *next) {
                     if (next == nullptr) return;
                     delete port;
                     port = new std::string(*next);
                 }}});

    if (port != nullptr)
        sock::share_passwords(filestore.read_file_raw(), atoi(port->c_str()));
    else
        sock::share_passwords(filestore.read_file_raw());
}
void retrieve_passwords(int argc, char **argv) {
    if (argc <= 2) not_enough_arguments();

    std::string ip = argv[2];
    std::string *port = nullptr;
    check_args(argc, argv, 3,
               {{{PORT_OPT}, [&](std::string *current, std::string *next) {
                     if (next == nullptr) return;
                     delete port;
                     port = new std::string(*next);
                 }}});

    struct sock::receive_info ri;
    ri.custom_ip = ip;
    if (port != nullptr) ri.port = atoi(port->c_str());

    std::string passwords_raw;
    try {
        passwords_raw = sock::receive_passwords_raw(ri);
    } catch (std::exception const &err) {
        exit_err(STRING(_s_wrong_remote_address), 1);
    } catch (const char *err) {
        exit_err(STRING(_s_error_) + err, 1);
    }
    filestore.write_file_raw(passwords_raw);

    std::cout << STRING(_s_passwords_retrieved) << std::endl;
}
void sync_passwords(int argc, char **argv) {
    if (argc <= 2) not_enough_arguments();

    std::string ip = argv[2];
    std::string key = "";
    std::string remote_key = "";
    std::string *port = nullptr;
    check_args(argc, argv, 3,
               {{{REMOTE_KEY_OPT},
                 [&](std::string *current, std::string *next) {
                     if (next != nullptr) remote_key = *next;
                 }},
                {{KEY_OPT, KEY_OPT_ALIAS},
                 [&](std::string *current, std::string *next) {
                     if (next != nullptr) key = *next;
                 }},
                {{PORT_OPT}, [&](std::string *current, std::string *next) {
                     if (next == nullptr) return;
                     delete port;
                     port = new std::string(*next);
                 }}});

    struct sock::receive_info ri;
    ri.custom_ip = ip;
    if (port != nullptr) ri.port = atoi(port->c_str());

    struct pass::passwords_data new_pd, current_pd;
    try {
        new_pd = sock::receive_passwords(ri, remote_key);
    } catch (std::exception const &err) {
        exit_err(STRING(_s_wrong_remote_address), 1);
    } catch (const char *err) {
        exit_err(STRING(_s_error_) + err, 1);
    }

    try {
        current_pd = filestore.read_file(key);
    } catch (std::exception const &err) {
        wrong_key();
    }

    for (pass::password &p : new_pd.passwords) {
        bool exists = false;
        for (int i = 0; i < current_pd.passwords.size(); i++) {
            if ((p.get_group() == current_pd.passwords[i].get_group()) &&
                (p.get_user() == current_pd.passwords[i].get_user())) {
                if ((p.get_time() > current_pd.passwords[i].get_time()))
                    current_pd.passwords[i].set_pass(p.get_pass());

                exists = true;
                break;
            }
        }

        if (!exists) current_pd.passwords.push_back(p);
    }

    filestore.write_file(current_pd, key);
    std::cout << STRING(_s_passwords_synced) << std::endl;
}
void change_key(int argc, char **argv) {
    std::string old_key = "";
    std::string new_key = "";
    check_args(argc, argv, 2,
               {{{OLD_OPT},
                 [&](std::string *current, std::string *next) {
                     if (next != nullptr) old_key = *next;
                 }},
                {{NEW_OPT}, [&](std::string *current, std::string *next) {
                     if (next != nullptr) new_key = *next;
                 }}});

    struct pass::passwords_data pd;
    try {
        pd = filestore.read_file(old_key);
    } catch (std::exception const &err) {
        wrong_key();
    }
    filestore.write_file(pd, new_key);

    std::cout << STRING(_s_key_changed) << std::endl;
}
