#pragma once
#include "chunk.h"
#include <string.h>

int saveChunk(char* folderName, chunk_t* chunk);
int loadChunk(chunk_t* returnChunk, char* folderName, int chunkX, int chunkY);
int createWorldFolder(char* folderName);