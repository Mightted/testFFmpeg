//
// Created by xk on 2021/3/29.
//

#include "TransferData.h"

TransferData::TransferData() {
    video_pkt_mutex = SDL_CreateMutex();
    audio_pkt_mutex = SDL_CreateMutex();
    subtitle_pkt_mutex = SDL_CreateMutex();
    video_mutex = SDL_CreateMutex();
    audio_mutex = SDL_CreateMutex();
    subtitle_mutex = SDL_CreateMutex();
}


void TransferData::init_pkt_frame(MyPacket *pkt, MyFrame *frame) {
//    pkt = new MyPacket();
//    frame = new MyFrame();
}


void TransferData::pkt_push(AVMediaType type, AVPacket *pkt) {
    switch (type) {
        case AVMEDIA_TYPE_VIDEO:
            _pkt_push(queue_video, pkt, video_pkt_mutex);
            break;
        case AVMEDIA_TYPE_AUDIO:
            _pkt_push(queue_audio, pkt, audio_pkt_mutex);
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            _pkt_push(queue_subtitle, pkt, subtitle_pkt_mutex);
            break;
        default:
            break;
    }
}

void TransferData::_pkt_push(queue<MyPacket> &q, AVPacket *pkt, SDL_mutex *mutex) {
    SDL_LockMutex(mutex);
    MyPacket p(pkt);
    q.push(p);
    unref_pkt(pkt);
    SDL_UnlockMutex(mutex);
}


bool TransferData::pkt_pop(AVMediaType type, AVPacket *pkt) {
    switch (type) {
        case AVMEDIA_TYPE_VIDEO:
            return _pkt_pop(queue_video, pkt, video_pkt_mutex);
        case AVMEDIA_TYPE_AUDIO:
            return _pkt_pop(queue_audio, pkt, audio_pkt_mutex);
        case AVMEDIA_TYPE_SUBTITLE:
            return _pkt_pop(queue_subtitle, pkt, subtitle_pkt_mutex);
        default:
            break;
    }
    return false;
}


bool TransferData::_pkt_pop(queue<MyPacket> &q, AVPacket *pkt, SDL_mutex *mutex) {
    SDL_LockMutex(mutex);
//    cout << "_pkt_pop" << endl;
//    int size = q.size();
    if (q.size() <= 0) {
        SDL_UnlockMutex(mutex);
        return false;
    }
    MyPacket packet = q.front();
//    AVPacket* p = packet._packet;
    av_packet_ref(pkt, &packet._packet);
    unref_pkt(&packet._packet);
    q.pop();
    SDL_UnlockMutex(mutex);
    return true;
}


void TransferData::frame_push(AVMediaType type, AVFrame *frame) {
    switch (type) {
        case AVMEDIA_TYPE_VIDEO:
            _frame_push(video_frames, frame, video_mutex);
            break;
        case AVMEDIA_TYPE_AUDIO:
            _frame_push(audio_frames, frame, audio_mutex);
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            _frame_push(subtitle_frames, frame, subtitle_mutex);
            break;
        default:
            break;
    }
}


void TransferData::_frame_push(queue<MyFrame> &q, AVFrame *frame, SDL_mutex *mutex) {
    SDL_LockMutex(mutex);
    MyFrame p(frame);
    p.pts = frame->pts;
    q.push(p);
    unref_frame(frame);
    SDL_UnlockMutex(mutex);
}


bool TransferData::frame_pop(AVMediaType type, AVFrame *frame) {
    switch (type) {
        case AVMEDIA_TYPE_VIDEO:
            return _frame_pop(video_frames, frame, video_mutex);
        case AVMEDIA_TYPE_AUDIO:
            return _frame_pop(audio_frames, frame, audio_mutex);
        case AVMEDIA_TYPE_SUBTITLE:
            return _frame_pop(subtitle_frames, frame, subtitle_mutex);
        default:
            break;
    }
    return false;
}


bool TransferData::_frame_pop(queue<MyFrame> &q, AVFrame *frame, SDL_mutex *mutex) {
    SDL_LockMutex(mutex);
//    int size = q.size();
    if (q.size() <= 0) {
        SDL_UnlockMutex(mutex);
        return false;
    }
    MyFrame f = q.front();
    av_frame_ref(frame, f._frame);
    unref_frame(f._frame);
    q.pop();
    SDL_UnlockMutex(mutex);
    return true;
}

bool TransferData::enough_pkt() {
    return queue_audio.size() > QUEUE_MAX_PKT
           || queue_video.size() > QUEUE_MAX_PKT
           || queue_subtitle.size() > QUEUE_MAX_PKT;
}


bool TransferData::get_audio_frame() {
    return frame_pop(AVMEDIA_TYPE_AUDIO, &audio_frame);
}


void TransferData::initSwr(AudioParams params) {
    if (audioContext == nullptr) {
        return;
    }
    swrContext = swr_alloc_set_opts(nullptr,
                                    params.channel_layout,
                                    params.fmt,
                                    params.freq,
                                    audioContext->channel_layout,
                                    audioContext->sample_fmt,
                                    audioContext->sample_rate,
                                    0, nullptr);
    if (swrContext == nullptr) {
        cout << "swr_alloc_set_opts failed" << endl;
        return;
    }
    SDL_PauseAudioDevice(params.deviceID, 0);
    swr_init(swrContext);
}


int TransferData::swr_audio_frame() {
    if (audio_buff == nullptr) {
        audio_buff = (uint8_t *) (av_malloc(192000 * 2));
    }
    if (swrContext == nullptr) {
        return 0;
    }
    int ret = swr_convert(swrContext, &audio_buff, 192000, (const uint8_t **) audio_frame.data, audio_frame.nb_samples);
    if ( ret <= 0) {
        cout << "swr_convert failed" <<  endl;
        log_error(ret);
        return 0;
    }
//    audio_buffer_size = size;
    audio_buffer_index = 0;
    return audio_buffer_size;
}


void TransferData::log_error(int ret) {
    av_strerror(ret, error, sizeof(error));
    cout << error << endl;
}
