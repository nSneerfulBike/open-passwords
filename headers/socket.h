// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <unistd.h>

// #include <array>
// #include <cstdlib>
// #include <iostream>
// #include <memory>
// #include <vector>

// #include "base64.h"
// #include "encryption.h"
// #include "passwords.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <functional>
#include <iostream>
#include <thread>
#include <vector>

namespace sock {

#define DEFAULT_PORT 8080
// #define BUFFER_SIZE 65535

// #define ACTION_SCAN '\x01'
// #define ACTION_PULL '\x02'

// #define ACTIONS_LEN 1
// #define SCAN_ACTIVE '\x0f'

// #ifdef _WIN32
// #define IP_COMMAND "ipconfig"
// #define IP_SEARCH_STRING "ifconfig"  // todo
// #define IP_SEARCH_STRING_PLUS 5      // todo
// #define IP_SEARCH_SPACE_STRING " "   // todo

// #define popen _popen
// #define pclose _pclose
// #endif

// #ifdef __linux__
// #define IP_COMMAND "ifconfig"
// #define IP_SEARCH_STRING "inet "
// #define IP_SEARCH_STRING_PLUS 5
// #define IP_SEARCH_SPACE_STRING " "
// #endif

struct receive_info {
    std::string custom_ip = "";
    int port; // = DEFAULT_PORT;
};

std::string exec(const char *cmd) {
    // std::array<char, 128> buffer;
    // std::string result;
    // std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    // if (!pipe) {
    //     throw std::runtime_error("popen() failed!");
    // }
    // while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    //     result += buffer.data();
    // }
    // return result;
    return "";
}
std::vector<std::string> get_ip_addresses() {
    std::vector<std::string> result;

    // std::string out = exec(IP_COMMAND);
    // std::vector<int> positions;
    // int last = 0, pos;
    // while ((pos = out.find(IP_SEARCH_STRING, last)) != std::string::npos) {
    //     last = pos + IP_SEARCH_STRING_PLUS;
    //     positions.push_back(last);
    // }

    // for (int &pos : positions) {
    //     result.push_back(
    //         out.substr(pos, out.find(IP_SEARCH_SPACE_STRING, pos) - pos));
    // }

    return result;
}

void share_passwords(std::string passwords_raw, int port = DEFAULT_PORT) {
    // int server_fd;
    // int opt = 1;
    // struct sockaddr_in address;
    // int addrlen = sizeof(address);
    // int new_socket;

    // if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    //     throw "socket error";
    // }
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
    //                sizeof(opt))) {
    //     throw "socket options error";
    // }
    // address.sin_family = AF_INET;
    // address.sin_addr.s_addr = INADDR_ANY;
    // address.sin_port = htons(port);

    // if (bind(server_fd, (struct sockaddr *)(&address), sizeof(address)) < 0) {
    //     throw "bind error";
    // }
    // if (listen(server_fd, 3) < 0) {
    //     throw "listen error";
    // }

    // std::string passwords_b64 = b64::encode(passwords_raw) + '\0';

    // std::cout
    //     << "Connect to this device using one of the following IP addresses:"
    //     << std::endl;
    // for (std::string &ip : get_ip_addresses()) {
    //     std::cout << ip << std::endl;
    // }
    // std::cout << std::endl << "Waiting for connections..." << std::endl;
    // while (true) {
    //     if ((new_socket = accept(server_fd, (struct sockaddr *)(&address),
    //                              (socklen_t *)(&addrlen))) < 0) {
    //         throw "accept error";
    //     }

    //     char ip_char[INET_ADDRSTRLEN];
    //     inet_ntop(AF_INET, &address.sin_addr, ip_char, INET_ADDRSTRLEN);
    //     std::string ip = ip_char;

    //     char action[1];
    //     read(new_socket, action, ACTIONS_LEN);
    //     switch (action[0]) {
    //         case ACTION_SCAN:
    //             action[0] = SCAN_ACTIVE;
    //             send(new_socket, action, ACTIONS_LEN, 0);
    //             break;

    //         case ACTION_PULL:
    //             send(new_socket, passwords_b64.c_str(), passwords_b64.length(),
    //                  0);
    //             std::cout << "Data sent to device with IP: " << ip << std::endl;
    //             break;
    //     }
    //     close(new_socket);
    // }
}

std::string receive_passwords_raw(struct receive_info ri) {
    // int sock = 0;
    // struct sockaddr_in serv_addr;
    // char buffer[BUFFER_SIZE];
    // std::string res = "";

    // if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    //     throw "socket creation error";
    // }

    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_port = htons(ri.port);

    // std::cout << "Scanning \"" << ri.custom_ip << "\"..." << std::endl;

    // if (inet_pton(AF_INET, ri.custom_ip.c_str(), &serv_addr.sin_addr) <= 0) {
    //     throw "unable to connect";
    // }
    // if (connect(sock, (struct sockaddr *)(&serv_addr), sizeof(serv_addr)) < 0) {
    //     throw "unable to connect";
    // }

    // char action[1] = {ACTION_PULL};
    // send(sock, action, ACTIONS_LEN, 0);
    // while (read(sock, buffer, BUFFER_SIZE)) res += std::string(buffer);
    // return b64::decode(res);
    return "";
}
struct pass::passwords_data receive_passwords(struct receive_info ri,
                                              std::string key) {
    std::string passwords_raw = receive_passwords_raw(ri);
    return pass::file_data_to_passwords(passwords_raw, key);
}
//
// everything above is useless
//

#define PORT 8080

#define ERROR_SOCKET_OK 0
#define ERROR_SOCKET_DESCRIPTOR 1
#define ERROR_SOCKET_OPTIONS 2
#define ERROR_SOCKET_BIND 3
#define ERROR_SOCKET_LISTEN 4
#define ERROR_SOCKET_ACCEPT 5

class server {
   private:
    int server_fd, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

   public:
    int initialize() {
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            return ERROR_SOCKET_DESCRIPTOR;
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                       sizeof(opt))) {
            return ERROR_SOCKET_OPTIONS;
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            return ERROR_SOCKET_BIND;
        }
        return ERROR_SOCKET_OK;
    }
    void listen_socket(std::function<void(int socket)> callback,
                       std::function<void(int which)> onerror) {
        if (listen(server_fd, 3) < 0) {
            return onerror(ERROR_SOCKET_LISTEN);
        }

        std::vector<std::thread> threads;
        int sock;
        for (;;) {
            if ((sock = accept(server_fd, (struct sockaddr*)&address,
                               (socklen_t*)&addrlen)) < 0) {
                return onerror(ERROR_SOCKET_ACCEPT);
            }
            threads.push_back(std::thread([&]() {
                callback(sock);
                close(sock);
            }));
        }
        shutdown(server_fd, SHUT_RDWR);
        for (std::thread& t : threads) t.join();
    }
    std::string read_line(int sock) {
        std::string res = "";
        char buff[1];
        for (;;) {
            read(sock, buff, 1);
            if (buff[0] == '\n') break;
            res += buff[0];
        }
        return res;
    }
};

}  // namespace sock