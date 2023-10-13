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
} Cell;

typedef struct World {
    unsigned int height;
    unsigned int width;
    Cell **map;
} *World;

World new_world(int, int, int);
bool isPossibleFinish(World, Tileset);
void findLowestEntropy(World, Tileset, int *, int *, int *);
void propagateCollapse(int , int, int, World, Tileset);
int isCollapsed(World);
void collapse(World, Tileset);
void waveFuctionCollapse(Tileset, World);
void print_world(World);
void free_world(World);
Pgm convertWfc(World , Tileset );

#endif