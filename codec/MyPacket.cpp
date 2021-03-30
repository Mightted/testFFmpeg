//
// Created by xk on 2021/3/24.
//

#include "MyPacket.h"
//
//MyPacket::MyPacket() {
//    _packet = av_packet_alloc();
//}

MyPacket::MyPacket(AVPacket *pkt) {
    av_packet_ref(&_packet, pkt);
}
