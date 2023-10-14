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

void findLowestEntropy(World world, Tileset tileset, int *lowXResult, int *lowYResult, int *lowEntropy){
    *lowEntropy = tileset->qtd+1;
    *lowYResult = 0;
    *lowXResult = 0;

    #pragma omp parallel
    {
        int private_lowEntropy = tileset->qtd+1;
        int private_lowYResult = 0;
        int private_lowXResult = 0;

        #pragma omp for collapse(2)
        for (int y = 0; y < world->height; y++) {
            for (int x = 0; x < world->width; x++) {

                int entropy = 0;
                for (int i = 0; i < tileset->qtd; i++) {
                    entropy += world->map[y][x].options[i];
                }

                if (entropy < private_lowEntropy && 
                world->map[y][x].collapsedValue == -1) {
                    private_lowYResult = y;
                    private_lowXResult = x;
                    private_lowEntropy = entropy;
                }
            }
        }

        #pragma omp critical (find_low_entropy)
        {
            if(private_lowEntropy < *lowEntropy && private_lowEntropy != 0){
                *lowEntropy = private_lowEntropy;
                *lowYResult = private_lowYResult;
                *lowXResult = private_lowXResult;
            }
        }

    }
}

void propagateCollapse(int collapseTarget, int y, int x, World world, Tileset tileset) {
    const int size = tileset->size;

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < tileset->qtd; i++) {
        for (int j = 0; j < tileset->size; j++)
        {
            //UP NEIGHBOR
            if (y-1 >= 0) {
                if(world->map[y-1][x].collapsedValue == -1 && 
                world->map[y-1][x].options[i] != 0 &&
                tileset->tile[i][size-1][j] != tileset->tile[collapseTarget][0][j]
                ){
                    #pragma omp atomic write
                    world->map[y-1][x].options[i] = 0;
                }
            }
        
        
            //DOWN NEIGHBOR
            if (y+1 < world->height) {
                if(world->map[y+1][x].collapsedValue == -1 && 
                world->map[y+1][x].options[i] != 0 &&
                tileset->tile[i][0][j] != tileset->tile[collapseTarget][size-1][j]
                ){
                    #pragma omp atomic write
                    world->map[y+1][x].options[i] = 0;
                }
                
            }
        
        
            //LEFT NEIGHBOR
            if (x-1 >= 0) {
                if(world->map[y][x-1].collapsedValue == -1 && 
                world->map[y][x-1].options[i] != 0 &&
                tileset->tile[i][j][size-1] != tileset->tile[collapseTarget][j][0]
                ){
                    #pragma omp atomic write
                    world->map[y][x-1].options[i] = 0;
                }
                
            }
        
        
            //RIGHT NEIGHBOR
            if (x+1 < world->width) {
                if(world->map[y][x+1].collapsedValue == -1 && 
                world->map[y][x+1].options[i] != 0 &&
                tileset->tile[i][j][0] != tileset->tile[collapseTarget][j][size-1]){
                    #pragma omp atomic write
                    world->map[y][x+1].options[i] = 0;
                }
            }
        }
    }

}

void collapse(World world, Tileset tileset) {

    int lowestTotalEntropy;
    int lowestY;
    int lowestX;

    findLowestEntropy(world, tileset, &lowestX, &lowestY, &lowestTotalEntropy);

    if(lowestTotalEntropy == 0)
        return;

    int *entropyPossibility =(int *) malloc(sizeof(int) * lowestTotalEntropy);
    int index = 0;
    #pragma omp parallel for
    for (int j = 0; j < tileset->qtd; j++)
    {
        if(world->map[lowestY][lowestX].options[j] == 1 ){
            #pragma omp critical
            {
                entropyPossibility[index] = j;
                index += 1;
            }
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

        #pragma omp for nowait
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
