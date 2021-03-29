//
// Created by xk on 2021/3/29.
//

#ifndef TESTFFMPEG_TRANSFORMDATA_H
#define TESTFFMPEG_TRANSFORMDATA_H

#include "MyPacket.h"
#include "MyFrame.h"
#include <iostream>
#include "queue"
using namespace std;

const int QUEUE_MAX_PKT = 10;

const int FLAG_INIT_VIDEO = 1;
const int FLAG_INIT_AUDIO = 2;
const int FLAG_INIT_SUBTITLE = 4;

class TransferData {
protected:

    queue<MyPacket> queue_video;
    queue<MyPacket> queue_audio;
    queue<MyPacket> queue_subtitle;

    queue<MyFrame> video_frames;
    queue<MyFrame> audio_frames;
    queue<MyFrame> subtitle_frames;

    MyPacket *pkt_video = nullptr;
    MyPacket *pkt_audio = nullptr;
    MyPacket *pkt_subtitle = nullptr;

    MyFrame *frame_video = nullptr;
    MyFrame *frame_audio = nullptr;
    MyFrame *frame_subtitle = nullptr;

public:

    AVCodecContext *audioContext = nullptr;
    AVCodecContext *videoContext = nullptr;
    AVCodecContext *subtitleContext = nullptr;

    char *audio_buff = nullptr;
    int audio_buffer_index = 0;
    int audio_buffer_size = 0;

    void init_pkt_frame(MyPacket *pkt, MyFrame *frame);

    bool enough_pkt();

    void pkt_push(AVMediaType type, AVPacket *pkt);

    bool pkt_pop(AVMediaType type, AVPacket *pkt);

    void frame_push(AVMediaType type, AVFrame *frame);

    bool frame_pop(AVMediaType type, AVFrame *frame);


    bool get_audio_frame(AVFrame *frame);

    void swr_audio_frame(AVFrame *frame);


    inline void unref_frame(AVFrame *frame) {
        if (frame != nullptr) {
            av_frame_unref(frame);
        }
    }

    inline void unref_pkt(AVPacket *pkt) {
        if (pkt != nullptr) {
            av_packet_unref(pkt);
        }
    }

    inline bool audio_need_update() {
        return audio_buffer_size != 0 && audio_buffer_index >= audio_buffer_size;
    };

private:
    void _pkt_push(queue<MyPacket> &q, AVPacket *pkt);

    bool _pkt_pop(queue<MyPacket> &q, AVPacket *pkt);

    void _frame_push(queue<MyFrame> &q, AVFrame *frame);

    bool _frame_pop(queue<MyFrame> &q, AVFrame *frame);

};


#endif //TESTFFMPEG_TRANSFORMDATA_H
