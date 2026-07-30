#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAXCHROMLENGTH 250
#define CITYNUM 5
#define MAXSIZE 20
#define ArcNum 10
#define MAX_RETRY 50
#define FIX_GEN 50  

typedef char CITYNAME[20];
typedef enum { FALSE, TRUE } STATUS;

// 边结构体
typedef struct {
    char from[20];
    char to[20];
    double weight;
} ARCTYPE;

// 图邻接矩阵
typedef struct {
    int vexnum;
    int arcnum;
    CITYNAME vexs[CITYNUM];
    double arcs[CITYNUM][CITYNUM];
} MAPGRAPH;

// 旅行路径
typedef struct {
    CITYNAME city[CITYNUM + 1];
    double globaldistance;
} TRAVELPATH;

// 染色体个体
typedef struct {
    int chrom[MAXCHROMLENGTH];
    int chromlength;
    double fitness;
} INDIVIDUAL;

//1. 基础工具函数
int Index(CITYNAME city, CITYNAME cities[CITYNUM])
{
    int i;
    for (i = 0; i < CITYNUM; i++)
        if (strcmp(city, cities[i]) == 0) break;
    return i;
}

void CityName(int index, CITYNAME cities[CITYNUM], CITYNAME city)
{
    int i;
    for (i = 0; i < CITYNUM; i++)
        if (i == index)
        {
            strcpy(city, cities[i]);
            break;
        }
}

void CreateMap(MAPGRAPH* map, CITYNAME cities[CITYNUM], ARCTYPE arc[])
{
    int i, u, v;
    map->vexnum = CITYNUM;
    map->arcnum = ArcNum;
    for (i = 0; i < CITYNUM; i++)
        strcpy(map->vexs[i], cities[i]);

    for (u = 0; u < CITYNUM; u++)
        for (v = 0; v < CITYNUM; v++)
            map->arcs[u][v] = (u == v) ? 0 : 1e9;

    for (i = 0; i < ArcNum; i++)
    {
        u = Index(arc[i].from, map->vexs);
        v = Index(arc[i].to, map->vexs);
        map->arcs[u][v] = arc[i].weight;
        map->arcs[v][u] = arc[i].weight;
    }
}

void TraPathDis(MAPGRAPH map, TRAVELPATH* trapath)
{
    int i, u, v;
    double total = 0;
    for (i = 0; i < CITYNUM; i++)
    {
        u = Index(trapath->city[i], map.vexs);
        v = Index(trapath->city[i + 1], map.vexs);
        total += map.arcs[u][v];
    }
    trapath->globaldistance = total;
}

void priTraPath(TRAVELPATH trapath)
{
    int i;
    printf("Result Path:");
    for (i = 0; i <= CITYNUM; i++)
    {
        printf("%s", trapath.city[i]);
        if (i < CITYNUM)
            printf("->");
    }
    printf("\nGlobal Distance=%.2f\n\n", trapath.globaldistance);
    fflush(stdout);
}

//  2. 编解码与合法性校验
void Coding(CITYNAME cities[CITYNUM], TRAVELPATH* trapath, INDIVIDUAL* indi)
{
    int i, j;
    indi->chromlength = CITYNUM * CITYNUM;
    for (i = 0; i < indi->chromlength; i++)
        indi->chrom[i] = 0;
    indi->fitness = trapath->globaldistance;
    for (j = 0; j < CITYNUM; j++)
    {
        i = Index(trapath->city[j], cities);
        indi->chrom[j * CITYNUM + i] = 1;
    }
}

void Decoding(CITYNAME cities[CITYNUM], TRAVELPATH* trapath, INDIVIDUAL* indi)
{
    int i, j;
    char city[MAXSIZE];
    for (j = 0; j < indi->chromlength; j++)
    {
        if (indi->chrom[j] == 1)
        {
            i = j % CITYNUM;
            CityName(i, cities, city);
            strcpy(trapath->city[j / CITYNUM], city);
        }
    }
    strcpy(trapath->city[CITYNUM], trapath->city[0]);
    trapath->globaldistance = indi->fitness;
}

STATUS Efficent(INDIVIDUAL indi)
{
    STATUS flag = TRUE;
    int i, j, count;
    int (*temp)[CITYNUM] = (int (*)[CITYNUM])indi.chrom;

    for (i = 0; i < CITYNUM; i++)
    {
        count = 0;
        for (j = 0; j < CITYNUM; j++)
            if (temp[j][i] == 1) count++;
        if (count != 1)
        {
            flag = FALSE;
            break;
        }
    }

    if (flag == TRUE)
    {
        for (i = 0; i < CITYNUM; i++)
        {
            count = 0;
            for (j = 0; j < CITYNUM; j++)
                if (temp[i][j] == 1) count++;
            if (count != 1)
            {
                flag = FALSE;
                break;
            }
        }
    }
    return flag;
}

//  3. 种群初始化
void InitTraPath(MAPGRAPH map, CITYNAME cities[CITYNUM], char start[], TRAVELPATH* trapath)
{
    int i, j = 0, k;
    CITYNAME cs[CITYNUM - 1], temp;

    for (i = 0; i < CITYNUM; i++)
    {
        if (strcmp(cities[i], start) != 0)
            strcpy(cs[j++], cities[i]);
    }
    strcpy(trapath->city[0], start);
    strcpy(trapath->city[CITYNUM], start);

    for (i = CITYNUM - 2; i > 0; i--)
    {
        k = rand() % (i + 1);
        strcpy(temp, cs[i]);
        strcpy(cs[i], cs[k]);
        strcpy(cs[k], temp);
    }

    for (i = 1; i < CITYNUM; i++)
        strcpy(trapath->city[i], cs[i - 1]);

    TraPathDis(map, trapath);
}

void InitIndividual(MAPGRAPH map, CITYNAME cities[CITYNUM], char start[], INDIVIDUAL* indi)
{
    TRAVELPATH trapath;
    InitTraPath(map, cities, start, &trapath);
    Coding(cities, &trapath, indi);
}

void InitPopulation(MAPGRAPH map, CITYNAME cities[CITYNUM], char start[], int popsize, INDIVIDUAL population[])
{
    int i;
    for (i = 0; i < popsize; i++)
    {
        InitIndividual(map, cities, start, &population[i]);
    }
}

//  4. 适应度评估
void EvaluateIndividual(MAPGRAPH map, CITYNAME cities[CITYNUM], INDIVIDUAL* indi)
{
    TRAVELPATH trapath;
    Decoding(cities, &trapath, indi);
    TraPathDis(map, &trapath);
    indi->fitness = trapath.globaldistance;
}

void EvaluatePopulation(MAPGRAPH map, CITYNAME cities[CITYNUM], int popsize, INDIVIDUAL population[])
{
    int i;
    for (i = 0; i < popsize; i++)
        EvaluateIndividual(map, cities, &population[i]);
}

// 5. 遗传算子 
void SelectOperator(int popsize, INDIVIDUAL population[])
{
    int i, j;
    double p, sum_reverse;
    double* prob;
    INDIVIDUAL* new_pop;

    prob = (double*)malloc(popsize * sizeof(double));
    new_pop = (INDIVIDUAL*)malloc(popsize * sizeof(INDIVIDUAL));

    sum_reverse = 0.0;
    for (i = 0; i < popsize; i++)
    {
        prob[i] = 1.0 / population[i].fitness;
        sum_reverse += prob[i];
    }

    for (i = 0; i < popsize; i++)
        prob[i] /= sum_reverse;
    for (i = 1; i < popsize; i++)
        prob[i] += prob[i - 1];

    for (i = 0; i < popsize; i++)
    {
        p = rand() % 10000 / 10000.0;
        j = 0;
        while (j < popsize - 1 && prob[j] < p)
        {
            j++;
        }
        new_pop[i] = population[j];
    }

    for (i = 0; i < popsize; i++)
        population[i] = new_pop[i];

    free(prob);
    free(new_pop);
}

void CrossoverOperator(int popsize, INDIVIDUAL population[], double pc)
{
    int i, j, * index, point, temp, retry;
    double p;

    index = (int*)malloc(popsize * sizeof(int));
    for (i = 0; i < popsize; i++)
        index[i] = i;

    for (i = 0; i < popsize; i++)
    {
        point = rand() % (popsize - i);
        temp = index[i];
        index[i] = index[point + i];
        index[point + i] = temp;
    }

    i = 0;
    while (i < popsize - 1)
    {
        p = rand() % 10000 / 10000.0;
        if (p < pc)
        {
            retry = 0;
            while (retry < MAX_RETRY)
            {
                point = rand() % (population[i].chromlength - CITYNUM) + CITYNUM;

                for (j = point; j < population[i].chromlength; j++)
                {
                    temp = population[index[i]].chrom[j];
                    population[index[i]].chrom[j] = population[index[i + 1]].chrom[j];
                    population[index[i + 1]].chrom[j] = temp;
                }

                if (Efficent(population[index[i]]) && Efficent(population[index[i + 1]]))
                    break;
                else
                {
                    for (j = point; j < population[i].chromlength; j++)
                    {
                        temp = population[index[i]].chrom[j];
                        population[index[i]].chrom[j] = population[index[i + 1]].chrom[j];
                        population[index[i + 1]].chrom[j] = temp;
                    }
                    retry++;
                }
            }
        }
        i += 2;
    }
    free(index);
}

void MutateOperator(int popsize, INDIVIDUAL population[], double pm)
{
    int i, j, retry;
    double p;
    INDIVIDUAL temp_indi;

    i = 0;
    while (i < popsize)
    {
        retry = 0;
        while (retry < MAX_RETRY)
        {
            temp_indi = population[i];
            for (j = CITYNUM; j < population[i].chromlength; j++)
            {
                p = rand() % 10000 / 10000.0;
                if (p < pm)
                    temp_indi.chrom[j] = (temp_indi.chrom[j] == 0) ? 1 : 0;
            }
            if (Efficent(temp_indi) == TRUE)
            {
                population[i] = temp_indi;
                break;
            }
            retry++;
        }
        i++;
    }
}

// 6. 进化控制
void GenerateNextPopulation(int popsize, INDIVIDUAL population[], double pc, double pm)
{
    SelectOperator(popsize, population);
    CrossoverOperator(popsize, population, pc);
    MutateOperator(popsize, population, pm);
}

void GetBestIndividual(int popsize, INDIVIDUAL population[], INDIVIDUAL* best_individual, INDIVIDUAL* global_best)
{
    int i;
    *best_individual = population[0];
    for (i = 1; i < popsize; i++)
    {
        if (population[i].fitness < best_individual->fitness)
            *best_individual = population[i];
    }
    if (best_individual->fitness < global_best->fitness)
        *global_best = *best_individual;
}

void GA(MAPGRAPH map, CITYNAME cities[CITYNUM], char start[], int popsize, double pc, double pm, INDIVIDUAL* currentbest)
{
    INDIVIDUAL* population;
    INDIVIDUAL gen_best, elite;
    int i, worst_idx, j;

    population = (INDIVIDUAL*)malloc(popsize * sizeof(INDIVIDUAL));
    InitPopulation(map, cities, start, popsize, population);

    currentbest->fitness = 1e9;
    GetBestIndividual(popsize, population, &gen_best, currentbest);

    for (i = 0; i < FIX_GEN; i++)
    {
        elite = gen_best;

        GenerateNextPopulation(popsize, population, pc, pm);
        EvaluatePopulation(map, cities, popsize, population);

        worst_idx = 0;
        for (j = 1; j < popsize; j++)
        {
            if (population[j].fitness > population[worst_idx].fitness)
                worst_idx = j;
        }
        population[worst_idx] = elite;

        GetBestIndividual(popsize, population, &gen_best, currentbest);
    }
    free(population);
}

//  7. 主函数 
int main()
{
    double pc = 0.5;
    double pm = 0.1;
    int popsize = 12;
    int count = 10;
    int i;
    INDIVIDUAL currentbest, global_best;
    TRAVELPATH dectrapath, best_path;
    MAPGRAPH map;
    char start[] = "Urumqi";

    srand((unsigned int)time(NULL));

    CITYNAME cities[CITYNUM] = { "Urumqi", "Karamay", "Burqin", "Yining", "Korla" };
    ARCTYPE map_arc[ArcNum] = {
        {"Urumqi", "Karamay", 310}, {"Urumqi", "Burqin", 680},
        {"Urumqi", "Yining", 700},  {"Urumqi", "Korla", 480},
        {"Karamay", "Burqin", 320}, {"Karamay", "Yining", 550},
        {"Karamay", "Korla", 600},  {"Burqin", "Yining", 780},
        {"Burqin", "Korla", 850},   {"Yining", "Korla", 620}
    };

    CreateMap(&map, cities, map_arc);
    global_best.fitness = 1e9;

    //printf("算法开始运行，请稍候...\n");
    //fflush(stdout);

    for (i = 0; i < count; i++)
    {
        GA(map, cities, start, popsize, pc, pm, &currentbest);

        if (currentbest.fitness < global_best.fitness)
            global_best = currentbest;

        // 【修正】先解码再打印，避免输出乱码
        Decoding(cities, &dectrapath, &currentbest);
        printf("The %dth solution:", i + 1);
        priTraPath(dectrapath);
    }

    Decoding(cities, &best_path, &global_best);
    printf("\n========== Preferred Plan ==========\n");
    priTraPath(best_path);
    printf("Total driving mileage:%.2f km\n", best_path.globaldistance);

    system("pause");
    return 0;
}