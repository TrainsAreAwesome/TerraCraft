#include "../header\chunk.h"
#include "../header\entity.h"
#include "../header\block.h"
#include "../header\wall.h"
#include "../header\cordinates.h"
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
                            if(r != 0){
                                --r; 
                            }
                            if(g != 0){
                                --g;
                            }
                            if(b != 0){
                                --b;
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