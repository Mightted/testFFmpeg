//
// Created by xk on 2021/3/25.
//

#ifndef TESTFFMPEG_SDLDISPLAY_H
#define TESTFFMPEG_SDLDISPLAY_H

#include "SDL2/SDL.h"
#include "../codec/TransferData.h"
#include "../AudioParams.h"

extern "C" {
#include <libavformat/avformat.h>
};
using namespace std;

const int WANTED_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
const int WANTED_SAMPLE_SIZE = 44100;
const AVSampleFormat WANT_SAMPLE_FMT = AV_SAMPLE_FMT_S16;
const int WANT_SAMPLE_BUFFER_SIZE = 1024;

class SDLDisplay {
private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
    SDL_AudioDeviceID audioDeviceID;

    static void sdl_audio_callback(void *opaque, Uint8 *stream, int len);

public:
    void initVideo(int width, int height);

    AudioParams *initAudio(TransferData *transferData);

    void playVideo(AVFrame *frame);

    bool initSDL(int flag);

};


#endif //TESTFFMPEG_SDLDISPLAY_H
