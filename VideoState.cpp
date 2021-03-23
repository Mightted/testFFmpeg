//
// Created by xk on 2021/3/11.
//

#include <iostream>
#include <string>
#include "VideoState.h"
//#include "Audio.h"
//#include "Video.h"

extern "C" {

#include "libswscale/swscale.h"
#include "libavutil/time.h"
}

using namespace std;


static int audio_read_frame(void *arg) {
    auto *is = (VideoState *) arg;

    for (;;) {
        if (av_q2d(is->audio->time_base) == 0) {
            continue;
        }
        if (!is->audio->need_update_pkt) {
//        av_frame_unref(avFrame);
            int ret = avcodec_receive_frame(is->audio->avCodecContext, is->audio->avFrame);
            if (ret == 0) {
                cout << "avcodec_receive_frame:" << ret <<  endl;
                dynamic_cast<Audio *>(is->audio)->do_swr();
                while (!dynamic_cast<Audio *>(is->audio)->need_update()) {
                    SDL_Delay(1);
                }

            } else if (ret == AVERROR(EAGAIN)) {
//            cout << "avcodec_send_packet" << endl;
                avcodec_send_packet(is->audio->avCodecContext, is->audio->avPacket);
                cout << "avcodec_send_packet" << endl;
                av_packet_unref(is->audio->avPacket);
                is->audio->need_update_pkt = true;
            } else {
                av_strerror(ret, is->error_str, sizeof(is->error_str));
                cout << "error message:" << is->error_str << endl;
                break;
            }
        } else {
            SDL_Delay(1);
        }
    }
    return 0;


}


static int read_thread(void *arg) {
    auto *is = (VideoState *) arg;
    if (!is->ff_init()) {
        return 0;
    }
    is->aud_thread_id = SDL_CreateThread(audio_read_frame, "audio_read_frame", arg);
    int ret;
    for (;;) {
        ret = 0;
//        ret += is->video->read_frame(is->avFormatContext);
        ret += is->audio->read_frame(is->avFormatContext);
        if (ret == -2) {
            break;
        }
//        if (is->video->need_update_pkt) {
//            if (av_read_frame(is->avFormatContext, is->video->avPacket) == 0) {
//                if (is->video->avPacket->stream_index == is->video->index_stream) {
//                    cout << "av_read_frame" << endl;
//                    is->video->need_update_pkt = false;
//                } else {
//                    av_packet_unref(is->video->avPacket);
//                }
//            } else {
//                break;
//            }
//        }
    }
    return 0;
}


void VideoState::loop() {
    sdl_init();
//    if (!ff_init()) {
//        return;
//    }
    read_thread_id = SDL_CreateThread(read_thread, "read_thread", this);
    int doExit = 0;
    SDL_Event event;
    for (;;) {
        if (doExit) {
            break;
        }
//        cout << "loop" << endl;
        SDL_PumpEvents();
        while (!SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
            video_refresh();
            SDL_PumpEvents();
        }
        switch (event.type) {
            case SDL_QUIT:
                doExit = 1;
                break;
            default:
                break;
        }
    }
}


void VideoState::video_refresh() {

    if (av_q2d(video->time_base) == 0) {
        return;
    }
//    cout << "video_refresh" << endl;
    if (!video->need_update_pkt && (frame_start_time == 0 || frame_start_time + cur_duration < av_gettime_relative())) {
//        av_frame_unref(avFrame);
        int ret = avcodec_receive_frame(video->avCodecContext, video->avFrame);
        if (ret == 0) {
            cout << "avcodec_receive_frame" << endl;
//            if (frame_start_time == 0) {
//                frame_start_time = av_gettime_relative();
//            }
            frame_start_time = av_gettime_relative();
            cur_duration = av_q2d(video->time_base);
            cur_frame_time = video->avFrame->pts * av_q2d(video->time_base);

            video->play();

            return;
        } else if (ret == AVERROR(EAGAIN)) {
            cout << "avcodec_send_packet" << endl;
            avcodec_send_packet(video->avCodecContext, video->avPacket);
            av_packet_unref(video->avPacket);
            video->need_update_pkt = true;
        } else {
            av_strerror(ret, error_str, sizeof(error_str));
            cout << "error message:" << error_str << endl;
        }
    }

}


bool VideoState::sdl_init(int width, int height) {
    cout << "width:" << width << ", height:" << height << endl;
    int flag = SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER;
    SDL_Init(flag);
    dynamic_cast<Video *>(video)->sdl_init();

    return true;
}

bool VideoState::ff_init() {

    if (avformat_open_input(&avFormatContext, file_name.c_str(), NULL, NULL) != 0) {
        cout << "avformat_open_input failed" << endl;
        return false;
    }

    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        cout << "avformat_find_stream_info failed" << endl;
        return false;
    }

    for (int i = 0; i < avFormatContext->nb_streams; i++) {
        switch (avFormatContext->streams[i]->codecpar->codec_type) {
            case AVMEDIA_TYPE_VIDEO:
                if (video->index_stream == AVMEDIA_TYPE_UNKNOWN) {
                    video->index_stream = i;
                }
                break;
            case AVMEDIA_TYPE_AUDIO:
                if (audio->index_stream == AVMEDIA_TYPE_UNKNOWN) {
                    audio->index_stream = i;
                }
                break;
//            case AVMEDIA_TYPE_SUBTITLE:
//                if (index_subtitle_stream == AVMEDIA_TYPE_UNKNOWN) {
//                    index_subtitle_stream = i;
//                }
//                break;
            default:
                break;
        }
    }

    if (video->index_stream == AVMEDIA_TYPE_UNKNOWN && audio->index_stream == AVMEDIA_TYPE_UNKNOWN) {
        cout << "stream index init failed" << endl;
        return false;
    }

    video->init(avFormatContext);
//    dynamic_cast<Video*>(video)->sdl_init(avFormatContext);
    audio->init(avFormatContext);
    dynamic_cast<Audio *>(audio)->sdl_init(this, sdl_audio_callback);

}


VideoState::VideoState(string file) {
    file_name = file;
    audio = new Audio();
    video = new Video();
//    index_audio_stream = index_video_stream = index_subtitle_stream = AVMEDIA_TYPE_UNKNOWN;
}

VideoState::~VideoState() {
    if (avFormatContext != NULL) {
        avformat_free_context(avFormatContext);
    }

}

void VideoState::sdl_audio_callback(void *opaque, Uint8 *stream, int len) {
    auto *vs = (VideoState *) opaque;
    Audio::playback(vs, stream, len);
//    dynamic_cast<Audio *>(vs->audio)->playback(nullptr, stream, len);
}





