#include "List.h"

//TODO: Maybe error checking?
// Private procedure declaration
void List_CheckNotNull(List *list);
List *List_Create(void *elem);

void List_CheckNotNull(List *list)
{
	if (list == NULL) {
		fprintf(stderr, "Expected List *, got NULL\n");
		exit(1);
	}
}

List *List_Create(void *elem)
{
	List *list; 
	if ((list = malloc(sizeof(List))) == NULL) {
		fprintf(stderr, "Allocation error\n");
		exit(1);
	}
	list->element = elem;
	list->next = NULL;
	list->_isNil = 0;

	return list;
}

// List modifying
List *List_Nil()
{
	List *list = malloc(sizeof(List));
	list->element = NULL;
	list->next = NULL;
	list->_isNil = 1;
	return list;
}

List *List_Cons(List *list, void *element)
{
	List_CheckNotNull(list);
	List *newHead = List_Create(element);
	newHead->next = list;
	return newHead;
}

List *List_Append(List *list, void *element)
{
	List_CheckNotNull(list);
	List *newNode = List_Create(element);
	if (List_IsEmpty(list)) {
		list->_isNil = 0;
		list->next = List_Nil();
		List_Set(list, element, 0);
	} else {
		while (List_HasTail(list)) {
			list = List_Tail(list);
		}
		newNode->next = list->next;
		list->next = newNode;
	}
	return list;
}

List *List_Insert(List *list, void *element, int index)
{
	List_CheckNotNull(list);
	while (!List_IsEmpty(list->next) && index > 1) {
		list = list->next;
		index--;
	}
	if (index > 1) {
		fprintf(stderr, "List index out of bounds\n");
		exit(1);
	}
	List *tmpPtr = list->next;
	list->next = List_Create(element);
	list->next->next = tmpPtr;
	return list;	
}

List *List_Set(List *list, void *element, int index)
{
	List_CheckNotNull(list);
	if (List_IsEmpty(list)) {
		fprintf(stderr, "List index out of bounds (Empty list)\n");
		exit(1);
	} else if (index == 0) {
		list->element = element;
		return list;
	}
	// else
	while (!List_IsEmpty(list->next) && index > 0) {
		list = list->next;
		index--;
	}
	if (index > 0) {
		fprintf(stderr, "List index out of bounds\n");
		exit(1);
	}
	list->element = element;
	return list;	
}

List *List_Tail(List *list)
{
	List_CheckNotNull(list);
	if (!List_IsEmpty(list)) {
		return list->next;
	} else {
		fprintf(stderr, "Empty list has no tail\n");
		exit(1);
	}
}

List *List_SliceFrom(List *list, int index)
{
	List_CheckNotNull(list);
	if (List_IsEmpty(list)) {
		fprintf(stderr, "Empty list can't be sliced\n");
		exit(1);
	}
	while (!List_IsEmpty(list->next) && index > 0) {
		list = list->next;
		index--;
	}
	if (index > 0) {
		fprintf(stderr, "List index out of bounds\n");
		exit(1);
	}
	return list;
}

List *List_Delete(List *list, int index)
{
	List_CheckNotNull(list);
	if (List_IsEmpty(list)) {
		fprintf(stderr, "Can't delete element from empty list\n");
		exit(1);
	}
	if (index == 0) {
		list->element = list->next->element;
		list->_isNil = list->next->_isNil;
		List *tmpPtr = list->next;
		list->next = list->next->next;
		free(tmpPtr);
	} else {
		while (!List_IsEmpty(list->next) && index > 1) {
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
	return list;
}

// List accessing
void *List_GetLast(List *list)
{
	List_CheckNotNull(list);
	if (List_IsEmpty(list)) {
		fprintf(stderr, "Empty list has no last element\n");
		exit(1);
	}
	while (!List_IsEmpty(list->next)) {
		list = list->next;
	}
	return list->element;
}

void *List_Head(List *list)
{
	List_CheckNotNull(list);
	if (List_IsEmpty(list)) {
		fprintf(stderr, "Empty list has no head\n");
		exit(1);
	}
	return list->element;
}

void *List_GetFromIndex(List *list, int index)
{
	List_CheckNotNull(list);
	if (List_IsEmpty(list)) {
		fprintf(stderr, "Empty list can't be indexed\n");
		exit(1);
	}
	while (!List_IsEmpty(list->next) && index > 0) {
		list = list->next;
		index--;
	}
	if (index > 0) {
		fprintf(stderr, "List index out of bounds\n");
		exit(1);
	}
	return list->element;
}

int List_HasTail(List *list)
{
	List_CheckNotNull(list);
	return !List_IsEmpty(list) && !List_IsEmpty(list->next);
}

int List_IsEmpty(List *list)
{
	List_CheckNotNull(list);
	return list->_isNil;
}

int List_GetLength(List *list)
{
	int length;

	List_CheckNotNull(list);
	length = 0;
	while (!List_IsEmpty(list)) {
		list = list->next;
		length++;
	}
	return length;
}

// List destructor 
void List_Destructor(List *list)
{
	List *next;
	while (!List_IsEmpty(list)) {
		next = list->next;
		free(list);
		list = next;
	}
	free(list);
}

// Debug
void List_Print(List *list)
{
	printf("[ ");
	while (list != NULL) {
		if (List_IsEmpty(list)) {
			printf("Nil");
		} else {
			printf("(%p, %p), ", (void *)list->element, (void *)list->next);
		}
		list = list->next;
	}
	printf(" ]\n");
}
