#include <string>
#include <iomanip>

#include "BallSim.h"
#include "D3D.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

void BallSim::Initialise(MeshManager& mgr)
{
	Mesh *p = mgr.GetMesh("sphere");
	assert(p);
	mBall.Initialise(*p);
	mBall.GetScale() = Vector3(0.1f, 0.1f, 0.1f);
	MaterialExt mat;
	mat.gfxData.Set(Vector4(0, 0, 1, 1), Vector4(0, 0, 1, 1), Vector4(0, 0, 1, 1));
	mat.flags &= ~MaterialExt::TFlags::LIT;
	mat.flags &= ~MaterialExt::TFlags::CCW_WINDING;
	mBall.SetOverrideMat(&mat);
	mLastMode = mMode = WAITING;
	p = mgr.GetMesh("cube");
	assert(p);
	mCube.Initialise(*p);
	Start();
}

void BallSim::Input(char key)
{
	Mode mode = mLastMode;
	switch (key)
	{
	case 'x':
		break;
	case '1':
		mode = WAITING;
		break;
	case '2':
		mode = LINEAR;
		break;
	case '3':
		mode = LINEAR_ACCEL;
		break;
	case '4':
		mode = BOUNCE_INF;
		break;
	case '5':
		mode = BOUNCE;
		break;
	case '6':
		mode = CUBE;
		break;
	case '7':
		mode = CUBE_MOUSE;
		break;
	case '0':
		return;
	default:
		return;
	}
	mLastMode = mMode;
	mMode = mode;
	Start();
}

void BallSim::Start()
{
	switch (mMode)
	{
	case WAITING:
		mBall.GetPosition() = Vector3(-2, 0.5f, 0);
		mVel = Vector3(0, 1, 0) * 3;
		mGrav = Vector3(0, -9.81f, 0) * 0.5f;
		mAccel = Vector3(0, 0, 0);
		mCOR = 1;
		break;
	case LINEAR:
		mBall.GetPosition() = Vector3(-2, 1, 0);
		mVel = Vector3(1, 0, 0) * 1;
		mGrav = Vector3(0, 0, 0);
		mAccel = Vector3(0, 0, 0);
		mCOR = 1;
		break;
	case LINEAR_ACCEL:
		mBall.GetPosition() = Vector3(-2, 1, 0);
		mVel = Vector3(1, 0, 0) * 1;
		mGrav = Vector3(0, 0, 0);
		mAccel = Vector3(10, 0, 0);
		mCOR = 1;
		break;
	case BOUNCE_INF:
		mBall.GetPosition() = Vector3(-2, 0.5f, 0);
		mVel = Vector3(0.25f, 0.9f, 0) * 3;
		mGrav = Vector3(0, -9.81f, 0) * 0.5f;
		mAccel = Vector3(0, 0, 0);
		mCOR = 1;
		break;
	case BOUNCE:
		mBall.GetPosition() = Vector3(-2, 0.5f, 0);
		mVel = Vector3(0.25f, 0.9f, 0) * 3.5f;
		mGrav = Vector3(0, -9.81f, 0) * 0.5f;
		mAccel = Vector3(0, 0, 0);
		mCOR = 0.8f;
		break;
	case CUBE:
		mBall.GetPosition() = Vector3(-2, 0.5f, 0);
		mVel = Vector3(0.25f, 0.9f, 0) * GetRandom(3.f, 4.f);
		mGrav = Vector3(0, -9.81f, 0) * 0.5f;
		mAccel = Vector3(0, 0, 0);
		mCOR = 0.8f;
		mCube.GetPosition() = Vector3(GetRandom(-0.5f, 0.5f), GetRandom(0.25f, 1.f), 0);
		break;
	case CUBE_MOUSE:
		mBall.GetPosition() = Vector3(-2, 0.5f, 0);
		mVel = Vector3(0.25f, 0.9f, 0) * 3.5f;
		mGrav = Vector3(0, -9.81f, 0) * 0.5f;
		mAccel = Vector3(0, 0, 0);
		mCOR = 0.8f;
		if (mMode != mLastMode)
			mCube.GetPosition() = Vector3(0, 0.25f, 0);
		break;
	}
}

void BallSim::Update(float dTime, const Vector3& camPos, MouseAndKeys& input, Model& rock)
{
	Vector3 pos = mBall.GetPosition();
	pos += mVel * dTime;


	switch (mMode)
	{
	case CUBE:
	case CUBE_MOUSE:
	case BOUNCE:
	case BOUNCE_INF:
		//rock collision
		BoundingSphere head(rock.GetPosition(), 0.5f);
		Vector3 cn;
		if (SphereToSphere(BoundingSphere(mBall.GetPosition(), mRadius), head, cn))
		{
			//we're inside, but are we already trying to escape?
			//if we are not trying to escape then reflect our velocity and move away
			Vector3 d(mVel), d2(mBall.GetPosition() - rock.GetPosition());
			d.Normalize();
			d2.Normalize();
			float escaping = d2.Dot(d);
			if (escaping < 0)
			{
				//not an escape angle so reflect
				mVel = Vector3::Reflect(mVel, cn);
				mVel *= mCOR;
				pos = mBall.GetPosition();
				pos += mVel * dTime;
			}
		}
		break;
	}

	switch (mMode)
	{
	case CUBE:
	case CUBE_MOUSE:

		Vector3 cn, cubeDim(0.25f, 0.25f, 0.25f);
		if (SphereToSphere(BoundingSphere(mBall.GetPosition(), mRadius), BoundingSphere(mCube.GetPosition(),cubeDim.Length()), cn))
		{
			//cube collision
			BoundingBox aabb(mCube.GetPosition(), cubeDim);
			if (SphereToAABBox(aabb, BoundingSphere(mBall.GetPosition(), mRadius), cn))
			{
				//we're inside, but are we already trying to escape?
				//if we are not trying to escape then reflect our velocity and move away
				Vector3 d(mVel);
				d.Normalize();
				float escaping = cn.Dot(d);
				if (escaping < 0)
				{
					//not an escape angle so reflect
					mVel = Vector3::Reflect(mVel, cn);
					mVel *= mCOR;
					pos = mBall.GetPosition();
					pos += mVel * dTime;
				}
			}
		}
		break;
	}

	//are we beneath the floor
	if (pos.y < mRadius)
	{
		//bounce up
		mVel.y *= -1;
		//lose energy
		mVel *= mCOR;
		//ensure we aren't left under the floor
		mBall.GetPosition().y = mRadius;
		pos = mBall.GetPosition();
	}
	mBall.GetPosition() = pos;

	//apply accelerations unless we've come to a halt
	if (mVel.LengthSquared() < 0.001f && fabsf(pos.y - mRadius) < 0.001f)
		mVel = Vector3(0, 0, 0);
	else
	{
		mVel += mGrav * dTime;
		mVel += mAccel * dTime;
	}

	switch (mMode)
	{
	case CUBE_MOUSE:

		if (input.GetMouseButton(MouseAndKeys::LBUTTON))
		{
			Ray ray;
			input.MousePosToWorldRay(FX::GetViewMatrix(), FX::GetProjectionMatrix(), camPos, ray);
			float dist;
			if (RayToSphere(mCube.GetPosition(), 1.f * mCube.GetScale().x, ray, dist))
			{
				if (RayToPlane(Plane(mCube.GetPosition(), FX::GetViewMatrix().Backward()), ray, dist))
				{
					Vector3 pos(ray.position + ray.direction * dist);
					mCube.GetPosition() = pos;
				}
			}
		}
	}
}

void BallSim::Render(FX::MyFX& fx, float dTime)
{
	fx.Render(mBall, gd3dImmediateContext);
	FX::SetupPointLight(1, true, mBall.GetPosition(), Vector3(0, 0, 0.7f), Vector3(0, 0, 0), Vector3(0, 0, 1), 10, 0.1f);

	switch (mMode)
	{
	case CUBE:
	case CUBE_MOUSE:
		mCube.GetScale() = Vector3(0.25f, 0.25f, 0.25f);
		fx.Render(mCube, gd3dImmediateContext);
		break;
	}
}

void BallSim::RenderText(SpriteFont *pF, SpriteBatch *pBatch)
{
	wstring mssg;
	switch (mMode)
	{
	case WAITING:
		mssg = L"mode = Waiting";
		break;
	case LINEAR:
		mssg = L"mode = Linear, no gravity";
		break;
	case LINEAR_ACCEL:
		mssg = L"mode = Linear accel, no gravity";
		break;
	case BOUNCE_INF:
		mssg = L"mode = Bounce infinite";
		break;
	case BOUNCE:
		mssg = L"mode = Bounce";
		break;
	case CUBE:
		mssg = L"mode = Cube";
		break;
	case CUBE_MOUSE:
		mssg = L"mode = Cube - mouse control";
		break;
	}
	pF->DrawString(pBatch, mssg.c_str(), Vector2(10, 10), Colours::White, 0, Vector2(0, 0));

	mssg = L"Keys: x=restart, 1=waiting, 2=linear, 3=linear accel, 4=bounce infinite, 5=bounce, 6=cube, 7=cube mouse";
	pF->DrawString(pBatch, mssg.c_str(), Vector2(10, 30), Colours::White, 0, Vector2(0, 0), Vector2(0.75f, 0.75f));

	wstringstream ss;
	ss << std::setprecision(3);
	ss << L"Velocity=" << mVel.x << L", " << mVel.y;
	pF->DrawString(pBatch, ss.str().c_str(), Vector2(10, 45), Colours::White, 0, Vector2(0, 0), Vector2(0.75f, 0.75f));
}

