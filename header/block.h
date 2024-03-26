#pragma once
#include "tigr.h"
#include <stdint.h>

enum blocks{
    AIR,
    STONE,
    GRASS,
    COBBLESTONE,
    OAK_LOG
};

typedef struct {
    uint16_t id;
    uint8_t state;
} block_t;
int getTextureAtlasCords(int blockID, int* x, int* y);
int renderBlock(Tigr* screen, Tigr* textureAtlas, block_t* block, int x, int y);