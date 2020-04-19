#ifndef COMPONENT_H
#define COMPONENT_H

#include "Typedefs.h"

Component *CM_CreateComponent(int type, void *componentData, void (*destructor)(void *), void (*mount)(Component *), List *dependencies);

#endif //COMPONENT_H
