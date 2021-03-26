//
// Created by xk on 2021/3/24.
//

#include "DecodeHelper.h"

void DecodeHelper::_init_pkt_frame(MyPacket *pkt, MyFrame *frame) {
    pkt = new MyPacket();
    frame = new MyFrame();
}

void DecodeHelper::init(char *path, int flag) {
    _path = path;
    initAvFormat(flag);
}

void DecodeHelper::start_read_frame() {
    if (stream_index_video != AVMEDIA_TYPE_UNKNOWN) {
        SDL_CreateThread(read_video, "video_thread", this);
    }
    if (stream_index_audio != AVMEDIA_TYPE_UNKNOWN) {
        SDL_CreateThread(read_audio, "audio_thread", this);
    }

    // subtitle暂时不写
}


void DecodeHelper::initAvFormat(int flag) {
    if (flag <= 0 || _path == nullptr) {
        cout << "decode init failed!" << endl;
        return;
    }
    avformat_open_input(&avFormatContext, _path, nullptr, nullptr);
    avformat_find_stream_info(avFormatContext, nullptr);

    int new_flag = 0;

    for (int i = 0; i < avFormatContext->nb_streams; i++) {
        AVStream *stream = avFormatContext->streams[i];
        stream->discard = AVDISCARD_ALL;
        switch (stream->codecpar->codec_type) {
            case AVMEDIA_TYPE_VIDEO:
                if (stream_index_video == AVMEDIA_TYPE_UNKNOWN && flag & FLAG_INIT_VIDEO) {
                    stream->discard = AVDISCARD_DEFAULT;
                    new_flag |= FLAG_INIT_VIDEO;
                    stream_index_video = i;
                }
                break;
            case AVMEDIA_TYPE_AUDIO:
                if (stream_index_audio == AVMEDIA_TYPE_UNKNOWN && flag & FLAG_INIT_AUDIO) {
                    stream->discard = AVDISCARD_DEFAULT;
                    new_flag |= FLAG_INIT_AUDIO;
                    stream_index_audio = i;
                }
                break;
            case AVMEDIA_TYPE_SUBTITLE:
                if (stream_index_subtitle == AVMEDIA_TYPE_UNKNOWN && flag & FLAG_INIT_SUBTITLE) {
                    stream->discard = AVDISCARD_DEFAULT;
                    new_flag |= FLAG_INIT_SUBTITLE;
                    stream_index_subtitle = i;
                }
                break;
            default:
                break;
        }
    }


    if ((new_flag & FLAG_INIT_VIDEO) == FLAG_INIT_VIDEO) {
        _init_pkt_frame(pkt_video, frame_video);
        initCodec(videoContext, stream_index_video);
    }
    if ((new_flag & FLAG_INIT_AUDIO) == FLAG_INIT_AUDIO) {
        _init_pkt_frame(pkt_audio, frame_audio);
        initCodec(audioContext, stream_index_audio);
    }
    if ((new_flag & FLAG_INIT_SUBTITLE) == FLAG_INIT_SUBTITLE) {
        _init_pkt_frame(pkt_subtitle, frame_subtitle);
        initCodec(subtitleContext, stream_index_subtitle);
    }
}


void DecodeHelper::initCodec(AVCodecContext *codecContext, int stream_index) {
    codecContext = avcodec_alloc_context3(nullptr);
    AVStream *stream = avFormatContext->streams[stream_index];

    if (codecContext == NULL) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_alloc_context3 failed");

    }
    if (avcodec_parameters_to_context(codecContext, stream->codecpar) < 0) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_parameters_to_context failed");
    }
    codecContext->pkt_timebase = stream->time_base;
    AVCodec *avCodec = avcodec_find_decoder(codecContext->codec_id);

    if (avCodec == NULL) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_find_decoder failed");
    }

    if (avcodec_open2(codecContext, avCodec, NULL) != 0) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_open2 failed");
    }

}


void DecodeHelper::loop_read_frame() {
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

        if ((ret = av_read_frame(avFormatContext, pkt_raw)) < 0) {
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
//                if (pkt_video == nullptr) {
//                    unref_pkt(pkt_raw);
//                    continue;
//                }
                pkt_push(queue_video, pkt_raw);
                break;
            case AVMEDIA_TYPE_SUBTITLE:
//                if (pkt_video == nullptr) {
//                    unref_pkt(pkt_raw);
//                    continue;
//                }
                pkt_push(queue_subtitle, pkt_raw);
                break;
            case AVMEDIA_TYPE_AUDIO:
//                if (pkt_video == nullptr) {
//                    unref_pkt(pkt_raw);
//                    continue;
//                }
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


bool DecodeHelper::pkt_pop(queue<MyPacket> &q, AVPacket *pkt) {
    if (q.size() < 0) {
        return false;
    }
    MyPacket packet = q.front();
    pkt = packet._packet;
    unref_pkt(packet._packet);
    return true;

}

void DecodeHelper::frame_push(queue<MyFrame> &q, AVFrame *frame) {
    MyFrame p(frame);
    q.push(p);
    unref_frame(frame);
}

bool DecodeHelper::frame_pop(queue<MyFrame> &q, AVFrame *frame) {
    if (q.size() < 0) {
        return false;
    }
    MyFrame f = q.front();
    frame = f._frame;
    unref_frame(f._frame);
    return true;
}

bool DecodeHelper::get_audio_frame(AVFrame *frame) {
    return frame_pop(audio_frames, frame);
}


bool DecodeHelper::enough_pkt() {
    return queue_audio.size() > QUEUE_MAX_PKT
           || queue_video.size() > QUEUE_MAX_PKT
           || queue_subtitle.size() > QUEUE_MAX_PKT;
}

int DecodeHelper::read_video(void *arg) {
    auto *helper = (DecodeHelper *) arg;
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    int ret = 0;
    for (;;) {
        ret = avcodec_receive_frame(helper->videoContext, frame);
        switch (ret) {
            case 0:
                helper->frame_push(helper->video_frames, frame);
                break;
            case AVERROR(EAGAIN):
                if (helper->pkt_pop(helper->queue_video, pkt)) {
                    if (avcodec_send_packet(helper->videoContext, pkt) == AVERROR(EAGAIN)) {
                        cout << "ERROR!!!!!!!!!!!" << endl;
                        return 0;
                    } else {
                        av_packet_unref(pkt);
                    }
                }
                break;
            default:
                cout << "unknown error!!" << endl;
                break;
        }
    }
}

int DecodeHelper::read_audio(void *arg) {
    auto *helper = (DecodeHelper *) arg;
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    int ret = 0;
    for (;;) {
        ret = avcodec_receive_frame(helper->audioContext, frame);
        switch (ret) {
            case 0:
                helper->frame_push(helper->audio_frames, frame);
                break;
            case AVERROR(EAGAIN):
                if (helper->pkt_pop(helper->queue_audio, pkt)) {
                    if (avcodec_send_packet(helper->audioContext, pkt) == AVERROR(EAGAIN)) {
                        cout << "ERROR!!!!!!!!!!!" << endl;
                        return 0;
                    } else {
                        av_packet_unref(pkt);
                    }
                }
                break;
            default:
                cout << "unknown error!!" << endl;
                break;
        }
    }
}

void DecodeHelper::initSwr() {
    if(audioParams == nullptr || audioContext == nullptr) {
        return;
    }
    swr_alloc_set_opts(swrContext, audioParams->channel_layout, audioParams->fmt, audioParams->freq, audioContext->channel_layout,
    audioContext->sample_fmt, audioContext->sample_rate, 0, nullptr);


}

void DecodeHelper::swr_audio_frame(AVFrame *frame) {


}

