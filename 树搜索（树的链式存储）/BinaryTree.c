#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PATH_MAX 256
//思考3 //树节点
#define MAX_DATA 20
typedef struct TreeNode { 
	char data[MAX_DATA];
	struct TreeNode* son;
	struct TreeNode* next;
}TreeNode;

//思考题2 显示树
void ShowTree(TreeNode* T, int depth) {
	if (!T) 
		return;
	for (int i = 0;i < depth;i++)
		printf("  ");
	printf("%s\n", T->data);
	ShowTree(T->son, depth + 1);
	ShowTree(T->next, depth);
}

//思考题1 查找到根目标节点路径
int FindPath(TreeNode* T, char path[][MAX_DATA], int *len, char target[]) {
	if (!T)
		return 0;
	strcpy(path[*len], T->data);
	(*len)++;
	if (strcmp(T->data, target) == 0)
		return 1;
	if (FindPath(T->son, path, len, target) || FindPath(T->next, path, len, target))
		return 1;
	(*len)--;
	return 0;
}

void PrintPath(TreeNode* root, char target[]) {
	char path[100][MAX_DATA];
	int len = 0;
	if (FindPath(root, path, &len, target)) {
		printf("根节点到%s的完整路线是：\n", target);
		for (int i = 0;i < len;i++) {
			printf("%s", path[i]);
			if (i < len - 1)
				printf("->");
		}
		printf("\n");
	}
	else
		printf("不存在该节点\n");
}

//遍历树写入文件
void SaveFile(TreeNode* T, FILE* fp) {
	if (!T)
		return;
	int hasson = T->son != NULL ? 1 : 0;
	int hasnext = T->next != NULL ? 1 : 0;
	fprintf(fp, "%s %d %d\n", T->data, hasson, hasnext); //标记是否有孩子、兄弟
	SaveFile(T->son, fp);
	SaveFile(T->next, fp);
}

//将树写入文件
void SaveTree(TreeNode* root, char filePath[]) {
	FILE* fp = fopen(filePath, "w");
	SaveFile(root, fp);
	fclose(fp);
	printf("树已保存至文件%s", filePath);
}

//从文件读取节点
TreeNode* LoadFile(FILE* fp) {
	char data[MAX_DATA];
	int hasson ,hasnext;
	if (fscanf(fp, "%s %d %d", data, &hasson, &hasnext) != 3)
		return NULL;
	TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
	strcpy(newNode->data, data);
	newNode->next = NULL;
	newNode->son = NULL;
	if (hasson) 
		newNode->son = LoadFile(fp);
	if (hasnext)
		newNode->next = LoadFile(fp);
	return newNode;
}

//从文件读取树
TreeNode* LoadTree(char filePath[]) {
	FILE* fp = fopen(filePath, "r");
	if (!fp) {
		printf("文件打开失败，无法加载\n");
		return NULL;
	}
	TreeNode* root = LoadFile(fp);
	fclose(fp);
	printf("已从文件中读取树");
	return root;
}

//创建树
TreeNode* CreateNode(char data[]) {
	TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
	strcpy(newNode->data, data);
	newNode->next = NULL;
	newNode->son = NULL;
	return newNode;
}

void AddSon(TreeNode* parent, char childData[]) {
	if (!(parent->son))
		parent->son = CreateNode(childData);
	else {
		printf("已经有一个孩子，无法增加长子");
		return;
	}
}

void AddBrother(TreeNode* node, char brotherData[]) {
	TreeNode *p = node;
	while (p->next) 
		p = p->next;
	p->next = CreateNode(brotherData);
}

//清空树
void DestroyTree(TreeNode* T) {
	if (!T) return;
	DestroyTree(T->son);
	DestroyTree(T->next);
	free(T);
}

//搜索节点
TreeNode* FindNode(TreeNode* T, char target[]) {
	if (!T) 
		return NULL;
	if (!(strcmp(T->data, target)))
		return T;
	TreeNode* res = FindNode(T->son, target);
	if (res)
		return res;
	return FindNode(T->next, target);
}

int main() {
	char buf[MAX_DATA];
	TreeNode* root = NULL;
	char file[PATH_MAX];
	int op;
	while (1) {
		printf("\n========主菜单========\n");
		printf("1. 创建树\n");
		printf("2. 显示树形\n");
		printf("3. 查询根到节点路径\n");
		printf("4. 将树保存到文件\n");
		printf("5. 将树从文件中加载\n");
		printf("0. 退出程序\n");
		printf("请选择功能：\n");
		scanf("%d", &op);

		switch (op) {
		case 1: {
			int sop;
			while (1) {
				printf("\n========建树菜单========\n");
				printf("1. 创建根节点\n");
				printf("2. 指定节点添加孩子\n");
				printf("3. 指定节点添加右兄弟\n");
				printf("0. 返回主菜单\n");
				printf("请选择功能：\n");
				scanf("%d", &sop);
				if (sop==0)
					break;

				switch (sop) {
				case 1: {
					if (root) {
						printf("根节点已存在\n");
						break;
					}
					printf("请输入根节点数据：\n");
					scanf("%s", buf);
					root = CreateNode(buf);
					printf("根节点%s成功创建：\n", buf);
					break;
				}

				case 2: {
					if (!root) {
						printf("无树，请先创建根节点\n");
						break;
					}
					printf("请输入双亲名称：\n");
					scanf("%s", buf);
					TreeNode* parents = FindNode(root, buf);
					if (!parents) {
						printf("未找到该节点\n");
						break;
					}
					printf("请输入长子名称：\n");
					scanf("%s", buf);
					AddSon(parents, buf);
					printf("长子%s已添加完成\n", buf);
					break;
				}

				case 3: {
					if (!root) {
						printf("无树，请先创建根节点\n");
						break;
					}
					printf("请输入已有节点名称：\n");
					scanf("%s", buf);
					TreeNode* brother = FindNode(root, buf);
					if (!brother) {
						printf("未找到该节点\n");
						break;
					}
					printf("请输入新增右兄弟名称：\n");
					scanf("%s", buf);
					AddBrother(brother, buf);
					printf("右兄弟%s已添加完成\n", buf);
					break;
				}

				default: {
					printf("选项不存在，请重新输入！\n");
					break;
				}
				}
			}
			break;
		}

		case 2:
			if (!root) {
				printf("当前无树，请先构建树\n");
				break;
			}
			printf("\n=======展示树形=======\n");
			ShowTree(root, 0);
			break;

		case 3:
			if (!root) {
				printf("当前无树，请先构建树\n");
				break;
			}
			printf("输入要查询路径的节点：");
			scanf("%s", buf);
			PrintPath(root, buf);
			break;

		case 4:
			if (!root) {
				printf("当前无树，请先构建树\n");
				break;
			}
			printf("请输入你的保存路径：\n");
			scanf("%s", file);
			SaveTree(root, file);
			break;

		case 5:
			printf("请输入你的下载路径：\n");
			scanf("%s", file);
			DestroyTree(root);
			root = LoadTree(file);
			break;

		case 0:
			DestroyTree(root);
			printf("程序结束\n");
			return 0;

		default:
			printf("选项不存在，请重新输入！\n");
		}
	}
}