//
// Created by xk on 2021/3/11.
//

#ifndef TESTFFMPEG_VIDEOSTATE_H
#define TESTFFMPEG_VIDEOSTATE_H

//extern "C" {
//#include "libavformat/avformat.h"
//}
//
//#include "SDL.h"
#include "SDL2/SDL_thread.h"
#include "AVCommon.h"
#include "Audio.h"
#include "Video.h"
#include <string>

using namespace std;


class VideoState {


public:

    AVFormatContext *avFormatContext = nullptr;

    AVDictionary *avformat_opts = nullptr;
    AVCommon *video, *audio;
    SDL_Thread *aud_thread_id = nullptr;

    double frame_start_time = 0;
    double cur_frame_time = 0;
    double cur_duration = 0;

//    int index_video_stream, index_audio_stream, index_subtitle_stream;
    char error_str[1024];


    VideoState(string file);

    ~VideoState();

    void loop();

//    static int read_thread(void *arg);

    bool sdl_init(int width = 320, int height = 180);

    bool ff_init();


    void video_refresh();


private:


//    SDL_Window *window = nullptr;
//    SDL_Renderer *renderer = nullptr;
//    SDL_Texture *texture = nullptr;
    SDL_Thread *read_thread_id = nullptr;
    string file_name;



//    void audio_read_frame();

    static void sdl_audio_callback(void *opaque, Uint8 *stream, int len);
};


#endif //TESTFFMPEG_VIDEOSTATE_H
