#include "../header\block.h"
#include "../header\chunk.h"
#include "../header\tigr.h"
#include "../header\player.h"
#include "../header\entity.h"
#include "../header\saveAndLoad.h"
#include "../header\generateTerrain.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memcpy

int getChunk(chunk_t* chunk, int chunkX, int chunkY){
    if(loadChunk(chunk, worldName, chunkX, chunkY)){ //try to load chunk from file. if it doesnt exist then generate it
        printf("\nIN GETCHUNK WITH CHUNK CORDS %d %d", chunkX, chunkY);
        generateTerrainSurface(chunkX, chunkY, 12345, chunk);
        chunk->x = chunkX;
        chunk->y = chunkY;
    }
    return 0;
}

int moveCArrayX(chunkArray_t* chunkArray){
    for(int x = chunkArray->width; x > 0; --x){
        for(int y = chunkArray->height; y > 0; --y){
            chunkArray->chunkArray[x][y] = chunkArray->chunkArray[x - 1][y];
        }
    }
}

int moveCArrayY(chunkArray_t* chunkArray){
    for(int x = chunkArray->width; x > 0; --x){
        for(int y = chunkArray->height; y > 0; --y){
            chunkArray->chunkArray[x][y] = chunkArray->chunkArray[x][y - 1];
        }
    }
}

int getChunkFromChunkArray(int chunkX, int chunkY, chunkArray_t* chunkArray, chunk_t* returnChunk){
    //printf("\nLooking for chunk %d %d", chunkX, chunkY);
    for(int x = 0; x <= chunkArray->width; ++x){
        for(int y = 0; y <= chunkArray->height; ++y){
            if(chunkArray->chunkArray[x][y].x == chunkX && chunkArray->chunkArray[x][y].y == chunkY){
                memcpy(returnChunk, &chunkArray->chunkArray[x][y], sizeof(chunk_t));
                //returnChunk = &chunkArray->chunkArray[x][y];
                //printf("\n200 chunk found");
                return 1;
            }
        }
    }
    //printf("\n404 chunk not found");
    return 0;
}

chunkArray_t* getChunkArray(int chunkXStart, int chunkYStart, int width, int height, chunkArray_t* chunkArray, int generateAll, Tigr* screen){
    chunkArray->width = (screen->w / 512) + 2;
    chunkArray->height = (screen->h / 512) + 2;
    chunk_t tempChunk;
    chunkArray_t tempChunkArray;
    tempChunkArray.width = chunkArray->width;
    tempChunkArray.height = chunkArray->height;

    if(generateAll){
        //printf("\nGenerating all new chunks...");
        for(int x = chunkXStart; x <= chunkXStart + width; ++x){
            for(int y = chunkYStart; y <= chunkYStart + height; ++y){
                getChunk(&tempChunkArray.chunkArray[x - chunkXStart][y - chunkYStart], x, y);
            }
        }
        //printf("\nDone!");
    } else {
        for(int x = chunkXStart; x <= chunkXStart + width; ++x){
            for(int y = chunkYStart; y <= chunkYStart + height; ++y){
                if(getChunkFromChunkArray(x, y, chunkArray, &tempChunk)){
                    memcpy(&tempChunkArray.chunkArray[x - chunkXStart][y - chunkYStart], &tempChunk, sizeof(chunk_t));
                    //printf("\nChunk loaded %d %d (local %d %d) - not generated again\n", x, y, x - chunkXStart, y - chunkYStart);
                } else {
                    //printf("\nGenerating chunk %d %d (local %d %d)\n", x, y, x - chunkXStart, y - chunkYStart);
                    getChunk(&tempChunkArray.chunkArray[x - chunkXStart][y - chunkYStart], x, y);
                }
            }
        }
        //printf("\nDone!");
    }
    memcpy(chunkArray, &tempChunkArray, sizeof(tempChunkArray));
    return chunkArray;
}

int renderChunk(chunk_t* chunk, Tigr* screen, Tigr* textureAtlas, entity_t* player, int offsetX, int offsetY, entity_t* targetedBlock){
    if((offsetX + 512 < 0) || (offsetY + 512 < 0) || (offsetX > screen->w) || (offsetY > screen->h)){
        return 10;
    }
    for(int y = 0; y < 16; ++y){
        for(int x = 0; x < 16; ++x){
            renderBlock(screen, textureAtlas, &chunk->blocks[x][y], offsetX + (x * 32), offsetY + (y * 32));
            if(chunk->x == player->chunkX && chunk->y == player->chunkY && x == player->xBlockInChunk && y == player->yBlockInChunk){
                block_t missing;
                missing.id = 99;
                renderBlock(screen, textureAtlas, &missing, offsetX + (x * 32), offsetY + (y * 32));
            }
            if(chunk->x == targetedBlock->chunkX && chunk->y == targetedBlock->chunkY && x == targetedBlock->xBlockInChunk && y == targetedBlock->yBlockInChunk && targetedBlock->health){
                tigrBlitAlpha(screen, textureAtlas, offsetX + (x * 32), offsetY + (y * 32), 32, 32, 32, 32, 1);
            }

        }
    }
}

int renderWorld(entity_t* player, Tigr* screen, Tigr* textureAtlas, chunkArray_t* loadedChunks, entity_t* targetedBlock){
    int amountOfChunksX = (screen->w / 512) + 2; //getting screen info
    int amountOfChunksY = (screen->h / 512) + 2;
    double wierdX, wierdY;
    wierdX = 240; //for 1440p: 240 for 1080p: 432
    wierdY = 192; //for 1440p: 192 for 1080p: 12
    //printf("\nwierdX: %lf wierdY: %lf", wierdX, wierdY);
    int currentXOffset = ((int)wierdX) - player->pixelX - 512; //why does this alogorithm work? even though i made it, i dont know.
    int currentYOffset = ((int)wierdY - player->pixelY) - 512;

    //works out which chunks it needs to render, gets them and renders them
    int loopX = 0;
    int loopY = 0;
    for(int currentCY = player->chunkY - (amountOfChunksY / 2); currentCY < player->chunkY + (amountOfChunksY / 2) + 1; ++currentCY){ //extra plus one because it gets lost in truncating
        loopX = 0; //this is so messy as i redid it to support chunkArrays but it broke when i tried to make it look nicer, so have fun with this mess
        for(int currentCX = player->chunkX - (amountOfChunksX / 2 ); currentCX < player->chunkX + (amountOfChunksX / 2) + 1; ++currentCX){ //same in this loop
            //printf("\nAbout to get chunk %d %d from chunk array\nTotal loop iterations: %d", loopX, loopY, amountOfChunksY * amountOfChunksX);
            chunk_t* chunk = &loadedChunks->chunkArray[loopX][loopY];
            //printf("\nDone!");
            renderChunk(chunk, screen, textureAtlas, player, currentXOffset, currentYOffset, targetedBlock);
            currentXOffset += 512;
            ++loopX;
        }
        currentXOffset = ((int)wierdX) - player->pixelX - 512;
        currentYOffset += 512;
        ++loopY;
    }
}


int prosesPlayerBlockStuff(entity_t* player, entity_t* targetedBlock, chunkArray_t* loadedChunks, Tigr* screen){
    int amountOfChunksX = screen->w / 512 + 2;
    int amountOfChunksY = screen->h / 512 + 2;
    int mouseX, mouseY, mouseButtons;
    int chunkXDifference = targetedBlock->chunkX - player->chunkX;
    int chunkYDifference = targetedBlock->chunkY - player->chunkY;
    tigrMouse(screen, &mouseX, &mouseY, &mouseButtons);
    if(!targetedBlock->health){
        return 1; //if no block in range is targeted, return. the "health" of the entity is used to store that.
    }
    if(mouseButtons & 1){
        loadedChunks->chunkArray[(amountOfChunksX / 2) + chunkXDifference][(amountOfChunksY / 2) + chunkYDifference].blocks[targetedBlock->xBlockInChunk][targetedBlock->yBlockInChunk].id = 0;
        loadedChunks->chunkArray[(amountOfChunksX / 2) + chunkXDifference][(amountOfChunksY / 2) + chunkYDifference].explored = true;
    } if((mouseButtons & 4) && (loadedChunks->chunkArray[(amountOfChunksX / 2) + chunkXDifference][(amountOfChunksY / 2) + chunkYDifference].blocks[targetedBlock->xBlockInChunk][targetedBlock->yBlockInChunk].id == 0)){
        loadedChunks->chunkArray[(amountOfChunksX / 2) + chunkXDifference][(amountOfChunksY / 2) + chunkYDifference].explored = true;
        loadedChunks->chunkArray[(amountOfChunksX / 2) + chunkXDifference][(amountOfChunksY / 2) + chunkYDifference].blocks[targetedBlock->xBlockInChunk][targetedBlock->yBlockInChunk].id = 2;
    }
}

int setExplored(entity_t* entity, chunkArray_t* loadedChunks, int amountChunksX, int amountChunksY){
    loadedChunks->chunkArray[amountChunksX / 2][amountChunksY / 2].explored = true;
    printf("\n acx/2 %d acy/2 %d just set %d %d explored true", amountChunksX / 2, amountChunksY / 2, loadedChunks->chunkArray[amountChunksX / 2][amountChunksY / 2].x, loadedChunks->chunkArray[amountChunksX / 2][amountChunksY / 2].y);
    return 0;
}

int worldToChunkCords(int* chunkX, int* chunkY, int* xInChunk, int* yInChunk, int x, int y){
    int i = x;
    if(x >= 0 && x < 16){
        i = 0;
        goto finishX;
    }
    while(abs(i) % 16 != 0){
        --i;
    }
    finishX: *chunkX = i / 16;
    *xInChunk = x - (*chunkX * 16);

    i = y;
    if(y >= 0 && y < 16){
        i = 0;
        goto finishY;
    }
    while(abs(i) % 16 != 0){
        --i;
    }
    finishY: *chunkY = i / 16;
    return 0;
    *yInChunk = y - (*chunkY * 16);
}

int chunkToWorldCords(int* x, int* y, int chunkX, int chunkY, int xInChunk, int yInChunk){
    *x = (chunkX * 16) + xInChunk;
    *y = (chunkY * 16) + yInChunk;
}