#pragma once
int worldToChunkCords(int* chunkX, int* chunkY, int* xInChunk, int* yInChunk, int x, int y);
int chunkToWorldCords(int* x, int* y, int chunkX, int chunkY, int xInChunk, int yInChunk);
int chunkXToWorldCords(int* x, int chunkX, int xInChunk);
int chunkYToWorldCords(int* y, int chunkY, int yInChunk);
int worldYToChunkCords(int* chunkY, int* yInChunk, int y);
int worldXToChunkCords(int* chunkX, int* xInChunk, int x);