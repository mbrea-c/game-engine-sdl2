#ifndef ENGINE_H
#define ENGINE_H

#include "Typedefs.h"

#define FPS_CAP 60
#define TICKS_PER_FRAME 1000 / FPS_CAP

void EN_RunEngine(Object *sceneRoot);

#endif //ENGINE_H
