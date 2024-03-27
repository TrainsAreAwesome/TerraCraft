#include "../header\wall.h"
#include "../header\tigr.h"
#include "../header\block.h"
#include <stdio.h>

int getTextureAtlasCordsWall(uint16_t wallID, int* x, int* y){
    switch (wallID)
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

int renderWall(Tigr* screen, Tigr* textureAtlas, wall_t* wall, int x, int y){
    int textureX, textureY;
    getTextureAtlasCordsWall(wall->id, &textureX, &textureY);
    customBlitTintNoAlpha(screen, textureAtlas, x, y, textureX, textureY, 32, 32, ((wall->light & RED) >> 12) * 17, ((wall->light & GREEN) >> 8) * 17, ((wall->light & BLUE) >> 4) * 17, 255);
    return 0;
}