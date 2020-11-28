#ifndef INPUT_H
#define INPUT_H

#include "Graphics.h"

Real2 IN_GetMouseCameraPos(void);
Real2 IN_GetMouseLocalPos(Component *localSpaceTransform);
Real2 IN_GetMouseRootPos(void);
const Uint8 *IN_GetKeyStates(void);
void IN_UpdateInputHandlers(void);

#endif //INPUT_H
