//
// Created by xk on 2021/3/24.
//

#include "MyPacket.h"

MyPacket::MyPacket() {
    _packet = av_packet_alloc();
}

MyPacket::MyPacket(AVPacket *pkt) {
    _packet = pkt;
}
