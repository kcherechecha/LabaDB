#ifndef LABADB_MASTER_H
#define LABADB_MASTER_H
#pragma once

struct Movie{
    int id;
    char name[50];
    char genre[20];
    char duration[5];
    char rating[5];
    int actorCount;
    int actorFirstAddress;
};

#endif //LABADB_MASTER_H
