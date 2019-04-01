#include "GameObject.h"


GameObject::GameObject()
{
	forward = forward.UnitZ;
	forward.z = 1;
	//Set forward vector to 0,0,1
}

void GameObject::Initialise(Model* mModel)
{
	this->mModel = mModel;
}


void GameObject::Render(FX::MyFX fx)
{
	//fx.Render(*mModel, gd3dImmediateContext);
}

//Vector3 GameObject::GetPosition()
//{
//	return mModel->GetPosition();
//}