//
// Created by xk on 2021/3/24.
//

#ifndef TESTFFMPEG_MYFRAME_H
#define TESTFFMPEG_MYFRAME_H

extern "C" {
#include <libavformat/avformat.h>
};

class MyFrame {
public:
    MyFrame();
    MyFrame(AVFrame* frame);
    AVFrame *_frame = nullptr;


};


#endif //TESTFFMPEG_MYFRAME_H