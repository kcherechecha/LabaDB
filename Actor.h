#ifndef LABADB_ACTOR_H
#define LABADB_ACTOR_H
#pragma once
#include "Master.h"
#include "Slave.h"
#include "Output.h"

#define Actor_DATA "actor.fl"
#define Actor_GARBAGE "actorGarbage.txt"
#define Actor_SIZE sizeof(struct Actor)

int updateMovie(struct Movie movie, char* error, int id);

struct Actor FindLastAddress(FILE *database, struct Movie *movie, struct Actor *previous){
    for (int i = 0; i < movie->actorCount; i++) {
        fread(previous, Actor_SIZE, 1, database);
        fseek(database, previous->nextAddress, SEEK_SET);
    }
    return (*previous);
}

void NextAddress(FILE* database, struct Movie *movie, struct Actor *actor){
    fclose(database);
    database = fopen(Actor_DATA, "r+b");
    struct Actor previous;
    fseek(database, movie->actorFirstAddress, SEEK_SET);
    previous = FindLastAddress(database, movie, &previous);
    previous.nextAddress = actor->selfAddress;
    fwrite(&previous, Actor_SIZE, 1, database);
}

void overwriteGarbageAddress(int garbageCount, FILE* garbageZone, struct Actor* record) {
    int * deletedIds = malloc(garbageCount * sizeof(int));
    for (int i = 0; i < garbageCount; i++) {
        fscanf(garbageZone, "%d", deletedIds + i);
    }
    record->selfAddress = deletedIds[0];
    record->nextAddress = deletedIds[0];
    fclose(garbageZone);
    garbageZone = fopen(Actor_GARBAGE, "wb");
    fprintf(garbageZone, "%d", garbageCount - 1);
    for (int i = 1; i < garbageCount; i++){
        fprintf(garbageZone, " %d", deletedIds[i]);
    }
    free(deletedIds);
    fclose(garbageZone);
}

int insertActor(struct Movie movie, struct Actor actor, char * error){
    FILE* database = fopen(Actor_DATA, "a+b");
    FILE* garbageZone = fopen(Actor_GARBAGE, "a+b");
    int garbageCount = 0;
    fscanf(garbageZone, "%d", &garbageCount);
    actor.exists=1;
    struct Actor actorTemp;
    fseek(database, 0, SEEK_END);
    if (garbageCount != 0){
        overwriteGarbageAddress(garbageCount, garbageZone, &actor);
        fclose(database);
        database = fopen(Actor_DATA, "rb");
        fseek(database, actor.selfAddress, SEEK_SET);
        fread(&actorTemp, Actor_SIZE, 1, database);
        actor.id= actorTemp.id;
        fclose(database);
        database = fopen(Actor_DATA, "r+b");
        fseek(database, actor.selfAddress, SEEK_SET);
    }
    else{
        if (ftell(database) != 0) {
            fseek(database, -(Actor_SIZE), SEEK_END);
            fread(&actorTemp, Actor_SIZE, 1, database);
            actor.id = actorTemp.id + 1;
        }
        else{
            actor.id = 1;
        }
        int dbSize = ftell(database);
        actor.selfAddress = dbSize;
        actor.nextAddress = dbSize;
        fseek(database, 0, SEEK_END);
    }
    printf("Inserted actor id is %d \n", actor.id);
    fwrite(&actor, Actor_SIZE, 1, database);
    if (!movie.actorCount) {
        movie.actorFirstAddress = actor.selfAddress;
    }
    else{
        NextAddress(database, &movie, &actor);
    }
    fclose(database);
    int count = movie.actorCount + 1;
    movie.actorCount=count;
    updateMovie(movie, error, movie.id);
    return 1;
}

int getActor(struct Movie movie, struct Actor* actor, int actorId, char* error) {
    FILE* database = fopen(Actor_DATA, "rb");
    fseek(database, movie.actorFirstAddress, SEEK_SET);
    fread(actor, Actor_SIZE, 1, database);

    for (int i = 0; i < movie.actorCount; i++) {
        if (actor->id == actorId){
            fclose(database);
            return 1;
        }
        fseek(database, actor->nextAddress, SEEK_SET);
        fread(actor, Actor_SIZE, 1, database);
    }
    fclose(database);
    return 0;
}

int updateActor(struct Actor actor){
    FILE* database = fopen(Actor_DATA, "r+b");
    fseek(database, actor.selfAddress, SEEK_SET);
    fwrite(&actor, Actor_SIZE, 1, database);
    fclose(database);
    return 1;
}

void noteDeletedActor(int address)
{
    FILE* garbageZone = fopen(Actor_GARBAGE, "rb");
    int garbageCount;
    fscanf(garbageZone, "%d", &garbageCount);
    int* deletedAddresses = malloc(garbageCount * sizeof(long));
    for (int i = 0; i < garbageCount; i++) {
        fscanf(garbageZone, "%d", deletedAddresses + i);
    }
    fclose(garbageZone);
    garbageZone = fopen(Actor_GARBAGE, "wb");
    fprintf(garbageZone, "%d", garbageCount + 1);
    for (int i = 0; i < garbageCount; i++) {
        fprintf(garbageZone, " %d", deletedAddresses[i]);
    }
    fprintf(garbageZone, " %d", address);
    free(deletedAddresses);
    fclose(garbageZone);
}

void relinkAddresses(FILE* database, struct Actor actor, struct Actor previous, struct Movie* movie) {
    if (actor.selfAddress == movie->actorFirstAddress) {
        if (actor.selfAddress == actor.nextAddress) {
            movie->actorFirstAddress = -1;
        } else {
            movie->actorFirstAddress = actor.nextAddress;
        }
    } else {
        if (actor.selfAddress == actor.nextAddress) {
            previous.nextAddress = previous.selfAddress;
        }
        else {
            previous.nextAddress = actor.nextAddress;
        }
        fseek(database, previous.selfAddress, SEEK_SET);
        fwrite(&previous, Actor_SIZE, 1, database);
    }
}

void deleteActor(struct Movie movie, struct Actor actor, char* error){
    FILE* database = fopen(Actor_DATA, "r+b");
    struct Actor previous;
    fseek(database, movie.actorFirstAddress, SEEK_SET);
    do {
        fread(&previous, Actor_SIZE, 1, database);
        fseek(database, previous.nextAddress, SEEK_SET);
    }
    while (previous.nextAddress != actor.selfAddress && actor.selfAddress != movie.actorFirstAddress);
    relinkAddresses(database, previous, actor, &movie);
    noteDeletedActor(actor.selfAddress);
    actor.exists = 0;

    fseek(database, actor.selfAddress, SEEK_SET);
    fwrite(&actor, Actor_SIZE, 1, database);
    fclose(database);

    movie.actorCount--;
    updateMovie(movie, error, movie.id);
}

void showListOfActors(struct Movie movie){
    FILE* actorDb = fopen(Actor_DATA, "a+b");
    struct Actor actor;
    fseek(actorDb, movie.actorFirstAddress, SEEK_SET);
    for (int i = 0; i < movie.actorCount; i++) {
        fread(&actor, Actor_SIZE, 1, actorDb);
        if (actor.exists)
        {
            outputActor(movie,actor);
            printf("\n");
        }
        fseek(actorDb, actor.nextAddress, SEEK_SET);
    }
    fclose(actorDb);
}

#endif //LABADB_ACTOR_H
