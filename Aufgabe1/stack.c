#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

int
stackInit(intstack_t* self) {
	if (self == NULL) {
		fprintf(stderr, "got null pointer instead of intstack_t\n");
		exit(-1);
	}
	self->top = NULL;
	return 0;
}

void
stackRelease(intstack_t* self) {
    if (self == NULL) {
        fprintf(stderr, "got null pointer instead of intstack_t\n");
        exit(-1);
    }
	while (stackIsEmpty(self) == 0) {
		stackPop(self);
	}
}

void
stackPush(intstack_t* self, int i) {
    if (self == NULL) {
        fprintf(stderr, "got null pointer instead of intstack_t\n");
        exit(-1);
    }
	Node* new;
	new = malloc(sizeof(Node));
	if (new == NULL) {
		fprintf(stderr, "not enough memory\n");
		exit(-1);
	}
	if (self != NULL) {
		new->next = self->top;
	}
	else {
		new->next = NULL;
	}
	new->value = i;
	self->top = new;
}

int
stackTop(const intstack_t* self) {
    if (self == NULL) {
        fprintf(stderr, "got null pointer instead of intstack_t\n");
        exit(-1);
    }
	return self->top->value;
}

int
stackPop(intstack_t* self) {
    if (self == NULL) {
        fprintf(stderr, "got null pointer instead of intstack_t\n");
        exit(-1);
    }
	if (stackIsEmpty(self) == 1) {
		fprintf(stderr, "intstack_t is empty\n");
		exit(-1);
	}
	int popValue = stackTop(self);
	Node* oldTop = self->top;
	self->top = self->top->next;
	free(oldTop);
	return popValue;
}

int
stackIsEmpty(const intstack_t* self) {
    if (self == NULL) {
        fprintf(stderr, "got null pointer instead of intstack_t\n");
        exit(-1);
    }
	if (self->top == NULL) {
		return 1;
	}
	else {
		return 0;
	}
}

void
stackPrint(const intstack_t* self) {
    if (self == NULL) {
        fprintf(stderr, "got null pointer instead of intstack_t\n");
        exit(-1);
    }
	Node* current = self->top;
	int index = 0;
	while (current != NULL) {
		printf("%d\n", current->value);
		current = current->next;
		index++;
	}
}