#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAXSIZE 3
#define ZERO 0
#define TRUE 1
#define FALSE 0
#define MAXDEPTH 30
typedef int STATUS;

//启发式节点结构体（书上标准）
typedef int NODETYPE[MAXSIZE][MAXSIZE];
typedef struct {
    NODETYPE node;     //九宫格棋盘
    float evaluate;    //h：错位格子数（未来开销）
    double step;       //g：已走步数（历史开销）
} NODE;

//邻接子节点链表
typedef struct NeighborNode {
    NODE state;
    struct NeighborNode* next;
} NeighborNode, * NEIGHBOR;

//路径/栈链表（STACK就是PATH别名，和书一致）
typedef struct PathNode {
    NODE node;
    struct PathNode* next;
} PathNode, * PATH, * STACK;

// 仅比较棋盘数字，忽略g/h
STATUS equal(NODE node1, NODE node2) {
    int i, j;
    STATUS flag = TRUE;
    for (i = 0; i < MAXSIZE; i++) {
        for (j = 0; j < MAXSIZE; j++)
            if (node1.node[i][j] != node2.node[i][j]) {
                flag = FALSE;
                break;
            }
        if (flag == FALSE) break;
    }
    return flag;
}

//完整拷贝结构体：棋盘+step+evaluate（书上setvalue）
void setValue(NODE* node1, NODE* node2) {
    int i, j;
    for (i = 0; i < MAXSIZE; i++)
        for (j = 0; j < MAXSIZE; j++)
            node1->node[i][j] = node2->node[i][j];
    node1->step = node2->step;
    node1->evaluate = node2->evaluate;
}

//打印节点，输出g/h/f
void PriNode(NODE node) {
    int i, j;
    for (i = 0; i < MAXSIZE; i++) {
        for (j = 0; j < MAXSIZE; j++) printf("%5d", node.node[i][j]);
        printf("\n");
    }
    printf("g=%.0lf, h=%.0f, f=%.0f\n********************\n", node.step, node.evaluate, node.step + node.evaluate);
}

//查找空白格0坐标
STATUS Locate(NODE node, int* hori, int* vert) {
    int i, j;
    STATUS flag = FALSE;
    for (i = 0; i < MAXSIZE; i++) {
        for (j = 0; j < MAXSIZE; j++)
            if (node.node[i][j] == ZERO) {
                *hori = i;
                *vert = j;
                flag = TRUE;
                break;
            }
        if (flag == TRUE) break;
    }
    return flag;
}

//交换空白格与相邻数字
void Exchange(NODE* temp, NODE* ori, int hori1, int vert1, int hori2, int vert2) {
    setValue(temp, ori);
    int t = temp->node[hori1][vert1];
    temp->node[hori1][vert1] = temp->node[hori2][vert2];
    temp->node[hori2][vert2] = t;
}

//新增邻接节点
NEIGHBOR AddAnAdjacents(NEIGHBOR head, NODE newState) {
    NEIGHBOR newNode = (NEIGHBOR)malloc(sizeof(NeighborNode));
    setValue(&newNode->state, &newState);
    newNode->next = NULL;
    if (!head) return newNode;
    NEIGHBOR adjacents = head;
    while (adjacents->next) adjacents = adjacents->next;
    adjacents->next = newNode;
    return head;
}

//释放邻接链表
void FreeNeighbor(NEIGHBOR head) {
    NEIGHBOR adjacents;
    while (head) {
        adjacents = head;
        head = head->next;
        free(adjacents);
    }
}

//扩展空白格所有移动子节点
NEIGHBOR ExpandNodes(NODE* node, int zero)
{
    NEIGHBOR adjacents = NULL;
    int h, v;
    NODE tempnode;

    if (!Locate(*node, &h, &v))
        return adjacents;

    if (h == 0 && v == 0)
    {
        Exchange(&tempnode, node, h, v, h, v + 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h + 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 0 && v == 1)
    {
        Exchange(&tempnode, node, h, v, h, v - 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h, v + 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h + 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 0 && v == 2)
    {
        Exchange(&tempnode, node, h, v, h, v - 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h + 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 1 && v == 0)
    {
        Exchange(&tempnode, node, h, v, h - 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h + 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h, v + 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 1 && v == 1)
    {
        Exchange(&tempnode, node, h, v, h - 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h + 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h, v - 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h, v + 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 1 && v == 2)
    {
        Exchange(&tempnode, node, h, v, h - 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h + 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h, v - 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 2 && v == 0)
    {
        Exchange(&tempnode, node, h, v, h - 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h, v + 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 2 && v == 1)
    {
        Exchange(&tempnode, node, h, v, h - 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h, v - 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h, v + 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    else if (h == 2 && v == 2)
    {
        Exchange(&tempnode, node, h, v, h - 1, v);
        adjacents = AddAnAdjacents(adjacents, tempnode);
        Exchange(&tempnode, node, h, v, h, v - 1);
        adjacents = AddAnAdjacents(adjacents, tempnode);
    }
    return adjacents;
}

//判断节点是否在当前路径（去重防回头）
STATUS IsInPath(PATH path, NODE node) {
    PATH adj = path;
    while (adj) {
        if (equal(adj->node, node))
            return TRUE;
        adj = adj->next;
    }
    return FALSE;
}

//路径头插
void PushPath(PATH* path, NODE state) {
    PATH newPath = (PATH)malloc(sizeof(PathNode));
    setValue(&newPath->node, &state);
    newPath->next = *path;
    *path = newPath;
}

//复制整条路径（修复：取出最优节点时不销毁原路径）
PATH CopyPath(PATH src)
{
    PATH dst = NULL;
    PATH p = src;
    while (p)
    {
        PushPath(&dst, p->node);
        p = p->next;
    }
    return dst;
}

//反转路径
PATH RevPath(PATH path) {
    PATH newPath = NULL;
    PATH adj = path;
    while (adj) {
        PushPath(&newPath, adj->node);
        adj = adj->next;
    }
    return newPath;
}

//打印路径
void PriPath(PATH path) {
    PATH adj = path;
    int step = 0;
    while (adj) {
        printf("=====第 %d 步=====\n", step++);
        PriNode(adj->node);
        adj = adj->next;
    }
}

//路径长度
int LengthPath(PATH path) {
    PATH adj = path;
    int len = 0;
    while (adj) {
        len++;
        adj = adj->next;
    }
    return len;
}

//清空路径链表
void CleanPath(PATH path) {
    PATH adj;
    while (path) {
        adj = path;
        path = path->next;
        free(adj);
    }
}

//逆序数（判断是否可解）
int GetInversion(NODE node)
{
    int arr[9], cnt = 0, inv = 0;
    for (int i = 0;i < 3;i++)
        for (int j = 0;j < 3;j++)
            if (node.node[i][j] != 0) arr[cnt++] = node.node[i][j];
    for (int i = 0;i < 8;i++)
        for (int j = i + 1;j < 8;j++)
            if (arr[i] > arr[j]) inv++;
    return inv;
}

//===================== 书上启发式核心函数 =====================
//1. 修复估价函数h：忽略空白格0，只统计数字错位
float Evaluate(NODE* n1, NODE* n2) {
    int i, j;
    float res = 0;
    for (i = 0; i < MAXSIZE; i++)
        for (j = 0; j < MAXSIZE; j++)
        {
            if (n1->node[i][j] == 0 || n2->node[i][j] == 0)
                continue;
            if (n1->node[i][j] != n2->node[i][j])
                res++;
        }
    return res;
}

//2. 批量给邻接子节点赋值g、h
void EvaluateAdjacents(NEIGHBOR adjacents, NODE* curNode, NODE* endNode) {
    NEIGHBOR br = adjacents;
    while (br) {
        br->state.step = curNode->step + 1;
        br->state.evaluate = Evaluate(&br->state, endNode);
        br = br->next;
    }
}

//3. 选出f=g+h最小节点并弹出（书上PopANode_Heuristic_by_Mini，修复路径销毁BUG）
STACK PopANode_Heuristic_by_Mini(STACK stack, NODE* outNode, PATH* outPath) {
    STACK p = stack, p1 = NULL, p2 = NULL, p3 = NULL;
    PATH temppath;
    double ev = 99999.0;
    if (p == NULL) return stack;
    *outPath = NULL;

    while (p != NULL) {
        temppath = p;
        double f = temppath->node.step + temppath->node.evaluate;
        if (f < ev) {
            ev = f;
            p2 = p1;
            p3 = p;
        }
        p1 = p;
        p = p->next;
    }
    //取出最优节点：先复制路径，再删除原节点，不销毁有效路径
    temppath = p3;
    setValue(outNode, &temppath->node);
    *outPath = CopyPath(temppath); //复制一份路径，原链表节点可以安全删除

    //从链表删除p3
    if (p3 == stack)
        stack = p3->next;
    else
        p2->next = p3->next;
    free(p3);
    return stack;
}

//===================== A*主搜索函数 =====================
STATUS SearchPath(NODE* start, NODE* end, PATH* path, int zero) {
    NODE curNode;
    NEIGHBOR adjacents;
    STACK stack = NULL;
    STATUS flag = FALSE;
    PATH temppath = NULL;

    //无解判定
    int invS = GetInversion(*start);
    int invE = GetInversion(*end);
    if ((invS % 2) != (invE % 2)) {
        printf("判定：起始与目标逆序数奇偶不同，无解\n");
        return FALSE;
    }

    // 初始化起点g、h（关键赋值）
    start->step = 0.0;
    start->evaluate = Evaluate(start, end);
    PushPath(&stack, *start);

    // 打印正确初始化后的初始状态
    printf("初始状态（已计算g/h）：\n");
    PriNode(*start);

    // 目标状态h单独计算
    end->step = 0.0;
    end->evaluate = Evaluate(end, end);
    printf("目标状态（已计算g/h）：\n");
    PriNode(*end);

    while (stack != NULL) {
        temppath = NULL;
        stack = PopANode_Heuristic_by_Mini(stack, &curNode, &temppath);

        //到达目标
        if (equal(curNode, *end) == TRUE) {
            flag = TRUE;
            CleanPath(*path);
            *path = temppath;
            break;
        }

        adjacents = ExpandNodes(&curNode, zero);
        EvaluateAdjacents(adjacents, &curNode, end);

        NEIGHBOR br = adjacents;
        while (br != NULL) {
            if (!IsInPath(temppath, br->state)) {
                //复制历史路径
                PATH newPath = CopyPath(temppath);
                PushPath(&newPath, br->state);
                newPath->next = stack;
                stack = newPath;
            }
            br = br->next;
        }
        FreeNeighbor(adjacents);
        CleanPath(temppath);
    }
    CleanPath(stack);
    return flag;
}

//===================== 主函数 =====================
int main() {
    PATH path = NULL;
    STATUS flag;
    NODE start, end;

    // 1. 先清零两个结构体的浮点成员，消除随机脏值
    memset(&start, 0, sizeof(NODE));
    memset(&end, 0, sizeof(NODE));

    // 2. 写入棋盘数据
    NODETYPE sMap = {
        {1, 2, 3},
        {8, 4, 0},
        {7, 6, 5}
    };
    NODETYPE eMap = {
        {1, 2, 3},
        {8, 0, 4},
        {7, 6, 5}
    };
    memcpy(start.node, sMap, sizeof(NODETYPE));
    memcpy(end.node, eMap, sizeof(NODETYPE));

    flag = SearchPath(&start, &end, &path, ZERO);

    if (flag) {
        PATH rev = RevPath(path);
        printf("===============求解路径===============\n");
        PriPath(rev);
        printf("总移动步数：%d\n", LengthPath(rev) - 1);
        CleanPath(rev);
    }
    else printf("无法找到路径！");

    CleanPath(path);
    system("pause");
    return 0;
}