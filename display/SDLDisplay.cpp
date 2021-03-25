//
// Created by xk on 2021/3/25.
//

#include "SDLDisplay.h"


void SDLDisplay::initVideo(int flag, int width, int height) {
    SDL_Init(flag);
    int flags = SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow("testFFmpeg", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


}

AudioParams SDLDisplay::initAudio(DecodeHelper *helper) {
    if (helper->audioContext == nullptr) {
        return nullptr;
    }
    SDL_AudioSpec wanted_spec, spec;
    int wanted_nb_channel = av_get_channel_layout_nb_channels(WANTED_CHANNEL_LAYOUT);

    av_samples_get_buffer_size(nullptr, wanted_nb_channel, helper->audioContext->frame_size, WANT_SAMPLE_FMT, 1);

    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = wanted_nb_channel;
    wanted_spec.freq = WANTED_SAMPLE_SIZE;
    wanted_spec.silence = 0;
    wanted_spec.samples = WANT_SAMPLE_BUFFER_SIZE;
    wanted_spec.userdata = helper;
    wanted_spec.callback = sdl_audio_callback;

//    SDL_OpenAudioDevice()

}


void SDLDisplay::sdl_audio_callback(void *opaque, Uint8 *stream, int len) {
    auto *helper = (DecodeHelper *) opaque;
    AVFrame *frame = nullptr;
    while (len > 0) {
        if (helper->swr_audio_frame(frame))

    }
    int ret = helper->get_audio_frame(frame);

}



