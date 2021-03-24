//
// Created by xk on 2021/3/17.
//

#ifndef TESTFFMPEG_AVCOMMON_H
#define TESTFFMPEG_AVCOMMON_H


#include "SDL2/SDL.h"
#include "string"
#include <iostream>
using namespace std;
extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
};


const AVRational NULL_TB = AVRational{0, 1};

class AVCommon {
public:
    AVCodecContext *avCodecContext = nullptr;
    AVCodec *avCodec = nullptr;
    AVPacket *avPacket = nullptr;
    AVFrame *avFrame = nullptr;
    bool need_update_pkt = true;
    AVRational time_base = NULL_TB;
    int index_stream = AVMEDIA_TYPE_UNKNOWN;

    virtual ~AVCommon() {}

    bool init(AVFormatContext *context);

    int read_frame(AVFormatContext *context);

//    virtual bool sdl_init(VideoState *vs) = 0;

    bool codec_open(AVFormatContext *context);

    virtual void play() = 0;


};


#endif //TESTFFMPEG_AVCOMMON_H
