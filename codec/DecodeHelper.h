//
// Created by xk on 2021/3/24.
//

#ifndef TESTFFMPEG_DECODEHELPER_H
#define TESTFFMPEG_DECODEHELPER_H

#include "MyFrame.h"
#include "MyPacket.h"
#include "queue"
#include <iostream>
#include <SDL2/SDL.h>

using namespace std;

const int QUEUE_MAX_PKT = 10;

const int FLAG_INIT_VIDEO = 1;
const int FLAG_INIT_AUDIO = 2;
const int FLAG_INIT_SUBTITLE = 4;

class DecodeHelper {
public:
    void init(AVFormatContext *context, int flag);

    void loop_read_frame(AVFormatContext *context);

private:

    void initCodec(AVFormatContext *context, AVCodecContext *codecContext, int stream_index);

    inline void unref_pkt(AVPacket *pkt) {
        if (pkt != nullptr) {
            av_packet_unref(pkt);
        }
    }

    queue<MyPacket> queue_video;
    queue<MyPacket> queue_audio;
    queue<MyPacket> queue_subtitle;

    MyPacket *pkt_video = nullptr;
    MyPacket *pkt_audio = nullptr;
    MyPacket *pkt_subtitle = nullptr;

    MyFrame *frame_video = nullptr;
    MyFrame *frame_audio = nullptr;
    MyFrame *frame_subtitle = nullptr;

    AVCodecContext *audioContext = nullptr;
    AVCodecContext *videoContext = nullptr;
    AVCodecContext *subtitleContext = nullptr;

    int stream_index_video, stream_index_audio, stream_index_subtitle = AVMEDIA_TYPE_UNKNOWN;

    void _init_pkt_frame(MyPacket *pkt, MyFrame *frame);

    bool enough_pkt();

    void pkt_push(queue<MyPacket> &q, AVPacket *pkt);

public:
    int getStream_index_video() const;

    void setStream_index_video(int stream_index_video);

    int getStream_index_audio() const;

    void setStream_index_audio(int stream_index_audio);

    int getStream_index_subtitle() const;

    void setStream_index_subtitle(int stream_index_subtitle);
};


#endif //TESTFFMPEG_DECODEHELPER_H
