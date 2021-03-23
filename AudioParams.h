//
// Created by xk on 2021/3/17.
//

#ifndef TESTFFMPEG_AUDIOPARAMS_H
#define TESTFFMPEG_AUDIOPARAMS_H


#include <cstdint>

extern "C" {
#include "libavformat/avformat.h"
};


class AudioParams {
public:
    int freq;
    int channels;
    int64_t channel_layout;
    AVSampleFormat fmt;
    int frame_size;
    int bytes_per_sec;

};


#endif //TESTFFMPEG_AUDIOPARAMS_H
