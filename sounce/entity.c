
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../header\entity.h"
#include "../header\tigr.h"
#include "../header\cordinates.h"

int calculatePixelX(entity_t* entity){
    entity->pixelX = (int)((entity->xFloat - (double)(entity->chunkX * 16)) / 16 * 512);
}

int calculatePixelY(entity_t* entity){
    entity->pixelY = (int)((entity->yFloat - (double)(entity->chunkY * 16)) / 16 * 512);
}

int moveEntityUp(entity_t* entity, double speed){

    entity->yFloat -= speed;
    entity->yInt = round(entity->yFloat);
    int i = entity->yInt;
    if(entity->yInt >= 0 && entity->yInt < 16){
        i = 0;
        goto finishU;
    }
    while(abs(i) % 16 != 0){
        --i;
    }
    finishU: entity->chunkY = i / 16;
    entity->pixelY = (int)((entity->yFloat - (double)(entity->chunkY * 16)) / 16 * 512);
    entity->yBlockInChunk = entity->yInt - (entity->chunkY * 16);
    return 0;
}

int moveEntityDown(entity_t* entity, double speed){
    entity->yFloat += speed;
    entity->yInt = round(entity->yFloat);
    int i = entity->yInt;
    if(entity->yInt >= 0 && entity->yInt < 16){
        i = 0;
        goto finishD;
    }
    while(abs(i) % 16 != 0){
        --i;
    }
    finishD: entity->chunkY = i / 16;
    entity->pixelY = (int)((entity->yFloat - (double)(entity->chunkY * 16)) / 16 * 512);
    entity->yBlockInChunk = entity->yInt - (entity->chunkY * 16);
    return 0;
}

int moveEntityLeft(entity_t* entity, double speed){
    entity->xFloat -= speed;
    entity->xInt = round(entity->xFloat);
    int i = entity->xInt;
    if(entity->xInt >= 0 && entity->xInt < 16){
        i = 0;
        goto finishL;
    }
    while(abs(i) % 16 != 0){
        --i;
    }
    
    finishL: entity->chunkX = i / 16;
    entity->pixelX = (int)((entity->xFloat - (double)(entity->chunkX * 16)) / 16 * 512);
    entity->xBlockInChunk = entity->xInt - (entity->chunkX * 16);
    return 0;
}

int moveEntityRight(entity_t* entity, double speed){
    entity->xFloat += speed;
    entity->xInt = round(entity->xFloat);
    int i = entity->xInt;
    if(entity->xInt >= 0 && entity->xInt < 16){
        i = 0;
        goto finishR;
    }
    while(abs(i) % 16 != 0){
        --i;
    }
    finishR: entity->chunkX = i / 16;
    entity->pixelX = (int)((entity->xFloat - (double)(entity->chunkX * 16)) / 16 * 512);
    entity->xBlockInChunk = entity->xInt - (entity->chunkX * 16);
    return 0;
}
