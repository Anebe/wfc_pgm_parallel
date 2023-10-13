#ifndef PGM_H
#define PGM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX_GRAY UCHAR_MAX

typedef struct Pgm
{
    int width;
    int height;
    int **data;
} *Pgm;

Pgm new_pgm(int, int);
int getGrayColor(int);
void pgm_file(char *, Pgm);
void printPgm(Pgm);
void free_pgm(Pgm);
int higher_number(Pgm);

#endif
