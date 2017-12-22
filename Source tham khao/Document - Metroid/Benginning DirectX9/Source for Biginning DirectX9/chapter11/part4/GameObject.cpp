#include ".\gameobject.h"
#include ".\Model.h"

CGameObject::CGameObject(void)
{
	// Set the model pointer to NULL
	Model = NULL;

	// Set the default position to the origin
	position.x = 0.0f;
	position.y = 0.0f;
	position.z = 0.0f;
}

CGameObject::~CGameObject(void)
{
	// Delete the Model pointer when the object is destroyed
	if (Model)
		delete Model;
}
