//
// Created by xk on 2021/3/24.
//

#include "DecodeHelper.h"

void DecodeHelper::_init_pkt_frame(MyPacket *pkt, MyFrame *frame) {
    pkt = new MyPacket();
    frame = new MyFrame();
}

void DecodeHelper::init(AVFormatContext *context, int flag) {
    if (flag & FLAG_INIT_VIDEO == FLAG_INIT_VIDEO) {
        _init_pkt_frame(pkt_video, frame_video);
        initCodec(context, videoContext, stream_index_video);
    }
    if (flag & FLAG_INIT_AUDIO == FLAG_INIT_AUDIO) {
        _init_pkt_frame(pkt_audio, frame_audio);
        initCodec(context, audioContext, stream_index_audio);
    }
    if (flag & FLAG_INIT_SUBTITLE == FLAG_INIT_SUBTITLE) {
        _init_pkt_frame(pkt_subtitle, frame_subtitle);
        initCodec(context, subtitleContext, stream_index_subtitle);
    }
}

void DecodeHelper::initCodec(AVFormatContext *context, AVCodecContext *codecContext, int stream_index) {
    codecContext = avcodec_alloc_context3(nullptr);

    if (codecContext == NULL) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_alloc_context3 failed");

    }
    if (avcodec_parameters_to_context(codecContext, context->streams[stream_index]->codecpar) < 0) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_parameters_to_context failed");
    }
    codecContext->pkt_timebase = context->streams[stream_index]->time_base;
    AVCodec *avCodec = avcodec_find_decoder(codecContext->codec_id);

    if (avCodec == NULL) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_find_decoder failed");
    }

    if (avcodec_open2(codecContext, avCodec, NULL) != 0) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_open2 failed");
    }

}


void DecodeHelper::loop_read_frame(AVFormatContext *context) {
    char error[1024];
    int ret = 0;
    AVPacket *pkt_raw = av_packet_alloc();
    for (;;) {
        cout << "loop" << endl;
        if (enough_pkt()) {
            cout << "packets are full!!" << endl;
            SDL_Delay(10);
            continue;
        }

        if ((ret =av_read_frame(context, pkt_raw)) < 0) {
            av_strerror(ret, error, sizeof(error));
//            av_log(nullptr, AV_LOG_ERROR, ret);
            cout << "av_read_frame failed:" << error << endl;
            break;
        };

        if (pkt_raw->pts < 0) {
            cout << "pkt's time base is wrong" << endl;
            unref_pkt(pkt_raw);
            continue;
        }

        switch (pkt_raw->stream_index) {
            case AVMEDIA_TYPE_VIDEO:
                if (pkt_video == nullptr) {
                    unref_pkt(pkt_raw);
                    continue;
                }
                pkt_push(queue_video, pkt_raw);
                break;
            case AVMEDIA_TYPE_SUBTITLE:
                if (pkt_video == nullptr) {
                    unref_pkt(pkt_raw);
                    continue;
                }
                pkt_push(queue_subtitle, pkt_raw);
                break;
            case AVMEDIA_TYPE_AUDIO:
                if (pkt_video == nullptr) {
                    unref_pkt(pkt_raw);
                    continue;
                }
                pkt_push(queue_audio, pkt_raw);
                break;
            default:
                unref_pkt(pkt_raw);
                break;
        }
    }
}

void DecodeHelper::pkt_push(queue<MyPacket> &q, AVPacket *pkt) {
    MyPacket p(pkt);
    q.push(p);
    unref_pkt(pkt);
}


bool DecodeHelper::enough_pkt() {
    return (pkt_video != nullptr && queue_audio.size() > QUEUE_MAX_PKT)
           || (pkt_video != nullptr && queue_video.size() > QUEUE_MAX_PKT)
           || (pkt_video != nullptr && queue_subtitle.size() > QUEUE_MAX_PKT);
}

int DecodeHelper::getStream_index_video() const {
    return stream_index_video;
}

void DecodeHelper::setStream_index_video(int stream_index_video) {
    DecodeHelper::stream_index_video = stream_index_video;
}

int DecodeHelper::getStream_index_audio() const {
    return stream_index_audio;
}

void DecodeHelper::setStream_index_audio(int stream_index_audio) {
    DecodeHelper::stream_index_audio = stream_index_audio;
}

int DecodeHelper::getStream_index_subtitle() const {
    return stream_index_subtitle;
}

void DecodeHelper::setStream_index_subtitle(int stream_index_subtitle) {
    DecodeHelper::stream_index_subtitle = stream_index_subtitle;
}

