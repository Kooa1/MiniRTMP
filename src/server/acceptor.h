//
// Created by 66 on 2026/6/21.
//

#ifndef MINIRTMP_ACCEPTOR_H
#define MINIRTMP_ACCEPTOR_H

#include <functional>

#include "../network/socket.h"

namespace server {
    class Acceptor {
    public:
        using NewConnectionCallback = std::function<void(net::Socket, sockaddr_in)>;

        explicit Acceptor(int port, NewConnectionCallback fn);

        Acceptor(const Acceptor &) = delete;

        Acceptor &operator=(const Acceptor &) = delete;

        Acceptor(Acceptor &&) noexcept;

        Acceptor &operator=(Acceptor &&) noexcept;

        ~Acceptor();

        bool listen();

        void accept_once();

    private:
        int port_;
        NewConnectionCallback new_connection_callback_;
        net::Socket listen_sock_;
    };
}

#endif //MINIRTMP_ACCEPTOR_H
