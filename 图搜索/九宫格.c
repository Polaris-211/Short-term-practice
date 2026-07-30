#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXSIZE 3
#define ZERO 0
#define TRUE 1
#define FALSE 0
#define MAXDEPTH 30
typedef int STATUS;

typedef int NODE[MAXSIZE][MAXSIZE];

typedef struct NeighborNode {
    NODE state;
    struct NeighborNode* next;
}NeighborNode, * NEIGHBOR;

typedef struct PathNode {
    NODE state;
    struct PathNode* next;
}PathNode, * PATH, * STACK;

STATUS equal(NODE node1, NODE node2) {
    int i, j;
    STATUS flag = TRUE;
    for (i = 0; i < MAXSIZE; i++) {
        for (j = 0; j < MAXSIZE; j++)
            if (node1[i][j] != node2[i][j]) {
                flag = FALSE;
                break;
            }
        if (flag == FALSE) break;
    }
    return flag;
}

void setValue(NODE node1, NODE node2) {
    int i, j;
    for (i = 0; i < MAXSIZE; i++)
        for (j = 0; j < MAXSIZE; j++)
            node1[i][j] = node2[i][j];
}

void PriNode(NODE node) {
    int i, j;
    for (i = 0; i < MAXSIZE; i++) {
        for (j = 0; j < MAXSIZE; j++) printf("%5d", node[i][j]);
        printf("\n");
    }
    printf("********************\n");
}

STATUS Locate(NODE node, int* hori, int* vert) {
    int i, j;
    STATUS flag = FALSE;
    for (i = 0; i < MAXSIZE; i++) {
        for (j = 0; j < MAXSIZE; j++)
            if (node[i][j] == ZERO) {
                *hori = i;
                *vert = j;
                flag = TRUE;
                break;
            }
        if (flag == TRUE) break;
    }
    return flag;
}

void Exchange(NODE temp, NODE ori, int hori1, int vert1, int hori2, int vert2) {
    setValue(temp, ori);
    int t = temp[hori1][vert1];
    temp[hori1][vert1] = temp[hori2][vert2];
    temp[hori2][vert2] = t;
}

NEIGHBOR AddAnAdjacents(NEIGHBOR head, NODE newState) {
    NEIGHBOR newNode = (NEIGHBOR)malloc(sizeof(NeighborNode));
    setValue(newNode->state, newState);
    newNode->next = NULL;
    if (!head) return newNode;
    NEIGHBOR adjacents = head;
    while (adjacents->next) adjacents = adjacents->next;
    adjacents->next = newNode;
    return head;
}

void FreeNeighbor(NEIGHBOR head) {
    NEIGHBOR adjacents;
    while (head) {
        adjacents = head;
        head = head->next;
        free(adjacents);
    }
}

NEIGHBOR ExpandNodes(NODE node, int zero)
{
    NEIGHBOR adjacents = NULL;
    int h, v;
    NODE tempnode;

    if (!Locate(node, &h, &v))
        return adjacents;

    if (h == 0 && v == 0)
    {
        // 左上角：右、下可以走
        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h, v + 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h + 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 0 && v == 1)
    {
        // 第一行中间：左、右、下
        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h, v - 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h, v + 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h + 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 0 && v == 2)
    {
        // 第一行最右：左、下
        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h, v - 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h + 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 1 && v == 0)
    {
        // 第二行第一列：上、下、右
        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h - 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h + 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h, v + 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 1 && v == 1)
    {
        // 中心位置：上下左右
        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h - 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h + 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h, v - 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h, v + 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 1 && v == 2)
    {
        // 第二行最右：上、下、左
        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h - 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h + 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h, v - 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 2 && v == 0)
    {
        // 第三行第一列：上、右
        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h - 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h, v + 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 2 && v == 1)
    {
        // 第三行中间：上、左、右
        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h - 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h, v - 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h, v + 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 2 && v == 2)
    {
        // 右下角：上、左
        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h - 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);

        setValue(tempnode, node);
        Exchange(tempnode, node, h, v, h, v - 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }

    return adjacents;
}

STATUS IsInPath(PATH path, NODE node) {
    PATH adjacents = path;
    while (adjacents) {
        if (equal(adjacents->state, node))
            return TRUE;
        adjacents = adjacents->next;
    }
    return FALSE;
}

void PushPath(PATH* path, NODE state) {
    PATH newPath = (PATH)malloc(sizeof(PathNode));
    setValue(newPath->state, state);
    newPath->next = *path;
    *path = newPath;
}

PATH RevPath(PATH path) {
    PATH newPath = NULL;
    PATH adjacents = path;
    while (adjacents) {
        PushPath(&newPath, adjacents->state);
        adjacents = adjacents->next;
    }
    return newPath;
}

void PriPath(PATH path) {
    PATH adjacents = path;
    int step = 0;
    while (adjacents) {
        printf("The %d steps\n", step++);
        PriNode(adjacents->state);
        adjacents = adjacents->next;
    }
}

int LengthPath(PATH path) {
    PATH adjacents = path;
    int len = 0;
    while (adjacents) {
        len++;
        adjacents = adjacents->next;
    }
    return len;
}

void CleanPath(PATH path) {
    PATH adjacents;
    while (path) {
        adjacents = path;
        path = path->next;
        free(adjacents);
    }
}

// 计算九宫逆序数（忽略0）
int GetInversion(NODE node)
{
    int arr[9], cnt = 0, inv = 0;
    for (int i = 0;i < 3;i++)
        for (int j = 0;j < 3;j++)
            if (node[i][j] != 0) arr[cnt++] = node[i][j];
    for (int i = 0;i < 8;i++)
        for (int j = i + 1;j < 8;j++)
            if (arr[i] > arr[j]) inv++;
    return inv;
}

STATUS SearchSubGraph(NODE current, NODE end, PATH* path, PATH* searchpath, int zero, int depth)
{
    NEIGHBOR adjacents = ExpandNodes(current, zero);
    STATUS flag = FALSE;

    if (IsInPath(*searchpath, current)) {
        FreeNeighbor(adjacents);
        return FALSE;
    }

    if (equal(current, end)) {
        PushPath(searchpath, current);
        FreeNeighbor(adjacents);
        return TRUE;
    }

    PushPath(searchpath, current);

    while (adjacents != NULL)
    {
        if (!IsInPath(*searchpath, adjacents->state))
        {
            if (SearchSubGraph(adjacents->state, end, path, searchpath, zero, depth + 1) == TRUE)
            {
                flag = TRUE;
                break;
            }
        }
        adjacents = adjacents->next;
    }

    if (flag == FALSE) {
        PATH top = *searchpath;
        *searchpath = (*searchpath)->next;
        free(top);
    }

    FreeNeighbor(adjacents);
    return flag;
}

STATUS SearchPath(NODE start, NODE end, PATH* path, int zero) {
    STATUS flag;
    PATH searchpath = NULL;
    CleanPath(*path);
    *path = NULL;
    int invS = GetInversion(start);
    int invE = GetInversion(end);
    if ((invS % 2) != (invE % 2)) {
        printf("判定：起始与目标逆序数奇偶不同，两状态不在同一连通分量，九宫图非连通图\n");
        return FALSE;
    }
    flag = SearchSubGraph(start, end, &path, &searchpath, zero, 0);
    if (flag == TRUE) {
        PATH p = searchpath;
        while (p != NULL) {
            PushPath(path, p->state);
            p = p->next;
        }
    }
    CleanPath(searchpath);
    return flag;
}

int main() {
    PATH path = NULL;
    STATUS flag;
    NODE start = {
        {1, 2, 3},
        {8, 4, 0},
        {7, 6, 5}
    };
    NODE end = {
        {2, 1, 3},
        {8, 0, 4},
        {7, 6, 5}
    };
    printf("Search from:\n");
    PriNode(start);
    printf("to:\n");
    PriNode(end);
    flag = SearchPath(start, end, &path, ZERO);
    if (flag) {
        PATH rev = RevPath(path);
        printf("Path:\n");
        PriPath(path);
        printf("Step:%d\n", LengthPath(rev) - 1);
        CleanPath(rev);
    }
    else printf("No Path");
    CleanPath(path);
    system("pause");
    return 0;
}


