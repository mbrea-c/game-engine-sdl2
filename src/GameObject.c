#include "GameObject.h"
#include "Transform.h"
#include "Debug.h"

// Declare local procedures
void _GO_LogObjectTreeRec(Object *root, FILE *outfile, int depth);

void GO_CheckObjNotNull(Object *object)
{
	if (object == NULL) {
		fprintf(stderr, "Was given NULL object, did not expect that. Dying now.\n");
		exit(1);
	}
}

// General object procedures
Object *GO_CreateObject(char *name, double x, double y, double rot, Object *parent)
{
	static int id = 0;
	Object *newObject = malloc(sizeof(Object));
	newObject->name = name;
	newObject->id = id++;
	newObject->parent = parent;
	newObject->children = List_Nil();
	newObject->components = List_Nil();
	if (parent != NULL) {
		GO_AddChild(parent, newObject);
	}
	GO_AddComponent(newObject, TR_CreateTransform((Real2) {x,y}, rot));

	return newObject;
}

void GO_AddChild(Object *parent, Object *child)
{
	GO_CheckObjNotNull(parent);
	GO_CheckObjNotNull(child);
	List_Append(parent->children, child);
}

Object *GO_GetParent(Object *object)
{
	GO_CheckObjNotNull(object);
	return object->parent;
}

Object *GO_GetChild(Object *object, char *name)
{
	List *children;

	GO_CheckObjNotNull(object);
	children = object->children;

	// Search children by name
	while (!List_HasTail(children)) {
		if (strcmp(((Object *) List_Head(children))->name, name))
			return (Object *) List_Head(children);
		children = List_Tail(children);
	}
	// No entry found by name
	return NULL;
}

void GO_LogObjectTree(Object *root, FILE *outfile)
{
	_GO_LogObjectTreeRec(root, outfile, 0);
}

void _GO_LogObjectTreeRec(Object *root, FILE *outfile, int depth)
{
	int i;
	List *children;

	GO_CheckObjNotNull(root);
	children = root->children;

	// Indent according to depth in tree
	for (i = 0; i < depth; i++) {
		fprintf(outfile, "  ");
	}
	fprintf(outfile, "|-%s\n", root->name);

	// Recursively call on all children
	while (!List_IsEmpty(children)) {
		_GO_LogObjectTreeRec(List_Head(children), outfile, depth + 1);
		children = List_Tail(children);

	}
}

Object *GO_GetComponentOwner(Component *component)
{
	return component->owner;
}

Component *GO_GetComponent(Object *obj, int componentType)
{
	Component *targetCompnent = NULL;
	Component *currentComponent = NULL;
	List *clist = obj->components;
	for (; !List_IsEmpty(clist); clist = List_Tail(clist)) {
		currentComponent = List_Head(clist);
		if (componentType == currentComponent->type) {
			targetCompnent = currentComponent;
		}
	}
	return targetCompnent;
}

Component *GO_GetComponentFromOwner(Component *comp, int componentType)
{
	GO_GetComponent(GO_GetComponentOwner(comp), componentType);
}

int GO_HasComponent(Object *obj, int componentType)
{
	List *clist = obj->components;
	for (; !List_IsEmpty(clist); clist = List_Tail(clist)) {
		if (componentType == ((Component *)List_Head(clist))->type) {
			return 1;
		}
	}
	return 0;
}

void GO_AddComponent(Object *obj, Component *component)
{
	int matchedRequirements = 1;
	List *dependencies = component->dependencies;

	// Component is already present?
	if (GO_HasComponent(obj, component->type)) {
		matchedRequirements = 0;
	} else { // Are dependencies satisfied?
		LOOP_OVER(dependencies) {
			if (!GO_HasComponent(obj, *(int *)List_Head(dependencies))) {
				matchedRequirements = 0;
				break;
			}
		}
	}

	// Mount component or fail and die
	if (matchedRequirements) {
		component->owner = obj;
		List_Append(obj->components, component);
		(component->mount)(component);
	} else {
		fprintf(stderr, "Error when adding component of type %d to %s\n", component->type, obj->name);
		exit(1);
	}
}

