//
// Created by 66 on 2026/6/21.
//

#ifndef MINIRTMP_SOCKET_H
#define MINIRTMP_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

namespace net {
    class Socket {
    public:
        Socket();

        explicit Socket(int fd);

        ~Socket();

        Socket(const Socket &) = delete;

        Socket &operator=(const Socket &) = delete;

        Socket(Socket &&other) noexcept;

        Socket &operator=(Socket &&other) noexcept;

        bool valid() const { return fd_ >= 0; }

        int get() const { return fd_; }

        int release();

        void close();

        static std::string addr_str(const sockaddr_in &addr);

    private:
        int fd_;
    };
}


#endif //MINIRTMP_SOCKET_H
