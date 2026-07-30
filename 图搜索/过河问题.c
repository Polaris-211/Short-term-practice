#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define MAXSTEP 50

// 状态结构体
typedef struct {
    int m, c, boat;
}State;
State path[MAXSTEP];
int step = 0;
int vis[4][4][2] = { 0 }; // 记录访问过的状态去重

// 判断当前状态是否合法安全
int IsSafe(int m, int c)
{
    // 人数不能负数、不能超过3
    if (m < 0 || c < 0 || m>3 || c>3) return 0;
    // 左岸M>0 且 M<C 危险
    if (m > 0 && m < c) return 0;
    // 右岸M=3-m，C=3-c；右岸M>0且M<C危险
    int rm = 3 - m, rc = 3 - c;
    if (rm > 0 && rm < rc) return 0;
    return 1;
}

// dfs搜索
int DFS(int m, int c, int boat)
{
    // 记录路径
    path[step].m = m;
    path[step].c = c;
    path[step].boat = boat;
    step++;
    vis[m][c][boat] = 1;

    // 全部到右岸，找到解
    if (m == 0 && c == 0 && boat == 0)
        return 1;

    // 船在左岸：载人去右岸，枚举船上(moveM, moveC)
    if (boat == 1)
    {
        for (int mm = 0;mm <= 2;mm++)
            for (int cc = 0;cc <= 2;cc++)
            {
                if (mm + cc < 1 || mm + cc > 2) continue; // 船1~2人
                int nm = m - mm;
                int nc = c - cc;
                if (IsSafe(nm, nc) && !vis[nm][nc][0])
                {
                    if (DFS(nm, nc, 0)) return 1;
                }
            }
    }
    // 船在右岸：载人回左岸
    else
    {
        for (int mm = 0;mm <= 2;mm++)
            for (int cc = 0;cc <= 2;cc++)
            {
                if (mm + cc < 1 || mm + cc>2) continue;
                int nm = m + mm;
                int nc = c + cc;
                if (IsSafe(nm, nc) && !vis[nm][nc][1])
                {
                    if (DFS(nm, nc, 1)) return 1;
                }
            }
    }
    // 回溯
    step--;
    vis[m][c][boat] = 0;
    return 0;
}

void PrintAns()
{
    printf("传教士野人过河步骤（左M,左C,船位置 1左/0右）\n");
    for (int i = 0;i < step;i++)
    {
        printf("第%d步：M=%d C=%d 船在%s\n",
            i, path[i].m, path[i].c, path[i].boat ? "左岸" : "右岸");
    }
}

int main()
{
    if (DFS(3, 3, 1))
        PrintAns();
    else
        printf("无安全过河方案\n");
    return 0;
}