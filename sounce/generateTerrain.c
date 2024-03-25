#define FNL_IMPL
#include "../header\tigr.h"
#include <stdio.h>
#include "../header\fastNoiseLite.h"
#include "../header\cordinates.h"
#include "../header\chunk.h"
#include "../header\block.h"
#include "../header\biome.h"

#define MAX_TERRAIN_HEIGHT 1024

float getPerlinNoise1D(int x, int seed){//gets one pixel of perlin noise based on the seed and cordinate
    fnl_state noise = fnlCreateState(seed);
    noise.fractal_type = FNL_FRACTAL_FBM;
    noise.octaves = 5;
    noise.frequency = 0.001;
    noise.noise_type = FNL_NOISE_PERLIN;
    noise.lacunarity = 1.5f;
    noise.weighted_strength = -1.f;
    noise.gain = -0.420f;

    float data;
    data = fnlGetNoise2D(&noise, x, 0);
    return data;
}

int getTerrainHeight(int x, int seed){
    float noise = getPerlinNoise1D(x, seed);
    noise *= MAX_TERRAIN_HEIGHT;
    printf("\nheight from func = %d", (int)noise);
    return (int)noise;
}

int generateTerrainSurface(int chunkX, int chunkY, int seed, chunk_t* chunk){ //generates a basic terrain height layer (first step in terrain generation)
    int x, y, none;
    chunkToWorldCords(&x, &y, chunkX, chunkY, 0, 0);
    for(int cx = 0; cx < 16; ++cx){
        int currentHeight = getTerrainHeight(x, seed);
        int currentHeightChunkSpace;
        worldToChunkCords(&none, &currentHeightChunkSpace, &none, &none, 0, currentHeight);
        if(currentHeightChunkSpace == chunkY){ //if this cord is at the surface
            chunk->biome = SURFACE; //surface biome is where the split between underground and air is, will reclasify later
            printf("\nchunk %d %d is at surface", chunkX, chunkY);
            for(int cy = 0; cy < 16; ++cy){
                if((chunkY * 16 + cy) > currentHeight){
                    chunk->blocks[cx][cy].id = 1;
                } else {
                    chunk->blocks[cx][cy].id = 0;
                }
            }
        } else if(currentHeightChunkSpace < chunkY){ //if this cord is under the surface
            printf("\nchunk %d %d is underground", chunkX, chunkY);
            chunk->biome = UNDERGROUND; //generic underground biome to reclassify later
            for(int cy = 0; cy < 16; ++cy){
                chunk->blocks[cx][cy].id = 1;
            }
        } else { //if this cord is above the surface
            printf("\nchunk %d %d is air", chunkX, chunkY);
            chunk->biome = AIR; //biome that repersents everything above the surface layer of chunks
            for(int cy = 0; cy < 16; ++cy){
                chunk->blocks[cx][cy].id = 0;
            }
        }
        ++x;
    }
}