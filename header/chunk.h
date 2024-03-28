#pragma once
#include "block.h"
#include "tigr.h"
#include "player.h"
#include "entity.h"
#include "wall.h"
#include <stdbool.h>

typedef struct {
    block_t blocks[16][16];
    wall_t walls[16][16];
    int x;
    int y;
    bool explored;
    uint16_t biome;
} chunk_t;

typedef struct {
    chunk_t chunkArray[16][16];
    int width;
    int height;
} chunkArray_t;

typedef struct {
    chunk_t* chunk;
    int chunkX;
    int chunkY;
} getChunkArgs;

int getChunk(chunk_t* chunk, int chunkX, int chunkY, long long int seed);
int renderChunk(chunk_t* chunk, Tigr* screen, Tigr* textureAtlas, entity_t* player, int offsetX, int offsetY, entity_t* targetedBlock);
int renderWorld(entity_t* player, Tigr* screen, Tigr* textureAtlas, chunkArray_t* loadedChunks, entity_t* targetedBlock);
chunkArray_t* getChunkArray(int chunkXStart, int chunkYStart, int width, int height, chunkArray_t* chunkArray, int generateAll, Tigr* screen, long long int seed);
int prosesPlayerBlockStuff(entity_t* player, entity_t* targetedBlock, chunkArray_t* loadedChunks, Tigr* screen);
int setExplored(entity_t* entity, chunkArray_t* loadedChunks, int amountChunksX, int amountChunksY);
int isChunkInChunkArray(int chunkX, int chunkY, chunkArray_t* chunkArray);
int getChunkArrayCords(int chunkX, int chunkY, chunkArray_t* chunkArray, int* chunkArrayX, int* chunkArrayY);
int getChunkFromChunkArray(int chunkX, int chunkY, chunkArray_t* chunkArray, chunk_t* returnChunk);

extern char worldName[51];