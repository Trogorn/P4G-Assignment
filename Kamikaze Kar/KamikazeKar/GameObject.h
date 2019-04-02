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

	virtual void Render(FX::MyFX fx);
	virtual void Update(float dTime) = 0;
	virtual void OnCollide(const GameObject* apOther) = 0;

	//Return Reference to the Position
	Vector3* GetPosition() { return mModel->GetPosition(); }
	//Return Reference to the Rotation
	Vector3* GetRotation() { return mModel->GetRotation(); }
	//Return Reference to the Scale
	Vector3* GetScale() { return mModel->GetScale(); }

	Matrix GetWorldMatrix();

protected:
	Vector3 forward;

private:
	Model* mModel;

};

