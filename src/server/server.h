//
// Created by 66 on 2026/6/21.
//

#ifndef MINIRTMP_SERVER_H
#define MINIRTMP_SERVER_H

#include <memory>
#include <atomic>

#include "session.h"
#include "acceptor.h"

namespace server {
    class Server {
    public:
        explicit Server(int port);

        ~Server();

        Server(const Server &) = delete;

        Server &operator=(const Server &) = delete;

        bool start();

        void stop();

    private:
        void on_new_connection(net::Socket client_sock, sockaddr_in client_addr);

        int port_;
        Acceptor acceptor_;
        std::atomic_bool running_{false};
    };
}


#endif //MINIRTMP_SERVER_H
