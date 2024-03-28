#include "../header\chunk.h"
#include "../header\entity.h"
#include "../header\block.h"
#include "../header\wall.h"
#define TORCH_ORANGE 0b1111110000001111;

int calculateSunLighting(chunkArray_t* loadedChunks);
int calculateLightFromSource(block_t* lightSourceBlock, chunkArray_t* loadedChunks, int x, int y, int chunkArrayX, int chunkArrayY, int xInChunk, int yInChunk);