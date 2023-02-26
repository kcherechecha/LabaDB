#ifndef LABADB_VALIDATION_H
#define LABADB_VALIDATION_H
#pragma once
#include "Master.h"
#include "Movie.h"
#include "stdio.h"
#include "Ind.h"

#define IND_SIZE sizeof(struct Ind)
#define Movie_DATA "movie.fl"
#define Movie_SIZE sizeof(struct Movie)
#define Actor_DATA "actor.fl"
#define Actor_SIZE sizeof(struct Actor)
#define Movie_IND "movie.ind"

int getMovie(struct Movie* movie, int id, char* error);

int checkFileExistence(FILE* indexTable, FILE* database, char* error){
    if (indexTable == NULL || database == NULL) {
        strcpy(error, "Can't find this file, it doesn`t exist");
        return 0;
    }
    return 1;
}

int checkIndexExistence(FILE* indexTable, char* error, int id) {
    fseek(indexTable, 0, SEEK_END);
    long indexTableSize = ftell(indexTable);
    if (indexTableSize == 0 || id * IND_SIZE > indexTableSize) {
        strcpy(error, "this id doesn't exist1");
        return 0;
    }
    return 1;
}

int checkRecordExistence(struct Ind ind, char* error) {
    if (ind.exists == 0 ) {
        strcpy(error, "there is no such record");
        return 0;
    }
    return 1;
}

int checkIfRecordExists(struct Movie movie, int actorId, char *error ) {
    FILE* actorsDb = fopen(Actor_DATA, "a+b");
    struct Actor actor;
    fseek(actorsDb, movie.actorFirstAddress, SEEK_SET);
    for (int i = 0; i < movie.actorCount; i++) {
        fread(&actor, Actor_SIZE, 1, actorsDb);
        fclose(actorsDb);
        if (actor.id == actorId){
            if(actor.exists)
                return 1;
            else{
                strcpy(error, "This actor doesn't exist");
                return 0;
            }
        }
        actorsDb = fopen(Actor_DATA, "r+b");
        fseek(actorsDb, actor.nextAddress, SEEK_SET);
    }
    strcpy(error, "this movie doesn't have this actor, he might have been deleted or doesn't exist");
    fclose(actorsDb);
    return 0;
}

void showGeneralInfo() {
    FILE* indexTable = fopen(Movie_IND, "rb");
    if (indexTable == NULL) {
        printf("There has been an error: files do not exist\n");
        return;
    }
    int movieCount = 0;
    int actorCount = 0;
    fseek(indexTable, 0, SEEK_END);
    int indQuantity = ftell(indexTable) / sizeof(struct Ind);
    struct Movie movie;
    char error[100];
    for (int i = 1; i <= indQuantity; i++){
        if (getMovie(&movie, i, error)){
            movieCount++;
            actorCount += movie.actorCount;
            printf("Movie №%d has %d actor(s)\n", i, movie.actorCount);
        }
    }
    fclose(indexTable);
    printf("Total movies: %d\n", movieCount);
    printf("Total actors: %d\n", actorCount);
}

#endif //LABADB_VALIDATION_H
