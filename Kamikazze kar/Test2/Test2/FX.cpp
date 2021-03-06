#include <d3dcompiler.h>
#include <assert.h>

#include "DDSTextureLoader.h"

#include "D3D.h"
#include "D3DUtil.h"
#include "FX.h"
#include "Mesh.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace FX
{


GfxParamsPerObj gGfxPerObj;
GfxParamsPerFrame gGfxPerFrame;
GfxParamsPerMesh gGfxPerMesh;
ID3D11Buffer *gpGfxPerObj = nullptr, *gpGfxPerFrame = nullptr, *gpGfxPerMesh = nullptr;
Matrix gView, gProj;

DirectX::SimpleMath::Matrix& GetProjectionMatrix()
{
	return gProj;
}

DirectX::SimpleMath::Matrix& GetViewMatrix()
{
	return gView;
}


void CreateConstantBuffers()
{
	CreateConstantBuffer(sizeof(GfxParamsPerFrame), &gpGfxPerFrame);
	CreateConstantBuffer(sizeof(GfxParamsPerObj), &gpGfxPerObj);
	CreateConstantBuffer(sizeof(GfxParamsPerMesh), &gpGfxPerMesh);
}

void ReleaseConstantBuffers()
{
	ReleaseCOM(gpGfxPerFrame);
	ReleaseCOM(gpGfxPerObj);
	ReleaseCOM(gpGfxPerMesh);
}

void SetPerObjConsts(ID3D11DeviceContext *pD3DContext, DirectX::SimpleMath::Matrix& world)
{
	gGfxPerObj.world = world;
	gGfxPerObj.worldInvT = InverseTranspose(world);
	gGfxPerObj.worldViewProj = world * gView * gProj;

	pD3DContext->UpdateSubresource(gpGfxPerObj, 0, nullptr, &gGfxPerObj, 0, 0);
}

void SetPerFrameConsts(ID3D11DeviceContext *pD3DContext, const Vector3& eyePos)
{
	gGfxPerFrame.eyePosW = Vector4(eyePos.x, eyePos.y, eyePos.z, 0);
	pD3DContext->UpdateSubresource(gpGfxPerFrame, 0, nullptr, &gGfxPerFrame, 0, 0);

}

void CreateRasterStates(ID3D11Device *pDevice, ID3D11RasterizerState *pStates[RasterType::MAX_STATES])
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
	desc.FrontCounterClockwise = true;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_FRONT;
	HR(pDevice->CreateRasterizerState(&desc, &pStates[RasterType::CCW_FILLED]));

	desc.FillMode = D3D11_FILL_WIREFRAME;
	HR(pDevice->CreateRasterizerState(&desc, &pStates[RasterType::CCW_WIRE]));

	desc.FillMode = D3D11_FILL_SOLID;
	desc.FrontCounterClockwise = false;
	HR(pDevice->CreateRasterizerState(&desc, &pStates[RasterType::CW_FILLED]));
	desc.FillMode = D3D11_FILL_WIREFRAME;
	HR(pDevice->CreateRasterizerState(&desc, &pStates[RasterType::CW_WIRE]));

	desc.CullMode = D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_WIREFRAME;
	HR(pDevice->CreateRasterizerState(&desc, &pStates[RasterType::NOCULL_WIRE]));
	desc.CullMode = D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_SOLID;
	HR(pDevice->CreateRasterizerState(&desc, &pStates[RasterType::NOCULL_FILLED]));
}


void CreateTransparentBlendState(ID3D11Device* pd3dDevice, ID3D11BlendState* &pTransparent)
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;

	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;

	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	HR(pd3dDevice->CreateBlendState(&blendDesc, &pTransparent));


}


void CreateAlphaTransparentBlendState(ID3D11Device* pd3dDevice, ID3D11BlendState* &pBlend)
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;	
	rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;

	rtbd.SrcBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0] = rtbd;
	//rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;

	HR(pd3dDevice->CreateBlendState(&blendDesc, &pBlend));
}


void SetupDirectionalLight(int lightIdx, bool enable, const Vector3 &direction,
	const Vector3& diffuse, const Vector3& ambient, const Vector3& specular)
{
	assert(lightIdx >= 0 && lightIdx < 8);
	if (!enable)
	{
		gGfxPerFrame.lights[lightIdx].type = Light::Type::OFF;
		return;
	}

	Light& l = gGfxPerFrame.lights[lightIdx];
	l.type = Light::Type::DIR;
	l.Diffuse = Vec3To4(diffuse, 0);
	l.Ambient = Vec3To4(ambient, 0);
	l.Specular = Vec3To4(specular,0);
	l.Direction = Vec3To4(direction, 0);
}

void SetupPointLight(int lightIdx, bool enable,
	const Vector3& position,
	const Vector3& diffuse,
	const Vector3& ambient,
	const Vector3& specular,
	float range/*=1000.f*/, float atten1/*=0.05f*/)
{
	assert(lightIdx >= 0 && lightIdx < 8);
	if (!enable)
	{
		gGfxPerFrame.lights[lightIdx].type = Light::Type::OFF;
		return;
	}

	Light& l = gGfxPerFrame.lights[lightIdx];
	l.type = Light::Type::POINT;
	l.Diffuse = Vec3To4(diffuse, 0);
	l.Ambient = Vec3To4(ambient, 0);
	l.Specular = Vec3To4(specular,0);
	l.Position = Vec3To4(position, 0);
	l.Attenuation = Vector4(0, atten1, 0, 0);
	l.range = range;
}

void SetupSpotLight(int lightIdx, bool enable,
	const DirectX::SimpleMath::Vector3 &position,
	const DirectX::SimpleMath::Vector3 &direction,
	const DirectX::SimpleMath::Vector3& diffuse,
	const DirectX::SimpleMath::Vector3& ambient,
	const DirectX::SimpleMath::Vector3& specular,
	float range, float atten1, float innerConeTheta, float outerConePhi)
{
	assert(lightIdx >= 0 && lightIdx < 8);
	if (!enable)
	{
		gGfxPerFrame.lights[lightIdx].type = Light::Type::OFF;
		return;
	}

	Light& l = gGfxPerFrame.lights[lightIdx];
	l.type = Light::Type::SPOT;
	l.Diffuse = Vec3To4(diffuse, 0);
	l.Ambient = Vec3To4(ambient, 0);
	l.Specular = Vec3To4(specular,0);
	l.Position = Vec3To4(position, 0);
	l.Direction = Vec3To4(direction, 0);
	l.Attenuation = Vector4(0, atten1, 0, 0);
	l.range = range;
	l.theta = innerConeTheta;
	l.phi = outerConePhi;
}


void CheckShaderModel5Supported(ID3D11Device* pd3dDevice)
{
	D3D_FEATURE_LEVEL featureLevel = pd3dDevice->GetFeatureLevel();
	bool featureLevelOK = true;
	switch (featureLevel)
	{
	case D3D_FEATURE_LEVEL_11_1:
	case D3D_FEATURE_LEVEL_11_0:
		break;
	default:
		featureLevelOK = false;
	}

	if (!featureLevelOK)
	{
		DBOUT("feature level too low for shader model 5");
		assert(false);
	}
}

void CreateConstantBuffer(UINT sizeOfBuffer, ID3D11Buffer **pBuffer)
{
	// Create the constant buffers for the variables defined in the vertex shader.
	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeOfBuffer;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	HR(gd3dDevice->CreateBuffer(&constantBufferDesc, nullptr, pBuffer));

}

void CompileBlob(const wstring& fileName, const string& entryPoint, const string& model, ID3DBlob* &pShaderBlob)
{
	ID3DBlob* pErrorBlob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	if (D3DCompileFromFile(fileName.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint.c_str(),
		model.c_str(),
		flags,
		0,
		&pShaderBlob,
		&pErrorBlob) != S_OK)
	{
		if (pErrorBlob)
		{
			std::string errorMessage = (char*)pErrorBlob->GetBufferPointer();
			DBOUT(errorMessage.c_str());
			assert(false);
		}
	}

	assert(pShaderBlob);
	ReleaseCOM(pErrorBlob);
}

void CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC vdesc[], int numElements, ID3DBlob* pVSBlob, ID3D11InputLayout** pLayout)
{
	assert(pVSBlob);
	HR(gd3dDevice->CreateInputLayout(vdesc, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), pLayout));

}

void CreateVertexShader(ID3DBlob* pVSBlob, ID3D11VertexShader* &pVS)
{
	assert(pVSBlob);
	HR(gd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		nullptr,
		&pVS));
	assert(pVS);
}

void CreatePixelShader(ID3DBlob* pPSBlob, ID3D11PixelShader* &pPS)
{
	assert(pPSBlob);
	HR(gd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(),
		nullptr,
		&pPS));
	assert(pPS);
}

void CreateSampler(ID3D11SamplerState* &pSampler)
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(gd3dDevice->CreateSamplerState(&sampDesc, &pSampler));
}


bool MyFX::Init(ID3D11Device* pd3dDevice)
{
	CheckShaderModel5Supported(pd3dDevice);
	CreateSampler(mpSamAnisotropic);
	CreateRasterStates(pd3dDevice, mpRasterStates);

	ID3DBlob* pShaderBlob = nullptr;

	HR(D3DReadFileToBlob(L"../bin/data/TextureVS.cso", &pShaderBlob));
	CreateVertexShader(pShaderBlob, mpVS);
	CreateInputLayout(VertexPosNormTex::sVertexDesc, 3, pShaderBlob, &mpInputLayout);
	ReleaseCOM(pShaderBlob);

	HR(D3DReadFileToBlob(L"../bin/data/PSLitNoTex.cso", &pShaderBlob));
	CreatePixelShader(pShaderBlob, mpPSLit);
	ReleaseCOM(pShaderBlob);

	HR(D3DReadFileToBlob(L"../bin/data/PSUnlitNoTex.cso", &pShaderBlob));
	CreatePixelShader(pShaderBlob, mpPSUnlit);
	ReleaseCOM(pShaderBlob);

	HR(D3DReadFileToBlob(L"../bin/data/PSLitTex.cso", &pShaderBlob));
	CreatePixelShader(pShaderBlob, mpPSLitTex);
	ReleaseCOM(pShaderBlob);

	HR(D3DReadFileToBlob(L"../bin/data/PSUnlitTex.cso", &pShaderBlob));
	CreatePixelShader(pShaderBlob, mpPSUnlitTex);
	ReleaseCOM(pShaderBlob);

	CreateConstantBuffers();
	CreateTransparentBlendState(pd3dDevice, mpBlendTransparent);
	CreateAlphaTransparentBlendState(pd3dDevice, mpBlendAlphaTrans);

	//preload textures we'll need
	mCache.SetAssetPath("../bin/data/");

	return true;
}

void MyFX::Release()
{
	ReleaseCOM(mpVS);
	ReleaseCOM(mpPSLit);
	ReleaseCOM(mpPSUnlit);
	ReleaseCOM(mpPSLitTex);
	ReleaseCOM(mpPSUnlitTex);
	ReleaseCOM(mpInputLayout);
	ReleaseCOM(mpSamAnisotropic);
	ReleaseCOM(mpBlendTransparent);
	ReleaseCOM(mpBlendAlphaTrans);
	ReleaseConstantBuffers();
	for (int i = 0; i < RasterType::MAX_STATES; ++i)
		ReleaseCOM(mpRasterStates[i]);
}

void MyFX::Render(Model_Kami& model, ID3D11DeviceContext *pD3DContext, MaterialExt* pOverrideMat)
{
	//setup shaders
	pD3DContext->VSSetShader(mpVS, nullptr, 0);

	Matrix w;
	model.GetWorldMatrix(w);
	SetPerObjConsts(pD3DContext, w);

	Mesh& mesh = model.GetMesh();
	for (int i = 0; i < mesh.GetNumSubMeshes(); ++i)
	{
		//update material
		SubMesh& sm = mesh.GetSubMesh(i);

		InitInputAssembler(mpInputLayout, sm.mpVB, sizeof(VertexPosNormTex), sm.mpIB);
		MaterialExt *pM;
		if (pOverrideMat)
			pM = pOverrideMat;
		else if (model.HasOverrideMat())
			pM = model.HasOverrideMat();
		else
			pM = &sm.material;


		PreRenderObj(pD3DContext, *pM);
		pD3DContext->DrawIndexed(sm.mNumIndices, 0, 0);
	
	}
	pD3DContext->OMSetBlendState(0, 0, 0xffffffff);
}

void MyFX::PreRenderObj(ID3D11DeviceContext *pD3DContext, MaterialExt& mat)
{
	//texture transform
	gGfxPerMesh.material = mat.gfxData;
	gGfxPerMesh.textureTrsfm = Matrix::CreateScale(mat.texTrsfm.scale.x, mat.texTrsfm.scale.y, 1) *
		Matrix::CreateRotationZ(mat.texTrsfm.angle) *
		Matrix::CreateTranslation(mat.texTrsfm.translate.x, mat.texTrsfm.translate.y, 0);
	pD3DContext->UpdateSubresource(gpGfxPerMesh, 0, nullptr, &gGfxPerMesh, 0, 0);

	//buffers
	pD3DContext->VSSetConstantBuffers(0, 1, &gpGfxPerFrame);
	pD3DContext->VSSetConstantBuffers(1, 1, &gpGfxPerObj);
	pD3DContext->VSSetConstantBuffers(2, 1, &gpGfxPerMesh);

	pD3DContext->PSSetConstantBuffers(0, 1, &gpGfxPerFrame);
	pD3DContext->PSSetConstantBuffers(1, 1, &gpGfxPerObj);
	pD3DContext->PSSetConstantBuffers(2, 1, &gpGfxPerMesh);

	//select pixel shader to use
	ID3D11PixelShader* p;
	if ((mat.flags&MaterialExt::TFlags::LIT)!=0)
	{
		if (mat.pTextureRV)
			p = mpPSLitTex;
		else
			p = mpPSLit;
	}
	else
	{
		if (mat.pTextureRV)
			p = mpPSUnlitTex;
		else
			p = mpPSUnlit;
	}
	//do we have a texture
	if (mat.pTextureRV)
	{
		pD3DContext->PSSetSamplers(0, 1, &mpSamAnisotropic);
		pD3DContext->PSSetShaderResources(0, 1, &mat.pTextureRV);
	}
	pD3DContext->PSSetShader(p, nullptr, 0);

	//how is it blended?
	if ((mat.flags&MaterialExt::TFlags::TRANSPARENCY) != 0)
		pD3DContext->OMSetBlendState(mpBlendTransparent, mat.blendFactors, 0xffffffff);
	else if ((mat.flags&MaterialExt::TFlags::ALPHA_TRANSPARENCY) != 0)
	{
		float b[] = { 1, 1, 1, 1 };
		pD3DContext->OMSetBlendState(mpBlendAlphaTrans, b, 0xffffffff);
	}

	//should we cull?
	if ((mat.flags&MaterialExt::TFlags::CULL) == 0)
		if ((mat.flags&MaterialExt::TFlags::WIRE_FRAME) != 0)
			pD3DContext->RSSetState(mpRasterStates[RasterType::NOCULL_WIRE]);
		else
			pD3DContext->RSSetState(mpRasterStates[RasterType::NOCULL_FILLED]);
	else if ((mat.flags&MaterialExt::TFlags::CCW_WINDING) != 0)
		if ((mat.flags&MaterialExt::TFlags::WIRE_FRAME) != 0)
			pD3DContext->RSSetState(mpRasterStates[RasterType::CCW_WIRE]);
		else
			pD3DContext->RSSetState(mpRasterStates[RasterType::CCW_FILLED]);
	else
		if ((mat.flags&MaterialExt::TFlags::WIRE_FRAME) != 0)
			pD3DContext->RSSetState(mpRasterStates[RasterType::CW_WIRE]);
		else
			pD3DContext->RSSetState(mpRasterStates[RasterType::CW_FILLED]);
}


ID3D11ShaderResourceView* TexCache::LoadTexture(const std::string& fileName, bool appendPath, ID3D11Device*pDevice)
{
	//search the cache
	Data *pSlot = nullptr;
	for (int i = 0; i < MAX_TEX; ++i)
		if (fileName == mCache[i].fileName)
			//we already have the texture so just return it
			return mCache[i].pTex;
		else if (mCache[i].pTex == nullptr)
			//we found an empty slot, we might need that
			pSlot = &mCache[i];
	//check we have capacity for a new texture
	assert(pSlot);

	//prepare the path for loading
	const string *pPath = &fileName;
	string path;
	if (appendPath)
	{
		path = mAssetPath + fileName;
		pPath = &path;
	}
	std::wstring ws(pPath->begin(), pPath->end());
	//load it
	DDS_ALPHA_MODE alpha;
	ID3D11ShaderResourceView *pT;
	if (CreateDDSTextureFromFile(pDevice, ws.c_str(), nullptr, &pT, 0, &alpha) != S_OK)
	{
		DBOUT("Cannot load " << *pPath << "\n");
		assert(false);
	}
	//save it
	pSlot->pTex = pT;
	pSlot->fileName = fileName;
	return pT;
}
}