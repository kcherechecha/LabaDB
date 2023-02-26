#ifndef LABADB_MOVIE_H
#define LABADB_MOVIE_H
#pragma once
#include <stdlib.h>
#include "Master.h"
#include "Slave.h"
#include "Input.h"
#include "Output.h"
#include "Validation.h"
#include "Actor.h"
#include "Ind.h"

#define Movie_IND "movie.ind"
#define Movie_DATA "movie.fl"
#define Movie_GARBAGE "movieGarbage.txt"
#define IND_SIZE sizeof(struct Ind)
#define Movie_SIZE sizeof(struct Movie)
#define Actor_SIZE sizeof(struct Actor)
#define Actor_DATA "actor.fl"

int showMoviesList(char *error);

void overwriteGarbageMovieElement(int garbageCount, FILE* garbageZone, struct Movie* movie){
    int* deleteIdsFile = malloc(garbageCount * sizeof(int));
    for (int i = 0; i < garbageCount; i++){
        fscanf(garbageZone, "%d", deleteIdsFile + i);
    }
    fclose(garbageZone);
    movie->id = deleteIdsFile[0];
    fopen(Movie_GARBAGE, "wb");
    fprintf(garbageZone, "%d", garbageCount - 1);
    for (int i = 1; i < garbageCount; i++){
        fprintf(garbageZone, " %d", deleteIdsFile[i]);
    }
    fclose(garbageZone);
    free(deleteIdsFile);
}

int insertMovie(struct Movie movie){
    FILE* garbageZone = fopen(Movie_GARBAGE, "a+b");
    FILE* indexTable = fopen(Movie_IND, "a+b");
    FILE* database = fopen(Movie_DATA, "a+b");
    int garbageCount = 0;
    fscanf(garbageZone, "%d", &garbageCount);
    struct Ind ind;
    if (garbageCount !=0 ){
        overwriteGarbageMovieElement(garbageCount, garbageZone, &movie);
        fclose(database);
        fclose(indexTable);
        indexTable = fopen(Movie_IND, "r+b");
        database = fopen(Movie_DATA, "r+b");
        fseek(indexTable, (movie.id - 1) * IND_SIZE, SEEK_SET);
        fread(&ind, IND_SIZE, 1, indexTable);
        fseek(database, ind.address, SEEK_SET);
    }
    else {
        fseek(indexTable, 0, SEEK_END);
        if (ftell(indexTable) != 0){
            fseek(indexTable, -(IND_SIZE), SEEK_END);
            fread(&ind, IND_SIZE, 1, indexTable);
            movie.id = ind.id + 1;
        } else {
            movie.id = 1;
        }
    }
    movie.actorFirstAddress = -1;
    movie.actorCount = 0;
    fwrite(&movie, Movie_SIZE, 1, database);
    ind.id = movie.id;
    ind.exists = 1;
    ind.address = (movie.id - 1) * Movie_SIZE;
    printf("Inserted movie id is %d\n", movie.id);
    fseek(indexTable, (movie.id - 1) * IND_SIZE, SEEK_SET);
    fwrite(&ind, IND_SIZE, 1, indexTable);
    fclose(database);
    fclose(indexTable);
    return 1;
}

int getMovie(struct Movie* movie, int id, char* error){
    FILE* indexTable = fopen(Movie_IND, "rb");
    FILE* database = fopen(Movie_DATA, "rb");
    if(checkFileExistence(indexTable, database, error) == 0) return 0;
    struct Ind ind;
    if(checkIndexExistence(indexTable, error, id) ==0) return 0;
    fseek(indexTable, (id - 1) * IND_SIZE, SEEK_SET);
    fread(&ind, IND_SIZE, 1, indexTable);
    if (checkRecordExistence(ind, error) == 0) return 0;
    fseek(database, ind.address, SEEK_SET);
    fread(movie, Movie_SIZE, 1, database);
    fclose(indexTable);
    fclose(database);
    return 1;
}

int updateMovie(struct Movie movie, char* error, int id) {
    FILE* indexTable = fopen(Movie_IND, "r+b");
    FILE* database = fopen(Movie_DATA, "r+b");
    struct Ind ind;
    if (checkFileExistence(indexTable, database, error)==0) return 0;
    if (checkIndexExistence(indexTable, error, id)==0) return 0;
    fseek(indexTable, (id - 1) * IND_SIZE, SEEK_SET);
    fread(&ind, IND_SIZE, 1, indexTable);
    if(checkRecordExistence(ind, error) == 0) return 0;
    movie.id = id;
    fseek(database, ind.address, SEEK_SET);
    fwrite(&movie, Movie_SIZE, 1, database);
    fclose(indexTable);
    fclose(database);
    return 1;
}

void listOfDeletedMovies(int id){
    FILE* garbage = fopen(Movie_GARBAGE, "rb");
    if (garbage == NULL){
        printf("Error. Can't open the file \n");
        return;
    }
    int garbageCount;
    fscanf(garbage, "%d", &garbageCount);
    int* deleteIds = malloc(garbageCount * sizeof(int));
    for (int i = 0; i < garbageCount; i++){
        fscanf(garbage, "%d", deleteIds + i);
    }
    fclose(garbage);
    garbage = fopen(Movie_GARBAGE, "wb");
    fprintf(garbage, "%d", garbageCount + 1);
    for (int i = 0; i < garbageCount; i++) {
        fprintf(garbage, " %d", deleteIds[i]);
    }
    fprintf(garbage, " %d", id);
    free(deleteIds);
    fclose(garbage);
}

int deleteMovie(int id, char* error) {
    FILE* indexTable = fopen(Movie_IND, "r+b");
    struct Ind ind;
    if (indexTable == NULL) {
        strcpy(error, "Sorry, there's no database files");
        return 0;
    }
    if (checkIndexExistence(indexTable, error, id)==0) return 0;
    fseek(indexTable, (id - 1) * IND_SIZE, SEEK_SET);
    fread(&ind, IND_SIZE, 1, indexTable);
    if(checkRecordExistence(ind, error) == 0) return 0;
    struct Movie movie;
    getMovie(&movie, id, error);
    ind.exists = 0;
    fseek(indexTable, (id - 1) * IND_SIZE, SEEK_SET);
    fwrite(&ind, IND_SIZE, 1, indexTable);
    fclose(indexTable);
    listOfDeletedMovies(id);
    if (movie.actorCount != 0) {
        FILE* actorDb = fopen(Actor_DATA, "r+b");
        struct Actor actor;
        fseek(actorDb, movie.actorFirstAddress, SEEK_SET);
        for (int i = 0; i < movie.actorCount; i++){
            fread(&actor, Actor_SIZE, 1, actorDb);
            fclose(actorDb);
            deleteActor(movie, actor, error);
            actorDb = fopen(Actor_DATA, "r+b");
            fseek(actorDb, actor.nextAddress, SEEK_SET);
        }
        fclose(actorDb);
    }
    return 1;
}

int showMoviesList(char *error){
    FILE* database = fopen(Movie_DATA, "rb");
    FILE* indexTable = fopen(Movie_IND, "rb");
    struct Movie movie;
    struct Ind ind;
    if (checkFileExistence(indexTable, database, error)==0) return 0;
    fseek(indexTable, -IND_SIZE, SEEK_END);
    fread(&ind, IND_SIZE, 1, indexTable);
    rewind(indexTable);
    int endIndex =0;
    getMovie(&movie, ind.id, error);
    endIndex = ind.id;
    int id = 0;
    int count = 0;
    while(id < endIndex)
    {
        fseek(indexTable, id * IND_SIZE, SEEK_SET);
        fread(&ind, IND_SIZE, 1, indexTable);
        if(ind.exists)
        {
            getMovie(&movie, ind.id, error);
            printf("\nMovie ID: %d\n", movie.id);
            outputMovie(movie);
            count++;
        }
        id++;
    }
    if(count == 0) printf("Sorry, there are no movies\n");
    return 1;
}

#endif //LABADB_MOVIE_H
