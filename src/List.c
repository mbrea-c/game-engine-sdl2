#include "List.h"

//TODO: Maybe error checking?
// Private procedure declaration
void List_CheckNotNull(List *list);

void List_CheckNotNull(List *list)
{
	if (list == NULL) {
		fprintf(stderr, "Expected List *, got NULL\n");
		exit(1);
	}
}

void List_Append(List *list, void *element)
{
	List_CheckNotNull(list);
	while (list->next != NULL) {
		list = list->next;
	}
	list->next = List_Create(element);
}

List *List_Create(void *firstElem)
{
	List *list = malloc(sizeof(List));
	list->element = firstElem;
	list->next = NULL;

	return list;
}

void *List_GetLast(List *list)
{
	List_CheckNotNull(list);
	while (list->next != NULL) {
		list = list->next;
	}
	return list->element;
}

void* List_GetFromIndex(List *list, int index)
{
	List_CheckNotNull(list);
	while (list->next != NULL && index > 0) {
		list = list->next;
	}
	if (index > 0) {
		fprintf(stderr, "List index out of bounds\n");
		exit(1);
	}
	return list->element;
}

void List_Remove(List *list, int index)
{
	List_CheckNotNull(list);
	while (list->next != NULL && index > 1) {
		list = list->next;
	}
	if (index > 1) {
		fprintf(stderr, "List index out of bounds\n");
		exit(1);
	}
	List *tmpPtr = list->next->next;
	free(list->next);
	list->next = tmpPtr;
}

void *List_Head(List *list)
{
	List_CheckNotNull(list);
	return list->element;
}

List *List_Tail(List *list)
{
	List_CheckNotNull(list);
	return list->next;
}

int List_HasTail(List *list)
{
	List_CheckNotNull(list);
	return list->next != NULL;
}

void List_AppendAtIndex(List *list, void *element, int index)
{
	List_CheckNotNull(list);
	while (list->next != NULL && index > 1) {
		list = list->next;
	}
	if (index > 1) {
		fprintf(stderr, "List index out of bounds\n");
		exit(1);
	}
	List *tmpPtr = list->next;
	list->next = List_Create(element);
	list->next->next = tmpPtr;
}

int List_GetLength(List *list)
{
	int length;

	List_CheckNotNull(list);
	length = 1;
	while (list->next != NULL) {
		list = list->next;
		length++;
	}
	return length;
}
