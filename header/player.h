#pragma once
#include "entity.h"
#include "tigr.h"
#include "chunk.h"
#include "block.h"

typedef struct{
    char name[51];
    entity_t playerEntity;
    int mouseX;
    int mouseY;
    int mouseButtons;
} player_t;

int movePlayer(entity_t* player, Tigr* screen);
int getTargetedBlock(entity_t* entity, Tigr* screen, entity_t* targetedBlockEntityReturn);