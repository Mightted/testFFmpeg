//
// Created by xk on 2021/3/17.
//

#ifndef TESTFFMPEG_AUDIO_H
#define TESTFFMPEG_AUDIO_H


#include "AVCommon.h"
#include "AudioParams.h"
extern "C" {
#include "libswresample/swresample.h"
};

class Audio : public AVCommon {
public:
    bool sdl_init(void *vs, void(*callback)(void *, uint8_t *, int));

    void play() override;

    bool do_swr();

    static void  playback(void *opaque, Uint8 *stream, int len);

    bool need_update();


    uint8_t *audio_buf = nullptr;
    int audio_buf_index = -1;
    int audio_buf_size = 0;
//    bool need_next_frame = true;

private:
    AudioParams params;
    SDL_AudioDeviceID audio_dev;
    struct SwrContext *swrContext = nullptr;
};


#endif //TESTFFMPEG_AUDIO_H
