//
// Created by 66 on 2026/6/21.
//

#ifndef MINIRTMP_HANDSHAKE_H
#define MINIRTMP_HANDSHAKE_H

#include <cstdint>
#include <cstring>
#include <ctime>
#include <cstdlib>

#include "../utils/logger.h"

namespace protocol {
    class Handshake {
    public:
        enum class State {
            WAIT_C0C1, // 等待收到 C0(1字节) + C1(1536字节)
            SEND_S0S1S2, // 需要发送 S0 + S1 + S2 给客户端
            WAIT_C2, // 等待客户端发来 C2(1536字节)
            DONE, // 握手成功
            FAILED, // 握手失败（版本不支持等）
        };

        Handshake();

        State state() const { return state_; }

        int input(const char *data, int len);

        int get_send_data(char *out, int capacity);

        void advance_after_send();

    private:
        void parse_c0c1();

        State state_ = State::WAIT_C0C1;

        char recv_buf_[1537];
        int recv_buf_idx_ = 0;

        uint32_t c1_timestamp_ = 0;
    };
}


#endif //MINIRTMP_HANDSHAKE_H
