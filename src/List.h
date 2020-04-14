#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdio.h>

typedef struct List List;

struct List {
	void *element;
	List *next;
	int _isNil;
};

// List modifying procedures modify the list in place and also return the
// pointer to the list, for convenience.

// List modifying
List *List_Nil();
List *List_Cons(List *list, void *element);
List *List_Append(List *list, void *element);
List *List_Insert(List *list, void *element, int index);
List *List_Set(List *list, void *element, int index);
List *List_Tail(List *list);
List *List_Delete(List *list, int index);

// List accessing
void *List_GetLast(List *list);
void *List_Head(List *list);
void *List_GetFromIndex(List *list, int index);
int List_HasTail(List *list);
int List_IsEmpty(List *list);
int List_GetLength(List *list);

// Debug
void List_Print(List *list);

#endif //LIST_H
