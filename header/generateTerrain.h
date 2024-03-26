#pragma once
#include "tigr.h"
#include "chunk.h"
#include "block.h"
float getPerlinNoise1D(int offsetX, int seed);
int getTerrainHeight(int x, int seed);
int generateTerrainSurface(int chunkX, int chunkY, int seed, chunk_t* chunk);
int generateChunk(chunk_t* chunk, int chunkX, int chunkY, int seed);