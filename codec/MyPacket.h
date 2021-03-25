//
// Created by xk on 2021/3/24.
//

#ifndef TESTFFMPEG_MYPACKET_H
#define TESTFFMPEG_MYPACKET_H

extern "C" {
#include <libavformat/avformat.h>
};


class MyPacket {
public:
    MyPacket();

    MyPacket(AVPacket *pkt);

    AVPacket *_packet = nullptr;

};


#endif //TESTFFMPEG_MYPACKET_H
