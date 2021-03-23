//
// Created by xk on 2021/3/17.
//

#ifndef TESTFFMPEG_VIDEO_H
#define TESTFFMPEG_VIDEO_H


#include "AVCommon.h"

class Video : public AVCommon {
public:
//    bool codec_open(AVFormatContext *context) override;

    bool sdl_init();

    void play() override;

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;

};


#endif //TESTFFMPEG_VIDEO_H
