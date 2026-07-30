#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

//==================== 宏定义与类型定义 ====================
#define MAXSIZE 3       // 3*3九宫格
#define ZERO 0          // 数字0代表空格
#define TRUE 1
#define FALSE 0
typedef int STATUS;

// 九宫格状态：二维数组存储一盘棋
typedef int NODE[MAXSIZE][MAXSIZE];

// 邻接节点链表：保存Expand生成的所有后继棋盘状态
typedef struct NeighborNode
{
    NODE state;
    struct NeighborNode* next;
}NeighborNode, * NEIGHBOR;

// 路径节点：栈/链表保存整条搜索路径
typedef struct PathNode
{
    NODE state;
    struct PathNode* next;
}PathNode, * PATH, * STACK;

//==================== 第一组基础工具函数：棋盘判断、拷贝、打印 ====================

/**
 * @brief 判断两个九宫格状态是否完全相等
 * @param node1 棋盘1
 * @param node2 棋盘2
 * @return 相等返回TRUE，否则FALSE
 */
STATUS equal(NODE node1, NODE node2)
{
    int i, j;
    STATUS flag = TRUE;
    for (i = 0; i < MAXSIZE; i++)
    {
        for (j = 0; j < MAXSIZE; j++)
        {
            if (node1[i][j] != node2[i][j])
            {
                flag = FALSE;
                break;
            }
        }
        if (flag == FALSE)
            break;
    }
    return flag;
}

/**
 * @brief 棋盘赋值拷贝：把src完整复制到dst
 * @param dst 目标棋盘
 * @param src 源棋盘
 */
void setValue(NODE dst, NODE src)
{
    int i, j;
    for (i = 0; i < MAXSIZE; i++)
    {
        for (j = 0; j < MAXSIZE; j++)
        {
            dst[i][j] = src[i][j];
        }
    }
}

/**
 * @brief 打印一盘九宫格
 * @param node 要打印的棋盘状态
 */
void priNode(NODE node)
{
    int i, j;
    for (i = 0; i < MAXSIZE; i++)
    {
        for (j = 0; j < MAXSIZE; j++)
        {
            printf("%d ", node[i][j]);
        }
        printf("\n");
    }
    printf("----------------\n");
}

/**
 * @brief 查找空格(0)的坐标
 * @param node 当前棋盘
 * @param hor 输出：空格行号
 * @param ver 输出：空格列号
 * @return 找到空格返回TRUE
 */
STATUS Locate(NODE node, int* hor, int* ver)
{
    int i, j;
    STATUS flag = FALSE;
    for (i = 0; i < MAXSIZE; i++)
    {
        for (j = 0; j < MAXSIZE; j++)
        {
            if (node[i][j] == ZERO)
            {
                *hor = i;
                *ver = j;
                flag = TRUE;
                break;
            }
        }
        if (flag == TRUE)
            break;
    }
    return flag;
}

/**
 * @brief 交换棋盘两个位置，生成新棋盘
 * @param temp 新棋盘存放处
 * @param ori 原棋盘
 * @param h1,v1 第一个坐标
 * @param h2,v2 第二个坐标
 */
void Exchange(NODE temp, NODE ori, int h1, int v1, int h2, int v2)
{
    setValue(temp, ori);
    int t = temp[h1][v1];
    temp[h1][v1] = temp[h2][v2];
    temp[h2][v2] = t;
}

//==================== 第二组：邻接链表操作函数 ====================

/**
 * @brief 把新生成的棋盘状态加入邻接链表尾部
 * @param head 链表头
 * @param newState 新棋盘状态
 * @return 新链表头
 */
NEIGHBOR AddAnAdjacents(NEIGHBOR head, NODE newState)
{
    NEIGHBOR newNode = (NEIGHBOR)malloc(sizeof(NeighborNode));
    setValue(newNode->state, newState);
    newNode->next = NULL;

    if (head == NULL)
    {
        return newNode;
    }
    NEIGHBOR p = head;
    while (p->next != NULL)
        p = p->next;
    p->next = newNode;
    return head;
}

/**
 * @brief 释放邻接链表内存
 */
void FreeNeighbor(NEIGHBOR head)
{
    NEIGHBOR p;
    while (head != NULL)
    {
        p = head;
        head = head->next;
        free(p);
    }
}

//==================== 第三组：核心扩展函数 ExpandNodes（隐式图灵魂） ====================
/**
 * @brief 根据空格位置，动态生成所有合法后继棋盘（上下左右移动）
 * @param node 当前棋盘
 * @param zero 空格标记0
 * @return 存放所有新状态的邻接链表
 */
NEIGHBOR ExpandNodes(NODE node, int zero)
{
    NEIGHBOR adjacents = NULL;
    int h, v;
    NODE tempnode;

    // 找不到空格直接返回空链表
    if (!Locate(node, &h, &v))
        return adjacents;

    // 分位置判断边界，生成移动后的新状态（完全对照书本逻辑）
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
        // 中心位置：上下左右都可以走
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

//==================== 第四组：路径栈操作（判重、入栈、反转、打印） ====================

/**
 * @brief 判断当前棋盘是否已经在路径中（去回路，图搜索核心）
 */
STATUS IsInPath(PATH path, NODE cur)
{
    PathNode* p = path;
    while (p != NULL)
    {
        if (equal(p->state, cur))
            return TRUE;
        p = p->next;
    }
    return FALSE;
}

/**
 * @brief 路径头插法入栈
 */
void PushPath(PATH* path, NODE state)
{
    PathNode* newp = (PathNode*)malloc(sizeof(PathNode));
    setValue(newp->state, state);
    newp->next = *path;
    *path = newp;
}

/**
 * @brief 反转整条路径（DFS得到的是逆序路径）
 */
PATH RevPath(PATH old)
{
    PATH newPath = NULL;
    PathNode* p = old;
    while (p != NULL)
    {
        PushPath(&newPath, p->state);
        p = p->next;
    }
    return newPath;
}

/**
 * @brief 打印整条完整路径
 */
void priPath(PATH path)
{
    PathNode* p = path;
    int step = 0;
    while (p != NULL)
    {
        printf("第%d步：\n", step++);
        priNode(p->state);
        p = p->next;
    }
}

/**
 * @brief 清空释放路径内存
 */
void ClearPath(PATH path)
{
    PATH p;
    while (path != NULL)
    {
        p = path;
        path = path->next;
        free(p);
    }
}

/**
 * @brief 获取路径长度
 */
int LengthOfPath(PATH path)
{
    int len = 0;
    PathNode* p = path;
    while (p != NULL)
    {
        len++;
        p = p->next;
    }
    return len;
}

//==================== 第五组：DFS深度回溯搜索主函数 ====================
/**
 * @brief DFS递归搜索路径
 * @param cur 当前状态
 * @param end 目标状态
 * @param path 路径栈
 * @param zero 空格标记
 * @return 找到返回TRUE
 */
 /**
  * @brief DFS回溯搜索（修复入栈回溯逻辑）
  */
  // 增加最大递归深度，防止无限递归爆栈
#define MAX_DEPTH 30

//STATUS SearchPath(NODE cur, NODE end, PATH* path, int zero, int depth)
//{
//    // 超出最大搜索深度，直接剪枝退出
//    if (depth > MAX_DEPTH)
//        return FALSE;
//
//    // 到达终点
//    if (equal(cur, end))
//    {
//        PushPath(path, cur);
//        return TRUE;
//    }
//
//    // 路径判重，禁止环路
//    if (IsInPath(*path, cur))
//        return FALSE;
//
//    // 当前节点入栈
//    PushPath(path, cur);
//
//    NEIGHBOR neighbors = ExpandNodes(cur, zero);
//    NEIGHBOR p = neighbors;
//
//    while (p != NULL)
//    {
//        // 深度+1递归
//        if (SearchPath(p->state, end, path, zero, depth + 1))
//        {
//            FreeNeighbor(neighbors);
//            return TRUE;
//        }
//        p = p->next;
//    }
//
//    // 完整回溯：释放栈顶节点，修正指针
//    PathNode* top = *path;
//    *path = (*path)->next;
//    free(top);
//
//    FreeNeighbor(neighbors);
//    return FALSE;
//}

#define MAXDEPTH 30

// 内层递归（严格书本逻辑+深度限制+邻居判重）
STATUS SearchSubGraph(NODE current, NODE end, PATH* path, PATH* searchpath, int zero, int depth)
{
    // 注释掉这段深度限制 = 裸DFS（教材风格）
    /*
    if (depth > MAXDEPTH)
    {
        NEIGHBOR adj = ExpandNodes(current, zero);
        FreeNeighbor(adj);
        return FALSE;
    }
    */

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

    // 教材核心：遍历邻居时，先判断邻居不在路径再递归
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
// 外层入口（书本原版，删除手动追加start）
STATUS SearchPath(NODE start, NODE end, PATH* path, int zero) {
    STATUS flag;
    PATH searchpath = NULL;
    ClearPath(*path);
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
    ClearPath(searchpath);
    return flag;
}
// 计算九宫格逆序数（忽略0）
int GetInverse(NODE node)
{
    int arr[9], k = 0, cnt = 0;
    for (int i = 0; i < MAXSIZE; i++)
    {
        for (int j = 0; j < MAXSIZE; j++)
        {
            if (node[i][j] != ZERO)
            {
                arr[k++] = node[i][j]; // 存入后k自增
            }
        }
    }
    // 统计逆序对
    for (int i = 0; i < 8; i++)
    {
        for (int j = i + 1; j < 8; j++)
        {
            if (arr[i] > arr[j])
                cnt++;
        }
    }
    return cnt;
}


//==================== 主函数 程序入口 ====================
int main()
{
    PATH path = NULL;
    STATUS flag;
    
    // 初始棋盘（和书本例子一致）
    NODE start = {
        {1, 2, 3},
        {8, 4, 0},
        {7, 6, 5}
    };
    // 目标棋盘
    NODE end = {
        // 可解目标（逆序数偶数）

    {1, 2, 3},
    {8, 0, 4},
    {7, 6, 5}

    };


    int invS = GetInverse(start);
    int invE = GetInverse(end);
    printf("起点逆序数：%d，终点逆序数：%d\n", invS, invE);
    if ((invS % 2) != (invE % 2))
    {
        printf("逆序数奇偶不同，本题无解！\n");
        ClearPath(path);
        system("pause");
        return 0;
    }
    // 奇偶相同才进搜索
    //flag = SearchPath(start, end, &path, ZERO);


    printf("初始九宫格：\n");
    priNode(start);
    printf("目标九宫格：\n");
    priNode(end);

    flag = SearchPath(start, end, &path, ZERO);
    if (flag)
    {
        PATH rev = RevPath(path);
        printf("===== 找到完整路径 =====\n");
        priPath(rev);
        printf("总步数：%d\n", LengthOfPath(rev) - 1);
        ClearPath(rev);
    }
    else
    {
        printf("无解！\n");
    }

    ClearPath(path);
    system("pause");
    return 0;
}