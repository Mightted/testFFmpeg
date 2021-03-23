//
// Created by xk on 2021/3/17.
//

#include "Video.h"


bool Video::sdl_init() {

    int winFlag = SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 320, 180, winFlag);
    if (!window) {
        av_log(nullptr, AV_LOG_ERROR, "SDL_CreateWindow failed");
//        cout << "SDL_CreateWindow failed" << endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        av_log(NULL, AV_LOG_WARNING, "SDL_CreateRenderer failed %s\n", SDL_GetError());
        renderer = SDL_CreateRenderer(window, -1, 0);
        if (!renderer) {
            av_log(NULL, AV_LOG_ERROR, "SDL_CreateRenderer error");
            return false;
        }
    }

//    AVCodecParameters *parameters = context->streams[index_stream]->codecpar;
//    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, 320, 180);

    return true;
}

void Video::play() {

    if (texture == nullptr) {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, avCodecContext->width,
                                    avCodecContext->height);
//        av_log(NULL, AV_LOG_ERROR, "width?:%d, height?:%d", avCodecContext->width, avCodecContext->height);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
//            SDL_UpdateTexture(texture, nullptr, avFrame->data[0], avFrame->linesize[0]);
    SDL_UpdateYUVTexture(texture, nullptr,
                         avFrame->data[0], avFrame->linesize[0],
                         avFrame->data[1], avFrame->linesize[1],
                         avFrame->data[2], avFrame->linesize[2]);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);

}
