#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NAMESIZE 20

typedef char nodeName[NAMESIZE];

//思考3：多叉树改二叉树
typedef struct TreeNode {
    char data[NAMESIZE];
    struct TreeNode* son;
    struct TreeNode* next;
}TreeNode;

typedef struct brotherNode {
    nodeName name;
    struct brotherNode* next;
}brotherNode, * brotherPtr;

typedef struct parentNode {
    nodeName name;
    brotherPtr children;
}parentNode, * parentPtr;

typedef struct treeNode {
    parentNode node;
    struct treeNode* next;
}treeNode, * treePtr;

brotherPtr addBrotherNode(brotherPtr brother, nodeName name) {
    brotherPtr newNode = (brotherPtr)malloc(sizeof(brotherNode));
    strcpy(newNode->name, name);
    newNode->next = NULL;
    if (!brother)
        return newNode;
    brotherPtr p = brother;
    while (p->next)
        p = p->next;
    p->next = newNode;
    return brother;
}

treePtr formParentChild(nodeName parentName, brotherPtr brotherList) {
    treePtr parentUnit = (treePtr)malloc(sizeof(treeNode));
    strcpy(parentUnit->node.name, parentName);
    parentUnit->node.children = brotherList;
    parentUnit->next = NULL;
    return parentUnit;
}

treePtr addSubTree(treePtr tree, treePtr subTree) {
    if (!tree)
        return subTree;
    treePtr p = tree;
    while (p->next)
        p = p->next;
    p->next = subTree;
    return tree;
}

void createStr(char* brotherSet) {
    if (brotherSet == NULL)
        return;
    char* c = brotherSet;
    while (*c != '\0') {
        if (*c == '/')
            *c = '\0';
        c++;
    }
}

brotherPtr createBrothers(brotherPtr brothers, char* brotherSet) {
    char* p = brotherSet;
    nodeName tempName;
    createStr(brotherSet);
    while (*p != '\0') {
        strcpy(tempName, p);
        int len = strlen(tempName);
        if (len > 0)
        {
            brothers = addBrotherNode(brothers, tempName);
        }
        p = p + len + 1;
    }
    return brothers;
}

treePtr createTreeFile(treePtr tree, char* filename) {
    treePtr subtree;
    brotherPtr brothers;
    FILE* fp;
    char parent[200], brotherSet[5000];
    fp = fopen(filename, "r");
    if (!fp) {
        perror("文件打开失败");
        return tree;
    }
    while (1)
    {
        // 读取父节点整行
        if (fgets(parent, sizeof(parent), fp) == NULL) break;
        parent[strcspn(parent, "\n")] = '\0';
        if (strlen(parent) == 0) continue;

        // 读取子节点整行
        if (fgets(brotherSet, sizeof(brotherSet), fp) == NULL) break;
        brotherSet[strcspn(brotherSet, "\n")] = '\0';

        brothers = NULL;
        brothers = createBrothers(brothers, brotherSet);
        subtree = formParentChild(parent, brothers);
        tree = addSubTree(tree, subtree);
    }
    fclose(fp);
    return tree;
}

treePtr createTreeInput(treePtr tree) {
    int n;
    scanf("%d", &n);
    treePtr subtree;
    brotherPtr brothers;
    nodeName parentName;
    char brotherSet[5000];
    for (int i = 0; i < n; i++) {
        scanf("%s", parentName);
        scanf("%s", brotherSet);
        brothers = NULL;
        brothers = createBrothers(brothers, brotherSet);
        subtree = formParentChild(parentName, brothers);
        tree = addSubTree(tree, subtree);
    }
    return tree;
}

brotherPtr cleanBrothers(brotherPtr brotherRoot) {
    brotherPtr p;
    while (brotherRoot) {
        p = brotherRoot;
        brotherRoot = brotherRoot->next;
        free(p);
    }
    return NULL;
}

treePtr fullCleanTree(treePtr tree) {
    treePtr p = tree, next;
    while (p)
    {
        next = p->next;
        cleanBrothers(p->node.children);
        free(p);
        p = next;
    }
    return NULL;
}

brotherPtr copyBrother(brotherPtr children) {
    brotherPtr copyNode, lastNode = NULL, head = NULL;
    while (children) {
        copyNode = (brotherPtr)malloc(sizeof(brotherNode));
        strcpy(copyNode->name, children->name);
        copyNode->next = NULL;
        if (!head)
            head = copyNode;
        else
            lastNode->next = copyNode;
        lastNode = copyNode;
        children = children->next;
    }
    return head;
}

brotherPtr expandNodes(treePtr tree, nodeName parentname) {
    brotherPtr children = NULL;
    treePtr p = tree;
    while (p) {
        if (strcmp(p->node.name, parentname) == 0) {
            children = copyBrother(p->node.children);
            break;
        }
        p = p->next;
    }
    return children;
}

brotherPtr pushChildren(brotherPtr stack, brotherPtr children) {
    brotherPtr head = copyBrother(children);
    if (!head) return stack;
    brotherPtr p = head;
    while (p->next)
        p = p->next;
    p->next = stack;
    return head;
}

brotherPtr popChild(brotherPtr stack, nodeName child) {
    if (!stack) return NULL;
    brotherPtr p = stack;
    strcpy(child, p->name);
    stack = p->next;
    free(p);
    return stack;
}

brotherPtr cleanStack(brotherPtr stack) {
    return cleanBrothers(stack);
}

int searchNode(treePtr tree, nodeName start, nodeName end) {
    nodeName tempNode;
    brotherPtr childList;
    brotherPtr stack = NULL;
    int flag = 0;
    if (!tree)
        return flag;
    stack = (brotherPtr)malloc(sizeof(brotherNode));
    strcpy(stack->name, start);
    stack->next = NULL;
    while (stack) {
        stack = popChild(stack, tempNode);
        if (strcmp(tempNode, end) == 0) {
            flag = 1;
            break;
        }
        childList = expandNodes(tree, tempNode);
        stack = pushChildren(stack, childList);
        cleanBrothers(childList);
    }
    cleanStack(stack);
    return flag;
}

typedef brotherPtr PATH;
typedef struct Path {
    PATH path;
    struct Path* next;
}Path, * PathPtr, * Stack;

PATH addNodePath(nodeName node, PATH path) {
    PATH p = (brotherPtr)malloc(sizeof(brotherNode));
    strcpy(p->name, node);
    p->next = path;
    return p;
}

PATH copyPath(PATH path) {
    return (PATH)copyBrother((brotherPtr)path);
}

PathPtr addPathPath(PATH path, PathPtr paths) {
    if (!path) return paths;
    PATH copypath = copyPath(path);
    PathPtr ps = (PathPtr)malloc(sizeof(Path));
    ps->path = copypath;
    ps->next = NULL;
    if (!paths)
        return ps;
    PathPtr p = paths;
    while (p->next)
        p = p->next;
    p->next = ps;
    return paths;
}

PathPtr formPath(brotherPtr brothers, PATH path, PathPtr paths) {
    while (brothers) {
        PATH tempath = copyPath(path);
        tempath = addNodePath(brothers->name, tempath);
        paths = addPathPath(tempath, paths);
        brothers = brothers->next;
    }
    return paths;
}

PathPtr copyPaths(PathPtr paths) {
    PathPtr head = NULL, copypath, lastpath = NULL;
    while (paths) {
        copypath = (PathPtr)malloc(sizeof(Path));
        copypath->path = copyPath(paths->path);
        copypath->next = NULL;
        if (!head)
            head = copypath;
        else
            lastpath->next = copypath;
        lastpath = copypath;
        paths = paths->next;
    }
    return head;
}

void revPath(PATH path) {
    int num = 0, i = 0;
    PATH p = path;
    while (p) {
        p = p->next;
        num++;
    }
    if (num == 0) return;
    nodeName* node = (nodeName*)malloc(num * sizeof(nodeName));
    for (i = 0, p = path; i < num; p = p->next, i++)
        strcpy(node[i], p->name);
    for (i = num - 1, p = path; p; p = p->next, i--)
        strcpy(p->name, node[i]);
    free(node);
}

PATH cleanPath(PATH path) {
    return (PATH)cleanBrothers((brotherPtr)path);
}

PathPtr cleanPaths(PathPtr paths) {
    PathPtr p = paths;
    while (paths) {
        p = paths;
        cleanBrothers(p->path);
        paths = paths->next;
        free(p);
    }
    return NULL;
}

Stack pushPath(Stack stack, PATH path) {
    PATH tempath = copyPath(path);
    Stack st = (PathPtr)malloc(sizeof(Path));
    st->path = tempath;
    st->next = stack;
    return st;
}

Stack pushPaths(Stack stack, PathPtr paths) {
    PathPtr head = copyPaths(paths);
    if (!head) return stack;
    PathPtr p = head;
    while (p->next)
        p = p->next;
    p->next = stack;
    return head;
}

Stack popPath(Stack stack, nodeName node, PATH* path) {
    if (!stack) return NULL;
    Stack p = stack;
    PATH tempath = p->path;
    strcpy(node, tempath->name);
    *path = copyPath(tempath);
    stack = p->next;
    free(p);
    return stack;
}

Stack clean(Stack stack) {
    return cleanPaths((PathPtr)stack);
}

int searchPath(treePtr tree, nodeName start, nodeName end, PATH* path) {
    nodeName node;
    brotherPtr children;
    Stack stack = NULL;
    int flag = 0;
    PATH tempath = NULL;
    PathPtr paths = NULL;
    if (!tree) return flag;
    tempath = addNodePath(start, tempath);
    stack = pushPath(stack, tempath);
    while (stack) {
        stack = popPath(stack, node, &tempath);
        if (strcmp(node, end) == 0) {
            flag = 1;
            *path = copyPath(tempath);
            break;
        }
        children = expandNodes(tree, node);
        paths = formPath(children, tempath, paths);
        stack = pushPaths(stack, paths);
        cleanPaths(paths);
        cleanBrothers(children);
        paths = NULL;
    }
    clean(stack);
    return flag;
}

//思考1：显示树的路径
void showPath(PATH path) {
    if (!path) {
        printf("无路径\n");
        return;
    }
    PATH copypath = copyPath(path);
    revPath(copypath);
    PATH p = copypath;
    printf("%s", p->name);
    p = p->next;
    while (p) {
        printf("->%s", p->name);
        p = p->next;
    }
    printf("\n");
    cleanPath(copypath);
}

// 思考2：显示树
void printWholeTree(treePtr tree)
{
    if (tree == NULL)
    {
        printf("当前树为空！\n");
        return;
    }
    printf("==================== 读取完成，树结构 ====================\n");
    treePtr cur = tree;
    while (cur != NULL)
    {
        printf("父节点：%s  子节点：", cur->node.name);
        brotherPtr child = cur->node.children;
        if (child == NULL)
        {
            printf("无");
        }
        else
        {
            while (child != NULL)
            {
                printf("%s", child->name);
                child = child->next;
                if (child != NULL)
                    printf("/");
            }
        }
        printf("\n");
        cur = cur->next;
    }
    printf("=========================================================\n\n");
}

int main() {
    nodeName start, end;
    treePtr tree = NULL;
    int nodeFlag, pathFlag;
    PATH path = NULL;
    char* filename = "D:\\test\\tree.txt";
    tree = createTreeFile(tree, filename);
    printWholeTree(tree);
    printf("The Start Node:\n");
    scanf("%s", start);
    while (getchar() != '\n');
    printf("The end Node:\n");
    scanf("%s", end);
    while (getchar() != '\n');

    nodeFlag = searchNode(tree, start, end);
    if (nodeFlag == 1) {
        pathFlag = searchPath(tree, start, end, &path);
        printf("Has Found %s\n", end);
        printf("Path = ");
        showPath(path);
    }
    else {
        printf("Can't find the %s node\n", end);
    }

    cleanPath(path);
    tree = fullCleanTree(tree);
    system("pause");
    return 0;
}