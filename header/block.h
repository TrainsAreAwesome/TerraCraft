#pragma once
#include "tigr.h"
#include <stdint.h>

enum blocks{
    AIR,
    STONE,
    GRASS,
    COBBLESTONE,
    OAK_LOG,
    DIRT
};

typedef struct {
    uint16_t id;
    uint16_t state;
    uint16_t light;
        
    
} block_t;

#define RED 0b1111000000000000
#define GREEN 0b0000111100000000
#define BLUE 0b0000000011110000
#define BRIGHTNESS 0b0000000000001111
int getTextureAtlasCords(int blockID, int* x, int* y);
int renderBlock(Tigr* screen, Tigr* textureAtlas, block_t* block, int x, int y);