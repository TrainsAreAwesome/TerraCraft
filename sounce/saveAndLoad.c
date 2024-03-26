#include <stdio.h>
#include "../header\chunk.h"
#include <string.h>
#include <direct.h>
#include <errno.h>
#include "../header\metadata.h"

int formatFileName(char* returnName, char* folderName, chunk_t* chunk){
    char chunkCordString[51];
    memset(returnName, 0x0, sizeof(returnName));
    snprintf(chunkCordString, 51, "%d_%d", chunk->x, chunk->y);
    strcat(returnName, "./");
    strcat(returnName, folderName);
    strcat(returnName, "/chunks/chunk_");
    strcat(returnName, chunkCordString);
    strcat(returnName, ".tcc");
}

int formatMDFileName(char* returnName, char* folderName){
    memset(returnName, 0x0, sizeof(returnName));
    strcat(returnName, "./");
    strcat(returnName, folderName);
    strcat(returnName, "/metadata");
    strcat(returnName, ".tcm");
    printf("\nfilename: %s", returnName);
    return 0;
}

int formatFileNameRead(char* returnName, char* folderName, int chunkX, int chunkY){
    char chunkCordString[51];
    memset(returnName, 0x0, sizeof(returnName));
    snprintf(chunkCordString, 51, "%d_%d", chunkX, chunkY);
    strcat(returnName, "./");
    strcat(returnName, folderName);
    strcat(returnName, "/chunks/chunk_");
    strcat(returnName, chunkCordString);
    strcat(returnName, ".tcc");
}


int createWorldFolder(char* folderName){
    printf("creating world folder: %s\n", folderName);
    _mkdir(folderName);
    char fnc[101];
    strcpy(fnc, folderName);
    strcat(fnc, "\\chunks");
    _mkdir(fnc);
    if(errno == ENOENT){
        printf("\npath not found");
    }
    return 0;
}

int saveChunk(char* folderName, chunk_t* chunk){
    FILE* file;
    char fileName[101];
    formatFileName(fileName, folderName, chunk);

    file = fopen(fileName, "wb");
    if(file == NULL){
        printf("\nfile is null (trying to save chunk)");
        return 1;
    }
    fwrite(chunk, sizeof(chunk_t), 1, file);
    fclose(file);
    return 0;
}

int loadChunk(chunk_t* returnChunk, char* folderName, int chunkX, int chunkY){
    FILE* file;
    char fileName[101];
    formatFileNameRead(fileName, folderName, chunkX, chunkY);

    file = fopen(fileName, "rb");
    if(file == NULL){
        return 1;
    }
    fread(returnChunk, sizeof(chunk_t), 1, file);
    fclose(file);
    returnChunk->explored = false; //sets the explored (aka modified) flag to false, so the disk doesnt get hammered when saving
    return 0;
}

int saveMDF(char* folderName, int seed, entity_t* player){
    char fileNameString[51];
    formatMDFileName(fileNameString, folderName);
    metadata_t md;
    md.playerX = player->xInt;
    md.playerY = player->yInt;
    md.seed = seed;
    md.playerHealth = player->health;
    printf("\nplayerX %d playerY %d", md.playerX, md.playerY);

    FILE* file;
    file = fopen(fileNameString, "wb");
    if(file == NULL){
        printf("\ncoulnt write to metadata file, its null :(");
        return 1;
    }
    fwrite(&md, sizeof(metadata_t), 1, file);
    fclose(file);
    return 0;
}

int readMDF(char* folderName, metadata_t* md){
    char fileNameString[51];
    formatMDFileName(fileNameString, folderName);
    FILE* file;
    file = fopen(fileNameString, "rb");
    if(file == NULL){
        return 1;
    }
    fread(md, sizeof(metadata_t), 1, file);
    printf("\nplayerX %d playerY %d", md->playerX, md->playerY);
    fclose(file);
    return 0;
}