#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pgm.h"
#include <omp.h>

Pgm new_pgm(int height, int width){
    Pgm pgm = (Pgm) malloc(sizeof(struct Pgm));

    pgm->height = height;
    pgm->width = width;

    pgm->data = (int **) malloc(sizeof(int *) * height);
    for (int i = 0; i < height; i++)
    {
        pgm->data[i] = (int *) malloc(sizeof(int) * width);
    }
    return pgm;
}

int getGrayColor(int val){
    if(val > 0){
        if(val < MAX_GRAY){
            return MAX_GRAY/val;
        }
        return MAX_GRAY;
    }
    return 0;
}

void pgm_file(char *path, Pgm pgm){
    FILE *arq;
    arq = fopen(path, "w");
    
    if(arq != NULL){
        fprintf(arq, "P2\n%d %d\n%d\n", pgm->width, pgm->height, MAX_GRAY);
        for (int i = 0; i < pgm->height; i++)
        {
            for (int j = 0; j < pgm->width; j++)
            {
                fprintf(arq, "%d", getGrayColor(pgm->data[i][j]));
                if(j != pgm->width-1)
                    fprintf(arq, " ");
                
            }
            if(i != pgm->height-1){
                fprintf(arq, "\n");
            }
        }
        fclose(arq);
    }
    
}

void printPgm(Pgm pgm){
    for (int i = 0; i < pgm->height; i++)
    {
        for (int j = 0; j < pgm->width; j++)
        {
            printf("%d ", pgm->data[i][j]);
        }
        printf("\n");
        
    }
    
}

void free_pgm(Pgm pgm){
    for (int i = 0; i < pgm->height; i++)
    {
        free(pgm->data[i]);
    }
    free(pgm->data);
    free(pgm);
}
