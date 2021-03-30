//
// Created by xk on 2021/3/24.
//

#include "Player.h"


void Player::init(char *path, int flag) {
    helper.init(path, flag);
    helper.start_read_frame();

}

void Player::play() {
    bool doExit = false;

    SDL_Event event;
    while (!doExit) {
        SDL_PumpEvents();
        while (!SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
//            cout << "video_refresh" <<endl;
            helper.video_refresh();
            SDL_PumpEvents();
        }
        switch (event.type) {
            case SDL_QUIT:
                doExit = true;
                break;
            default:
                break;
        }
    }
}
