#include <stdio.h>
#include "../header\chunk.h"
#include <string.h>
#include <direct.h>
#include <errno.h>

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