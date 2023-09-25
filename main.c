#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include "wfc.h"
#include "pgm.h"
#include "tile.h"

#include "cJSON.h"

#define MAX_THREADS 4
#define MIN_SIZE_CELL 70
#define MAX_SIZE_CELL 110
#define QTD_RESULT 5

Pgm convertWfc(World world, Tileset tileset)
{
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


int main()
{
    srand((unsigned int)time(NULL));

    double start_time, end_time;
    cJSON *rootArray = cJSON_CreateArray();

    for (int i = MIN_SIZE_CELL; i <= MAX_SIZE_CELL; i+=(MAX_SIZE_CELL-MIN_SIZE_CELL)/QTD_RESULT)
    {
        cJSON *elementObject = cJSON_CreateObject();
        cJSON *resultArray = cJSON_CreateArray();
        
        for (int j = 1; j <= MAX_THREADS; j++)
        {
            if(j == 1){
                cJSON_AddNumberToObject(elementObject, "cell_height", i);
                cJSON_AddNumberToObject(elementObject, "cell_width", i);
            }
            omp_set_num_threads(j);
            start_time = omp_get_wtime();

            Tileset t = open_tileset("tileset/line.txt");
            World w = new_world(i, i, t->qtd);
            waveFuctionCollapse(t, w);
            Pgm p = convertWfc(w, t);
            char *name = malloc(sizeof(char) * 30);
            sprintf(name, "result/teste%d-%d.pgm", i,j);
            pgm_file(name, p);
            free(name);
            
            
            #pragma omp parallel sections
            {
                #pragma omp section
                {
                    free_world(w);
                }
                #pragma omp section
                {
                    free_pgm(p);
                }
                #pragma omp section
                {
                    free_tileset(t);
                }
            }

            end_time = omp_get_wtime();
            double total_time = end_time - start_time;

            cJSON *resultItem = cJSON_CreateObject();
            cJSON_AddNumberToObject(resultItem, "threads", j);
            cJSON_AddNumberToObject(resultItem, "time", total_time);
            cJSON_AddItemToArray(resultArray, resultItem);
        }
        cJSON_AddItemToObject(elementObject, "result", resultArray);
        cJSON_AddItemToArray(rootArray, elementObject); 
    }

    char *json_string = cJSON_Print(rootArray);
    FILE *arquivo = fopen("result/dados.json", "w");
    if (arquivo != NULL) {
        fprintf(arquivo, json_string);
        fclose(arquivo);
    }
    cJSON_Delete(rootArray);
    free(json_string);

    system("python graficos.py");
    return 0;
}
