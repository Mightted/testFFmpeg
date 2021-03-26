//
// Created by xk on 2021/3/24.
//

#include "Player.h"


void Player::init(char *path, int flag) {
//    avformat_open_input(&avFormatContext, path, nullptr, nullptr);
//    avformat_find_stream_info(avFormatContext, nullptr);
//
//    int new_flag = 0;
//
//    for (int i = 0; i < avFormatContext->nb_streams; i++) {
//        AVStream *stream = avFormatContext->streams[i];
//        stream->discard = AVDISCARD_ALL;
//        switch (stream->codecpar->codec_type) {
//            case AVMEDIA_TYPE_VIDEO:
//                if (helper.getStream_index_video() == AVMEDIA_TYPE_UNKNOWN && flag & FLAG_INIT_VIDEO) {
//                    stream->discard = AVDISCARD_DEFAULT;
//                    new_flag |= FLAG_INIT_VIDEO;
//                    helper.setStream_index_video(i);
//                }
//                break;
//            case AVMEDIA_TYPE_AUDIO:
//                if (helper.getStream_index_audio()  == AVMEDIA_TYPE_UNKNOWN && flag & FLAG_INIT_AUDIO) {
//                    stream->discard = AVDISCARD_DEFAULT;
//                    new_flag |= FLAG_INIT_AUDIO;
//                    helper.setStream_index_audio(i);
//                }
//                break;
//            case AVMEDIA_TYPE_SUBTITLE:
//                if (helper.getStream_index_subtitle()  == AVMEDIA_TYPE_UNKNOWN &&flag & FLAG_INIT_SUBTITLE) {
//                    stream->discard = AVDISCARD_DEFAULT;
//                    new_flag |= FLAG_INIT_SUBTITLE;
//                    helper.setStream_index_subtitle(i);
//                }
//                break;
//            default:
//                break;
//        }
//    }

    helper.init(path, flag);

}

void Player::play() {
//    helper.loop_read_frame(avFormatContext);
}
