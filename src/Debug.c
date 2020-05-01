#include "Debug.h"
#include "Real2.h"
#include "Typedefs.h"

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
char *Debug_GetComponentTypeString(int type)
{
	switch (type) {
		case COMP_SHIP:
			return "COMP_SHIP";
			break;
		case COMP_CAMERA:
			return "COMP_CAMERA";
			break;
		case COMP_COLLIDER:
			return "COMP_COLLIDER";
			break;
		case COMP_TRANSFORM:
			return "COMP_TRANSFORM";
			break;
		case COMP_PHYSICS:
			return "COMP_PHYSICS";
			break;
		case COMP_PROJECTILE:
			return "COMP_PROJECTILE";
			break;
		case COMP_TRAIL:
			return "COMP_TRAIL";
			break;
		default:
			return "UNKNOWN";
			break;
	}
}
