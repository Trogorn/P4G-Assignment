#pragma once

#include "d3d.h"
#include "Model.h"
#include "Mesh.h"
#include "FX.h"

using namespace DirectX::SimpleMath;


class GameObject
{
public:
	GameObject();
	void Initialise(Model* mModel);


	//This needs to be merged into Update
	// Or moved so that its called in Update (Lochlann)
	virtual void Move();

	virtual void Render();

	virtual void Update(float dTime) = 0;

	virtual void OnCollide(Model* Other, float dTime) = 0;

	Vector3 Getforward() { return forward; }

	//Return Reference to the Position
	Vector3* GetPosition() const { return &mModel->GetPosition(); }
	//Return Reference to the Rotation
	Vector3* GetRotation() const { return &mModel->GetRotation(); }
	//Return Reference to the Scale
	Vector3* GetScale() const { return &mModel->GetScale(); }

	Matrix GetWorldMatrix();

	Model* GetModel() { return mModel; };

protected:
	//This may be changed in not too long
	Vector3 forward;

private:
	Model* mModel;
	//BoundingBox Collider
	DirectX::BoundingBox box;

};

