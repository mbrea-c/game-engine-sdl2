#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdio.h>

typedef struct List List;

struct List {
	void *element;
	List *next;
};

List *List_Create(void *firstElem);
void List_Append(List *list, void *element);
void *List_GetLast(List *list);
void *List_GetFromIndex(List *list, int index);
void *List_Head(List *list);
List *List_Tail(List *list);
int List_HasTail(List *list);
int List_GetLength(List *list);
void List_Remove(List *list, int index);
void List_AppendAtIndex(List *list, void *element, int index);

#endif //LIST_H
