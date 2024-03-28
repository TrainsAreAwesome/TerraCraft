#include "../header\chunk.h"
#include "../header\entity.h"
#include "../header\block.h"
#include "../header\wall.h"
#include "../header\cordinates.h"
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

//calculates sun lighting for loaded chunks
int calculateSunLighting(chunkArray_t* loadedChunks){
    #pragma omp parallel for
    for(int xChunk = 0; xChunk <= loadedChunks->width; ++xChunk){//runs for each column of chunks

        #pragma omp parallel for
        for(int cxBlock = 0; cxBlock < 16; ++cxBlock){

            #pragma omp parallel for
            for(int yChunk = 0; yChunk <= loadedChunks->height; ++yChunk){

                uint16_t currentLight;
                for(int cyBlock = 0; cyBlock < 16; ++cyBlock){ //runs for every block in every chunk
                    
                    if(cyBlock == 0 && yChunk == 0){
                        currentLight = loadedChunks->chunkArray[xChunk][yChunk].blocks[cxBlock][cyBlock].light; //set the starting light value to the highest block in the chunk
                    } else{
                        int8_t r, g, b, wr, wg, wb; //rgb is the current colour (0-15) wrwgwb is the current wall colour (0-15)
                        r = (currentLight & 0b1111000000000000) >> 12;
                        g = (currentLight & 0b111100000000) >> 8;
                        b = (currentLight & 0b11110000) >> 4;
                        if(loadedChunks->chunkArray[xChunk][yChunk].blocks[cxBlock][cyBlock].id != AIR || loadedChunks->chunkArray[xChunk][yChunk].walls[cxBlock][cyBlock].id != AIR){
                            if(loadedChunks->chunkArray[xChunk][yChunk].blocks[cxBlock][cyBlock].id != AIR){
                                if(r != 0){
                                    --r; 
                                }
                                if(g != 0){
                                    --g;
                                }
                                if(b != 0){
                                    --b;
                                }
                            }
                            wr = r; wg = g; wb = b;
                            currentLight = ((r & 0b1111)<< 12) | ((g & 0b1111) << 8) | ((b & 0b1111) << 4);
                            loadedChunks->chunkArray[xChunk][yChunk].blocks[cxBlock][cyBlock].light = ((r & 0b1111)<< 12) | ((g & 0b1111) << 8) | ((b & 0b1111) << 4);

                            if(loadedChunks->chunkArray[xChunk][yChunk].walls[cxBlock][cyBlock].id != AIR && loadedChunks->chunkArray[xChunk][yChunk].blocks[cxBlock][cyBlock].id == AIR){ //if we need to colour a non air wall
                                if(r >= 5){
                                    wr = r - 5;
                                } else {
                                    wr = 0;
                                }
                                if(g >= 5){
                                    wg = g - 5;
                                } else {
                                    wg = 0;
                                }
                                if(b >= 5){
                                    wb = b - 5;
                                } else {
                                    wb = 0;
                                }
                                loadedChunks->chunkArray[xChunk][yChunk].walls[cxBlock][cyBlock].light = ((wr & 0b1111)<< 12) | ((wg & 0b1111) << 8) | ((wb & 0b1111) << 4);
                            }

                        } else{
                            loadedChunks->chunkArray[xChunk][yChunk].blocks[cxBlock][cyBlock].light = ((r & 0b1111)<< 12) | ((g & 0b1111) << 8) | ((b & 0b1111) << 4);
                            loadedChunks->chunkArray[xChunk][yChunk].walls[cxBlock][cyBlock].light = ((r & 0b1111)<< 12) | ((g & 0b1111) << 8) | ((b & 0b1111) << 4);
                        }
                    }
                }
            }
        }
    }
}

int calculateLightFromSource(block_t* lightSourceBlock, chunkArray_t* loadedChunks, int x, int y, int chunkArrayX, int chunkArrayY, int xInChunk, int yInChunk){ //runs for every light source
    int lightRange = lightSourceBlock->light & 0b1111;
    #pragma omp parallel for
    for(int offsetX = -lightRange; offsetX <= lightRange; ++offsetX){
        for(int offsetY = -lightRange; offsetY <= lightRange; ++offsetY){
            if((offsetX == 0 && offsetY == 0) || (abs(offsetX) + abs(offsetY)) > lightRange){
                continue;
            }
            int tempChunkX, tempChunkY, tempXInChunk, tempYInChunk, tempChunkArrayX, tempChunkArrayY;
            uint16_t currentLight = 0, currentWallLight = 0;
            uint8_t r, g, b, nr = 0, ng = 0, nb = 0, wr = 0, wg = 0, wb = 0;
            r = ((lightSourceBlock->light & RED) >> 12);
            g = ((lightSourceBlock->light & GREEN) >> 8);
            b = ((lightSourceBlock->light & BLUE) >> 4);
            

            if(r > (abs(offsetX) + abs(offsetY)) && lightRange > (abs(offsetX) + abs(offsetY))){
                //currentLight |= ((r - (abs(offsetX) + abs(offsetY)) & 0b1111) << 12);
                nr = (r - (abs(offsetX) + abs(offsetY)) & 0b1111);
            }
            if(g > (abs(offsetX) + abs(offsetY)) && lightRange > (abs(offsetX) + abs(offsetY))){
                //currentLight |= ((g - (abs(offsetX) + abs(offsetY)) & 0b1111) << 8);
                ng = (g - (abs(offsetX) + abs(offsetY)) & 0b1111);
            }
            if(b > (abs(offsetX) + abs(offsetY)) && lightRange > (abs(offsetX) + abs(offsetY))){
                //currentLight |= ((b - (abs(offsetX) + abs(offsetY)) & 0b1111) << 4);
                nb = (b - (abs(offsetX) + abs(offsetY)) & 0b1111);
            }
            worldToChunkCords(&tempChunkX, &tempChunkY, &tempXInChunk, &tempYInChunk, x + offsetX, y + offsetY);
            getChunkArrayCords(tempChunkX, tempChunkY, loadedChunks, &tempChunkArrayX, &tempChunkArrayY);
            //loadedChunks->chunkArray[tempChunkArrayX][tempChunkArrayY].blocks[tempXInChunk][tempYInChunk].light |= currentLight;

            uint8_t sr, sg, sb, wsr, wsg, wsb, fbr, fbg, fbb;
            sr = (loadedChunks->chunkArray[tempChunkArrayX][tempChunkArrayY].blocks[tempXInChunk][tempYInChunk].light & RED) >> 12;
            sg = (loadedChunks->chunkArray[tempChunkArrayX][tempChunkArrayY].blocks[tempXInChunk][tempYInChunk].light & GREEN) >> 8;
            sb = (loadedChunks->chunkArray[tempChunkArrayX][tempChunkArrayY].blocks[tempXInChunk][tempYInChunk].light & BLUE) >> 4;
            wsr = (loadedChunks->chunkArray[tempChunkArrayX][tempChunkArrayY].walls[tempXInChunk][tempYInChunk].light & RED) >> 12;
            wsg = (loadedChunks->chunkArray[tempChunkArrayX][tempChunkArrayY].walls[tempXInChunk][tempYInChunk].light & GREEN) >> 8;
            wsb = (loadedChunks->chunkArray[tempChunkArrayX][tempChunkArrayY].walls[tempXInChunk][tempYInChunk].light & BLUE) >> 4;

            if((sr + nr) > 15){
                fbr = 15;
            } else{
                fbr = sr + nr;
            }

            if((sg + ng) > 15){
                fbg = 15;
            } else{
                fbg = sg + ng;
            }

            if((sb + nb) > 15){
                fbb = 15;
            } else{
                fbb = sb + nb;
            }

            currentLight |= (fbr << 12);
            currentLight |= (fbg << 8);
            currentLight |= (fbb << 4);
            loadedChunks->chunkArray[tempChunkArrayX][tempChunkArrayY].blocks[tempXInChunk][tempYInChunk].light = currentLight;

            if(nr < 5){
                wr = 0;
            } else {
                wr = nr - 5;
            }
            if(ng < 5){
                wg = 0;
            } else {
                wg = ng - 5;
            }
            if(nb < 5){
                wb = 0;
            } else {
                wb = nb - 5;
            }

            if((wr + wsr) > 10){
                wr = 10;
            } else {
                wr += wsr;
            }
            if((wg + wsg) > 10){
                wg = 10;
            } else {
                wg += wsg;
            }
            if((wb + wsb) > 10){
                wb = 10;
            } else {
                wb += wsb;
            }

            currentWallLight |= (wr << 12);
            currentWallLight |= (wg << 8);
            currentWallLight |= (wb << 4);

            loadedChunks->chunkArray[tempChunkArrayX][tempChunkArrayY].walls[tempXInChunk][tempYInChunk].light = currentWallLight;


        }
    }
}