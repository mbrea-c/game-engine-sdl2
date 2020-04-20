#include "Camera.h"
#include "Component.h"

int cameraDependencies[] = { COMP_TRANSFORM };

// Basic
Component *CA_Create(double width)
{
	unsigned int i;
	Component *camera;
	CameraData *cameraData;
	List *dependenciesList;

	dependenciesList = List_Nil();
	for (i = 0; i < sizeof(cameraDependencies)/sizeof(int); i++) {
		List_Append(dependenciesList, &cameraDependencies[i]);
	}
	cameraData = malloc(sizeof(CameraData));
	cameraData->width = width;
	
	camera = CM_CreateComponent(COMP_CAMERA, cameraData, &CA_Destructor, &CA_Mount, dependenciesList);
	return camera;
}

void CA_Mount(Component *camera) {}

void CA_Destructor(void *cameraData)
{
	free(cameraData);
}

// Getters
double CA_GetWidth(Component *camera)
{
	return ((CameraData *)camera->componentData)->width;
}

// Setters
void CA_SetWidth(Component *camera, double width)
{
	((CameraData *)camera->componentData)->width = width;
}
