#ifndef WFC_H
#define WFC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>

#include "pgm.h"
#include "tile.h"

#define MAX_ATTEMPTS 100

typedef struct Cell {
    unsigned char *options;
    short int collapsedValue;
    int totalEntropy;
} Cell;

typedef struct World {
    unsigned int height;
    unsigned int width;
    Cell **map;
} *World;

World new_world(int, int, int);
bool isPossibleFinish(World, Tileset);
void findLowestEntropy(World, Tileset, int *, int *);
void propagateCollapse(const int , const int, const int, World, const Tileset);
int isCollapsed(World);
void collapse(World, Tileset);
void waveFuctionCollapse(Tileset, World);
void print_world(World);
void free_world(World);
Pgm convertWfc(World , Tileset );

#endif