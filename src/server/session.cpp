//
// Created by 66 on 2026/6/21.
//

#include "session.h"

#include "../utils/logger.h"

server::Session::Session(net::Socket sock, std::string peer_addr)
    : sock_(std::move(sock)),
      peer_addr_(std::move(peer_addr)) {
    utils::logger::info("[+] New client connected from %s", peer_addr_.c_str());
}

server::Session::~Session() {
    utils::logger::info("[-] Client %s disconnected", peer_addr_.c_str());
}

void server::Session::run() {
    char buf[4096];

    while (true) {
        // recv 返回：
        //   > 0 ：实际读到的字节数
        //   = 0 ：对端关闭连接（EOF）
        //   < 0 ：出错
        ssize_t n = ::recv(sock_.get(), buf, sizeof(buf), 0);

        if (n > 0) {
            utils::logger::info("[DATA] Received %zd bytes from %s", n, peer_addr_.c_str());
            print_hex(buf, static_cast<int>(n));
        } else if (n == 0) {
            break;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            utils::logger::error("recv() failed from %s, errno=%d", peer_addr_.c_str(), errno);
            break;
        }
    }
    sock_.close();
}

void server::Session::print_hex(const char *data, int len) {
    constexpr int BYTES_PER_LINE = 16;
    for (int i = 0; i < len; i += BYTES_PER_LINE) {
        int remain = len - i;
        int count = (remain < BYTES_PER_LINE) ? remain : BYTES_PER_LINE;

        utils::logger::raw("  %04x  ", i);

        for (int j = 0; j < count; ++j) {
            utils::logger::raw("%02x ", static_cast<unsigned char>(data[i + j]));
        }

        for (int j = count; j < BYTES_PER_LINE; ++j) {
            utils::logger::raw("   ");
        }

        utils::logger::raw(" |");
        for (int j = 0; j < count; ++j) {
            auto c = static_cast<unsigned char>(data[i + j]);
            utils::logger::raw("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        utils::logger::raw("|\n");
    }
}
