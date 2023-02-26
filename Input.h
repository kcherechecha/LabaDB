#ifndef LABADB_INPUT_H
#define LABADB_INPUT_H
#pragma once
#include <stdio.h>
#include <string.h>
#include "Master.h"
#include "Slave.h"

void inputMovie(struct Movie *movie){
    char  name[50];
    char genre[20];
    char duration[5];
    char rating[5];

    printf("Enter movie name: ");
    scanf("%s", name);
    strcpy(movie->name, name);
    printf("Enter movie's genre: ");
    scanf("%s", genre);
    strcpy(movie->genre, genre);
    printf("Enter movie's duration: ");
    scanf("%s", duration);
    strcpy(movie->duration, duration);
    printf("Enter movie's rating: ");
    scanf("%s", rating);
    strcpy(movie->rating, rating);
}

void inputActor(struct Actor *actor){
    char firstName[20];
    char lastName[20];
    char role[10];

    printf("Enter actor's fist name: ");
    scanf("%s", firstName);
    strcpy(actor->firstName, firstName);
    printf("Enter actor's last name: ");
    scanf("%s", lastName);
    strcpy(actor->lastName, lastName);
    printf("Enter actor's role: ");
    scanf("%s", role);
    strcpy(actor->role, role);
}


#endif //LABADB_INPUT_H
