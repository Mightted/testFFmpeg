//
// Created by xk on 2021/3/24.
//

#ifndef TESTFFMPEG_MYFRAME_H
#define TESTFFMPEG_MYFRAME_H

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include "libavutil/time.h"
};

class MyFrame {
public:
//    MyFrame();

    MyFrame(AVFrame *frame);

    AVFrame *_frame = nullptr;
    double pts = 0;


};


#endif //TESTFFMPEG_MYFRAME_H
