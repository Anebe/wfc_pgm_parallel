#include "wfc.h"

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
            world->map[y][x].totalEntropy = entropy;
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

void findLowestEntropy(World world, Tileset tileset, int *lowXResult, int *lowYResult){

    //#pragma omp parallel 
    {
        int lowEntropy = tileset->qtd;
        int height = world->height;
        int width = world->width;

        //#pragma omp for collapse(2) nowait
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {

                if(world->map[y][x].collapsedValue == -1)
                {
                    if(world->map[y][x].totalEntropy < lowEntropy){
                        //#pragma omp critical
                        {
                            //if(world->map[y][x].totalEntropy < lowEntropy)
                            {
                                lowEntropy = world->map[y][x].totalEntropy;
                                *lowXResult = x;
                                *lowYResult = y;
                            }
                        }
                        
                    }
                }
            }
        }
    }
}

void propagateCollapse(const int collapseTarget, const int y, const int x, World world, const Tileset tileset) {

    #pragma omp parallel
    {
        const int size = tileset->size;

        if (y-1 >= 0 && world->map[y-1][x].collapsedValue == -1){
            Cell *top = &world->map[y-1][x];
            #pragma omp for
            for (int i = 0; i < tileset->qtd; i++) {
                if (top->options[i] != 0)
                {
                    for (int j = 0; j < tileset->size; j++)
                    {
                        if(tileset->tile[i][size-1][j] != tileset->tile[collapseTarget][0][j]){
                            top->options[i] = 0;
                            top->totalEntropy -= 1;
                            break;
                        }
                    }
                }
            }

        }

        if (y+1 < world->height && world->map[y+1][x].collapsedValue == -1){
            Cell *down =  &world->map[y+1][x];
            #pragma omp for
            for (int i = 0; i < tileset->qtd; i++) {
                if (down->options[i] != 0)
                {
                    for (int j = 0; j < tileset->size; j++)
                    {
                        if(tileset->tile[i][0][j] != tileset->tile[collapseTarget][size-1][j])
                        {
                            down->options[i] = 0;
                            down->totalEntropy -= 1;
                            break;
                        }
                    }
                }

            }
        
        }

        if (x-1 >= 0 && world->map[y][x-1].collapsedValue == -1){
            Cell *left = &world->map[y][x-1];
            #pragma omp for
            for (int i = 0; i < tileset->qtd; i++) {
                if (left->options[i] != 0)
                {
                    for (int j = 0; j < tileset->size; j++)
                    {
                        if(tileset->tile[i][j][size-1] != tileset->tile[collapseTarget][j][0])
                        {
                            left->options[i] = 0;
                            left->totalEntropy -= 1;
                            break;
                        }
                    }
                }

            }
        
        }
        
        if (x+1 < world->width && world->map[y][x+1].collapsedValue == -1){
            Cell *right = &world->map[y][x+1];
            #pragma omp for
            for (int i = 0; i < tileset->qtd; i++) {
                if (right->options[i] != 0)
                {
                    for (int j = 0; j < tileset->size; j++)
                    {
                        if(tileset->tile[i][j][0] != tileset->tile[collapseTarget][j][size-1])
                        {
                            right->options[i] = 0;
                            right->totalEntropy -= 1;
                            break;
                        }
                    }
                }
            }

        }
    
    }

}

void collapse(World world, Tileset tileset) {
    int lowestY = 0;
    int lowestX = 0;

    findLowestEntropy(world, tileset, &lowestX, &lowestY);
    int lowestTotalEntropy = world->map[lowestY][lowestX].totalEntropy;
    if(lowestTotalEntropy == 0)
        return;

    int *entropyPossibility =(int *) malloc(sizeof(int) * lowestTotalEntropy);
    int index = 0;

    #pragma omp parallel for
    for (int j = 0; j < tileset->qtd; j++)
    {
        if(world->map[lowestY][lowestX].options[j] == 1 ){
            entropyPossibility[index] = j;
            #pragma omp atomic
            index++;
        }
    }

    int collapseValue = entropyPossibility[rand() % lowestTotalEntropy];
    free(entropyPossibility);

    #pragma omp parallel 
    {
        int priv_x = lowestX;
        int priv_y = lowestY;
        Cell map = world->map[priv_y][priv_x];
        int priv_qtd = tileset->qtd;
        #pragma omp for  schedule(static,32) nowait
        for (int i = 0; i < priv_qtd; i++) {
            map.options[i] = 0;
        }
    
        #pragma omp sections
        {
            #pragma omp section
            {
                propagateCollapse(collapseValue, lowestY, lowestX, world, tileset);
            }
            #pragma omp section
            {
                world->map[lowestY][lowestX].options[collapseValue] = 1;
                world->map[lowestY][lowestX].collapsedValue = collapseValue;
                world->map[lowestY][lowestX].totalEntropy = 1;
            }
        }
    }
}

void waveFuctionCollapse(Tileset tileset, World world){
    for (int i = 0; i < world->height * world->width; i++)
    {
        collapse(world, tileset);
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

Pgm convertWfc(World world, Tileset tileset){

    int linha = world->height * tileset->size;
    int coluna = world->width * tileset->size;

    Pgm result = new_pgm(linha, coluna);

    int a = 0, b = 0;
    for (int y = 0; y < world->height; y++)
    {
        for (int i = 0; i < tileset->size; i++)
        {
            for (int x = 0; x < world->width; x++)
            {
                for (int j = 0; j < tileset->size; j++)
                {
                    int index = world->map[y][x].collapsedValue;
                    result->data[a][b++] = tileset->tile[(index != -1) ? index : 0][i][j];
                }
            }
            b = 0;
            a++;
        }
    }
    return result;
}
