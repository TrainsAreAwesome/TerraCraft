#define FNL_IMPL
#include "../header\tigr.h"
#include <stdio.h>
#include "../header\fastNoiseLite.h"
#include "../header\cordinates.h"
#include "../header\chunk.h"
#include "../header\block.h"
#include "../header\biome.h"
#include "../header\entity.h"
#include "../header\cordinates.h"
#include <omp.h>

#define MAX_TERRAIN_HEIGHT 1024 //this is the maximum distnace from y=0 the surface height can be (surface height can go either direction, so surface height variance is double this number)

//gets one pixel of perlin noise based on the seed and cordinate
float getPerlinNoiseSurface(int x, int seed){
    fnl_state noise = fnlCreateState(seed);
    noise.seed = seed;
    noise.fractal_type = FNL_FRACTAL_FBM;
    noise.octaves = 10;
    noise.frequency = 0.0005;
    noise.noise_type = FNL_NOISE_PERLIN;
    noise.lacunarity = 1.5f;
    noise.weighted_strength = -1.f;
    noise.gain = -0.420f;

    float data;
    data = fnlGetNoise2D(&noise, x, 0);
    return data;
}

//gets a 2d array of perlin noise for cave generation
int getPerlinNoiseCave(int x, int y, int seed, float data[16][16]){
    fnl_state noise = fnlCreateState(seed);
    noise.seed = seed;
    noise.noise_type = FNL_NOISE_PERLIN;
    noise.frequency = 0.008;

    #pragma omp parallel for
    for(int cx = x; cx < x + 16; ++cx){
        #pragma omp parallel for
        for(int cy = y; cy < y + 16; ++cy){
            data[cx - x][cy - y] = fnlGetNoise2D(&noise, cx, cy);
        }
    }  
    return 0;
}

int generateCaves(int chunkX, int chunkY, chunk_t* chunk, int seed){
    //printf("\nAbout to generate caves for chunk %d %d", chunkX, chunkY);
    int x, y;
    chunkXToWorldCords(&x, chunkX, 0);
    chunkYToWorldCords(&y, chunkY, 0);
    float data[16][16];
    getPerlinNoiseCave(x, y, seed, data);
    #pragma omp parallel for
    for(int cx = 0; cx < 16; ++cx){
        for(int cy = 0; cy < 16; ++cy){
            //printf("\ncave? %f", data[cx][cy]);
            if(data[cx][cy] < .025 && data[cx][cy] > -.025 && chunk->blocks[cx][cy].id == STONE){ //if the noise is less than .3 but also greater than -.3 then place air
                //printf("\njust set chunk %d %d block %d %d to air", chunkX, chunkY, cx, cy);
                chunk->blocks[cx][cy].id = AIR;
                chunk->walls[cx][cy].id = STONE;
            } //we dont need to place stone, thats allready done for us
        }
    }
}

int getTerrainHeight(int x, int seed){
    float noise = getPerlinNoiseSurface(x, seed);
    noise *= MAX_TERRAIN_HEIGHT;
    //printf("\nheight from func = %d", (int)noise);
    return (int)noise;
}

int placeGrass(chunk_t* chunk, int chunkX, int chunkY, int seed){
    int blockY, blockX, terrainHeight;
    chunkToWorldCords(&blockX, &blockY, chunkX, chunkY, 0, 0);
    #pragma omp parallel for
    for(int x = 0; x < 16; ++x){
        for(int y = 15; y >= 0; --y){
            if(chunk->blocks[x][y].id == AIR){
                if(chunk->walls[x][y].id != AIR){
                    terrainHeight = getTerrainHeight(blockX + x, seed);
                    if(blockY + y == terrainHeight + 1){
                        chunk->walls[x][y].id = GRASS;
                    } else if((blockY + y - terrainHeight) < 5 && (blockY + y - terrainHeight) > 0){
                        chunk->walls[x][y].id = DIRT;
                    }
                }
                continue;
            }
            terrainHeight = getTerrainHeight(blockX + x, seed);
            if(blockY + y == terrainHeight + 1){
                chunk->blocks[x][y].id = GRASS;
                chunk->walls[x][y].id = GRASS;

            } else if((blockY + y - terrainHeight) < 5 && (blockY + y - terrainHeight) > 0){
                chunk->blocks[x][y].id = DIRT;
                chunk->walls[x][y].id = DIRT;

            }
        }
    }
}

int generateTerrainSurface(int chunkX, int chunkY, int seed, chunk_t* chunk){ //generates a basic terrain height layer (first step in terrain generation)
    int x;
    chunkXToWorldCords(&x, chunkX, 0);
    #pragma omp parallel for
    for(int cx = 0; cx < 16; ++cx){
        int currentHeight = getTerrainHeight(x + cx, seed);
        int currentHeightChunkSpace;
        worldYToChunkCords(&currentHeightChunkSpace, 0, currentHeight);
        if(currentHeightChunkSpace == chunkY){ //if this cord is at the surface
            chunk->biome = BIOME_SURFACE; //surface biome is where the split between underground and air is, will reclasify later
            //printf("\nchunk %d %d is at surface", chunkX, chunkY);
            for(int cy = 0; cy < 16; ++cy){
                if((chunkY * 16 + cy) > currentHeight){
                    chunk->blocks[cx][cy].id = STONE;
                    chunk->walls[cx][cy].id = STONE;

                    chunk->blocks[cx][cy].light = 0b1111111111111111;
                    chunk->walls[cx][cy].light = 0b0111011101110111;
                } else {
                    chunk->blocks[cx][cy].light = 0b1111111111111111;
                    chunk->walls[cx][cy].light = 0b1111111111111111;
                    
                    chunk->blocks[cx][cy].id = AIR;
                    chunk->walls[cx][cy].id = AIR;
                }
            }
        } else if(currentHeightChunkSpace < chunkY){ //if this cord is under the surface
            //printf("\nchunk %d %d is underground", chunkX, chunkY);
            chunk->biome = BIOME_UNDERGROUND; //generic underground biome to reclassify later
            for(int cy = 0; cy < 16; ++cy){
                chunk->blocks[cx][cy].id = STONE;
                chunk->walls[cx][cy].id = STONE;

                chunk->blocks[cx][cy].light = 0b1111111111111111;
                chunk->walls[cx][cy].light = 0b0111011101110111;
            }
        } else { //if this cord is above the surface
            //printf("\nchunk %d %d is air", chunkX, chunkY);
            chunk->biome = BIOME_AIR; //biome that repersents everything above the surface layer of chunks
            for(int cy = 0; cy < 16; ++cy){
                chunk->blocks[cx][cy].id = AIR;
                chunk->walls[cx][cy].id = AIR;

                chunk->blocks[cx][cy].light = 0b1111111111111111;
                chunk->walls[cx][cy].light = 0b1111111111111111;
            }
        }
    }
}

//generates a new chunk. takes a pointer to the chunk, the cordinates of the chunk and the world seed
int generateChunk(chunk_t* chunk, int chunkX, int chunkY, int seed){
    generateTerrainSurface(chunkX, chunkY, seed, chunk);
    if(chunk->biome != BIOME_AIR){
        generateCaves(chunkX, chunkY, chunk, seed);
    }
    placeGrass(chunk, chunkX, chunkY, seed);
}