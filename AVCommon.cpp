//
// Created by xk on 2021/3/17.
//

#include "AVCommon.h"


bool AVCommon::init(AVFormatContext *context) {
    if (index_stream == AVMEDIA_TYPE_UNKNOWN) {
        return false;
    }

    avCodecContext = avcodec_alloc_context3(NULL);
    if (avCodecContext == NULL) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_alloc_context3 failed");
        return false;
    }
    if (avcodec_parameters_to_context(avCodecContext, context->streams[index_stream]->codecpar) < 0) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_parameters_to_context failed");
        return false;
    }
    avCodecContext->time_base = context->streams[index_stream]->time_base;
    avCodec = avcodec_find_decoder(avCodecContext->codec_id);

    if (avCodec == NULL) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_find_decoder failed");
        return false;
    }

    return codec_open(context);


}

bool AVCommon::codec_open(AVFormatContext *context) {
    if (avcodec_open2(avCodecContext, avCodec, NULL) != 0) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_open2 failed");
        return false;
    }

//    AVCodecParameters *parameters = context->streams[index_stream]->codecpar;
//    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, parameters->width,
//                                parameters->height);

    avPacket = av_packet_alloc();
    avFrame = av_frame_alloc();

    time_base = av_guess_frame_rate(context, context->streams[index_stream], nullptr);
    return avPacket != nullptr && avFrame != nullptr;;
}

int AVCommon::read_frame(AVFormatContext *context) {
    if (need_update_pkt) {
        if (av_read_frame(context, avPacket) == 0) {
            if (avPacket->stream_index == index_stream) {
                cout << "av_read_frame" << endl;
                need_update_pkt = false;
            } else {
                av_packet_unref(avPacket);
            }
        } else {
            return -1;
        }
    }
    return 0;
}
