//
// Created by xk on 2021/3/25.
//

#include "SDLDisplay.h"


void SDLDisplay::initVideo(int width, int height) {
    int flags = SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow("testFFmpeg", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, width, height);

}

AudioParams *SDLDisplay::initAudio(TransferData *transferData) {
    if (transferData->audioContext == nullptr) {
        return nullptr;
    }
    SDL_AudioSpec wanted_spec, spec;
    int wanted_nb_channel = av_get_channel_layout_nb_channels(WANTED_CHANNEL_LAYOUT);

    transferData->audio_buffer_size = av_samples_get_buffer_size(nullptr, wanted_nb_channel,
                                                                 transferData->audioContext->frame_size,
                                                                 WANT_SAMPLE_FMT, 1);

    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = wanted_nb_channel;
    wanted_spec.freq = WANTED_SAMPLE_SIZE;
    wanted_spec.silence = 0;
    wanted_spec.samples = WANT_SAMPLE_BUFFER_SIZE;
    wanted_spec.userdata = transferData;
    wanted_spec.callback = sdl_audio_callback;

    audioDeviceID = SDL_OpenAudioDevice(nullptr, 0, &wanted_spec, &spec,
                                        SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE);

    if (audioDeviceID < 2) {
        cout << "SDL_OpenAudioDevice faild:" << SDL_GetError() << endl;
        return nullptr;
    }

    AudioParams *params = new AudioParams();
    params->freq = spec.freq;
    params->channels = spec.channels;
    params->fmt = WANT_SAMPLE_FMT;
    params->channel_layout = WANTED_CHANNEL_LAYOUT;
    params->deviceID = audioDeviceID;

    return params;
//    SDL_PauseAudioDevice(audioDeviceID, 0);


}


bool SDLDisplay::initSDL(int flag) {
    int actual_flag = SDL_INIT_TIMER;
    if ((flag & FLAG_INIT_VIDEO) == FLAG_INIT_VIDEO) {
        actual_flag |= SDL_INIT_VIDEO;
    }
    if ((flag & FLAG_INIT_AUDIO) == FLAG_INIT_AUDIO) {
        actual_flag |= SDL_INIT_AUDIO;
    }

    return SDL_Init(actual_flag) == 0;
}


void SDLDisplay::playVideo(AVFrame *frame) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_UpdateYUVTexture(texture, nullptr,
                         frame->data[0], frame->linesize[0],
                         frame->data[1], frame->linesize[1],
                         frame->data[2], frame->linesize[2]);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}


void SDLDisplay::sdl_audio_callback(void *opaque, Uint8 *stream, int len) {
    auto *data = (TransferData *) opaque;
    int len1;
//    AVFrame *frame = nullptr;
//    cout << "sdl_audio_callback" << endl;
    while (len > 0) {

        if (data->audio_need_update()) {
            while (!data->get_audio_frame()) {
                SDL_Delay(1);
            }
            if (data->swr_audio_frame() == 0) {
                continue;
            }
        }
        len1 = (data->audio_buffer_size - data->audio_buffer_index) < len ?
              data->audio_buffer_size - data->audio_buffer_index : len;
        cout << "play audio" << endl;
        memset(stream, 0, len1);
        SDL_MixAudioFormat(stream, data->audio_buff + data->audio_buffer_index, AUDIO_S16SYS, len1, SDL_MIX_MAXVOLUME);
        data->audio_buffer_index += len1;
        len -= len1;
        stream += len1;
    }

}



