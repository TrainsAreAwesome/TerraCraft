#pragma once
typedef struct {
    int id;
    int count;
    int durability;
    char customName[51];
} item_t;

typedef struct {
    item_t itemArray[27];
} inventory_t;

typedef struct {
    double xFloat;
    double yFloat;
    long xInt;
    long yInt;
    int chunkX;
    int chunkY;
    int health;
    inventory_t inventory;
    int id;
    int xBlockInChunk;
    int yBlockInChunk;
    int pixelY;
    int pixelX;
} entity_t;

int calculatePixelX(entity_t* player);
int calculatePixelY(entity_t* player);

int moveEntityUp(entity_t* entity, double speed);
int moveEntityDown(entity_t* entity, double speed);
int moveEntityLeft(entity_t* entity, double speed);
int moveEntityRight(entity_t* entity, double speed);