#ifndef TILE_H
#define TILE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Tileset
{
    unsigned int size;
    unsigned int qtd;
    int ***tile;
} *Tileset;

Tileset new_tileset(unsigned int, unsigned int);
Tileset open_tileset(char *);
void print_tile(Tileset, int);
void print_tileset(Tileset);
void free_tileset(Tileset);

#endif