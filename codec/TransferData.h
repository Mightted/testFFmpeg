//
// Created by xk on 2021/3/29.
//

#ifndef TESTFFMPEG_TRANSFORMDATA_H
#define TESTFFMPEG_TRANSFORMDATA_H

#include "MyPacket.h"
#include "MyFrame.h"
#include "../AudioParams.h"
#include <iostream>
#include <SDL2/SDL_thread.h>
#include "queue"

using namespace std;

const int QUEUE_MAX_PKT = 10;

const int FLAG_INIT_VIDEO = 1;
const int FLAG_INIT_AUDIO = 2;
const int FLAG_INIT_SUBTITLE = 4;

class TransferData {
protected:


    queue<MyFrame> video_frames;
    queue<MyFrame> audio_frames;
    queue<MyFrame> subtitle_frames;

    MyPacket *pkt_video = nullptr;
    MyPacket *pkt_audio = nullptr;
    MyPacket *pkt_subtitle = nullptr;

    MyFrame *frame_video = nullptr;
    MyFrame *frame_subtitle = nullptr;

public:

    AVFrame audio_frame;
    queue<MyPacket> queue_video;
    queue<MyPacket> queue_audio;
    queue<MyPacket> queue_subtitle;


    AVCodecContext *audioContext = nullptr;
    AVCodecContext *videoContext = nullptr;
    AVCodecContext *subtitleContext = nullptr;
    SDL_mutex *video_pkt_mutex, *audio_pkt_mutex, *subtitle_pkt_mutex, *video_mutex, *audio_mutex, *subtitle_mutex;

    uint8_t *audio_buff = nullptr;
    int audio_buffer_index = 0;
    int audio_buffer_size = 0;

    TransferData();

    void init_pkt_frame(MyPacket *pkt, MyFrame *frame);

    void log_error(int ret);

    bool enough_pkt();

    void pkt_push(AVMediaType type, AVPacket *pkt);

    bool pkt_pop(AVMediaType type, AVPacket *pkt);

    void frame_push(AVMediaType type, AVFrame *frame);

    bool frame_pop(AVMediaType type, AVFrame *frame);

    void initSwr(AudioParams params);

    bool get_audio_frame();

    int swr_audio_frame();


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
        return audio_buff == nullptr || (audio_buffer_size != 0 && audio_buffer_index >= audio_buffer_size);
    };

private:

    struct SwrContext *swrContext = nullptr;

    char error[1024];

    void _pkt_push(queue<MyPacket> &q, AVPacket *pkt, SDL_mutex *mutex);

    bool _pkt_pop(queue<MyPacket> &q, AVPacket *pkt, SDL_mutex *mutex);

    void _frame_push(queue<MyFrame> &q, AVFrame *frame, SDL_mutex *mutex);

    bool _frame_pop(queue<MyFrame> &q, AVFrame *frame, SDL_mutex *mutex);

};


#endif //TESTFFMPEG_TRANSFORMDATA_H
