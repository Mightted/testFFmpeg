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
#include "../AudioParams.h"

using namespace std;

const int QUEUE_MAX_PKT = 10;

const int FLAG_INIT_VIDEO = 1;
const int FLAG_INIT_AUDIO = 2;
const int FLAG_INIT_SUBTITLE = 4;

class DecodeHelper {
public:
    void init(char *path, int flag);

    void loop_read_frame();

    bool get_audio_frame(AVFrame *frame);

    void swr_audio_frame(AVFrame *frame);

    inline bool audio_need_update() {
        return audio_buffer_size != 0 && audio_buffer_index >= audio_buffer_size;
    };

    char *audio_buff = nullptr;
    int audio_buffer_index = 0;
    int audio_buffer_size = 0;

    AVCodecContext *audioContext = nullptr;
    AVCodecContext *videoContext = nullptr;
    AVCodecContext *subtitleContext = nullptr;


private:

    void initAvFormat(int flag);

    void start_read_frame();

    void initCodec(AVCodecContext *codecContext, int stream_index);

    void initSwr();

    inline void unref_pkt(AVPacket *pkt) {
        if (pkt != nullptr) {
            av_packet_unref(pkt);
        }
    }

    inline void unref_frame(AVFrame *frame) {
        if (frame != nullptr) {
            av_frame_unref(frame);
        }
    }

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

    SDL_threadID *video_thread = nullptr;
    SDL_threadID *audio_thread = nullptr;
    SDL_threadID *subtitle_thread = nullptr;

    AVFormatContext *avFormatContext = nullptr;
    struct SwrContext *swrContext = nullptr;
    AudioParams *audioParams = nullptr;

    char *_path = nullptr;


    int stream_index_video = AVMEDIA_TYPE_UNKNOWN;
    int stream_index_audio = AVMEDIA_TYPE_UNKNOWN;
    int stream_index_subtitle = AVMEDIA_TYPE_UNKNOWN;

    void _init_pkt_frame(MyPacket *pkt, MyFrame *frame);

    bool enough_pkt();

    void pkt_push(queue<MyPacket> &q, AVPacket *pkt);

    bool pkt_pop(queue<MyPacket> &q, AVPacket *pkt);

    void frame_push(queue<MyFrame> &q, AVFrame *frame);

    bool frame_pop(queue<MyFrame> &q, AVFrame *frame);


    static int read_video(void *arg);

    static int read_audio(void *arg);
};


#endif //TESTFFMPEG_DECODEHELPER_H
