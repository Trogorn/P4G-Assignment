
#include "Lines.h"
#include "WindowUtils.h"
#include "D3DUtil.h"
#include "SimpleMath.h"
#include "FX.h"
 
using namespace DirectX::SimpleMath;
using namespace std;
using namespace FX;

const D3D11_INPUT_ELEMENT_DESC Lines::VertexPosColour::sVertexDesc[3]{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOUR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "PAD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

void Lines::Release()
{
	ReleaseCOM(mpVSLine);
	ReleaseCOM(mpPSLine);
	ReleaseCOM(mpInputLayoutLine);
}

void Lines::FlushLines(ID3D11DeviceContext *pD3DContext, FX::MyFX& fx)
{
	assert(mLineIdx < MAX_LINES);
	// Create an orthographic projection matrix for 2D rendering.
	Matrix oldProj = FX::GetProjectionMatrix(), oldView = FX::GetViewMatrix();
	FX::GetViewMatrix() = Matrix::Identity;
	int sw, sh;
	GetClientExtents(sw, sh);
	FX::GetProjectionMatrix() = Matrix::CreateOrthographic((float)sw, (float)sh, 1, 1000);
	SetPerObjConsts(gd3dImmediateContext, FX::GetViewMatrix());

	pD3DContext->VSSetShader(mpVSLine, nullptr, 0);
	pD3DContext->PSSetShader(mpPSLine, nullptr, 0);
	InitInputAssembler(mpInputLayoutLine, mpVBLine, sizeof(VertexPosColour), mpIBLine);
	UpdateDynamicBuffer(mpVBLine, [&](void*pDest) {
		memcpy(pDest, &mLineVB[0], sizeof(VertexPosColour)*mLineIdx * 2);
	});
	UpdateDynamicBuffer(mpIBLine, [&](void*pDest) {
		memcpy(pDest, &mLineIB[0], sizeof(UINT)*mLineIdx * 3);
	});
	fx.SetRasterState(pD3DContext, RasterType::NOCULL_WIRE);
	pD3DContext->OMSetBlendState(0, 0, 0xffffffff);
	fx.EnableDepthTest(pD3DContext, false);
	pD3DContext->DrawIndexed(mLineIdx * 3, 0, 0);
	fx.EnableDepthTest(pD3DContext, true);

	mLineIdx = 0;
	FX::GetProjectionMatrix() = oldProj;
	FX::GetViewMatrix() = oldView;
}

void Lines::RenderLine(const Vector2& start, const Vector4& startCol, const Vector2& end, const Vector4& endCol, float z)
{
	assert(mLineIdx < MAX_LINES);
	VertexPosColour& v = mLineVB[mLineIdx * 2];
	v.Pos = Vector2(start.x, start.y);
	v.Colour = startCol;
	VertexPosColour& v2 = mLineVB[mLineIdx * 2 + 1];
	v2.Pos = Vector2(end.x, end.y);
	v2.Colour = endCol;

	mLineIB[mLineIdx * 3] = mLineIdx * 2;
	mLineIB[mLineIdx * 3 + 1] = mLineIdx * 2 + 1;
	mLineIB[mLineIdx * 3 + 2] = mLineIdx * 2 + 1;

	mLineIdx++;
}
void Lines::Init()
{
	mLineVB.insert(mLineVB.begin(), MAX_LINES * 2, VertexPosColour());
	CreateVertexBuffer(sizeof(VertexPosColour)*MAX_LINES * 2, &mLineVB[0], mpVBLine, true);

	int maxIndices = MAX_LINES * 3;
	mLineIB.insert(mLineIB.begin(), maxIndices, 0);
	for (int i = 0; i < maxIndices; ++i)
		mLineIB[i] = i;
	CreateIndexBuffer(sizeof(UINT)*maxIndices, &mLineIB[0], mpIBLine, true);

	unsigned int bytes = 0;
	char* pBuff = ReadAndAllocate("../bin/data/VSLine.cso", bytes);
	CreateVertexShader(pBuff, bytes, mpVSLine);
	CreateInputLayout(VertexPosColour::sVertexDesc, 3, pBuff, bytes, &mpInputLayoutLine);
	delete[] pBuff;
	pBuff = ReadAndAllocate("../bin/data/PSLine.cso", bytes);
	CreatePixelShader(pBuff, bytes, mpPSLine);
	delete[] pBuff;

}
