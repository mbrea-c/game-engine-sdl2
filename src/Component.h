#ifndef COMPONENT_H
#define COMPONENT_H

#include "Typedefs.h"

Component *CM_CreateComponent(
	int type, 
	void *componentData, 
	void (*destructor)(void *), 
	void (*mount)(Component *), 
	void (*update)(Component *), 
	List *dependencies
	);

void CM_Update(Component *component);

#endif //COMPONENT_H
