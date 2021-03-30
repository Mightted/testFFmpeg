//
// Created by xk on 2021/3/24.
//

#include "MyFrame.h"

//MyFrame::MyFrame() {
//    _frame = av_frame_alloc();
//}

MyFrame::MyFrame(AVFrame *frame) {
    _frame = av_frame_alloc();
    av_frame_ref(_frame, frame);
    AV_NOPTS_VALUE
//    _frame = frame;
}
