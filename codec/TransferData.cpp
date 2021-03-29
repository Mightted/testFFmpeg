//
// Created by xk on 2021/3/29.
//

#include "TransferData.h"


void TransferData::init_pkt_frame(MyPacket *pkt, MyFrame *frame) {
    pkt = new MyPacket();
    frame = new MyFrame();
}


void TransferData::pkt_push(AVMediaType type, AVPacket *pkt) {
    switch(type) {
        case AVMEDIA_TYPE_VIDEO:
            _pkt_push(queue_video, pkt);
            break;
        case AVMEDIA_TYPE_AUDIO:
            _pkt_push(queue_audio, pkt);
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            _pkt_push(queue_subtitle, pkt);
            break;
        default:
            break;
    }
}

void TransferData::_pkt_push(queue<MyPacket> &q, AVPacket *pkt) {
    MyPacket p(pkt);
    q.push(p);
    unref_pkt(pkt);
}


bool TransferData::pkt_pop(AVMediaType type, AVPacket *pkt) {
    switch(type) {
        case AVMEDIA_TYPE_VIDEO:
            return _pkt_pop(queue_video, pkt);
        case AVMEDIA_TYPE_AUDIO:
            return _pkt_pop(queue_audio, pkt);
        case AVMEDIA_TYPE_SUBTITLE:
            return _pkt_pop(queue_subtitle, pkt);
        default:
            break;
    }
    return false;
}


bool TransferData::_pkt_pop(queue<MyPacket> &q, AVPacket *pkt) {
    if (q.size() < 0) {
        return false;
    }
    MyPacket packet = q.front();
    pkt = packet._packet;
    unref_pkt(packet._packet);
    return true;

}


void TransferData::frame_push(AVMediaType type, AVFrame *frame) {
    switch(type) {
        case AVMEDIA_TYPE_VIDEO:
            _frame_push(video_frames, frame);
            break;
        case AVMEDIA_TYPE_AUDIO:
            _frame_push(audio_frames, frame);
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            _frame_push(subtitle_frames, frame);
            break;
        default:
            break;
    }
}



void TransferData::_frame_push(queue<MyFrame> &q, AVFrame *frame) {
    MyFrame p(frame);
    q.push(p);
    unref_frame(frame);
}



bool TransferData::frame_pop(AVMediaType type, AVFrame *frame) {
    switch(type) {
        case AVMEDIA_TYPE_VIDEO:
            return _frame_pop(video_frames, frame);
        case AVMEDIA_TYPE_AUDIO:
            return _frame_pop(audio_frames, frame);
        case AVMEDIA_TYPE_SUBTITLE:
            return _frame_pop(subtitle_frames, frame);
        default:
            break;
    }
    return false;
}



bool TransferData::_frame_pop(queue<MyFrame> &q, AVFrame *frame) {
    if (q.size() < 0) {
        return false;
    }
    MyFrame f = q.front();
    frame = f._frame;
    unref_frame(f._frame);
    return true;
}

bool TransferData::enough_pkt() {
    return queue_audio.size() > QUEUE_MAX_PKT
           || queue_video.size() > QUEUE_MAX_PKT
           || queue_subtitle.size() > QUEUE_MAX_PKT;
}


bool TransferData::get_audio_frame(AVFrame *frame) {
    return _frame_pop(audio_frames, frame);
}


void TransferData::swr_audio_frame(AVFrame *frame) {

}
