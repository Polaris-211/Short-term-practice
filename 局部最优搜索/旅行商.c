#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

#define MAXSIZE 20
#define CITYNUM 5
#define ArcNum (CITYNUM - 1)*CITYNUM / 2
#define STATUS int 
#define TRUE 1
#define FALSE 0

typedef char CITYNAME[MAXSIZE];

typedef struct ARCTYPE {
    CITYNAME v, u;
    float distance;
} *MAPGRAPH;

typedef struct {
    CITYNAME city[CITYNUM + 1];
    float globaldistance;
}TRAVELPATH;

float dist(MAPGRAPH map, CITYNAME u, CITYNAME v) {
    float dis = 0;
    int i;
    for (i = 0; i < ArcNum; i++)
        if ((strcmp(map[i].u, u) == 0 && strcmp(map[i].v, v) == 0) || (strcmp(map[i].u, v) == 0 && strcmp(map[i].v, u) == 0)) {
            dis = map[i].distance;
            break;
        }
    return dis;
}

void TraPathDis(MAPGRAPH map, TRAVELPATH* trapath) {
    float dis = 0;
    int i;
    for (i = 0; i < CITYNUM; i++)
        dis += dist(map, trapath->city[i], trapath->city[i + 1]);
    trapath->globaldistance = dis;
}

void CopyTraPath(TRAVELPATH* trapath, TRAVELPATH* newtrapath) {
    int i;
    // 循环到 CITYNUM，把回程起点 city[5] 一起复制
    for (i = 0; i <= CITYNUM; i++)
        strcpy(newtrapath->city[i], trapath->city[i]);
    // 同步复制总距离，避免距离计算错乱
    newtrapath->globaldistance = trapath->globaldistance;
}

void ExchangeCityForTraPath(TRAVELPATH* trapath, int loci, int locj) {
    CITYNAME temp;
    strcpy(temp, trapath->city[loci]);
    strcpy(trapath->city[loci], trapath->city[locj]);
    strcpy(trapath->city[locj], temp);
}

#define NEIGHBORNUM ((CITYNUM-1)*(CITYNUM-2)/2)

TRAVELPATH* ExpandTraPaths(MAPGRAPH map, TRAVELPATH* trapath) {
    CITYNAME* trapath1, temp;
    int i, loci, locj;
    TRAVELPATH* neighbors;
    neighbors = (TRAVELPATH*)malloc(sizeof(TRAVELPATH) * NEIGHBORNUM);
    for (i = 0; i < NEIGHBORNUM; i++)
        CopyTraPath(trapath, &neighbors[i]);
    for (i = 0, loci = 1; loci < CITYNUM - 1; loci++)
        for (locj = loci + 1; locj < CITYNUM; locj++) {
            ExchangeCityForTraPath(&neighbors[i], loci, locj);
            TraPathDis(map, &neighbors[i]);
            i++;
        }
    return neighbors;
}

void ClearAllTraPaths(TRAVELPATH* neighbors) { free(neighbors); }

void InitTraPath(MAPGRAPH map, CITYNAME cities[CITYNUM], CITYNAME start, TRAVELPATH* trapath) {
    int i, j = 0, k, n;
    CITYNAME cs[CITYNUM - 1], temp;
    for (i = 0; i < CITYNUM; i++)
        if (strcmp(cities[i], start) != 0)
            strcpy(cs[j++], cities[i]);
    strcpy(trapath->city[0], start);
    strcpy(trapath->city[CITYNUM], start);
    //srand((unsigned)time(NULL));
    n = rand() % (CITYNUM - 1);
    for (i = 0; i < n; i++) {
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

void SearchResult(MAPGRAPH map, TRAVELPATH* trapath, TRAVELPATH* trapathres) {
    TRAVELPATH* neighbors;
    STATUS flag = FALSE;
    int i;
    CopyTraPath(trapath, trapathres);
    TraPathDis(map, trapathres);
    while (TRUE) {
        neighbors = ExpandTraPaths(map, trapathres);
        for (i = 0; i < NEIGHBORNUM; i++)
            if (trapathres->globaldistance > neighbors[i].globaldistance) {
                flag = TRUE;
                CopyTraPath(&neighbors[i], trapathres);
                TraPathDis(map, trapathres);
                break;
            }
        if (flag == TRUE) flag = FALSE;
        else break;
        ClearAllTraPaths(neighbors);
    }
}

void PriTraPath(TRAVELPATH* trapath) {
    srand((unsigned int)time(NULL));
    int i;
    for (int i = 0; i <= CITYNUM; i++) {
        printf("%s", trapath->city[i]);
        if (i < CITYNUM) printf("->");
    }
    printf(" Global distance = %.2f\n", trapath->globaldistance);
}

int main() {
    int i, j, k;
    TRAVELPATH trapath, trapathres;
    CITYNAME cities[CITYNUM] = { "A", "B", "C", "D", "E" };
    struct ARCTYPE map[ArcNum] = { {"A", "B", 3}, {"A", "C", 2},
                                  {"A", "D", 9}, {"A", "E", 7},
                                  {"B", "C", 7}, {"B", "D", 2},
                                  {"B", "E", 5}, {"C", "D", 9},
                                  {"C", "E", 2}, {"D", "E", 3} };
    CITYNAME startCity;
    strcpy(startCity, "A");
    for (i = 0; i < 2; i++) {
        InitTraPath(map, cities, startCity, &trapath);
        printf("Init path ;");
        PriTraPath(&trapath);
        SearchResult(map, &trapath, &trapathres);
        printf("Result Path:");
        PriTraPath(&trapathres);
    }
    return 0;
}
