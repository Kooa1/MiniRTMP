//
// Created by 66 on 2026/6/21.
//

#ifndef MINIRTMP_RTMPMESSAGE_H
#define MINIRTMP_RTMPMESSAGE_H

#include <cstdint>
#include <vector>

namespace protocol {
    struct RtmpMessage {
        uint32_t timestamp_ = 0; // 绝对时间戳（ms）
        uint32_t stream_id_ = 0; // 消息流 ID（小端序原始值）
        uint8_t type_ = 0; // 消息类型（20=Command, 1=SetChunkSize, ...）
        std::vector<uint8_t> payload; // 完整载荷
    };
}

#endif //MINIRTMP_RTMPMESSAGE_H
