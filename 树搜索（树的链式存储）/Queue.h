#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

struct TreeNode;

typedef struct QueueType {
	TreeNode* ptr;
}QueueType;

typedef struct QueueNode {  //队列结构
	QueueType data;
	struct QueueNode* next;
}QueueNode;

typedef struct {
	QueueNode* front;
	QueueNode* rear;
}Queue;

Queue* InitQueue();
int isEmpty(Queue* q);
int EnQueue(Queue* q, QueueType temp);
int Dequeue(Queue* q, QueueType* temp);
