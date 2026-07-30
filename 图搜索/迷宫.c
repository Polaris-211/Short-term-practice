#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define N 4
#define M 4

int maze[N][M] = {
    {0,1,0,0},
    {0,0,0,1},
    {1,0,0,0},
    {0,0,1,0}
};
// 方向：上下左右
int dir[4][2] = { {-1,0},{1,0},{0,-1},{0,1} };
int vis[N][M];
int path[N * M][2]; // 存储单条路径坐标
int pathCnt = 0;
int allCount = 0; // 总路径数

// 打印单条路径
void PrintOnePath()
{
    allCount++;
    printf("第%d条路径：", allCount);
    for (int i = 0;i < pathCnt;i++)
        printf("(%d,%d) ", path[i][0], path[i][1]);
    printf("\n");
}

// dfs：找所有路径
void DFS(int x, int y)
{
    // 越界 / 墙 / 已访问
    if (x < 0 || x >= N || y < 0 || y >= M || maze[x][y] == 1 || vis[x][y] == 1)
        return;
    // 记录当前点
    vis[x][y] = 1;
    path[pathCnt][0] = x;
    path[pathCnt][1] = y;
    pathCnt++;

    // 到达终点，输出
    if (x == N - 1 && y == M - 1)
    {
        PrintOnePath();
        // 回溯
        pathCnt--;
        vis[x][y] = 0;
        return;
    }
    // 四个方向递归
    for (int d = 0;d < 4;d++)
    {
        int nx = x + dir[d][0];
        int ny = y + dir[d][1];
        DFS(nx, ny);
    }
    // 回溯撤销
    pathCnt--;
    vis[x][y] = 0;
}

// 只找第一条路径（提前退出）
int FindOnePath(int x, int y)
{
    if (x < 0 || x >= N || y < 0 || y >= M || maze[x][y] == 1 || vis[x][y] == 1)
        return 0;
    vis[x][y] = 1;
    path[pathCnt][0] = x;
    path[pathCnt][1] = y;
    pathCnt++;
    if (x == N - 1 && y == M - 1) return 1;

    for (int d = 0;d < 4;d++)
    {
        int nx = x + dir[d][0];
        int ny = y + dir[d][1];
        if (FindOnePath(nx, ny)) return 1;
    }
    pathCnt--;
    vis[x][y] = 0;
    return 0;
}

int main()
{
    printf("迷宫地图：\n");
    for (int i = 0;i < N;i++) {
        for (int j = 0;j < M;j++) printf("%d ", maze[i][j]);
        printf("\n");
    }
    // 1. 查找单条路径
    printf("\n===== 第一条可行路径 =====\n");
    pathCnt = 0;
    for (int i = 0;i < N;i++) for (int j = 0;j < M;j++) vis[i][j] = 0;
    if (FindOnePath(0, 0)) {
        for (int i = 0;i < pathCnt;i++)
            printf("(%d,%d) ", path[i][0], path[i][1]);
        printf("\n");
    }
    else printf("无通路\n");

    // 2. 查找全部路径
    printf("\n===== 所有可行路径 =====\n");
    pathCnt = allCount = 0;
    for (int i = 0;i < N;i++) for (int j = 0;j < M;j++) vis[i][j] = 0;
    DFS(0, 0);
    if (allCount == 0) printf("无任何通路\n");
    else printf("总路径数量：%d\n", allCount);
    return 0;
}