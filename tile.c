#include "tile.h"

Tileset new_tileset(unsigned int qtd, unsigned int size){
    Tileset tileset = (Tileset) malloc(sizeof(struct Tileset));

    tileset->qtd = qtd;
    tileset->size = size;

    tileset->tile = malloc(sizeof(int **) * tileset->qtd);
    for (int i = 0; i < tileset->qtd; i++) {
        tileset->tile[i] = (int **) malloc(sizeof(int *) * tileset->size);
        for (int j = 0; j < tileset->size; j++) {
            tileset->tile[i][j] = (int *) malloc(sizeof(int) * tileset->size);
        }
    }

    return tileset;
}

Tileset open_tileset(char *path){
    //Tileset tileset = (Tileset) malloc(sizeof(struct Tileset));
    int qtd, size;
    FILE *arq;
    arq = fopen(path, "r");

    //fscanf(arq, "%d", &tileset->qtd);
    //fscanf(arq, "%d", &tileset->size);
    fscanf(arq, "%d", &qtd);
    fscanf(arq, "%d", &size);

    Tileset tileset = new_tileset(qtd, size);
    //tileset->tile = malloc(sizeof(int **) * tileset->qtd);
    for (int i = 0; i < tileset->qtd; i++) {
        //tileset->tile[i] = (int **) malloc(sizeof(int *) * tileset->size);

        for (int j = 0; j < tileset->size; j++) {
            //tileset->tile[i][j] = (int *) malloc(sizeof(int) * tileset->size);

            for (int l = 0; l < tileset->size; l++)
            {
                int numero;
                if (fscanf(arq, "%d", &numero) == 1) {
                    tileset->tile[i][j][l] = numero;
                }
            }
        }
    }
    
    fclose(arq);
    return tileset;
}

void print_tile(Tileset tileset, int i){
    for (int j = 0; j < tileset->size; j++)
    {
        for (int k = 0; k < tileset->size; k++)
        {
            printf("%d ", tileset->tile[i][j][k]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_tileset(Tileset tileset){
    printf("Size %d\t Qtd %d\n", tileset->size, tileset->qtd);
    
    for (int i = 0; i < tileset->qtd; i++)
    {
        print_tile(tileset, i);
    }
}

void free_tileset(Tileset tileset){
    for (int i = 0; i < tileset->qtd; i++)
    {
        for (int j = 0; j < tileset->size; j++)
        {
            free(tileset->tile[i][j]);
        }
        free(tileset->tile[i]);
    }
    free(tileset->tile);
    free(tileset);
}


