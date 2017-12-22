#include "GameObject.h"

#include ".\ship.h"
#include ".\model.h"

#define MOVE_RATE 0.5f

CShip::CShip(void)
{
	forwardVector.x = MOVE_RATE;
}

CShip::~CShip(void)
{
}

bool CShip::create(LPDIRECT3DDEVICE9 device)
{
	position.x = 0.0f;
	position.y = 0.0f;
	position.z = 0.0f;

	forwardVector.x = 0.0f;
	forwardVector.y = 0.0f;
	forwardVector.z = 1.0f;

	Model = new CModel();
	return Model->loadModel(device, "bigship1.X");
}

void CShip::setPosition(D3DXVECTOR3 newPosition)
{
	position = newPosition;
}

void CShip::move(int direction, float anim_rate)
{
	switch (direction)
	{
		case FORWARD:
			forwardVector.x = -MOVE_RATE;
			
			position.x -= forwardVector.x * -sin(D3DXToRadian(forwardVector.y));
			position.z += forwardVector.x * cos(D3DXToRadian(forwardVector.y));
		break;

		case BACKWARD:
			forwardVector.x = MOVE_RATE;
			position.x -= forwardVector.x * -sin(D3DXToRadian(forwardVector.y));
			position.z += forwardVector.x * cos(D3DXToRadian(forwardVector.y));
		break;

		case LEFT:
			forwardVector.y -= 5.0f;
		break;

		case RIGHT:
			forwardVector.y += 5.0f;
		break;
	}
}

void CShip::render(LPDIRECT3DDEVICE9 device)
{
	D3DXMATRIX transMatrix;				// the translation matrix
	D3DXMATRIX rotMatrix;				// the rotation matrix
	
	// create the translation matrix
	D3DXMatrixTranslation(&transMatrix, position.x, position.y, position.z);
	
	// create the rotation matrix for the object
	D3DXMatrixRotationY(&rotMatrix,D3DXToRadian(forwardVector.y));
	
	// Multiply the translation matrix by the rotation matrix
	// The resulting matrix is stored in the transMatrix
	D3DXMatrixMultiply(&transMatrix, &rotMatrix, &transMatrix);
	
	// Transform the object into world space
	device->SetTransform(D3DTS_WORLD, &transMatrix);

	// render the model for this ship
	Model->render(device);
}