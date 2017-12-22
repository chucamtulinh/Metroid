#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9tex.h>

#include ".\Model.h"
#include ".\planet.h"

CPlanet::CPlanet(void)
{
}

CPlanet::~CPlanet(void)
{
}

bool CPlanet::create(LPDIRECT3DDEVICE9 device)
{
	position.x = 0.0f;
	position.y = 0.0f;
	position.z = 0.0f;

	// Create the model for this planet
	Model = new CModel();

	return Model->loadModel(device, "sphere.x");
}

void CPlanet::render(LPDIRECT3DDEVICE9 device)
{
	D3DXMATRIX transMatrix;				// the translation matrix
	D3DXMATRIX scaleMatrix;				// the scale matrix
	D3DXMATRIX rotateMatrix;			// The rotation matrix
	
	// Setup the translation matrix
	D3DXMatrixTranslation(&transMatrix, position.x, position.y, position.z);
	
	// Cause the planet to rotate
	D3DXMatrixRotationY(&rotateMatrix, timeGetTime()/1000.0f);

	// Scale the planet by the size amount
	D3DXMatrixScaling(&scaleMatrix, size, size, size);

	// Multiply the rotation matrix by the translation
	D3DXMatrixMultiply(&transMatrix, &rotateMatrix, &transMatrix);

	// Multiply the translation matrix by the scale
	D3DXMatrixMultiply(&transMatrix, &scaleMatrix, &transMatrix);

	// Transform the object into world space
	device->SetTransform(D3DTS_WORLD, &transMatrix);

	// Render the planet
	Model->render(device);
}

void CPlanet::setSize(float planetSize)
{
	size = planetSize;
}