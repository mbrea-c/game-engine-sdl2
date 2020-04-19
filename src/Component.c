#include "Component.h"
#include "Debug.h"

Component *CM_CreateComponent(int type, void *componentData, void (*destructor)(void *), void (*mount)(Component *), List *dependencies)
{
	Component *component = malloc(sizeof(Component));
	component->type = type;
	component->componentData = componentData;
	component->dependencies = dependencies;
	component->destructor = destructor;
	component->mount = mount;
	component->owner = NULL;

	return component;
}
