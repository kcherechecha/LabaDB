#ifndef LABADB_OUTPUT_H
#define LABADB_OUTPUT_H
#pragma once
#include "Master.h"
#include "Slave.h"

void outputMovie(struct Movie movie){
    printf("Movie name: %s\n", movie.name);
    printf("Movie genre: %s\n", movie.genre);
    printf("Movie duration: %s\n", movie.duration);
    printf("Movie rating: %s\n", movie.rating);
}

void outputActor(struct Movie movie, struct Actor actor){
//    printf("Movie id: %d\n", movie.id);
    printf("Movie name: %s\n", movie.name);
    printf("Actor's full name: %s %s\n", actor.firstName, actor.lastName);
    printf("Actor's role: %s\n", actor.role);
}

#endif //LABADB_OUTPUT_H
