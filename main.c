#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include "pgm.h"
#include "wfc.h"
#include "tile.h"

#include "cJSON.h"

#define MAX_THREADS 8
#define MIN_THREADS 1
#define MAX_SIZE_CELL 3
#define QTD_RESULT 1
#define REPEAT 3

int main()
{
    srand((unsigned int)time(NULL));

    double start_time, end_time;
    cJSON *rootArray = cJSON_CreateArray();

    int increment = (MAX_SIZE_CELL)/QTD_RESULT;

    for(int i = MAX_SIZE_CELL; i > 0; i-= increment)
    {
        cJSON *elementObject = cJSON_CreateObject();
        cJSON *resultArray = cJSON_CreateArray();

        cJSON_AddNumberToObject(elementObject, "cell_height", i);
        cJSON_AddNumberToObject(elementObject, "cell_width", i);

        for (int j = MIN_THREADS; j <= MAX_THREADS; j+=j)
        {
            omp_set_num_threads(j);

            cJSON *resultItem = cJSON_CreateObject();
            cJSON *timeArray = cJSON_CreateArray();

            cJSON_AddNumberToObject(resultItem, "threads", j);
            for(int k = 0; k < REPEAT; k++)
            {
                Tileset t = open_tileset("tileset/line.txt");
                World w = new_world(i, i, t->qtd);
                //------------------------------------------------
                start_time = omp_get_wtime();
                waveFuctionCollapse(t, w);
                end_time = omp_get_wtime();
                //------------------------------------------------
                Pgm p = convertWfc(w, t);
                char *name = malloc(sizeof(char) * 100);
                sprintf(name, "result/imagem/wfc(C-%dx%d)(T-%d)(R-%d).pgm", i, i, j, k);
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

                double total_time = end_time - start_time;
                cJSON *time = cJSON_CreateNumber(total_time);
                cJSON_AddItemToArray(timeArray, time);
            }
            cJSON_AddItemToObject(resultItem, "time", timeArray);
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
