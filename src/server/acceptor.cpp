//
// Created by 66 on 2026/6/21.
//

#include "acceptor.h"

#include "../utils/logger.h"

namespace server {
    Acceptor::Acceptor(int port, NewConnectionCallback fn)
        : port_(port),
          new_connection_callback_(std::move(fn)) {
    }

    Acceptor::~Acceptor() = default;

    bool Acceptor::listen() {
        int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (fd < 0) {
            utils::logger::error("socket failed, error=%d", errno);
            return false;
        }

        listen_sock_ = net::Socket(fd);

        int opt = 1;
        ::setsockopt(listen_sock_.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port_);

        if (::bind(listen_sock_.get(), reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
            utils::logger::error("bind failed on port %d, error=%d", port_, errno);
            return false;
        }

        if (::listen(listen_sock_.get(), SOMAXCONN < 0)) {
            utils::logger::error("listen failed, error=%d", errno);
            return false;
        }

        utils::logger::info("Listening on 0.0.0.0:%d", port_);
        return true;
    }

    void Acceptor::accept_once() {
        sockaddr_in client_addr{};
        socklen_t socklen = sizeof(client_addr);

        int client_fd = ::accept(listen_sock_.get(), reinterpret_cast<sockaddr *>(&client_addr), &socklen);
        if (client_fd < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
                utils::logger::error("accept failed, error=%d", errno);
                return;
            }
        }

        net::Socket client_socket(client_fd);
        new_connection_callback_(std::move(client_socket), client_addr);
    }
}
