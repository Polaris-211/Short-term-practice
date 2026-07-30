#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

#define MAXSIZE 20
#define CITYNUM 5
#define ArcNum ((CITYNUM - 1) * CITYNUM / 2)
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
} TRAVELPATH;

float dist(MAPGRAPH map, CITYNAME u, CITYNAME v) {
    float dis = 0;
    int i;
    for (i = 0; i < ArcNum; i++) {
        if ((strcmp(map[i].u, u) == 0 && strcmp(map[i].v, v) == 0) ||
            (strcmp(map[i].u, v) == 0 && strcmp(map[i].v, u) == 0)) {
            dis = map[i].distance;
            break;
        }
    }
    return dis;
}

void TraPathDis(MAPGRAPH map, TRAVELPATH* trapath) {
    float dis = 0;
    int i;
    for (i = 0; i < CITYNUM; i++) {
        dis += dist(map, trapath->city[i], trapath->city[i + 1]);
    }
    trapath->globaldistance = dis;
}

void CopyTraPath(TRAVELPATH* trapath, TRAVELPATH* newtrapath) {
    int i;
    for (i = 0; i <= CITYNUM; i++) {
        strcpy(newtrapath->city[i], trapath->city[i]);
    }
    newtrapath->globaldistance = trapath->globaldistance;
}

void ExchangeCityForTraPath(TRAVELPATH* trapath, int loci, int locj) {
    CITYNAME temp;
    strcpy(temp, trapath->city[loci]);
    strcpy(trapath->city[loci], trapath->city[locj]);
    strcpy(trapath->city[locj], temp);
}

void ReverseSegmentForTraPath(TRAVELPATH* trapath, int left, int right) {
    while (left < right) {
        ExchangeCityForTraPath(trapath, left, right);
        left++;
        right--;
    }
}

#define NEIGHBORNUM ((CITYNUM - 1) * (CITYNUM - 2) / 2)

TRAVELPATH* ExpandTraPaths(MAPGRAPH map, TRAVELPATH* trapath) {
    int i, loci, locj;
    TRAVELPATH* neighbors;
    neighbors = (TRAVELPATH*)malloc(sizeof(TRAVELPATH) * NEIGHBORNUM);
    for (i = 0; i < NEIGHBORNUM; i++) {
        CopyTraPath(trapath, &neighbors[i]);
    }
    for (i = 0, loci = 1; loci < CITYNUM - 1; loci++) {
        for (locj = loci + 1; locj < CITYNUM; locj++) {
            ReverseSegmentForTraPath(&neighbors[i], loci, locj);
            TraPathDis(map, &neighbors[i]);
            i++;
        }
    }
    return neighbors;
}

void ClearAllTraPaths(TRAVELPATH* neighbors) {
    free(neighbors);
}

void InitTraPath(MAPGRAPH map, CITYNAME cities[CITYNUM], CITYNAME start, TRAVELPATH* trapath) {
    int i, j, k;
    CITYNAME cs[CITYNUM - 1], temp;
    j = 0;
    for (i = 0; i < CITYNUM; i++) {
        if (strcmp(cities[i], start) != 0) {
            strcpy(cs[j++], cities[i]);
        }
    }
    strcpy(trapath->city[0], start);
    strcpy(trapath->city[CITYNUM], start);

    for (i = CITYNUM - 2; i > 0; i--) {
        k = rand() % (i + 1);
        strcpy(temp, cs[i]);
        strcpy(cs[i], cs[k]);
        strcpy(cs[k], temp);
    }

    for (i = 1; i < CITYNUM; i++) {
        strcpy(trapath->city[i], cs[i - 1]);
    }
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
        flag = FALSE;
        for (i = 0; i < NEIGHBORNUM; i++) {
            if (trapathres->globaldistance > neighbors[i].globaldistance) {
                flag = TRUE;
                CopyTraPath(&neighbors[i], trapathres);
                TraPathDis(map, trapathres);
                break;
            }
        }
        ClearAllTraPaths(neighbors);
        if (flag == FALSE) {
            break;
        }
    }
}

void PriTraPath(TRAVELPATH* trapath) {
    int i;
    for (i = 0; i <= CITYNUM; i++) {
        printf("%s", trapath->city[i]);
        if (i < CITYNUM) {
            printf("->");
        }
    }
    printf(" \nTotal distance = %.2f km\n", trapath->globaldistance);
}

int main() {
    int i;
    TRAVELPATH trapath, trapathres, bestpath;
    CITYNAME cities[CITYNUM] = { "Urumqi", "Karamay", "Burqin", "Yining", "Korla" };
    struct ARCTYPE map[ArcNum] = {
        {"Urumqi", "Karamay", 310}, {"Urumqi", "Burqin", 680},
        {"Urumqi", "Yining", 700},     {"Urumqi", "Korla", 480},
        {"Karamay", "Burqin", 320}, {"Karamay", "Yining", 550},
        {"Karamay", "Korla", 600}, {"Burqin", "Yining", 780},
        {"Burqin", "Korla", 850},   {"Yining", "Korla", 620}
    };
    CITYNAME startCity;

    srand((unsigned int)time(NULL));
    strcpy(startCity, "Urumqi");
    bestpath.globaldistance = 10000;

    for (i = 0; i < 5; i++) {
        InitTraPath(map, cities, startCity, &trapath);
        printf("Init path: ");
        PriTraPath(&trapath);

        SearchResult(map, &trapath, &trapathres);
        printf("Optimized path: ");
        PriTraPath(&trapathres);
        printf("----------------------------------------\n");

        if (trapathres.globaldistance < bestpath.globaldistance) {
            CopyTraPath(&trapathres, &bestpath);
        }
    }
    printf("\nBest self-driving route:\n");
    PriTraPath(&bestpath);
    return 0;
}