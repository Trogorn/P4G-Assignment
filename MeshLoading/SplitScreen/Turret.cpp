#include "Turret.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;


void Turret::Initialise(const Vector3& pos, const DirectX::SimpleMath::Vector3& tgt, Matrix& viewSpaceTfm)
{
	//standard camera setup
	mpViewSpaceTfm = &viewSpaceTfm;
	CreateViewMatrix(*mpViewSpaceTfm, pos, tgt, Vector3(0, 1, 0));
	mCamPos = pos;
	yaw = pitch = roll = 0;
}

void Turret::SetPosition(Vector3 position)
{
	Matrix ori = Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
	Vector3 dir(0, 0, 1), up(0, 1, 0);
	dir = Vector3::TransformNormal(dir, ori);
	up = Vector3::TransformNormal(up, ori);
	mCamPos = position;
	CreateViewMatrix(*mpViewSpaceTfm, mCamPos, mCamPos + dir, up);
	
}

void Turret::Update(float dTime)
{
	if (cooldown > 0)
		cooldown -= dTime;
}

void Turret::Move(float dTime, bool forward, bool back, bool left, bool right)
{
	//if we didn't move then nothing to do
	if (!forward && !back && !left && !right)
		return;

	//assuming we are sat at the origin looking straight down Z
	//yaw=rotating around Y, pitch=rotating around X, roll=rotate around Z
	Matrix ori = Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
	//figure out forwards and up from the new matrix
	Vector3 dir(0, 0, 1), up(0, 1, 0);
	dir = Vector3::TransformNormal(dir, ori);
	up = Vector3::TransformNormal(up, ori);
	//side to side motion must be orthogonal to forwards and up, so a cross product is perfect
	Vector3 strafe = dir.Cross(up);

	//moving the camera
	Vector3 pos(mCamPos);
	if (forward)
		pos += dir * dTime* speed;
	else if (back)
		pos += -dir * dTime * speed;
	if (left)
		pos += strafe * dTime * speed;
	else if (right)
		pos -= strafe * dTime * speed;

	//the camera might be fixed in certain axes e.g. to keep you on the floor
	if (mLockAxis.x != UNLOCK)
		pos.x = mLockAxis.x;
	if (mLockAxis.y != UNLOCK)
		pos.y = mLockAxis.y;
	if (mLockAxis.z != UNLOCK)
		pos.z = mLockAxis.z;

	//build the camera in its new position
	CreateViewMatrix(*mpViewSpaceTfm, pos, pos + dir, up);
	mCamPos = pos;
}

Vector3 Turret::GetRot()
{
	return Vector3(pitch, yaw, roll);
}



Vector3 Turret::GetShort()
{
//	Shoot();
	Vector3 dir(0, 0, 1);
	dir = Vector3::TransformNormal(dir, Matrix::CreateFromYawPitchRoll(yaw, pitch, roll));
	return mCamPos + dir * shortestdistrance;
}

void Turret::Rotate(float dTime, float _yaw, float _pitch, float _roll)
{
	yaw += _yaw * dTime * rspeed;
	pitch += _pitch * dTime * rspeed;
	roll += _roll * dTime * rspeed;
	Matrix ori;
	ori = Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
	Vector3 dir(0, 0, 1), up(0, 1, 0);
	dir = Vector3::TransformNormal(dir, ori);
	up = Vector3::TransformNormal(up, ori);
	CreateViewMatrix(*mpViewSpaceTfm, mCamPos, mCamPos + dir, up);
}




Model * Turret::Shoot()
{
	Model* box = nullptr;
	if (cooldown <= 0)
	{
		shortestdistrance = distance;
		
		bool hit = false;

		Vector3 dir(0, 0, 1);
		dir = Vector3::TransformNormal(dir, Matrix::CreateFromYawPitchRoll(yaw, pitch, roll));

		Ray ray(GetPos(), dir);

		if (mpMyVec->size() > 0)
			for (int i(0); i < mpMyVec->size(); i++)
			{
				float dummydistance(distance);

				if (mpMyVec->at(i)->GetAlive() && ray.Intersects(mpMyVec->at(i)->GetCollider(), _Out_ dummydistance) && dummydistance < shortestdistrance)
				{
					shortestdistrance = dummydistance;
					box = mpMyVec->at(i);
					hit = true;
				}


			}

		if (hit)
		{
			hitPosition = origin + direction * shortestdistrance;
			box->Die();
			//Damage linked reference in box if it has one
		}
		else
			hitPosition = Vector3(0, 0, 0);


		cooldown =  maxCooldown;
		
	}
	
	return box;
}

//Render first person turret
void Turret::Renderfirst(Model* turret, Model* laser)
{
	Matrix gm;
	turret->GetWorldMatrix(gm);
	Matrix cam = GetMatrix().Invert();
	gm = gm * cam;
	FX::GetMyFX()->Render(*turret, gd3dImmediateContext, nullptr, &gm);

	Matrix dm;
	laser->GetWorldMatrix(dm);
	dm = dm * cam;

	if (!CanFire())
		FX::GetMyFX()->Render(*laser, gd3dImmediateContext, nullptr, &dm);
}

//render third person Turret
void Turret::Renderthird(Model* turret, Model* laser)
{
	Matrix cm;
	turret->GetWorldMatrix(cm);
	Matrix cam = GetMatrix().Invert();
	cm = cm * cam;
	FX::GetMyFX()->Render(*turret, gd3dImmediateContext, nullptr, &cm);

	Matrix dm;
	laser->GetWorldMatrix(dm);

	dm = dm * cam;

	if (!CanFire())
		FX::GetMyFX()->Render(*laser, gd3dImmediateContext, nullptr, &dm);
}