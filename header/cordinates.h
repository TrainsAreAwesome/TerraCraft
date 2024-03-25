#pragma once
int worldToChunkCords(int* chunkX, int* chunkY, int* xInChunk, int* yInChunk, int x, int y);
int chunkToWorldCords(int* x, int* y, int chunkX, int chunkY, int xInChunk, int yInChunk);