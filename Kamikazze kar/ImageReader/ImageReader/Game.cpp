#include <sstream>
#include <thread>
#include <iomanip>

#include "WindowUtils.h"
#include "D3D.h"
#include "Game.h"
#include "GeometryBuilder.h"
#include "CommonStates.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

//our map image is 64x64 pixels
const int MAP_WIDTH = 64;
struct MapData
{
	bool tree = false;  //red pixels
	bool head = false;	//green pixel
};
MapData trees[MAP_WIDTH][MAP_WIDTH];


void Game::OnResize(int screenWidth, int screenHeight)
{
	
	OnResize_Default(screenWidth, screenHeight);
}


void Game::Load()
{

	//wood floor
	mQuad.Initialise(BuildQuad(mMeshMgr));
	MaterialExt *pMat = &mQuad.GetMesh().GetSubMesh(0).material;
	pMat->gfxData.Set(Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0, 0, 0, 1));
	pMat->pTextureRV = mFX.mCache.LoadTexture("floor.dds", true, gd3dDevice);
	pMat->texture = "floor";
	mLoadData.loadedSoFar++;


	//torch
	mTorch.Initialise(BuildSphere(mMeshMgr, 16,16));
	pMat = &mTorch.GetMesh().GetSubMesh(0).material;
	pMat->gfxData.Set(Vector4(1, 0, 0, 1), Vector4(1, 0, 0, 1), Vector4(1, 0, 0, 1));
	mTorch.GetPosition() = Vector3(-100, -100, -100);
	mLoadData.loadedSoFar++;

	mCube.Initialise(BuildCube(mMeshMgr));
	mLoadData.loadedSoFar++;

	

	mBallSim.Initialise(mMeshMgr);

}

void Game::LoadMap()
{
	int width, height, components;
	const char* filePath = "../bin/data/TheRock.jpg";
	//Load the main background image (track with grass)
	unsigned char *data = stbi_load(filePath, &width, &height, &components, 0);
	assert(components == 3); //The number of components must be 3 (RGB)
	//promote to 32bit, DX11 textures are RedGreenBlueAlpha (8bit per channel)
	unsigned char *data32 = new unsigned char[4 * width * height];
	unsigned char* pSource = data, *pDest = data32;
	for (int i = 0; i < (width * height); ++i)
	{
		for (int ii = 0; ii < 3; ii++)
		{
			*pDest = *pSource;
			pDest++;
			pSource++;
		}
		*pDest = 255;
		pDest++;
	}

	//create a texture to match, yuck, this M$ code is nasty looking
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ArraySize = 1;
	ID3D11Texture2D *pTexture = NULL;
	HR(gd3dDevice->CreateTexture2D(&desc, NULL, &pTexture));
	//grab it to write into
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HR(gd3dImmediateContext->Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	//copy the image data across
	memcpy(mappedResource.pData, data32, sizeof(unsigned char)* width * height * 4);
	gd3dImmediateContext->Unmap(pTexture, 0);
	//free our data, DX has it now
	stbi_image_free(data);
	delete[] data32;
	//convert the texture into a shader resource we can use
	D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
	ZeroMemory(&resViewDesc, sizeof(resViewDesc));
	resViewDesc.Format = desc.Format;
	resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resViewDesc.Texture2D.MostDetailedMip = 0;
	resViewDesc.Texture2D.MipLevels = 1;
	MaterialExt *pMat = &mQuad.GetMesh().GetSubMesh(0).material;
	HR(gd3dDevice->CreateShaderResourceView(pTexture, &resViewDesc, &pMat->pTextureRV));


	//load the map and convert to a 2d array
	data = stbi_load("../bin/data/TheRockMap.jpg", &width, &height, &components, 0);
	assert(components == 3); //Image must use RGB
	assert(width == MAP_WIDTH && height == MAP_WIDTH);
	for (int xIndex = 0; xIndex < MAP_WIDTH; ++xIndex)
		for (int yIndex = 0; yIndex < MAP_WIDTH; ++yIndex)
		{
			MapData& d = trees[MAP_WIDTH - 1 - yIndex][xIndex]; //invert it so it's orientated with the world coordinate system
			d.tree = (data[yIndex * height * 3 + xIndex * 3] > 200) ? true : false; //red?
			d.head = (data[yIndex * height * 3 + xIndex * 3 + 1] > 200) ? true : false; //green?
		}

	stbi_image_free(data);

}

void Game::LoadDisplay(float dTime)
{
	BeginRender(Colours::Black);

	mpSpriteBatch->Begin();

	static int pips = 0;
	static float elapsed = 0;
	elapsed += dTime;
	if (elapsed > 0.25f){
		pips++;
		elapsed = 0;
	}
	if (pips > 10)
		pips = 0;
	wstringstream ss;
	ss << L"Loading meshes(" << (int)(((float)mLoadData.loadedSoFar / (float)mLoadData.totalToLoad)*100.f) << L"%) ";
	for (int i = 0; i < pips; ++i)
		ss << L'.';
	mpFont2->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(100, 200), Colours::White, 0, Vector2(0, 0), Vector2(1.f, 1.f));

	ss.str(L"");
	ss << L"FPS:" << (int)(1.f / dTime);
	mpFont->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(10, 550), Colours::White, 0, Vector2(0, 0), Vector2(0.5f, 0.5f));

	mpSpriteBatch->End();

	EndRender();
}

void Game::Initialise()
{
	mFX.Init(gd3dDevice);


	FX::SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.8f,0.8f,0.8f), Vector3(0.15f,0.15f,0.15f), Vector3(0.25f,0.25f,0.25f));

	mpSpriteBatch = new SpriteBatch(gd3dImmediateContext);
	assert(mpSpriteBatch);
	mpFont = new SpriteFont(gd3dDevice, L"../bin/data/comicSansMS.spritefont");
	assert(mpFont);

	mpFont2 = new SpriteFont(gd3dDevice, L"../bin/data/algerian.spritefont");
	assert(mpFont2);

	mLoadData.totalToLoad = 4;
	mLoadData.loadedSoFar = 0;
	mLoadData.running = true;
	mLoadData.loader = std::async(launch::async, &Game::Load, this);

	mMKInput.Initialise(GetMainWnd());
	mGamepad.Initialise();

	LoadMap();
}

void Game::Release()
{
	mFX.Release();
	mMeshMgr.Release();
	delete mpFont;
	mpFont = nullptr;
	delete mpSpriteBatch;
	mpSpriteBatch = nullptr;
	delete mpFont2;
	mpFont2 = nullptr;
}

void Game::Update(float dTime)
{
	mGamepad.Update();

	const float camInc = 10.f * dTime;

	if (mMKInput.IsPressed(VK_A))
			mCamPos.y += camInc;
	else if(mMKInput.IsPressed(VK_Z))
			mCamPos.y -= camInc;
	else if (mMKInput.IsPressed(VK_D))
			mCamPos.x -= camInc;
	else if (mMKInput.IsPressed(VK_F))
			mCamPos.x += camInc;
	else if (mMKInput.IsPressed(VK_W))
			mCamPos.z += camInc;
	else if (mMKInput.IsPressed(VK_S))
			mCamPos.z -= camInc;

	mCamPos.x += mGamepad.GetState(0).leftStickX * dTime;
	mCamPos.z += mGamepad.GetState(0).leftStickY * dTime;
	mCamPos.y += mGamepad.GetState(0).rightStickY * dTime;

}


void Game::Render(float dTime)
{
	if (mLoadData.running)
	{
		if (!mLoadData.loader._Is_ready())
		{
			LoadDisplay(dTime);
			return;
		}
		mLoadData.loader.get();
		mLoadData.running = false;
		return;
	}

	BeginRender(Colours::Black);

	FX::SetPerFrameConsts(gd3dImmediateContext, mCamPos);

	CreateViewMatrix(FX::GetViewMatrix(), mCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
	CreateProjectionMatrix(FX::GetProjectionMatrix(), 0.25f*PI, GetAspectRatio(), 1, 1000.f);


	MaterialExt mat = MaterialExt::default;

	mBallSim.Render(mFX, dTime);

	//floor
	mQuad.GetPosition() = Vector3(0, 0, 0);
	mQuad.GetRotation() = Vector3(0, 0, 0);
	mQuad.GetScale() = Vector3(3, 1, 3);
	mFX.Render(mQuad, gd3dImmediateContext);


	//scale the map into world space
	const float wscale = 0.09f, offset=-2.9f;
	for(int x=0;x<MAP_WIDTH;++x)
		for (int y = 0; y < MAP_WIDTH; ++y)
			if(trees[y][x].tree)
			{
				//Red dot
				mCube.GetPosition() = Vector3(x*wscale+offset, 0.1f, y*wscale+offset);
				mCube.GetScale() = Vector3(0.05f, 0.1f, 0.05f);
				mFX.Render(mCube, gd3dImmediateContext);
			}
			else if (trees[y][x].head)
			{
				//Green dot
				mCube.GetPosition() = Vector3(x*wscale + offset, 0.1f, y*wscale + offset);
				mCube.GetScale() = Vector3(0.05f, 0.1f, 0.05f);
				mFX.Render(mCube, gd3dImmediateContext);
			}

	/*
	CommonStates state(gd3dDevice);
	mpSpriteBatch->Begin(SpriteSortMode_Deferred, state.NonPremultiplied());

	wstring mssg, mssg2;
	if (mMKInput.GetMouseButton(MouseAndKeys::LBUTTON))
	{
		//fire a ray from the camera to the point on the XY plane we mouse clicked on
		Ray ray;
		mMKInput.MousePosToWorldRay(FX::GetViewMatrix(), FX::GetProjectionMatrix(), mCamPos, ray);
		Plane p(Vector3(0, 0, 0), Vector3(0, 0, -1));
		float dist;
		ray.Intersects(p, dist);
		Vector3 pos(ray.position + ray.direction*dist);

		//show that we can generate 3D world space mouse click coordinates
		wstringstream sstr;
		sstr.precision(1);
		sstr << L"Mouse move 3D x=" << pos.x << L" y=" << pos.y << L" z=" << pos.z;
		mssg = sstr.str();
		mpFont->DrawString(mpSpriteBatch, mssg.c_str(), Vector2(10, 100), DirectX::Colors::White, 0, Vector2(0, 0), Vector2(1, 1));

		//display a saw blade on the mouse clicked XY plane to attack the rock with
		mat = MaterialExt::default;
		mat.pTextureRV = mFX.mCache.LoadTexture("saw2.dds", true, gd3dDevice);
		mat.flags |= MaterialExt::TFlags::ALPHA_TRANSPARENCY;
		mQuad.GetPosition() = pos;
		mQuad.GetRotation() = Vector3(-PI / 2, 0, GetClock() * 4);
		mQuad.GetScale() = Vector3(0.2f, 0.2f, 0.2f);
		mFX.Render(mQuad, gd3dImmediateContext, &mat);
	}



	//general messages
	wstringstream ss;
	if (dTime > 0)
		ss << L"FPS: " << (int)(1.f / dTime);
	else
		ss << L"FPS: 0";
	mpFont->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(10, 550), Colours::White, 0, Vector2(0, 0), Vector2(0.5f, 0.5f));

	mBallSim.RenderText(mpFont, mpSpriteBatch);

	mpSpriteBatch->End();

	*/

	EndRender();

	mMKInput.PostProcess();
}

LRESULT Game::WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	const float camInc = 20.f * GetElapsedSec();
	//do something game specific here
	switch (msg)
	{
		// Respond to a keyboard event.
	case WM_CHAR:
		mBallSim.Input(wParam);
		switch (wParam)
		{
		case 27:
		case 'q':
		case 'Q':
			PostQuitMessage(0);
			return 0;
		case ' ':
			mCamPos = mDefCamPos;
			break;
		}
	case WM_INPUT:
			mMKInput.MessageEvent((HRAWINPUT)lParam);
			break;
	}
	//default message handling (resize window, full screen, etc)
	return DefaultMssgHandler(hwnd, msg, wParam, lParam);
}

