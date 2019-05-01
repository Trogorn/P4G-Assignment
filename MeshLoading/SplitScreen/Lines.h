#pragma once

#include <vector>

#include "D3D.h"

//avoid circular #includes, FX needs to declare a Lines object, but the Lines object refers to FX
//forward declaring FX::MyFX is enough for Lines as there's just one reference in this header
namespace FX
{
	class MyFX;
}

/*
Render 2D lines using an orthogonal camera so it can be done in screen coordinates
x=-horizontal resolution / 2 to +horizontal resolution / 2
y=-vertical resolution / 2 to +vertical resolution / 2
*/
class Lines
{
public:
	void RenderLine(const DirectX::SimpleMath::Vector2& start, const DirectX::SimpleMath::Vector4& startCol, const DirectX::SimpleMath::Vector2& end, const DirectX::SimpleMath::Vector4& endCol, float z = 0);
	void FlushLines(ID3D11DeviceContext *pD3DContext, FX::MyFX& fx);
	void Init();
	void Release();

	//vertex format
	struct VertexPosColour
	{
		DirectX::SimpleMath::Vector2 Pos;
		DirectX::SimpleMath::Vector4 Colour;
		DirectX::SimpleMath::Vector2 pad;  //little bit of padding to keep everything 16byte register alignable

		static const D3D11_INPUT_ELEMENT_DESC sVertexDesc[3];
	};
	static_assert((sizeof(VertexPosColour) % 16) == 0, "CB size not padded correctly");



private:
	const int MAX_LINES = 2000;
	int mLineIdx = 0;
	ID3D11Buffer* mpVBLine = nullptr;
	ID3D11Buffer* mpIBLine = nullptr;
	std::vector<VertexPosColour> mLineVB;
	std::vector<UINT> mLineIB;

	ID3D11InputLayout *mpInputLayoutLine = nullptr;
	//vertex and pixel shaders
	ID3D11VertexShader *mpVSLine = nullptr;
	//a complicated one if it's lit, a simple one if it isn't, also a textured option now (lit and unlit)
	ID3D11PixelShader *mpPSLine = nullptr;
};