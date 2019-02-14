#include "WindowUtils.h"
#include "D3D.h"
#include "ModeManager.h"
#include "ModeTitle.h"
#include "ModeGameOver.h"
#include "ModeGame.h"
#include "FX.h"
#include "Mesh.h"
#include "Input.h"
#include "GameObjectManager.h"
#include "File.h"
#include "UserFolder.h"



using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;





void Update(float dTime)
{
	GetModeManager()->Update(dTime);
}

void Render(float dTime)
{
	GetModeManager()->Render(dTime);
	GetMouseAndKeys()->PostProcess();
}

void OnResize(int screenWidth, int screenHeight)
{
	//gGame.OnResize(screenWidth, screenHeight);
	OnResize_Default(screenWidth, screenHeight);
}


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INPUT:
		GetMouseAndKeys()->MessageEvent((HRAWINPUT)lParam);
		break;
	case WM_CHAR:
		GetModeManager()->OnKey(wParam);
		break;

	}

	//default message handling (resize window, full screen, etc)
	return DefaultMssgHandler(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	SeedRandom();
	File::initialiseSystem();
	USER::SetUserDataFolder("AstroBlaster");

	if (!InitMainWindow(1024, 768, hInstance, "MyApp", MainWndProc))
		assert(false);

	if (!InitDirect3D(OnResize))
		assert(false);

	//startup
	new FX::MyFX(gd3dDevice);
	new MeshManager;
	new MouseAndKeys;
	GetMouseAndKeys()->Initialise(GetMainWnd());
	new GameObjectManager;
	new ModeManager;
	ModeTitle* p = new ModeTitle;
	new ModeGameOver;
	new ModeGame;
	p->Load();
	GetModeManager()->SwitchMode("TITLE");

	Run(Update, Render);

	//shut down
	delete GetModeManager();
	delete GetGameObjectManager();
	delete GetMouseAndKeys();
	delete GetMeshManager();
	delete FX::GetMyFX();

	ReleaseD3D();

	return 0;
}
