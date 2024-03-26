#pragma once
#include "block.h"

typedef struct{
    uint16_t id;
    uint8_t state;
} wall_t;
int renderWall(Tigr* screen, Tigr* textureAtlas, wall_t* wall, int x, int y);