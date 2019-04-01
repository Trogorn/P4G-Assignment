#ifndef FX_H
#define FX_H

#include <string>
#include <d3d11.h>
#include "Model_Kami.h"
#include "ShaderTypes.h"
#include "D3DUtil.h"

//class Model;

namespace FX
{
DirectX::SimpleMath::Matrix& GetProjectionMatrix();
DirectX::SimpleMath::Matrix& GetViewMatrix();

namespace RasterType { typedef enum { CCW_FILLED = 0, CCW_WIRE = 1, CW_FILLED = 2, CW_WIRE = 3, NOCULL_WIRE = 4, NOCULL_FILLED = 5, MAX_STATES=6 }; }

void CreateRasterStates(ID3D11Device *pDevice, ID3D11RasterizerState *pStates[RasterType::MAX_STATES]);
void CreatePixelShader(ID3DBlob* pPSBlob, ID3D11PixelShader* &pPS);
void CreateVertexShader(ID3DBlob* pVSBlob, ID3D11VertexShader* &pVS);
void CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC vdesc[], int numElements, ID3DBlob* pVSBlob, ID3D11InputLayout** pLayout);
void CompileBlob(const std::wstring& fileName, const std::string& entryPoint, const std::string& model, ID3DBlob* &pShaderBlob);
void CreateConstantBuffer(UINT sizeOfBuffer, ID3D11Buffer **pBuffer);
void CheckShaderModel5Supported(ID3D11Device* pd3dDevice);
void CreateSampler(ID3D11SamplerState* &pSampler);
void CreateTransparentBlendState(ID3D11Device* pd3dDevice, ID3D11BlendState* &pTransparent);


void CreateConstantBuffers();
void ReleaseConstantBuffers();
void SetPerObjConsts(ID3D11DeviceContext *pD3DContext, DirectX::SimpleMath::Matrix &world);
void SetPerFrameConsts(ID3D11DeviceContext *pD3DContext, const DirectX::SimpleMath::Vector3& eyePos);
void SetupDirectionalLight(int lightIdx, bool enable,
	const DirectX::SimpleMath::Vector3 &direction,
	const DirectX::SimpleMath::Vector3& diffuse = DirectX::SimpleMath::Vector3(1, 1, 1),
	const DirectX::SimpleMath::Vector3& ambient = DirectX::SimpleMath::Vector3(0, 0, 0),
	const DirectX::SimpleMath::Vector3& specular = DirectX::SimpleMath::Vector3(0, 0, 0));

void SetupPointLight(int lightIdx, bool enable,
	const DirectX::SimpleMath::Vector3 &position,
	const DirectX::SimpleMath::Vector3& diffuse = DirectX::SimpleMath::Vector3(1, 1, 1),
	const DirectX::SimpleMath::Vector3& ambient = DirectX::SimpleMath::Vector3(0, 0, 0),
	const DirectX::SimpleMath::Vector3& specular = DirectX::SimpleMath::Vector3(0, 0, 0),
	float range = 1000.f, float atten1 = 0.05f);

void SetupSpotLight(int lightIdx, bool enable,
	const DirectX::SimpleMath::Vector3 &position,
	const DirectX::SimpleMath::Vector3 &direction,
	const DirectX::SimpleMath::Vector3& diffuse = DirectX::SimpleMath::Vector3(1, 1, 1),
	const DirectX::SimpleMath::Vector3& ambient = DirectX::SimpleMath::Vector3(0, 0, 0),
	const DirectX::SimpleMath::Vector3& specular = DirectX::SimpleMath::Vector3(0, 0, 0),
	float range = 1000.f, float atten1 = 0.05f, float innerConeTheta = D2R(30), float outerConePhi = D2R(40));



class TexCache
{
public:
	~TexCache() {
		Release();
	}
	void Release() {
		for (int i = 0; i < MAX_TEX; ++i)
			ReleaseCOM(mCache[i].pTex);
	}
	ID3D11ShaderResourceView* LoadTexture(const std::string& fileName, bool appendPath, ID3D11Device*pDevice);
	void SetAssetPath(const std::string& path) {
		mAssetPath = path;
	}
private:
	struct Data
	{
		std::string fileName;
		ID3D11ShaderResourceView* pTex = nullptr;
	};
	enum { MAX_TEX = 10 };
	std::string mAssetPath;
	Data mCache[MAX_TEX];
};


class MyFX
{
public:
	~MyFX() {
		Release();
	}
	bool Init(ID3D11Device* pd3dDevice);
	void Release();
	void Render(Model_Kami& model, ID3D11DeviceContext *pD3DContext, MaterialExt* pOverrideMat = nullptr);

	TexCache mCache;

private:
	void PreRenderObj(ID3D11DeviceContext *pD3DContext, MaterialExt& mat);

	ID3D11InputLayout* mpInputLayout = nullptr;
	ID3D11SamplerState *mpSamAnisotropic = nullptr;
	ID3D11VertexShader* mpVS = nullptr;
	ID3D11PixelShader* mpPSLit = nullptr, *mpPSUnlit = nullptr, *mpPSLitTex = nullptr, *mpPSUnlitTex = nullptr;
	ID3D11BlendState *mpBlendTransparent = nullptr, *mpBlendAlphaTrans = nullptr;
	ID3D11RasterizerState *mpRasterStates[RasterType::MAX_STATES];
};

}
#endif
