#pragma once
#include "entity.h"

typedef struct{
    int seed;
    int playerX;
    int playerY;
    int playerHealth;
} metadata_t;
int saveMDF(char* folderName, int seed, entity_t* player);
int readMDF(char* folderName, metadata_t* md);