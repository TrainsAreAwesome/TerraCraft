#include "../header\tigr.h"
#include "../header\onScreenText.h"
#include "../header\block.h"
#include "../header\chunk.h"
#include "../header\player.h"
#include "../header\entity.h"
#include <stdio.h>

int F3Menu(entity_t* player, Tigr* screen){
    char floatXDisplay[25], floatYDisplay[25], intXDisplay[25], intYDisplay[25], healthDisplay[25], chunkXDisplay[25], chunkYDisplay[25], xInChunkDisplay[25], yInChunkDisplay[25], pixelXDisplay[25], pixelYDisplay[25];
    snprintf(floatXDisplay, 25, "Float X Cord: %lf", player->xFloat);
    snprintf(floatYDisplay, 25, "Float Y Cord: %lf", player->yFloat);
    snprintf(intXDisplay, 25, "Int X Cord: %d", player->xInt);
    snprintf(intYDisplay, 25, "Int Y Cord: %d", player->yInt);
    snprintf(healthDisplay, 25, "Health: %d", player->health);
    snprintf(chunkXDisplay, 25, "In chunk X = %d", player->chunkX);
    snprintf(chunkYDisplay, 25, "In chunk Y = %d", player->chunkY);
    snprintf(xInChunkDisplay, 25, "X In Chunk = %d", player->xBlockInChunk);
    snprintf(yInChunkDisplay, 25, "Y In Chunk = %d", player->yBlockInChunk);
    snprintf(pixelXDisplay, 25, "PixelX = %d", player->pixelX);
    snprintf(pixelYDisplay, 25, "PixelY = %d", player->pixelY);

    tigrPrint(screen, tfont, 0, 0, tigrRGB(255, 255, 255), "-----DEBUG MENU-----");
    tigrPrint(screen, tfont, 0, 25, tigrRGB(255, 255, 255), "  --PLAYER CORDS--");
    tigrPrint(screen, tfont, 0, 50, tigrRGB(255, 255, 255), floatXDisplay);
    tigrPrint(screen, tfont, 0, 75, tigrRGB(255, 255, 255), floatYDisplay);
    tigrPrint(screen, tfont, 0, 100, tigrRGB(255, 255, 255), intXDisplay);
    tigrPrint(screen, tfont, 0, 125, tigrRGB(255, 255, 255), intYDisplay);
    tigrPrint(screen, tfont, 0, 150, tigrRGB(255, 255, 255), chunkXDisplay);
    tigrPrint(screen, tfont, 0, 175, tigrRGB(255, 255, 255), chunkYDisplay);
    tigrPrint(screen, tfont, 0, 200, tigrRGB(255, 255, 255), xInChunkDisplay);
    tigrPrint(screen, tfont, 0, 225, tigrRGB(255, 255, 255), yInChunkDisplay);
    tigrPrint(screen, tfont, 0, 250, tigrRGB(255, 255, 255), pixelXDisplay);
    tigrPrint(screen, tfont, 0, 275, tigrRGB(255, 255, 255), pixelYDisplay);
    tigrPrint(screen, tfont, 0, 300, tigrRGB(255, 255, 255), "--OTHER PLAYER INFO--");
    tigrPrint(screen, tfont, 0, 325, tigrRGB(255, 255, 255), healthDisplay);
}
