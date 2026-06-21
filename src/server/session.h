//
// Created by 66 on 2026/6/21.
//

#ifndef MINIRTMP_SESSION_H
#define MINIRTMP_SESSION_H

#include <string>
#include <thread>

#include "../network/socket.h"
#include "../protocol/handshake.h"

namespace server {
    class Session {
    public:
        Session(net::Socket sock, std::string peer_addr);

        ~Session();

        Session(const Session &) = delete;

        Session &operator=(const Session &) = delete;

        void run();

    private:
        static void print_hex(const char *data, int len);

        net::Socket sock_;
        std::string peer_addr_;
    };
}

#endif //MINIRTMP_SESSION_H
