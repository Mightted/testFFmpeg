//
// Created by xk on 2021/3/17.
//

#include "Audio.h"

/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

bool Audio::sdl_init(void *vs, void(*callback)(void *, uint8_t *, int)) {
    SDL_AudioSpec wanted_spec, spec;
//    int wanted_nb_channel = avCodecContext->channels;
//    int64_t wanted_channel_layout = avCodecContext->channel_layout;
//    int wanted_sample_rate = avCodecContext->sample_rate;
//
//    if (wanted_nb_channel != av_get_channel_layout_nb_channels(wanted_channel_layout)) {
//        wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channel);
//        wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
//    }
//    wanted_nb_channel = av_get_channel_layout_nb_channels(wanted_channel_layout);
//
//    audio_buf_size = av_samples_get_buffer_size(nullptr, wanted_nb_channel, avCodecContext->frame_size, AV_SAMPLE_FMT_S16, 1);


    int wanted_nb_channel = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    int64_t wanted_channel_layout = avCodecContext->channel_layout;
    int wanted_sample_rate = avCodecContext->sample_rate;

//    if (wanted_nb_channel != av_get_channel_layout_nb_channels(wanted_channel_layout)) {
//        wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channel);
//        wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
//    }
//    wanted_nb_channel = av_get_channel_layout_nb_channels(wanted_channel_layout);

    audio_buf_size = av_samples_get_buffer_size(nullptr, wanted_nb_channel, avCodecContext->frame_size,
                                                AV_SAMPLE_FMT_S16, 1);


    wanted_spec.channels = wanted_nb_channel;
    wanted_spec.freq = 44100;
    if (wanted_spec.channels < 0 || wanted_spec.freq < 0) {
        av_log(NULL, AV_LOG_ERROR, "Invalid sample rate or channel count!\n");
        return false;
    }
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.silence = 0;
    wanted_spec.callback = callback;
    wanted_spec.userdata = this;
//    wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE,
//                                2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
    wanted_spec.samples = avCodecContext->frame_size;

//    SDL_OpenAudio(&wanted_spec, nullptr);
    audio_dev = SDL_OpenAudioDevice(nullptr, 0, &wanted_spec, &spec,
                                    SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE);
//    if (audio_dev < 2) {
//        av_log(nullptr, AV_LOG_ERROR, "invalid audio device");
//        return false;
//    }
//
    params.freq = spec.freq;
    params.channels = spec.channels;
    params.fmt = AV_SAMPLE_FMT_S16;
    params.channel_layout = wanted_channel_layout;
    params.frame_size = av_samples_get_buffer_size(nullptr, params.channels, 1, params.fmt, 1);
    params.bytes_per_sec = av_samples_get_buffer_size(nullptr, params.channels, params.freq, params.fmt, 1);

    swrContext = swr_alloc_set_opts(NULL,  // we're allocating a new context
                                    AV_CH_LAYOUT_STEREO,  // out_ch_layout
                                    AV_SAMPLE_FMT_S16,    // out_sample_fmt
                                    44100,                // out_sample_rate
                                    avCodecContext->sample_fmt, // in_ch_layout    AV_CH_LAYOUT_5POINT1
                                    AV_SAMPLE_FMT_FLTP,   // in_sample_fmt
                                    avCodecContext->sample_rate,                // in_sample_rate    48000
                                    0,                    // log_offset
                                    NULL);                // log_ctx
    int ret = swr_init(swrContext);
    printf("ret:%d", ret);

//    SDL_PauseAudio(0);
    SDL_PauseAudioDevice(audio_dev, 0);
    return true;
}

bool Audio::do_swr() {
    if (swrContext == nullptr) {
        return false;
    }
//    if (!need_update()) {
//        return false;
//    }
//    if (!need_next_frame) {
//        return false;
//    }

    if (audio_buf == nullptr) {
        audio_buf = (unsigned char *) av_malloc(192000 * 2);
    }
//    audio_buf_size = av_samples_get_buffer_size(nullptr, avFrame->channels, avFrame->nb_samples, AV_SAMPLE_FMT_S16, 1);
    if (swrContext != nullptr) {
        swr_convert(swrContext, &audio_buf, 192000, (const uint8_t **) avFrame->data, avFrame->nb_samples);
    }
    audio_buf_index = 0;
//    need_next_frame = false;

    return true;

}

bool Audio::need_update() {
    return audio_buf_index >= audio_buf_size;
}

void Audio::play() {

}


void Audio::playback(void *opaque, Uint8 *stream, int len) {
    auto audio = (Audio *) opaque;
    if (audio->audio_buf_index < 0 || audio->need_update()) {
        return;
    }
//    if (audio->need_next_frame) {
//        return;
//    }

    len = len > (audio->audio_buf_size - audio->audio_buf_index) ? (audio->audio_buf_size - audio->audio_buf_index)
                                                                 : len;
    memset(stream, 0, len);
//    SDL_MixAudio(stream, audio->audio_buf + audio->audio_buf_index, len, SDL_MIX_MAXVOLUME);
    SDL_MixAudioFormat(stream, audio->audio_buf + audio->audio_buf_index, AUDIO_S16SYS, len, SDL_MIX_MAXVOLUME);
    cout << "play audio" << endl;
//    audio->audio_buf += len;
    audio->audio_buf_index += len;
//    if (audio_buf_index >= audio_buf_size) {
//        need_next_frame = true;
//    }
}
