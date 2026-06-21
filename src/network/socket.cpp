//
// Created by 66 on 2026/6/21.
//

#include "socket.h"

namespace net {
    Socket::Socket() : fd_(-1) {
    }

    Socket::Socket(int fd) : fd_(fd) {
    }

    Socket::~Socket() {
        close();
    }

    Socket::Socket(Socket &&other) noexcept
        : fd_(other.fd_) {
        other.fd_ = -1;
    }

    Socket &net::Socket::operator=(Socket &&other) noexcept {
        if (this != &other) {
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }

    int Socket::release() {
        const int fd = fd_;
        fd_ = -1;
        return fd;
    }

    void Socket::close() {
        if (fd_ > 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    std::string Socket::addr_str(const sockaddr_in &addr) {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), ip, sizeof(ip));
        return std::string(ip) + ":" + std::to_string(ntohs(addr.sin_port));
    }
}
