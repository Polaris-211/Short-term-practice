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

typedef char CITYNAME[20];
typedef enum { FALSE, TRUE } STATUS;

// 边结构体，用于初始化图
typedef struct {
    char from[20];
    char to[20];
    double weight;
} ARCTYPE;

// 图的邻接矩阵存储结构
typedef struct {
    int vexnum;
    int arcnum;
    CITYNAME vexs[CITYNUM];
    double arcs[CITYNUM][CITYNUM];
} MAPGRAPH;

// 旅行商路径结构体
typedef struct {
    CITYNAME city[CITYNUM + 1];
    double globaldistance;
} TRAVELPATH;

// 染色体个体结构体
typedef struct {
    int chrom[MAXCHROMLENGTH];
    int chromlength;
    double fitness;
} INDIVIDUAL;

// 函数声明
int Index(CITYNAME city, CITYNAME cities[CITYNUM]);
void CityName(int index, CITYNAME cities[CITYNUM], CITYNAME city);
void CreateMap(MAPGRAPH* map, CITYNAME cities[CITYNUM], ARCTYPE arc[]);
void TraPathDis(MAPGRAPH map, TRAVELPATH* trapath);
void priTraPath(TRAVELPATH trapath);

void Coding(CITYNAME cities[CITYNUM], TRAVELPATH* trapath, INDIVIDUAL* indi);
void Decoding(CITYNAME cities[CITYNUM], TRAVELPATH* trapath, INDIVIDUAL* indi);

void InitTraPath(MAPGRAPH map, CITYNAME cities[CITYNUM], CITYNAME start, TRAVELPATH* trapath);
void InitIndividual(MAPGRAPH map, CITYNAME cities[CITYNUM], CITYNAME start, INDIVIDUAL* indi);
void InitPopulation(MAPGRAPH map, CITYNAME cities[CITYNUM], CITYNAME start, int popsize, INDIVIDUAL population[]);

void EvaluateIndividual(MAPGRAPH map, CITYNAME cities[CITYNUM], INDIVIDUAL* indi);
void EvaluatePopulation(MAPGRAPH map, CITYNAME cities[CITYNUM], int popsize, INDIVIDUAL population[]);

STATUS Efficent(INDIVIDUAL indi);

void SelectOperator(int popsize, INDIVIDUAL population[]);
void CrossoverOperator(int popsize, INDIVIDUAL population[], double pc);
void MutateOperator(int popsize, INDIVIDUAL population[], double pm);

void GenerateNextPopulation(int popsize, INDIVIDUAL population[], double pc, double pm);
void GetBestIndividual(int popsize, INDIVIDUAL population[], INDIVIDUAL* bestindividual, INDIVIDUAL* currentbestindividual);

void GA(MAPGRAPH map, CITYNAME cities[CITYNUM], char* start, int popsize, double pc, double pm, int generation, INDIVIDUAL* currentbestindividual);

// 1. 工具函数
//1.通过城市名确定存储位置
int Index(CITYNAME city, CITYNAME cities[CITYNUM])
{
    int i;
    for (i = 0; i < CITYNUM; i++)
        if (strcmp(city, cities[i]) == 0) break;
    return i;
}

//2.给定存储位置获取城市名
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
}

// 2. 编码与解码
//1.对路径进行编码
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

//2.解码为路径
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

// 3. 初始化
//1.随机初始化一条路径
void InitTraPath(MAPGRAPH map, CITYNAME cities[CITYNUM], CITYNAME start, TRAVELPATH* trapath)
{
    int i, j = 0, k, n;
    CITYNAME cs[CITYNUM - 1], temp;

    for (i = 0; i < CITYNUM; i++)
    {
        if (strcmp(cities[i], start) != 0)
            strcpy(cs[j++], cities[i]);
    }
    strcpy(trapath->city[0], start);
    strcpy(trapath->city[CITYNUM], start);

    n = rand() % (CITYNUM - 1);
    for (i = 0; i < n; i++)
    {
        j = rand() % (CITYNUM - 1);
        k = rand() % (CITYNUM - 1);
        if (j == k) continue;
        strcpy(temp, cs[j]);
        strcpy(cs[j], cs[k]);
        strcpy(cs[k], temp);
    }
    for (i = 1; i < CITYNUM; i++)
        strcpy(trapath->city[i], cs[i - 1]);

    TraPathDis(map, trapath);
}

//对随机初始化的路径编码
void InitIndividual(MAPGRAPH map, CITYNAME cities[CITYNUM], CITYNAME start, INDIVIDUAL* indi)
{
    TRAVELPATH trapath;
    InitTraPath(map, cities, start, &trapath);
    Coding(cities, &trapath, indi);
}

//随机初始化路径的种群
void InitPopulation(MAPGRAPH map, CITYNAME cities[CITYNUM], CITYNAME start, int popsize, INDIVIDUAL population[])
{
    int i = 0, j, k;
    srand((unsigned)time(NULL));
    for (i = 0; i < popsize; i++)
    {
        InitIndividual(map, cities, start, &population[i]);
        for (j = 0; j < 32767; j++)
            for (k = 0; k < 3276; k++);
    }
}

// 4. 适应度计算
//1.求解路径长度
void EvaluateIndividual(MAPGRAPH map, CITYNAME cities[CITYNUM], INDIVIDUAL* indi)
{
    TRAVELPATH trapath;
    Decoding(cities, &trapath, indi);
    TraPathDis(map, &trapath);
    indi->fitness = trapath.globaldistance;
}

//2.计算种群个体适应度
void EvaluatePopulation(MAPGRAPH map, CITYNAME cities[CITYNUM], int popsize, INDIVIDUAL population[])
{
    int i;
    for (i = 0; i < popsize; i++)
        EvaluateIndividual(map, cities, &population[i]);
}

// 5. 有效性校验，判断个体有效性，每行每列和为1
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
        if (count > 1 || count == 0)
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
            if (count > 1 || count == 0)
            {
                flag = FALSE;
                break;
            }
        }
    }
    return flag;
}

// 6. 遗传算子
//1.轮盘赌选择法选择新的种群
void SelectOperator(int popsize, INDIVIDUAL population[])
{
    int i, j;
    double p, sum;
    double* ps;
    INDIVIDUAL* newpopulation;

    ps = (double*)malloc(popsize * sizeof(double));
    newpopulation = (INDIVIDUAL*)malloc(popsize * sizeof(INDIVIDUAL));

    for (sum = 0, i = 0; i < popsize; i++)
        sum += population[i].fitness;
    for (i = 0; i < popsize; i++)
        ps[i] = population[i].fitness / sum;

    for (sum = 0, i = 0; i < popsize; i++)
        sum += 1 - ps[i];
    for (i = 0; i < popsize; i++)
        ps[i] = (1 - ps[i]) / sum;

    // 补充累计概率计算（原书隐含逻辑，保证轮盘赌正常运行）
    for (i = 1; i < popsize; i++)
        ps[i] += ps[i - 1];

    for (i = 0; i < popsize; i++)
    {
        p = rand() % 1000 / 1000.0;
        for (j = 0; ps[j] < p; j++);
        newpopulation[i] = population[j];
    }

    for (i = 0; i < popsize; i++)
        population[i] = newpopulation[i];

    free(ps);
    free(newpopulation);
}

//2.交叉产生新的种群
void CrossoverOperator(int popsize, INDIVIDUAL population[], double pc)
{
    int i, j, * index, point, temp;
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
        p = rand() % 1000 / 1000.0;
        if (p < pc)
        {
        LOOP:
            point = rand() % (population[i].chromlength - 1) + 1;
            for (j = point; j < population[i].chromlength; j++)
            {
                temp = population[index[i]].chrom[j];
                population[index[i]].chrom[j] = population[index[i + 1]].chrom[j];
                population[index[i + 1]].chrom[j] = temp;
            }
            if (!Efficent(population[index[i]]) || !Efficent(population[index[i + 1]]))
            {
                for (j = point; j < population[i].chromlength; j++)
                {
                    temp = population[index[i]].chrom[j];
                    population[index[i]].chrom[j] = population[index[i + 1]].chrom[j];
                    population[index[i + 1]].chrom[j] = temp;
                }
                goto LOOP;
            }
        }
        i += 2;
    }
    free(index);
}

//3.通过变异产生新的种群
void MutateOperator(int popsize, INDIVIDUAL population[], double pm)
{
    int i, j;
    double p;
    INDIVIDUAL indi;
    i = 0;
    while (i < popsize)
    {
    LOOP:
        indi = population[i];
        for (j = CITYNUM; j < population[i].chromlength; j++)
        {
            p = rand() % 1000 / 1000.0;
            if (p < pm)
                indi.chrom[j] = (indi.chrom[j] == 0) ? 1 : 0;
        }
        if (Efficent(indi) == FALSE)
            goto LOOP;
        population[i] = indi;
        i++;
    }
}

// 7. 种群进化与最优个体
//1.通过遗传算子产生新的种群
void GenerateNextPopulation(int popsize, INDIVIDUAL population[], double pc, double pm)
{
    SelectOperator(popsize, population);
    CrossoverOperator(popsize, population, pc);
    MutateOperator(popsize, population, pm);
}

//2.从路径种群中选取并更新最短路径
void GetBestIndividual(int popsize, INDIVIDUAL population[], INDIVIDUAL* bestindividual, INDIVIDUAL* currentbestindividual)
{
    int i;
    *bestindividual = population[0];
    for (i = 1; i < popsize; i++)
    {
        if (population[i].fitness < bestindividual->fitness)
            *bestindividual = population[i];
    }
    if (bestindividual->fitness < currentbestindividual->fitness)
        *currentbestindividual = *bestindividual;
}

// 8. 遗传算法主流程
void GA(MAPGRAPH map, CITYNAME cities[CITYNUM], char* start, int popsize, double pc, double pm, int generation, INDIVIDUAL* currentbestindividual)
{
    INDIVIDUAL* population, bestindividual, oldbestindividual;
    int i;
    population = (INDIVIDUAL*)malloc(popsize * sizeof(INDIVIDUAL));
    InitPopulation(map, cities, start, popsize, population);

    bestindividual = population[0];
    for (i = 1; i < popsize; i++)
        if (population[i].fitness < bestindividual.fitness)
            bestindividual = population[i];
    *currentbestindividual = bestindividual;

    oldbestindividual = bestindividual;
    i = 0;

    while (i < generation)
    {
        GenerateNextPopulation(popsize, population, pc, pm);
        EvaluatePopulation(map, cities, popsize, population);
        GetBestIndividual(popsize, population, &bestindividual, currentbestindividual);

        if (fabs(currentbestindividual->fitness - oldbestindividual.fitness) < 1e-5)
        {
            i++;
        }
        else
        {
            oldbestindividual = *currentbestindividual;
            i = 0;
        }
    }
    free(population);
}

// 主函数
int main()
{
    double pc = 0.5;
    double pm = 0.1;
    int popsize = 12;
    int generation = 5;
    int count = 10;
    INDIVIDUAL currentbestindividual;
    TRAVELPATH dectrapath;
    MAPGRAPH map;

    CITYNAME cities[CITYNUM] = { "A", "B", "C", "D", "E" };
    ARCTYPE map_arc[ArcNum] = {
        {"A", "B", 3}, {"A", "C", 2}, {"A", "D", 9}, {"A", "E", 7},
        {"B", "C", 7}, {"B", "D", 2}, {"B", "E", 5},
        {"C", "D", 9}, {"C", "E", 2},
        {"D", "E", 3}
    };
    char* start = "A";

    CreateMap(&map, cities, map_arc);

    int i;
    for (i = 0; i < count; i++)
    {
        GA(map, cities, start, popsize, pc, pm, generation, &currentbestindividual);
        Decoding(cities, &dectrapath, &currentbestindividual);
        priTraPath(dectrapath);
    }

    return 0;
}