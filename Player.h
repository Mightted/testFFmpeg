//
// Created by xk on 2021/3/24.
//

#ifndef TESTFFMPEG_PLAYER_H
#define TESTFFMPEG_PLAYER_H

#include "codec/DecodeHelper.h"


class Player {
private:
    DecodeHelper helper;


public:
    void init(char *path, int flag);

    void play();
};


#endif //TESTFFMPEG_PLAYER_H
