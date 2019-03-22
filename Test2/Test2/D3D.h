#ifndef D3DH
#define D3DH

#include <d3d11.h>

#include "SimpleMath.h"

extern ID3D11Device* gd3dDevice;
extern ID3D11DeviceContext* gd3dImmediateContext;

extern IDXGISwapChain* gSwapChain;
extern ID3D11RenderTargetView* gRenderTargetView;
extern ID3D11DepthStencilView* gDepthStencilView;
extern D3D11_VIEWPORT gScreenViewport;
extern void(*gpOnResize)(int, int);

bool InitDirect3D(void(*pOnResize)(int,int));
void OnResize_Default(int clientWidth, int clientHeight);
void ReleaseD3D(bool extraReporting=true);

void InitInputAssembler(ID3D11InputLayout* pInputLayout, ID3D11Buffer* pVBuffer, UINT szVertex, 
	ID3D11Buffer* pIBuffer, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

float GetAspectRatio();

void BeginRender(const DirectX::SimpleMath::Vector4& backColour);

void EndRender();


#endif
