#include "../header\tigr.h"
#include "../header\block.h"
#include "../header\chunk.h"
#include "../header\player.h"
#include "../header\entity.h"
#include "../header\onScreenText.h"
#include "../header\saveAndLoad.h"
#include "../header\generateTerrain.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
char worldName[51];
long long int seed = 12345;

int main(){
    printf("%s\n", worldName);
    printf("Enter a world name to load or create: "); //so the user can select a world
    scanf("%s", &worldName);
    printf("\nLoading world: %s", worldName);
    //return 0;

    printf("This is the debug menu, I print debug stuff here when I need it (press F3 while in game to have a usfull debug menu like your used to)");
    printf("\nworld name: %s", worldName);
    createWorldFolder(worldName);
    Tigr* screen = tigrWindow(2560, 1440, "MWGIES TOOLS!!!", 64); //when changing resolution make sure to change wierdX and wierdY
    Tigr* textureAtlas = tigrLoadImage("./textures/textures.png");
    if(!textureAtlas){
        tigrError(screen, "Coulnt load the texture atlas! is it in the correct folder and with the correct name?");
    } else{
        printf("\nTexture atlas loaded!");
        printf("\nTexture atlas stuff: width: %d height: %d", textureAtlas->w, textureAtlas->h);
    }

    block_t exampleGrassBlock;
    exampleGrassBlock.id = 2;
    int amountOfChunksX = (screen->w / 512) + 2;
    int amountOfChunksY = (screen->h / 512) + 2;
    
    entity_t Player;

    Player.chunkX = 0;
    Player.chunkY = 0;
    Player.health = 0;
    Player.xInt = 0;
    Player.yInt = 0;
    Player.xFloat = 0;
    Player.yFloat = 0;
    Player.xBlockInChunk = 0;
    Player.yBlockInChunk = 0;
    calculatePixelX(&Player);
    calculatePixelY(&Player);
    int oldChunkX = 0;
    int oldChunkY = 0;
    chunk_t exampleChunk;
    chunkArray_t loadedChunks;
    for(int y = 0; y < 8; ++y){
        for(int x = 0; x < 8; ++x){
            exampleChunk.blocks[x][y] = exampleGrassBlock;
        }
    }


    int F3 = 0;
    clock_t start, end;
    getChunkArray(Player.chunkX - (amountOfChunksX / 2), Player.chunkY - (amountOfChunksY / 2), amountOfChunksX, amountOfChunksY, &loadedChunks, 1, screen);
    while(!tigrClosed(screen)){
        start = clock();
        tigrClear(screen, tigrRGB(0, 0, 0));
        //do game stuff
        amountOfChunksX = (screen->w / 512) + 2;
        amountOfChunksY = (screen->h / 512) + 2;
        int xc, xy;
        getTextureAtlasCords(2, &xc, &xy);
        movePlayer(&Player, screen);

        if((oldChunkX != Player.chunkX) || (oldChunkY != Player.chunkY)){ //if the player moves between chunks, we need to load new chunks and save ones that will be unloaded, so we update the chunkArray
            if(oldChunkX > Player.chunkX){
                printf("\nplayer moved left");
                for(int y = 0; y <= loadedChunks.height; ++y){
                    if(loadedChunks.chunkArray[amountOfChunksX][y].explored){ //only saves the chunk if its been explored to save disk space
                        saveChunk(worldName, &loadedChunks.chunkArray[amountOfChunksX][y]);
                    }
                }
            }
            if(oldChunkX < Player.chunkX){
                printf("\nplayer moved right");
                for(int y = 0; y <= loadedChunks.height; ++y){
                    if(loadedChunks.chunkArray[0][y].explored){
                        saveChunk(worldName, &loadedChunks.chunkArray[0][y]);
                    }
                }
            }
            if(oldChunkY > Player.chunkY){
                printf("\nplayer moved up");
                for(int x = 0; x <= loadedChunks.width; ++x){
                    if(loadedChunks.chunkArray[x][amountOfChunksY].explored){
                        saveChunk(worldName, &loadedChunks.chunkArray[x][amountOfChunksY]);
                    }
                }
            }
            if(oldChunkY < Player.chunkY){
                printf("\nplayer moved down");
                for(int x = 0; x <= loadedChunks.width; ++x){
                    if(loadedChunks.chunkArray[x][0].explored){
                        saveChunk(worldName, &loadedChunks.chunkArray[x][0]);
                    }
                }
            }
            getChunkArray(Player.chunkX - (amountOfChunksX / 2), Player.chunkY - (amountOfChunksY / 2), amountOfChunksX, amountOfChunksY, &loadedChunks, 0, screen);
        }

        entity_t targetedBlock; //works out what block the player is targeting
        getTargetedBlock(&Player, screen, &targetedBlock);

        prosesPlayerBlockStuff(&Player, &targetedBlock, &loadedChunks, screen); //handles block breaking placing etc
        renderWorld(&Player, screen, textureAtlas, &loadedChunks, &targetedBlock);

        renderBlock(screen, textureAtlas, &exampleGrassBlock, (screen->w / 2) - 16, (screen->h / 2) - 16); //renderes a grass block centered at the centre of the window

        if(tigrKeyDown(screen, TK_F3)){
            F3 = !F3;
        }
        if(F3){ //handles F3 debug menu
            F3Menu(&Player, screen);
        }

        // for(int x = -100; x <= 100; ++x){
        //     printf("\nTERRAIN HEIGHT AT %d = %d", x, getTerrainHeight(x, 12345));
        // }
        
        setExplored(&Player, &loadedChunks, amountOfChunksX, amountOfChunksY); //sets the current chunk the player is in to be "explored" (marked for saving)
        tigrUpdate(screen);
        end = clock(); //limits game to 60fps to not mess up timings. yes it locks the thread if the game runs too fast.
        double timeTaken = ((double) (end - start)) / CLOCKS_PER_SEC;
        double FPS = 1 / timeTaken;
        printf("\nFPS (withought limit): %lf", FPS);
        while(FPS > 60){
            end = clock();
            timeTaken = ((double) (end - start)) / CLOCKS_PER_SEC;
            FPS = 1 / timeTaken;
        }
        oldChunkX = Player.chunkX;
        oldChunkY = Player.chunkY;
        printf("\nFPS: %lf", FPS);
    }
    printf("\nGame about to close, saving world...");

    for(int x = 0; x < loadedChunks.width; ++x){ //saves the chunks around the player while the game is closing
        for(int y = 0; y < loadedChunks.height; ++y){
            if(loadedChunks.chunkArray[x][y].explored){
                saveChunk(worldName, &loadedChunks.chunkArray[x][y]);
                printf("\nsaving chunk %d %d to disk", loadedChunks.chunkArray[x][y].x, loadedChunks.chunkArray[x][y].y);
            } else {
                printf("\ndidnt save chunk %d %d to disk (not explored since last save and load)", loadedChunks.chunkArray[x][y].x, loadedChunks.chunkArray[x][y].y);
            }
        }
    }
    
    printf("\nSaved world, Game closing...");
}