#include "WindowUtils.h"
#include "D3D.h"
#include "Game.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Game gGame;




void Update(float dTime)
{
	gGame.Update(dTime);
}

void Render(float dTime)
{
	gGame.Render(dTime);
}

void OnResize(int screenWidth, int screenHeight)
{
	gGame.OnResize(screenWidth, screenHeight);
}


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return gGame.WindowsMssgHandler(hwnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{

	if (!InitMainWindow(800, 600, hInstance, "MyApp", MainWndProc))
		assert(false);

	if (!InitDirect3D(OnResize))
		assert(false);

	gGame.Initialise();
	Run(Update, Render);
	gGame.Release();
	ReleaseD3D();
	return 0;
}
