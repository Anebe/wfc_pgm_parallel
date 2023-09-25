#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>

#include "wfc.h"
#include "tile.h"

World new_world(int height, int width, int entropy){
    World world = (World) malloc(sizeof(struct World));

    world->height = height;
    world->width = width;

    world->map = (Cell **) malloc(sizeof(Cell *) * world->height);
    for (int y = 0; y < world->height; y++) {
        world->map[y] = (Cell *) malloc(sizeof(Cell) * world->width);

        for (int x = 0; x < world->width; x++) {
            world->map[y][x].options = (unsigned char *)  malloc(sizeof(unsigned char) * entropy);

            for (int i = 0; i < entropy; i++) {
                world->map[y][x].options[i] = 1;
            }

            world->map[y][x].collapsedValue = -1;
        }
    }
    return world;
}

void free_world(World world){
    for (int i = 0; i < world->height; i++)
    {
        for (int j = 0; j < world->width; j++)
        {
            free(world->map[i][j].options);
        }
        free(world->map[i]);
    }
    free(world->map);
    free(world);
}

bool isPossibleFinish(World world, Tileset tileset){
    bool hasNoEntropy = true;
    bool result = true;

    #pragma omp parallel shared(world, tileset)
    {
        #pragma omp for collapse(2) private(hasNoEntropy) reduction(&&:result)
        for (int i = 0; i < world->height; i++)
        {
            for (int j = 0; j < world->width; j++)
            {
                for (int k = 0; k < tileset->qtd; k++)
                {
                    if(!result)
                        continue;

                    hasNoEntropy = (world->map[i][j].options[k] == 0) && hasNoEntropy;
                }
                if(hasNoEntropy && world->map[i][j].collapsedValue == -1){
                    result = false;
                }
            }
        }
    }
    return result;
}

void findLowestEntropy(World world, Tileset tileset, int *lowXResult, int *lowYResult, int *lowEntropy){
    *lowEntropy = tileset->qtd+1;
    *lowYResult = 0;
    *lowXResult = 0;

    #pragma omp parallel
    {
        #pragma omp for collapse(2)
        for (int y = 0; y < world->height; y++) {
            for (int x = 0; x < world->width; x++) {

                int entropy = 0;
                for (int i = 0; i < tileset->qtd; i++) {
                    entropy += world->map[y][x].options[i];
                }

                if (entropy < *lowEntropy && world->map[y][x].collapsedValue == -1) {
                    #pragma omp critical
                    {
                        *lowYResult = y;
                        *lowXResult = x;
                        *lowEntropy = entropy;
                    }
                }
            }
        }
    }
}

void propagateCollapse(int collapseTarget, int y, int x, World world, Tileset tileset) {
    const int size = tileset->size;

    #pragma omp for collapse(2)
    for (int i = 0; i < tileset->qtd; i++) {
        for (int j = 0; j < tileset->size; j++)
        {
            //#pragma omp task default(shared)
            {
                //UP NEIGHBOR
                if (y-1 >= 0) {
                    if(tileset->tile[i][size-1][j] != tileset->tile[collapseTarget][0][j]){
                        world->map[y-1][x].options[i] = 0;
                    }
                }
            }

            //#pragma omp task default(shared)
            {
                //DOWN NEIGHBOR
                if (y+1 <= world->height - 1) {
                    if(tileset->tile[i][0][j] != tileset->tile[collapseTarget][size-1][j]){
                        world->map[y+1][x].options[i] = 0;
                    }
                }
            }

            //#pragma omp task default(shared)
            {
                //RIGHT NEIGHBOR
                if (x-1 >= 0) {
                    if(tileset->tile[i][j][size-1] != tileset->tile[collapseTarget][j][0]){
                        world->map[y][x-1].options[i] = 0;
                    }
                }
            }

            //#pragma omp task default(shared)
            {
                //LEFT NEIGHBOR
                if (x+1 <= world->width - 1) {
                    if(tileset->tile[i][j][0] != tileset->tile[collapseTarget][j][size-1]){
                        world->map[y][x+1].options[i] = 0;
                    }
                }
            }
        }
    }

}

int isCollapsed(World world){
    bool isCollapse = true;

    #pragma omp parallel for collapse(2) reduction(&&:isCollapse)
    for (int i = 0; i < world->height; i++)
    {
        for (int j = 0; j < world->width; j++)
        {
            if(!isCollapse)
                continue;
            if(world->map[i][j].collapsedValue == -1){
                isCollapse = false;
            }
        }
    }
    return isCollapse;
}

void collapse(World world, Tileset tileset) {

    int lowestEntropy = tileset->qtd+1;
    int lowestY = 0;
    int lowestX = 0;

    findLowestEntropy(world, tileset, &lowestX, &lowestY, &lowestEntropy);

    int totalEntropy = 0;
    #pragma omp parallel for reduction(+: totalEntropy)
    for (int i = 0; i < tileset->qtd; i++)
    {
        totalEntropy+= world->map[lowestY][lowestX].options[i];
    }

    if(totalEntropy == 0)
        return;

    int *entropyPossibility = malloc(sizeof(int) * totalEntropy);

    int index = 0;
    #pragma omp parallel for
    for (int j = 0; j < tileset->qtd; j++)
    {
        if(world->map[lowestY][lowestX].options[j] == 1 ){
            entropyPossibility[index++] = j;
        }
    }
    


    int collapseValue = entropyPossibility[rand() % totalEntropy];
    free(entropyPossibility);


    #pragma omp parallel for
    for (int i = 0; i < tileset->qtd; i++) {
        world->map[lowestY][lowestX].options[i] = 0;
    }

    world->map[lowestY][lowestX].options[collapseValue] = 1;
    world->map[lowestY][lowestX].collapsedValue = collapseValue;

    propagateCollapse(collapseValue, lowestY, lowestX, world, tileset);
}

void waveFuctionCollapse(Tileset tileset, World world){
    int attempts = 0;

    /*
    do {
        if(!isPossibleFinish(world, tileset)){
            attempts++;
            #pragma omp parallel for collapse(3)
            for (int y = 0; y < world->height; y++) {
                for (int x = 0; x < world->width; x++) {
                    for (int i = 0; i < tileset->qtd; i++) {
                        world->map[y][x].options[i] = 1;
                        world->map[y][x].collapsedValue = -1;
                    }
                }
            }
        }
        collapse(world, tileset);
    } while (!isCollapsed(world) && attempts < MAX_ATTEMPTS);*/
    for (int i = 0; i < world->height * world->width; i++)
    {
        //double j = omp_get_wtime();
        collapse(world, tileset);
        //printf("%lf\n", omp_get_wtime()-j);
    }
}

void print_world(World world){
    printf("height: %d width: %d\n", world->height, world->width);
    for (int i = 0; i < world->height; i++)
    {
        for (int j = 0; j < world->width; j++)
        {
            printf("%d ", world->map[i][j].collapsedValue);
        }
        printf("\n");
    }
    printf("\n");
}
