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
#include <omp.h>



//gets a chunk from disk, if it doesnt exist it generates it fresh with the given seed
int getChunk(chunk_t* chunk, int chunkX, int chunkY, long long int seed){
    if(loadChunk(chunk, worldName, chunkX, chunkY)){ //try to load chunk from file. if it doesnt exist then generate it
        generateChunk(chunk, chunkX, chunkY, seed);
        chunk->x = chunkX;
        chunk->y = chunkY;
    }
    return 0;
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

//checks if a chunk exists within the given chunk arary
int isChunkInChunkArray(int chunkX, int chunkY, chunkArray_t* chunkArray){
    for(int x = 0; x <= chunkArray->width; ++x){
        for(int y = 0; y <= chunkArray->height; ++y){
            if(chunkArray->chunkArray[x][y].x == chunkX && chunkArray->chunkArray[x][y].y == chunkY){
                return 1; //chunk was found
            }
        }
    }
    return 0; //chunk was not found
}

chunkArray_t* getChunkArray(int chunkXStart, int chunkYStart, int width, int height, chunkArray_t* chunkArray, int generateAll, Tigr* screen, long long int seed){
    chunkArray->width = (screen->w / 512) + 2;
    chunkArray->height = (screen->h / 512) + 2;
    chunkArray_t tempChunkArray;
    tempChunkArray.width = chunkArray->width;
    tempChunkArray.height = chunkArray->height;

    if(generateAll){ //if we dont want to load chunks from memory and want to load them all from disk, or generate them fresh
        #pragma omp parallel for
        for(int x = chunkXStart; x <= chunkXStart + width; ++x){
            #pragma omp parallel for
            for(int y = chunkYStart; y <= chunkYStart + height; ++y){
                getChunk(&tempChunkArray.chunkArray[x - chunkXStart][y - chunkYStart], x, y, seed);
            }
        }
        //printf("\nDone!");
    } else {
        #pragma omp parallel for
        for(int x = chunkXStart; x <= chunkXStart + width; ++x){
            #pragma omp parallel for
            for(int y = chunkYStart; y <= chunkYStart + height; ++y){
                if(getChunkFromChunkArray(x, y, chunkArray, &tempChunkArray.chunkArray[x - chunkXStart][y - chunkYStart])){
                    //printf("\nChunk loaded %d %d (local %d %d) - not generated again\n", x, y, x - chunkXStart, y - chunkYStart);
                } else {
                    //printf("\nGenerating chunk %d %d (local %d %d)\n", x, y, x - chunkXStart, y - chunkYStart);
                    getChunk(&tempChunkArray.chunkArray[x - chunkXStart][y - chunkYStart], x, y, seed);
                }
            }
        }
        //printf("\nDone!");
    }
    memcpy(chunkArray, &tempChunkArray, sizeof(chunkArray_t));
    return chunkArray;
}

int renderChunk(chunk_t* chunk, Tigr* screen, Tigr* textureAtlas, entity_t* player, int offsetX, int offsetY, entity_t* targetedBlock){
    if((offsetX + 512 < 0) || (offsetY + 512 < 0) || (offsetX > screen->w) || (offsetY > screen->h)){
        return 10;
    }
    #pragma omp parallel for
    for(int y = 0; y < 16; ++y){
        #pragma omp parallel for
        for(int x = 0; x < 16; ++x){
            if(!chunk->blocks[x][y].id && chunk->walls[x][y].id){
                renderWall(screen, textureAtlas, &chunk->walls[x][y], offsetX + (x * 32), offsetY + (y * 32));
            } else {
                renderBlock(screen, textureAtlas, &chunk->blocks[x][y], offsetX + (x * 32), offsetY + (y * 32));
            }


            //renderes the player as a missing texture for debugging
            if(chunk->x == player->chunkX && chunk->y == player->chunkY && x == player->xBlockInChunk && y == player->yBlockInChunk){
                block_t missing;
                missing.id = 99;
                renderBlock(screen, textureAtlas, &missing, offsetX + (x * 32), offsetY + (y * 32));
            }

            //renderes the targeted block overlay
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
    int xOffset = ((int)wierdX) - player->pixelX - 512; //why does this alogorithm work? even though i made it, i dont know.
    int yOffset = ((int)wierdY - player->pixelY) - 512;

    //works out which chunks it needs to render, gets them and renders them

    #pragma omp parallel for
    for(int loopY = 0; loopY < amountOfChunksY + 1; ++loopY){
        #pragma omp parallel for
        for(int loopX = 0; loopX < amountOfChunksX + 1; ++loopX){
            renderChunk(&loadedChunks->chunkArray[loopX][loopY], screen, textureAtlas, player, xOffset + (loopX * 512), yOffset + (loopY * 512), targetedBlock);
        }
    }

    //just incase we need the old chunk renderer later

    // int loopX = 0;
    // int loopY = 0;
    // for(int currentCY = player->chunkY - (amountOfChunksY / 2); currentCY < player->chunkY + (amountOfChunksY / 2) + 1; ++currentCY){ //extra plus one because it gets lost in truncating
    //     loopX = 0; //this is so messy as i redid it to support chunkArrays but it broke when i tried to make it look nicer, so have fun with this mess
    //     for(int currentCX = player->chunkX - (amountOfChunksX / 2 ); currentCX < player->chunkX + (amountOfChunksX / 2) + 1; ++currentCX){ //same in this loop
    //         //printf("\nAbout to get chunk %d %d from chunk array\nTotal loop iterations: %d", loopX, loopY, amountOfChunksY * amountOfChunksX);
    //         chunk_t* chunk = &loadedChunks->chunkArray[loopX][loopY];
    //         //printf("\nDone!");
    //         renderChunk(chunk, screen, textureAtlas, player, currentXOffset, currentYOffset, targetedBlock);
    //         currentXOffset += 512;
    //         ++loopX;
    //     }
    //     currentXOffset = ((int)wierdX) - player->pixelX - 512;
    //     currentYOffset += 512;
    //     ++loopY;
    // }
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

int worldXToChunkCords(int* chunkX, int* xInChunk, int x){
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
}

int worldYToChunkCords(int* chunkY, int* yInChunk, int y){
    int i = y;
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

int chunkXToWorldCords(int* x, int chunkX, int xInChunk){
    *x = (chunkX * 16) + xInChunk;
}

int chunkYToWorldCords(int* y, int chunkY, int yInChunk){
    *y = (chunkY * 16) + yInChunk;
}
