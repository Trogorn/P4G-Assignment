#include "GameObject.h"


GameObject::GameObject()
{
	forward = forward.UnitZ;
	//This second line feels redunant I'll take a look at it at some point (Lochlann)
	forward.z = 1;
	//Set forward vector to 0,0,1
}

void GameObject::Initialise(Model* mModel)
{
	this->mModel = mModel;
}


void GameObject::Render()
{
	FX::GetMyFX()->Render(*mModel, gd3dImmediateContext);
}

void GameObject::Move()
{
	
}

Matrix GameObject::GetWorldMatrix()
{
	Matrix w;
	mModel->GetWorldMatrix(w);
	return w;
}