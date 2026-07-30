#include"Queue.h"

Queue* InitQueue()
{
	Queue* q = (Queue*)malloc(sizeof(Queue));
	q->front = q->rear = NULL;
	return q;
}

int isEmpty(Queue* q) {
	return q->front == NULL;
}

int EnQueue(Queue* q, QueueType temp) {
	QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
	newNode->data = temp;
	newNode->next = NULL;
	if (isEmpty(q)) {
		q->front = newNode;
		q->rear = newNode;
	}
	else {
		q->rear->next = newNode;
		q->rear = newNode;
	}
	return 1;
}

int Dequeue(Queue* q, QueueType* temp) {
	if (isEmpty(q))
		return 0;
	QueueNode* delNode = q->front;
	*temp = delNode->data;
	q->front = q->front->next;
	free(delNode);
	if (q->front == NULL)
		q->rear == NULL;
	return 1;
}

