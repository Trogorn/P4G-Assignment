#include "Turret.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;


void Turret::Initialise(Model *FirstTurret, Model *ThirdTurret, Model *FirstLaser, Model *ThirdLaser, const Vector3& pos, const DirectX::SimpleMath::Vector3& tgt, Matrix& viewSpaceTfm)
{
	// Get Models
	this->FirstTurret = FirstTurret;
	this->ThirdTurret = ThirdTurret;
	this->Firstlaser = FirstLaser;
	this->Thirdlaser = ThirdLaser;

	//standard camera setup
	mpViewSpaceTfm = &viewSpaceTfm;
	CreateViewMatrix(*mpViewSpaceTfm, pos, tgt, Vector3(0, 1, 0));
	mCamPos = pos;
	yaw = pitch = roll = 0;
}

void Turret::UpdatePosition(Vector3 position)
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

	Rotate(dTime);

	if (cooldown > 0)
		cooldown -= dTime;
}


void Turret::MenuUpdate(float dTime)
{
	ThirdTurret->GetRotation() = Vector3(0, ThirdTurret->GetRotation().y + dTime, 0);
}

void Turret::Reset()
{
	ThirdTurret->GetRotation() = Vector3(0, 180 * MyUtils::Deg2Rad , 0);
}

Vector3 Turret::GetShort()
{
//	Shoot();
	Vector3 dir(0, 0, 1);
	dir = Vector3::TransformNormal(dir, Matrix::CreateFromYawPitchRoll(yaw, pitch, roll));
	return mCamPos + dir * shortestdistrance;
}

void Turret::Rotate(float dTime)
{
	Vector2 m;
	m = GetMouseAndKeys()->GetMouseMoveAndCentre();

	if (m.x != 0 || m.y != 0)
	{
		float _yaw = m.x;
		float _pitch = m.y;
		float _roll = 0;
		yaw += _yaw * dTime * rspeed;
		pitch += _pitch * dTime * rspeed;
		roll += _roll * dTime * rspeed;

		float pitchlower = 45;

		if (pitch < upperLimit)
			pitch = upperLimit;

		if (pitch > lowerLimit)
			pitch = lowerLimit;

		Matrix ori;
		ori = Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
		Vector3 dir(0, 0, 1), up(0, 1, 0);
		dir = Vector3::TransformNormal(dir, ori);
		up = Vector3::TransformNormal(up, ori);
		CreateViewMatrix(*mpViewSpaceTfm, mCamPos, mCamPos + dir, up);
	}
}

void Turret::Shoot()
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
		{
			hitPosition = Vector3(0, 0, 0);
		}
		cooldown =  maxCooldown;
		
	}
}

//Render first person turret
void Turret::Renderfirst()
{
	Matrix TurretMatrix;

	FirstTurret->GetWorldMatrix(TurretMatrix);

	Matrix cameraMatrixInverse = GetMatrix().Invert();

	TurretMatrix = TurretMatrix * cameraMatrixInverse;

	FX::GetMyFX()->Render(*FirstTurret, gd3dImmediateContext, nullptr, &TurretMatrix);

	Matrix LaserMatrix;

	Firstlaser->GetWorldMatrix(LaserMatrix);

	LaserMatrix = LaserMatrix * cameraMatrixInverse;

	if (!CanFire())
		FX::GetMyFX()->Render(*Firstlaser, gd3dImmediateContext, nullptr, &LaserMatrix);
}

//render third person Turret
void Turret::Renderthird()
{
	Matrix TurretMatrix;

	ThirdTurret->GetWorldMatrix(TurretMatrix);

	Matrix cameraMatrixInverse = GetMatrix().Invert();

	TurretMatrix = TurretMatrix * cameraMatrixInverse;


	FX::GetMyFX()->Render(*ThirdTurret, gd3dImmediateContext, nullptr, &TurretMatrix);

	Matrix LaserMatrix;
	Thirdlaser->GetWorldMatrix(LaserMatrix);

	LaserMatrix = LaserMatrix * cameraMatrixInverse;

	if (!CanFire())
		FX::GetMyFX()->Render(*Thirdlaser, gd3dImmediateContext, nullptr, &LaserMatrix);
}

void Turret::OnCollide(Model* Other, float dTime)
{
	return;
}