#ifndef LABADB_SLAVE_H
#define LABADB_SLAVE_H
#pragma once

struct Actor{
    int id;
    char firstName[20];
    char lastName[20];
    char role[10];
    int movieId;
    int exists;
    int selfAddress;
    int nextAddress;
};

#endif //LABADB_SLAVE_H
