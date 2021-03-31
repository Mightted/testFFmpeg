//
// Created by xk on 2021/3/24.
//

#include "DecodeHelper.h"

//void DecodeHelper::_init_pkt_frame(MyPacket *pkt, MyFrame *frame) {
//    pkt = new MyPacket();
//    frame = new MyFrame();
//}

void DecodeHelper::init(char *path, int flag) {
    _path = path;
    avFrame = av_frame_alloc();
    transferData = new TransferData();
    initAvFormat(flag);
}

void DecodeHelper::start_read_frame() {

    SDL_CreateThread(loop_read_frame, "read_thread", this);

    if (stream_index_video != AVMEDIA_TYPE_UNKNOWN) {
        SDL_CreateThread(read_video, "video_thread", transferData);
    }
    if (stream_index_audio != AVMEDIA_TYPE_UNKNOWN) {
        SDL_CreateThread(read_audio, "audio_thread", transferData);
    }

    // subtitle暂时不写
}


void DecodeHelper::video_refresh() {

    if (avFrame == nullptr) {
        transferData->frame_pop(AVMEDIA_TYPE_VIDEO, avFrame);
    }

    if (!video_frame_update(avFrame)) {
        cout << "it is not time" << endl;
        return;
    }
    display.playVideo(avFrame);
    if (avFrame->pts != AV_NOPTS_VALUE && avFrame->pts >= 0) {
        last_pts = avFrame->pts;
    }

    transferData->frame_pop(AVMEDIA_TYPE_VIDEO, avFrame);

}


bool DecodeHelper::video_frame_update(AVFrame *frame) {
    if (frame == nullptr) {
        return true;
    }
    double time = av_rescale_q(frame->pts, video_time_base, AV_TIME_BASE_Q);
//    cout << "pts:" << frame->pts << endl;
//    cout << "time:" << time << ", av_gettime_relative:" << av_gettime_relative() << endl;
//    av_rescale_q((frame->pts - last_pts), video_time_base, AV_TIME_BASE_Q)
    cout << "play_start_time:" << play_start_time << endl;
    cout << "second params:" << av_q2d(video_time_base) * (frame->pts - last_pts) << endl;
    cout << "av_gettime_relative:" << av_gettime_relative() << endl;
    cout << "psts??:" << frame->pts - last_pts << endl;
    cout << "frame pts :" << frame->pts << endl;
    cout << "last_pts" << last_pts << endl;
    cout << "av_q2d(video_time_base):" << av_q2d(video_time_base) << endl;
    cout << "av_rescale_q(frame->pts, video_time_base, AV_TIME_BASE_Q)"
         << av_rescale_q(frame->pts, video_time_base, AV_TIME_BASE_Q) << endl;
    return (play_start_time + av_q2d(video_time_base) * last_pts) <
           av_gettime_relative();

//    return (play_start_time + (frame->pts + 1) * av_q2d(video_time_base)) < av_gettime_relative();
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
                if (stream_index_video == AVMEDIA_TYPE_UNKNOWN && (flag & FLAG_INIT_VIDEO) == FLAG_INIT_VIDEO) {
                    stream->discard = AVDISCARD_DEFAULT;
                    new_flag |= FLAG_INIT_VIDEO;
                    stream_index_video = i;
                    video_time_base = av_guess_frame_rate(avFormatContext, stream, nullptr);
                }
                break;
            case AVMEDIA_TYPE_AUDIO:
                if (stream_index_audio == AVMEDIA_TYPE_UNKNOWN && (flag & FLAG_INIT_AUDIO) == FLAG_INIT_AUDIO) {
                    stream->discard = AVDISCARD_DEFAULT;
                    new_flag |= FLAG_INIT_AUDIO;
                    stream_index_audio = i;
                }
                break;
            case AVMEDIA_TYPE_SUBTITLE:
                if (stream_index_subtitle == AVMEDIA_TYPE_UNKNOWN &&
                    (flag & FLAG_INIT_SUBTITLE) == FLAG_INIT_SUBTITLE) {
                    stream->discard = AVDISCARD_DEFAULT;
                    new_flag |= FLAG_INIT_SUBTITLE;
                    stream_index_subtitle = i;
                }
                break;
            default:
                break;
        }
    }

    display.initSDL(new_flag);

    if ((new_flag & FLAG_INIT_VIDEO) == FLAG_INIT_VIDEO) {
//        transferData->init_pkt_frame(pkt_video, frame_video);
        transferData->videoContext = initCodec(stream_index_video);
        display.initVideo(transferData->videoContext->width, transferData->videoContext->height);
    }
    if ((new_flag & FLAG_INIT_AUDIO) == FLAG_INIT_AUDIO) {
//        _init_pkt_frame(pkt_audio, frame_audio);
        transferData->audioContext = initCodec(stream_index_audio);
        audioParams = display.initAudio(transferData);
        if (audioParams == nullptr) {
            av_log(nullptr, AV_LOG_ERROR, "create audio param failed \n");
            return;
        }
        transferData->initSwr(*audioParams);
    }
    if ((new_flag & FLAG_INIT_SUBTITLE) == FLAG_INIT_SUBTITLE) {
//        _init_pkt_frame(pkt_subtitle, frame_subtitle);
        transferData->subtitleContext = initCodec(stream_index_subtitle);
    }
}


AVCodecContext *DecodeHelper::initCodec(int stream_index) {
    AVCodecContext *codecContext = avcodec_alloc_context3(nullptr);
    AVStream *stream = avFormatContext->streams[stream_index];

    if (codecContext == nullptr) {
        av_log(nullptr, AV_LOG_ERROR, "avcodec_alloc_context3 failed");

    }
    if (avcodec_parameters_to_context(codecContext, stream->codecpar) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "avcodec_parameters_to_context failed");
    }
    codecContext->pkt_timebase = stream->time_base;
    AVCodec *avCodec = avcodec_find_decoder(codecContext->codec_id);

    if (avCodec == nullptr) {
        av_log(nullptr, AV_LOG_ERROR, "avcodec_find_decoder failed");
    }

    if (avcodec_open2(codecContext, avCodec, NULL) != 0) {
        av_log(nullptr, AV_LOG_ERROR, "avcodec_open2 failed");
    }
    return codecContext;

}


int DecodeHelper::loop_read_frame(void *arg) {
    auto helper = (DecodeHelper *) arg;
    char error[1024];
    int ret;
//    int index;
    AVPacket *pkt_raw = av_packet_alloc();
    for (;;) {
//        cout << "loop" << endl;
        if (helper->transferData->enough_pkt()) {
//            cout << "packets are full!!" << endl;
            SDL_Delay(10);
            continue;
        }

        if ((ret = av_read_frame(helper->avFormatContext, pkt_raw)) < 0) {
            av_strerror(ret, error, sizeof(error));
//            av_log(nullptr, AV_LOG_ERROR, ret);
            cout << "av_read_frame failed:" << error << endl;
            break;
        }

        if (pkt_raw->pts < 0 || pkt_raw->pts == AV_NOPTS_VALUE) {
            cout << "pkt's time base is wrong" << endl;
            helper->transferData->unref_pkt(pkt_raw);
            continue;
        }

//        cout << "push pkt" << endl;
//        int index = pkt_raw->stream_index;
//        cout << index << ":" << pkt_raw->stream_index << endl;
        if (pkt_raw->stream_index == helper->stream_index_video) {
            helper->transferData->pkt_push(AVMEDIA_TYPE_VIDEO, pkt_raw);
        } else if (pkt_raw->stream_index == helper->stream_index_audio) {
            helper->transferData->pkt_push(AVMEDIA_TYPE_AUDIO, pkt_raw);
        } else if (pkt_raw->stream_index == helper->stream_index_subtitle) {
            helper->transferData->pkt_push(AVMEDIA_TYPE_SUBTITLE, pkt_raw);
        } else {
            helper->transferData->unref_pkt(pkt_raw);

        }

//        switch (pkt_raw->stream_index) {
//            case AVMEDIA_TYPE_VIDEO:
////                if (pkt_video == nullptr) {
////                    unref_pkt(pkt_raw);
////                    continue;
////                }
//                transferData->pkt_push(AVMEDIA_TYPE_VIDEO, pkt_raw);
//                break;
//            case AVMEDIA_TYPE_SUBTITLE:
////                if (pkt_video == nullptr) {
////                    unref_pkt(pkt_raw);
////                    continue;
////                }
//                transferData->pkt_push(AVMEDIA_TYPE_SUBTITLE, pkt_raw);
//                break;
//            case AVMEDIA_TYPE_AUDIO:
////                if (pkt_video == nullptr) {
////                    unref_pkt(pkt_raw);
////                    continue;
////                }
//                transferData->(AVMEDIA_TYPE_AUDIO, pkt_raw);
//                break;
//            default:
//                transferData->unref_pkt(pkt_raw);
//                break;
//        }
    }
    return 0;
}

//void DecodeHelper::pkt_push(queue<MyPacket> &q, AVPacket *pkt) {
//    MyPacket p(pkt);
//    q.push(p);
//    unref_pkt(pkt);
//}
//
//
//bool DecodeHelper::pkt_pop(queue<MyPacket> &q, AVPacket *pkt) {
//    if (q.size() < 0) {
//        return false;
//    }
//    MyPacket packet = q.front();
//    pkt = packet._packet;
//    unref_pkt(packet._packet);
//    return true;
//
//}
//
//void DecodeHelper::frame_push(queue<MyFrame> &q, AVFrame *frame) {
//    MyFrame p(frame);
//    q.push(p);
//    unref_frame(frame);
//}
//
//bool DecodeHelper::frame_pop(queue<MyFrame> &q, AVFrame *frame) {
//    if (q.size() < 0) {
//        return false;
//    }
//    MyFrame f = q.front();
//    frame = f._frame;
//    unref_frame(f._frame);
//    return true;
//}

//bool DecodeHelper::get_audio_frame(AVFrame *frame) {
//    return frame_pop(audio_frames, frame);
//}


//bool DecodeHelper::enough_pkt() {
//    return queue_audio.size() > QUEUE_MAX_PKT
//           || queue_video.size() > QUEUE_MAX_PKT
//           || queue_subtitle.size() > QUEUE_MAX_PKT;
//}

int DecodeHelper::read_video(void *arg) {
    auto *helper = (TransferData *) arg;
    AVPacket pkt;
    AVFrame *frame = av_frame_alloc();
    char error[1024];
    int ret;
    for (;;) {
        ret = avcodec_receive_frame(helper->videoContext, frame);
        switch (ret) {
            case 0:
                cout << "read_video" << endl;
                helper->frame_push(AVMEDIA_TYPE_VIDEO, frame);
                break;
            case AVERROR(EAGAIN):
                if (helper->pkt_pop(AVMEDIA_TYPE_VIDEO, &pkt)) {
                    if (avcodec_send_packet(helper->videoContext, &pkt) == AVERROR(EAGAIN)) {
                        cout << "READ VIDEO ERROR!!!!!!!!!!!" << endl;
                        return 0;
                    } else {
                        av_packet_unref(&pkt);
                    }
                } else {
                    cout << "pkt is null" << endl;
                    SDL_Delay(10);
                }
                break;
            case AVERROR_EOF:
//                cout << "eof?" << helper->queue_video.size() << endl;
                avcodec_flush_buffers(helper->videoContext);
                break;
            default:
                av_strerror(ret, error, sizeof(error));
                cout << "read video error!!" << error << endl;
                break;
        }
    }
}

int DecodeHelper::read_audio(void *arg) {
    auto *helper = (TransferData *) arg;
    AVPacket pkt;
    AVFrame *frame = av_frame_alloc();
    char error[1024];
    int ret;
    for (;;) {
        ret = avcodec_receive_frame(helper->audioContext, frame);
        switch (ret) {
            case 0:
//                cout << "receive a frame" << endl;
                helper->frame_push(AVMEDIA_TYPE_AUDIO, frame);
                break;
            case AVERROR(EAGAIN):
                if (helper->pkt_pop(AVMEDIA_TYPE_AUDIO, &pkt)) {
//                    cout << "pop pkt" << endl;
                    if (avcodec_send_packet(helper->audioContext, &pkt) == AVERROR(EAGAIN)) {
                        cout << "READ AUDIO ERROR!!!!!!!!!!!" << endl;
                        return 0;
                    } else {
//                        cout << "successful" << endl;
                        av_packet_unref(&pkt);
                    }
                } else {
                    SDL_Delay(10);
                }
                break;
            case AVERROR_EOF:
                cout << "eof?" << endl;
                avcodec_flush_buffers(helper->audioContext);
                break;
            default:
                av_strerror(ret, error, sizeof(error));
                cout << "read audio error!!" << error << endl;
                break;
        }
    }
}

//void DecodeHelper::initSwr() {
//    if (audioParams == nullptr || transferData->audioContext == nullptr) {
//        return;
//    }
//    swrContext = swr_alloc_set_opts(nullptr,
//                                    audioParams->channel_layout,
//                                    audioParams->fmt,
//                                    audioParams->freq,
//                                    transferData->audioContext->channel_layout,
//                                    transferData->audioContext->sample_fmt,
//                                    transferData->audioContext->sample_rate,
//                                    0, nullptr);
//    if (swrContext == nullptr) {
//        cout << "swr_alloc_set_opts failed" << endl;
//        return;
//    }
//    swr_init(swrContext);
//}

//void DecodeHelper::swr_audio_frame(AVFrame *frame) {
//
//
//}

