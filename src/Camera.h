#ifndef CAMERA_H
#define CAMERA_H

#include "Typedefs.h"

// Basic
Component *CA_Create(double width);
void CA_Mount(Component *camera);
void CA_Destructor(void *cameraData);

// Getters
double CA_GetWidth(Component *camera);

// Setters
void CA_SetWidth(Component *camera, double width);

#endif //CAMERA_H
