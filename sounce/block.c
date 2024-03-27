#include <stdio.h>
#include "../header/block.h"
#include "../header/chunk.h"
#include "../header/tigr.h"

int getTextureAtlasCords(int blockID, int* x, int* y){
    switch (blockID)
    {
    case 0:
        *x = 0; *y = 0;
        break;
    case 1:
        *x = 32; *y = 0;
        break;
    case 2:
        *x = 64; *y = 0;
        break;
    case 3:
        *x = 96; *y = 0;
        break;
    case 4:
        *x = 128; *y = 0;
        break;
    case 5:
        *x = 160; *y = 0;
        break;
    default:
        *x = 96; *y = 32;
        break;
    }
}

int renderBlock(Tigr* screen, Tigr* textureAtlas, block_t* block, int x, int y){
    int textureX, textureY;
    getTextureAtlasCords(block->id, &textureX, &textureY);
    if(!block->light){ //if the block is just dark, dont render it (it would be invisible)
       return 0;
    } else if((block->light & 0b1111111111110000) == 0b1111111111110000){ //if the block is completly bright, we can just copy the texture
        tigrBlit(screen, textureAtlas, x, y, textureX, textureY, 32, 32);
    } else { //otherwise use the slower renderer that supports tints
        customBlitTintNoAlpha(screen, textureAtlas, x, y, textureX, textureY, 32, 32, (block->light & RED >> 12) * 17, (block->light & GREEN >> 8) * 17, (block->light & BLUE >> 4) * 17, 255);
    }
    return 0;
}