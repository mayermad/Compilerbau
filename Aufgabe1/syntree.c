#include <stdio.h>
#include <stdlib.h>
#include "syntree.h"

int syntreeInit(syntree_t* self)
{
	if (self == 0)
	{
		return(-1);
	}
	self->nodeSize = 3;
	self->nodeArray = malloc(sizeof(syntree_node) * (self->nodeSize));
	self->nodeCurrent = 0;
	if (self->nodeArray == 0)
	{
		return(-1);
	}
	return 0;
}

void syntreeRescale(syntree_t* self)
{
	self->nodeSize *= 2;
	syntree_node * tmpPtr = (syntree_node *)realloc(self->nodeArray, sizeof(syntree_node) * (self->nodeSize));
	self->nodeArray = tmpPtr;
	if (self->nodeArray == 0)
	{
		fprintf(stderr, "not enough memory\n");
		exit(-1);
	}
}

void syntree_nodeChildrenRescale(syntree_node * self)
{
	self->childrenSize *= 2;
	self->childrenArray = (syntree_nid *)realloc(self->childrenArray, sizeof(syntree_nid) * self->childrenSize);
	if (self->childrenArray == 0)
	{
		fprintf(stderr, "not enough memory\n");
		exit(-1);
	}
}

void syntreeRelease(syntree_t* self)
{
	for (int i = 0; i < self->nodeCurrent; i++)
	{
		free(self->nodeArray[i].childrenArray);

	}
	free(self->nodeArray);
}

void syntree_nodeInit(syntree_node * newNode, int number)
{
	newNode->value = number;
	newNode->childrenSize = 1;
	newNode->childrenArray = malloc(sizeof(syntree_nid) * newNode->childrenSize);
	newNode->childrenCurrent = 0;
	newNode->hasValidFollower = 0;
}

syntree_nid syntreeNodeNumber(syntree_t* self, int number)
{
	int ind = self->nodeCurrent;
	self->nodeCurrent++;
	if (self->nodeCurrent == self->nodeSize)
	{
		syntreeRescale(self);
	}
	syntree_nodeInit(&self->nodeArray[ind], number);
	return ind;
}

syntree_nid syntreeNodeTag(syntree_t* self, syntree_nid id)
{
	return id;
}

syntree_nid syntreeNodePair(syntree_t* self, syntree_nid id1, syntree_nid id2)
{
	if (self->nodeArray[id1].childrenCurrent == self->nodeArray[id1].childrenSize) syntree_nodeChildrenRescale(&self->nodeArray[id1]);
	self->nodeArray[id1].childrenArray[self->nodeArray[id1].childrenCurrent] = id2;
	self->nodeArray[id1].childrenCurrent++;
	return id1;
}

syntree_nid syntreeNodeAppend(syntree_t* self, syntree_nid list, syntree_nid elem)
{
	self->nodeArray[list].hasValidFollower = 1;
	self->nodeArray[list].next = elem;
	return list;
}

syntree_nid syntreeNodePrepend(syntree_t* self, syntree_nid elem, syntree_nid list)
{
	self->nodeArray[elem].hasValidFollower = 1;
	self->nodeArray[elem].next = list;
	return elem;
}

void recursivePrint(const syntree_t * self, syntree_node * current, int depth)
{
	for (int i = 0; i < depth; i++)
	{
		printf("\t");
	}
	printf("(%d)\n", current->value);
	for (int i = 0; i < current->childrenCurrent; i++)
	{
		for (int i = 0; i < depth; i++)
		{
			printf("\t");
		}
		printf("{\n");
		recursivePrint(self, &self->nodeArray[current->childrenArray[i]], depth + 1);
		for (int i = 0; i < depth; i++)
		{
			printf("\t");
		}
		printf("}\n");
	}
	if (current->hasValidFollower != 0) recursivePrint(self, &self->nodeArray[current->next], depth);
}

void syntreePrint(const syntree_t* self, syntree_nid root)
{
	printf("{\n");
	recursivePrint(self, &self->nodeArray[root], 1);
	printf("}\n");
}