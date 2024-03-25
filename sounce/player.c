#include "../header\entity.h"
#include "../header\player.h"
#include "../header\tigr.h"
#include "../header\block.h"
#include "../header\chunk.h"
#include <stdio.h>
#include <string.h> //for memcpy

int movePlayer(entity_t* player, Tigr* screen){
    double speed;
    if(tigrKeyHeld(screen, TK_CONTROL)){
        speed = 0.2;
    } else {
        speed = 0.14;
    }
    if(tigrKeyHeld(screen, 'W')){
        moveEntityUp(player, speed);
    }
    if(tigrKeyHeld(screen, 'S')){
        moveEntityDown(player, speed);
    }
    if(tigrKeyHeld(screen, 'A')){
        moveEntityLeft(player, speed);
    }
    if(tigrKeyHeld(screen, 'D')){
        moveEntityRight(player, speed);
    }
}

int getTargetedBlock(entity_t* entity, Tigr* screen, entity_t* targetedBlockEntityReturn){
    int mouseX, mouseY, mouseButtons;
    tigrMouse(screen, &mouseX, &mouseY, &mouseButtons); //get mouse input
    if(mouseX > (screen->w / 2 + 256) || mouseX < (screen->w / 2 - 256) || mouseY > (screen->h / 2 + 256) || mouseY < (screen->h / 2 - 256)){
        targetedBlockEntityReturn->health = 0;
        return 1; //if the mouse is too far away from the player, fail
    }
    memcpy(targetedBlockEntityReturn, entity, sizeof(entity_t)); //use the player as a base
    double differenceX = mouseX - screen->w / 2; //get the difference between the player pixel cords and the mouses pixel cords
    double differenceY = mouseY - screen->h / 2;
    double blocksMoveX = differenceX / 32; //work out, in blocks, how far away the targeted block is
    double blocksMoveY = differenceY / 32;
    moveEntityDown(targetedBlockEntityReturn, blocksMoveY); //and move the targeted block entity accordingly
    moveEntityRight(targetedBlockEntityReturn, blocksMoveX);
    targetedBlockEntityReturn->health = 20;
    return 0; //return sucsess
}