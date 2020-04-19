#include "Debug.h"
#include "Real2.h"

void Debug_PrintListIntPtrs(List *list)
{
	printf("[ ");
	while (list != NULL) {
		if (List_IsEmpty(list)) {
			printf("Nil");
		} else {
			printf("%d, ", *(int *)list->element);
		}
		list = list->next;
	}
	printf(" ]\n");
}
void Debug_PrintPolygon(Polygon *polygon)
{
	printf("[ ");
	List *list;
	list = polygon->vertices;
	while (list != NULL) {
		if (List_IsEmpty(list)) {
			printf("Nil");
		} else {
			printf("(%f,%f), ", (*(Real2 *)list->element).x, (*(Real2 *)list->element).y);
		}
		list = list->next;
	}
	printf(" ]\n");
}
