//
// Created by 66 on 2026/6/21.
//

#include "server.h"

#include "../utils/logger.h"

namespace server {
    server::Server::Server(int port)
        : port_(port),
          acceptor_(port, [this](net::Socket s, sockaddr_in addr) {
              on_new_connection(std::move(s), addr);
          }) {
    }

    Server::~Server() = default;

    bool Server::start() {
        if (!acceptor_.listen()) {
            return false;
        }

        running_ = true;
        utils::logger::info("MiniRTMP server started on 0.0.0.0:%d", port_);

        while (running_) {
            acceptor_.accept_once();
        }

        utils::logger::info("Server stopped");
        return true;
    }

    void Server::stop() {
        running_.store(false, std::memory_order_relaxed);
    }

    void Server::on_new_connection(net::Socket client_sock, sockaddr_in client_addr) {
        std::string peer = net::Socket::addr_str(client_addr);

        std::thread([sock = std::move(client_sock), peer]() mutable {
            Session session(std::move(sock), peer);
            session.run();
        }).detach();
    }
}
