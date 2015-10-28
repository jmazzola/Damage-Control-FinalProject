/***********************************************
* Filename:  		Renderer.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		Implementation for DirectX11
************************************************/

#include "Renderer.h"

#if defined(DEBUG) | defined(_DEBUG)
#ifndef TEST_MACHINE_BUILD
#define TEST_MACHINE_BUILD 0
#endif
#else
#ifndef TEST_MACHINE_BUILD
#define TEST_MACHINE_BUILD 0
#endif
#endif

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 2500
#endif

extern DirectX::XMFLOAT3 gAMBIENCE;

//class includes
#include "../Particle Manager/ParticleManager.h"

#include "..\Asset Manager\AssetManager.h"
#include "..\State Machine\GameplayState.h"
#include "..\Entity Manager\LivingEntites.h"
#include "..\Animation Manager\Deformer.h"

#include "RenderStateManager.h"
#include "VertexStructures.h"
#include "BitmapFont.h"
#include "DebugRenderer.h"

#include "ICamera.h"
#include "Scene.h"
#include "RObject.h"

#include "structs.h"
#include "../Entity Manager/AI/BaseEnemy.h"

//i'll need to migrate these eventually
#include "SamplerStateManager.h"
#include "BlendStateManager.h"
#include "DepthStencilStateManager.h"
#include "RasterizerStateManager.h"
#include "ShaderManager.h"
#include "PrimitiveTopologyManager.h"

//c++ schtuff
#include <algorithm>
#include <exception>
#include <memory>

//DirectX11 library linking
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d11.lib" )
#pragma comment(lib, "DXGI.lib" )

using namespace EDRendererD3D;

CRenderer &CRenderer::GetReference( )
{
	if (!instancePtr)
	{
		instancePtr = new CRenderer( );
	}
	return *instancePtr;
}

void CRenderer::DeleteInstance( )
{
	instancePtr->Shutdown( );

	delete instancePtr;
	instancePtr = nullptr;
}

bool CRenderer::Initialize(HWND hWnd,
						   UINT unWidth,
						   UINT unHeight)
{
	m_unWidth = unWidth;
	m_unHeight = unHeight;
	m_window = hWnd;

	UINT creationFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL featureLevels[ ] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	//since *technically* the SwapChain is almost immediately released [WM_SIZE],
	//	it's rather pointless to create one.
	HRESULT hr = D3D11CreateDevice(nullptr,						// specify null to use the default adapter
								   D3D_DRIVER_TYPE_HARDWARE,
								   nullptr,						// leave as nullptr unless software device
								   creationFlags,				// optionally set debug and Direct2D compatibility flags
								   featureLevels,				// list of feature levels this app can support
								   _countof(featureLevels),		// number of entries in above list
								   D3D11_SDK_VERSION,			// always set this to D3D11_SDK_VERSION
								   &m_d3dDevice,				// returns the Direct3D device created
								   &m_tFeatureLevel,			// returns feature level of device created
								   &m_d3dContext				// returns the device immediate context
								   );

	if (hr == E_INVALIDARG)
	{
		//if 11.1 isn't supported, fallback to 11.0
		hr = D3D11CreateDevice(nullptr,
							   D3D_DRIVER_TYPE_HARDWARE,
							   nullptr,
							   creationFlags,
							   &featureLevels[1],
							   _countof(featureLevels) - 1,
							   D3D11_SDK_VERSION,
							   &m_d3dDevice,
							   &m_tFeatureLevel,
							   &m_d3dContext
							   );
	}

	//if device creation failed again then like well *shrug*
	HR(hr);

	InputLayoutManager::GetReference( );

	BuildPerObjectConstantBuffers( );
	BuildPerCameraConstantBuffers( );
	BuildMiscBuffers( );
	BuildLighting( );

	if (SUCCEEDED(m_d3dDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&m_d3dDebugDev)))
	{
		ID3D11InfoQueue *d3dInfoQueue = nullptr;
		if (SUCCEEDED(m_d3dDebugDev->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif

			D3D11_MESSAGE_ID hide[ ] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,

				// Add more message IDs here as needed
				D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET,
			};

			D3D11_INFO_QUEUE_FILTER filter;
			memset(&filter, 0, sizeof(filter));
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
			d3dInfoQueue->Release( );
		}

		SafeRelease(m_d3dDebugDev);
	}

#ifdef _DEBUG
	HR(m_d3dContext->QueryInterface(IID_PPV_ARGS(&m_d3dUserDefinedAnnotation)));
#endif

	CDebugRenderer::GetReference( );
	InputLayoutManager::GetReference( );
	CRenderStateManager::GetReference( );

	BuildScreenQuad(&m_d3dScreenQuadVertex, &m_d3dScreenQuadIndex);

	return true;
}

void CRenderer::BuildScreenQuad(ID3D11Buffer** _ppVertex, ID3D11Buffer** _ppIndex)
{
	std::vector<VertexPositionTexture> vertices;
	std::vector<int> indices;

	vertices.push_back(VertexPositionTexture(DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)));
	vertices.push_back(VertexPositionTexture(DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)));
	vertices.push_back(VertexPositionTexture(DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)));
	vertices.push_back(VertexPositionTexture(DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)));

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(1);

	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	CAssetManager::CreateBuffer(m_d3dDevice, vertices, D3D11_BIND_VERTEX_BUFFER, _ppVertex);
	CAssetManager::CreateBuffer(m_d3dDevice, indices, D3D11_BIND_INDEX_BUFFER, _ppIndex);
}

void CRenderer::BuildRenderTargets( )
{
	D3D11_TEXTURE2D_DESC t2dDesc;
	m_d3dBackBuffer->GetDesc(&t2dDesc);

	auto ResolutionX = t2dDesc.Width;
	auto ResolutionY = t2dDesc.Height;

	// Create render targets

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 4;
	sampleDesc.Quality = 0;

	// Create the render targets for our optimized G-Buffer
	tTexture2dConfig RTConfig;
	RTConfig.SetColorBuffer(ResolutionX, ResolutionY);
	RTConfig.m_State.BindFlags = (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
	RTConfig.m_State.SampleDesc = (sampleDesc);

	// For the G-Buffer we'll use 4-component floating point format for
	// spheremap-encoded normals, specular albedo, and the coverage mask
	RTConfig.m_State.Format = (DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_GBufferTarget = CreateTexture2D(&RTConfig, NULL);

	// For the light buffer we'll use a 4-component floating point format
	// for storing diffuse RGB + mono specular
	RTConfig.m_State.Format = (DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_LightTarget = CreateTexture2D(&RTConfig, NULL);

	// We need one last render target for the final image
	RTConfig.m_State.Format = (DXGI_FORMAT_R10G10B10A2_UNORM);
	m_FinalTarget = CreateTexture2D(&RTConfig, NULL);

	// Next we create a depth buffer for depth/stencil testing. Typeless formats let us
	// write depth with one format, and later interpret that depth as a color value using
	// a shader resource view.
	tTexture2dConfig DepthTexConfig;
	DepthTexConfig.SetDepthBuffer(ResolutionX, ResolutionY);
	DepthTexConfig.m_State.Format = (DXGI_FORMAT_R24G8_TYPELESS);
	DepthTexConfig.m_State.BindFlags = (D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE);
	DepthTexConfig.m_State.SampleDesc = (sampleDesc);

	tDepthStencilViewConfig DepthDSVConfig;
	D3D11_TEX2D_DSV DSVTex2D;
	DSVTex2D.MipSlice = 0;
	DepthDSVConfig.m_State.Texture2D = (DSVTex2D);
	DepthDSVConfig.m_State.Format = (DXGI_FORMAT_D24_UNORM_S8_UINT);
	DepthDSVConfig.m_State.ViewDimension = (D3D11_DSV_DIMENSION_TEXTURE2DMS);

	tShaderResourceViewConfig DepthSRVConfig;
	D3D11_TEX2D_SRV SRVTex2D;
	SRVTex2D.MipLevels = 1;
	SRVTex2D.MostDetailedMip = 0;
	DepthSRVConfig.m_State.Texture2D = (SRVTex2D);
	DepthSRVConfig.m_State.Format = (DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
	DepthSRVConfig.m_State.ViewDimension = (D3D11_SRV_DIMENSION_TEXTURE2DMS);

	m_DepthTarget = CreateTexture2D(&DepthTexConfig, NULL, &DepthSRVConfig, NULL, &DepthDSVConfig);

	// Now we need to create a depth stencil view with read-only flags set, so
	// that we can have the same buffer set as both a shader resource view and as
	// a depth stencil view
	DepthDSVConfig.m_State.Flags = (D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL);

	m_ReadOnlyDepthTarget = new tTarget( );

	ID3D11Resource* pResource = m_DepthTarget->m_pResource;
	//m_DepthTarget->m_pDSV->GetResource(&pResource);

	auto BindFlags = DepthTexConfig.m_State.BindFlags;
	if ((BindFlags & D3D11_BIND_SHADER_RESOURCE) == D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc = &DepthSRVConfig.m_State;
		m_d3dDevice->CreateShaderResourceView(pResource, pDesc, &m_ReadOnlyDepthTarget->m_pSRV);
	}

	if ((BindFlags & D3D11_BIND_DEPTH_STENCIL) == D3D11_BIND_DEPTH_STENCIL)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc = &DepthDSVConfig.m_State;
		m_d3dDevice->CreateDepthStencilView(pResource, pDesc, &m_ReadOnlyDepthTarget->m_pDSV);
	}

	// Create a view port to use on the scene.  This basically selects the
	// entire floating point area of the render target.
	m_tScreenViewport.Width = static_cast<float>(ResolutionX);
	m_tScreenViewport.Height = static_cast<float>(ResolutionY);
	m_tScreenViewport.MinDepth = 0.0f;
	m_tScreenViewport.MaxDepth = 1.0f;
	m_tScreenViewport.TopLeftX = 0;
	m_tScreenViewport.TopLeftY = 0;

	m_d3dContext->RSSetViewports(1, &m_tScreenViewport);

	// Create a render target for MSAA resolve
	tTexture2dConfig resolveConfig;
	resolveConfig.SetColorBuffer(ResolutionX, ResolutionY);
	resolveConfig.m_State.Format = (DXGI_FORMAT_R10G10B10A2_UNORM);
	resolveConfig.m_State.BindFlags = (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
	m_ResolveTarget = CreateTexture2D(&resolveConfig, NULL);
}
void CRenderer::KillRenderTargets( )
{
	SafeDelete(m_GBufferTarget);
	SafeDelete(m_LightTarget);
	SafeDelete(m_FinalTarget);

	SafeDelete(m_DepthTarget);
	SafeDelete(m_ReadOnlyDepthTarget);

	SafeDelete(m_ResolveTarget);
}

void CRenderer::BuildLighting( )
{
	D3D11_BUFFER_DESC vbConfig;
	vbConfig.ByteWidth = 1;
	vbConfig.Usage = D3D11_USAGE_DEFAULT;
	vbConfig.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vbConfig.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbConfig.MiscFlags = 0;
	vbConfig.StructureByteStride = 0;

	// Create a dynamic vertex buffer full of points, where each point is a single light source
	vbConfig.BindFlags = (D3D11_BIND_VERTEX_BUFFER);
	vbConfig.ByteWidth = (MAX_LIGHTS * sizeof(LightParams));
	vbConfig.CPUAccessFlags = (D3D11_CPU_ACCESS_WRITE);
	vbConfig.Usage = (D3D11_USAGE_DYNAMIC);
	CreateBuffer(&vbConfig, &m_pVertexBuffer);

}

void CRenderer::BuildPerCameraConstantBuffers( )
{
	// The camera cbuffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(TPerCamera);
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HR(CRenderer::GetReference( ).GetDevice( )->CreateBuffer(&bd, nullptr, &m_d3dPerCameraCBuffer));

	//SetD3DName(m_d3dPerCameraCBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());
}
void CRenderer::BuildPerObjectConstantBuffers( )
{
	// per object CBuffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(TPerObject);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HR(m_d3dDevice->CreateBuffer(&bd, nullptr, &m_d3dPerObjectCBuffer));
	//SetD3DName(m_d3dPerObjectCBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	// per skinned object CBuffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(TPerSkinnedObject);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HR(m_d3dDevice->CreateBuffer(&bd, nullptr, &m_d3dPerSkinnedObjectCBuffer));
	//SetD3DName(m_d3dPerSkinnedObjectCBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());
}
void CRenderer::BuildMiscBuffers( )
{
	D3D11_BUFFER_DESC bd;

	// color cbuffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(TColor);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CreateBuffer(&bd, &m_d3dColorCBuffer);
	//HR(m_d3dDevice->CreateBuffer(&bd, nullptr, &m_d3dColorCBuffer));
	//SetD3DName(m_d3dColorCBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	// time cbuffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(TTime);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CreateBuffer(&bd, &m_d3dTimeCBuffer);
	//HR(m_d3dDevice->CreateBuffer(&bd, nullptr, &m_d3dTimeCBuffer));
	//SetD3DName(m_d3dColorCBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	// size cbuffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(TPerParticle);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CreateBuffer(&bd, &m_d3dSizeCBuffer);
	//HR(m_d3dDevice->CreateBuffer(&bd, nullptr, &m_d3dSizeCBuffer));
	//SetD3DName(m_d3dSizeCBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());
}

void CRenderer::CreateBuffer(D3D11_BUFFER_DESC* _pBD, ID3D11Buffer** _pBuffer)
{
	HR(m_d3dDevice->CreateBuffer(_pBD, nullptr, _pBuffer));
	//SetD3DName((*_pBuffer), std::string("Object Created At Line: " + std::to_string(__LINE__) + " File: " + __FILE__).c_str( ));
}

tTarget* CRenderer::CreateTexture2D(tTexture2dConfig* pConfig, D3D11_SUBRESOURCE_DATA* pData,
									tShaderResourceViewConfig* pSRVConfig,
									tRenderTargetViewConfig* pRTVConfig,
									tDepthStencilViewConfig* pDSVConfig)
{
	tTarget* pTarget = new tTarget;
	HRESULT hr = m_d3dDevice->CreateTexture2D(&pConfig->m_State, pData, &pTarget->m_pResource);

	if (pTarget->m_pResource)
	{
		auto BindFlags = pConfig->m_State.BindFlags;

		if ((BindFlags & D3D11_BIND_SHADER_RESOURCE) == D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc = pSRVConfig ? &pSRVConfig->m_State : nullptr;
			m_d3dDevice->CreateShaderResourceView((ID3D11Resource*)pTarget->m_pResource, pDesc, &pTarget->m_pSRV);
		}

		if ((BindFlags & D3D11_BIND_RENDER_TARGET) == D3D11_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC* pDesc = pRTVConfig ? &pRTVConfig->m_State : nullptr;
			m_d3dDevice->CreateRenderTargetView((ID3D11Resource*)pTarget->m_pResource, pDesc, &pTarget->m_pRTV);
		}

		if ((BindFlags & D3D11_BIND_DEPTH_STENCIL) == D3D11_BIND_DEPTH_STENCIL)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc = pDSVConfig ? &pDSVConfig->m_State : nullptr;
			m_d3dDevice->CreateDepthStencilView((ID3D11Resource*)pTarget->m_pResource, pDesc, &pTarget->m_pDSV);
		}
	}

	return pTarget;
}

void CRenderer::SetPerCameraData(CICamera* _pCamera, bool _slot2)
{
	TPerCamera perCamera;

	perCamera.gView = _pCamera->View( );
	perCamera.gProj = _pCamera->Projection( );
	//perCamera.gViewProj = _pCamera->ViewProjection( );
	perCamera.gInvProj = _pCamera->InvProjection( );
	perCamera.gClipPlanes = _pCamera->ClipPlanes( );

	D3D11_MAPPED_SUBRESOURCE edit;
	m_d3dContext->Map(m_d3dPerCameraCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
	memcpy(edit.pData, &perCamera, sizeof(TPerCamera));
	m_d3dContext->Unmap(m_d3dPerCameraCBuffer, 0);

	if (_slot2)
		m_d3dContext->VSSetConstantBuffers(TPerCamera::register_slot, 1, &m_d3dPerCameraCBuffer);
}

void CRenderer::SetPerObjectData(DirectX::XMMATRIX &mM,
								 DirectX::XMMATRIX &mMV,
								 DirectX::XMMATRIX &mMVP)
{
	TPerObject perObject;

	DirectX::XMStoreFloat4x4(&perObject.gM, mM);
	DirectX::XMStoreFloat4x4(&perObject.gMV, mMV);
	DirectX::XMStoreFloat4x4(&perObject.gMVP, mMVP);

	D3D11_MAPPED_SUBRESOURCE edit;
	m_d3dContext->Map(m_d3dPerObjectCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
	memcpy(edit.pData, &perObject, sizeof(TPerObject));
	m_d3dContext->Unmap(m_d3dPerObjectCBuffer, 0);

	m_d3dContext->VSSetConstantBuffers(TPerObject::register_slot, 1, &m_d3dPerObjectCBuffer);
}

void CRenderer::SetPerObjectData(DirectX::XMMATRIX &mSkinnedM,
								 DirectX::XMMATRIX &mSkinnedMV,
								 DirectX::XMMATRIX &mSkinnedMVP,
								 DirectX::XMFLOAT4X4 SkinnedMatrices[NUM_BONES],
								 float scaling)
{
	TPerSkinnedObject perSkinned;

	DirectX::XMStoreFloat4x4(&perSkinned.gSkinnedM, mSkinnedM);
	DirectX::XMStoreFloat4x4(&perSkinned.gSkinnedMV, mSkinnedMV);
	DirectX::XMStoreFloat4x4(&perSkinned.gSkinnedMVP, mSkinnedMVP);

	perSkinned.scaling = scaling;

	memcpy(perSkinned.gSkinMatrices, SkinnedMatrices,
		   NUM_BONES * sizeof(DirectX::XMFLOAT4X4));

	D3D11_MAPPED_SUBRESOURCE edit;
	m_d3dContext->Map(m_d3dPerSkinnedObjectCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
	memcpy(edit.pData, &perSkinned, sizeof(TPerSkinnedObject));
	m_d3dContext->Unmap(m_d3dPerSkinnedObjectCBuffer, 0);

	m_d3dContext->VSSetConstantBuffers(TPerSkinnedObject::register_slot, 1, &m_d3dPerSkinnedObjectCBuffer);
}

void CRenderer::SetParticleData(DirectX::XMFLOAT4X4 world, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj, DirectX::XMFLOAT2 size)
{
	TPerParticle perParticle;

	DirectX::XMMATRIX mWVP = DirectX::XMLoadFloat4x4(&world);
	mWVP = DirectX::XMMatrixMultiply(mWVP, DirectX::XMLoadFloat4x4(&view));
	mWVP = DirectX::XMMatrixMultiply(mWVP, DirectX::XMLoadFloat4x4(&proj));

	DirectX::XMMATRIX mInvView = DirectX::XMLoadFloat4x4(&view);
	mInvView = DirectX::XMMatrixInverse(NULL, mInvView);

	perParticle.mWorld = world;
	perParticle.gProj = proj;
	perParticle.mView = view;
	DirectX::XMStoreFloat4x4(&perParticle.gInvView, mInvView);

	perParticle.g_size.x = size.x;
	perParticle.g_size.y = size.y;

	D3D11_MAPPED_SUBRESOURCE edit;
	m_d3dContext->Map(m_d3dSizeCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
	memcpy(edit.pData, &perParticle, sizeof(TPerParticle));
	m_d3dContext->Unmap(m_d3dSizeCBuffer, 0);

	m_d3dContext->GSSetConstantBuffers(1, 1, &m_d3dSizeCBuffer);
}

void CRenderer::SetColorData(DirectX::XMFLOAT4 _XMColor)
{
	TColor color;
	color.colorMod = _XMColor;

	D3D11_MAPPED_SUBRESOURCE edit;
	m_d3dContext->Map(m_d3dColorCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
	memcpy(edit.pData, &color, sizeof(TColor));
	m_d3dContext->Unmap(m_d3dColorCBuffer, 0);

	m_d3dContext->PSSetConstantBuffers(TColor::register_slot, 1, &m_d3dColorCBuffer);
}

void CRenderer::SetTimeData(DirectX::XMFLOAT2 _totalTime)
{
	TTime time;
	time.Translate = { _totalTime.x, _totalTime.y };

	D3D11_MAPPED_SUBRESOURCE edit;
	m_d3dContext->Map(m_d3dTimeCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
	memcpy(edit.pData, &time, sizeof(TTime));
	m_d3dContext->Unmap(m_d3dTimeCBuffer, 0);

	m_d3dContext->VSSetConstantBuffers(TTime::register_slot, 1, &m_d3dTimeCBuffer);
}

void CRenderer::Render(const CScene* pScene,
					   CAssetManager* pAssetManager)
{
	m_d3dSwapChain->GetFullscreenState(&m_bFullScreen, NULL);

	// gamma correction -- MSDN says to do it every time
	if (m_bFullScreen)
	{
		// get the contained output from the swap chain
		CComPtr<IDXGIOutput> pDXGIOutput;
		m_d3dSwapChain->GetContainingOutput(&pDXGIOutput);

		// get the control capabilities
		DXGI_GAMMA_CONTROL_CAPABILITIES DXGI_GCC;
		pDXGIOutput->GetGammaControlCapabilities(&DXGI_GCC);

		// create a DXGI_GAMMA_CONTROL structure
		DXGI_GAMMA_CONTROL DXGI_GC;
		DXGI_GC.Scale = { 1.0f, 1.0f, 1.0f };
		DXGI_GC.Offset = { 0, 0, 0 };

		for (size_t i = 0; i < DXGI_GCC.NumGammaControlPoints; i++)
		{
			float rgb = min(1.0f, i * (m_fGamma) / (DXGI_GCC.NumGammaControlPoints - 1));
			DXGI_GC.GammaCurve[i] = { rgb, rgb, rgb };
		}

		pDXGIOutput->SetGammaControl(&DXGI_GC);
	}

	if (!m_pAssetManager)
	{
		m_pAssetManager = pAssetManager;
	}

	//CICamera* pCamera = CScene::GetCamera( );
	//
	////the view-projection matrix is always the same.
	////	since it occupies the same register slot, i can
	////	set it and forget it.
	//SetPerCameraData(pCamera);
	//
	//if (pScene->GetSkybox( ))
	//	pScene->GetSkybox( )->Render( );

	//if (pScene->obj.size( ))
	//{
	//	ProcessGeometry(pScene);
	//	ProcessLighting(pScene);
	//	ProcessMaterials(pScene);
	//}

	ProcessGeometry(pScene);
	ProcessLighting(pScene);
	ProcessMaterials(pScene);

	ProcessTransparency(pScene->GetRenderable(CRObject::RObjectType::ROBJECT_TRANSPARENT));

	BeginEvent(L"View Final");

	//by now deferred lighting is done

	//wipe the state to reset stuff
	m_d3dContext->ClearState( );
	CRenderStateManager::GetReference( ).ClearState( );

	float clearColours[ ] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_d3dContext->ClearRenderTargetView(m_d3dRenderTargetView, DirectX::Colors::MediumPurple);

	{
		//input layout
		{
			InputLayoutManager::GetReference( ).ApplyState(InputLayoutManager::VertexFormat::eVERTEX_POSTEX);
		}

		//index buffer
		{
			m_d3dContext->IASetIndexBuffer(m_d3dScreenQuadIndex, DXGI_FORMAT_R32_UINT, 0);
		}

		//vertex buffer
		{
			UINT stride = sizeof(VertexPositionTexture), offset = 0;
			m_d3dContext->IASetVertexBuffers(0, 1, &m_d3dScreenQuadVertex, &stride, &offset);
		}

		//topology
		{
			CPrimitiveTopologyManager::GetReference( ).ApplyTopology(CPrimitiveTopologyManager::TTopology::TOPOLOGY_TRIANGLELIST);
		}

		//vertex shader
		{
			CShaderManager::GetReference( ).ApplyVertexShader(CShaderManager::VertexShader::VS_Screen);
		}

		//rasterizer
		{
			RasterizerStateManager::GetReference( ).ApplyState(
				RasterizerStateManager::RasterStates::RS_NoCull_MSAA);
		}

		//viewport
		{
			m_d3dContext->RSSetViewports(1, &m_tScreenViewport);
		}

		m_d3dContext->ResolveSubresource(m_ResolveTarget->m_pResource, 0, m_FinalTarget->m_pResource, 0, DXGI_FORMAT_R10G10B10A2_UNORM);

		//PSSRV
		{
			m_d3dContext->PSSetShaderResources(0, 1, &m_ResolveTarget->m_pSRV);
		}

		//PSSS
		{
			CSamplerStateManager::GetReference( ).ApplyState(CSamplerStateManager::SamplerState::SS_WRAP);
		}

		//pixel shader
		{
			CShaderManager::GetReference( ).ApplyPixelShader(CShaderManager::PixelShader::PS_TEXTURE);
		}

		//RTV/DSV
		{
			ID3D11DepthStencilView* null = NULL;
			m_d3dContext->OMSetRenderTargets(1, &m_d3dRenderTargetView, null);
		}

		//DSS
		{
			DepthStencilStateManager::GetReference( ).ApplyState(
				DepthStencilStateManager::DSStates::DSS_Deferred_Lighting_Final_Combine);
		}

		SetColorData(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		m_d3dContext->DrawIndexed(6, 0, 0);
	}

	EndEvent( );

	//Present();
}

void CRenderer::ProcessGeometry(const CScene* _pScene)
{
	BeginEvent(L"GBuffer");

	//wipe the state to reset stuff
	m_d3dContext->ClearState( );
	CRenderStateManager::GetReference( ).ClearState( );

	// Clear the G-Buffer targets
	float clearColours[ ] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_d3dContext->ClearRenderTargetView(m_GBufferTarget->m_pRTV, clearColours);
	m_d3dContext->ClearDepthStencilView(m_DepthTarget->m_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	for (int i = 0; i < 2; i++)
	{
		std::vector<CRObject*> vRenderables = _pScene->GetRenderable((CRObject::RObjectType)i);

		if (!vRenderables.size( ))
		{
			continue;
		}

		// Run through the graph and render each of the entities
		for (auto& CRObject : vRenderables)
		{
			CAssetManager::TModel* pModel = m_pAssetManager->GetModelByTag(CRObject->GetTag( ));
			CAssetManager::TAnimation* pAnimation = NULL;

			if (NULL == pModel)
			{
				//DebugPrint(std::string(CRObject->GetTag( ) + ": Model is NULL\n").c_str( ), ConsoleColor::Green);
				//DebugPrint(std::string(".....Looking For Animation.....\n").c_str( ), ConsoleColor::Green);

				pAnimation = m_pAssetManager->GetAnimByTag(CRObject->GetTag( ));
				if (NULL == pAnimation)
				{
					//DebugPrint(std::string(CRObject->GetTag( ) + ": Animation is -also- NULL...\n").c_str( ), ConsoleColor::Green);
					continue;
				}

				pModel = pAnimation->m_pModel;
			}

			if (NULL == pModel->m_tTex.m_pNormalTexture)
			{
				CAssetManager::LoadTexture(std::wstring(std::wstring(L"Assets/Textures/") + std::wstring(L"blank_normal.png")).c_str( ),
										   &pModel->m_tTex.m_pNormalTexture);

				continue;
			}

			//we know we can process geometry [and normal map] at this point

			//input layout
			//int type = CRObject->GetRObjectType( );
			switch (CRObject->GetRObjectType( ))
			{
				case CRObject::RObjectType::ROBJECT_STATIC:
				{
					InputLayoutManager::GetReference( ).ApplyState(InputLayoutManager::VertexFormat::eVERTEX_POSNORMTEXTANBIN);
				}
				break;

				case CRObject::RObjectType::ROBJECT_ANIMATED:
				{
					if (NULL == pAnimation)
					{
						InputLayoutManager::GetReference( ).ApplyState(InputLayoutManager::VertexFormat::eVERTEX_POSNORMTEXTANBIN);
						break;
					}
					InputLayoutManager::GetReference( ).ApplyState(InputLayoutManager::VertexFormat::eVERTEX_POSNORMTEXTANBINSKINNED);
				}
				break;
			}

			//index buffer
			{
				if (!pModel->m_pIndexBuffer)
				{
					DebugPrint(std::string(CRObject->GetTag( ) + "No Index Buffer Bound!\n").c_str( ), ConsoleColor::DarkBlue);
					continue;
				}

				m_d3dContext->IASetIndexBuffer(pModel->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			}

			//vertex buffer
			//int type = CRObject->GetRObjectType( );
			switch (CRObject->GetRObjectType( ))
			{
				case CRObject::RObjectType::ROBJECT_STATIC:
				{
					UINT stride = sizeof(VertexPositionNormalTextureTangentBiNormal) + 4, offset = 0;
					m_d3dContext->IASetVertexBuffers(0, 1, &pModel->m_pVertexBuffer, &stride, &offset);
				}
				break;

				case CRObject::RObjectType::ROBJECT_ANIMATED:
				{
					if (NULL == pAnimation)
					{
						UINT stride = sizeof(VertexPositionNormalTextureTangentBiNormal) + 4, offset = 0;
						m_d3dContext->IASetVertexBuffers(0, 1, &pModel->m_pVertexBuffer, &stride, &offset);
						break;
					}
					UINT stride = sizeof(VertexPositionNormalTextureTangentBiNormalSkinned), offset = 0;
					m_d3dContext->IASetVertexBuffers(0, 1, &pModel->m_pVertexBuffer, &stride, &offset);
				}
				break;
			}

			//topology
			{
				CPrimitiveTopologyManager::GetReference( ).ApplyTopology(CPrimitiveTopologyManager::TTopology::TOPOLOGY_TRIANGLELIST);
			}

			//vs cbuffer
			{
				CICamera* pCamera = CScene::GetCamera( );

				DirectX::XMMATRIX m, mv, mvp;
				m = DirectX::XMLoadFloat4x4(&CRObject->GetModelWorld( ));
				mv = m * DirectX::XMLoadFloat4x4(&CScene::GetCamera( )->View( ));
				mvp = mv * DirectX::XMLoadFloat4x4(&CScene::GetCamera( )->Projection( ));

				//int type = CRObject->GetRObjectType( );
				switch (CRObject->GetRObjectType( ))
				{
					case CRObject::RObjectType::ROBJECT_STATIC:
					{
						SetPerObjectData(m, mv, mvp);
					}
					break;

					case CRObject::RObjectType::ROBJECT_ANIMATED:
					{
						if (NULL == pAnimation)
						{
							SetPerObjectData(m, mv, mvp);
							break;
						}
						CLivingEntities* pLEntity = static_cast<CLivingEntities*>(CRObject);

						DirectX::XMFLOAT4X4 mat[NUM_BONES];
						memset(mat, 0, sizeof(DirectX::XMFLOAT4X4) * NUM_BONES);
						memcpy(mat, &pLEntity->GetDeformer( ).GetSkinnedMatrices( )[0],
							   sizeof(DirectX::XMFLOAT4X4) * (pLEntity->GetDeformer( ).GetSkinnedMatrices( ).size( )));

						SetPerObjectData(m, mv, mvp, mat, pLEntity->GetScaling( ));
					}
					break;
				}
			}

			//vs
			//int type = CRObject->GetRObjectType( );
			switch (CRObject->GetRObjectType( ))
			{
				case CRObject::RObjectType::ROBJECT_STATIC:
				{
					CShaderManager::GetReference( ).ApplyVertexShader(CShaderManager::VertexShader::VS_GBUFFER);
				}
				break;

				case CRObject::RObjectType::ROBJECT_ANIMATED:
				{
					if (NULL == pAnimation)
					{
						CShaderManager::GetReference( ).ApplyVertexShader(CShaderManager::VertexShader::VS_GBUFFER);
						break;
					}
					CShaderManager::GetReference( ).ApplyVertexShader(CShaderManager::VertexShader::VS_GBUFFER_SKINNING);
				}
				break;
			}

			//rasterizer
			{
				RasterizerStateManager::GetReference( ).ApplyState(
					RasterizerStateManager::RasterStates::RS_Default_MSAA_CCW);
			}

			//viewport
			{
				m_d3dContext->RSSetViewports(1, &m_tScreenViewport);
			}

			//PSSRV
			{
				m_d3dContext->PSSetShaderResources(0, 1, &pModel->m_tTex.m_pNormalTexture.p);
			}

			//PSSS
			{
				CSamplerStateManager::GetReference( ).ApplyState(CSamplerStateManager::SamplerState::SS_ANISOTROPIC);
			}

			//PS
			{
				CShaderManager::GetReference( ).ApplyPixelShader(CShaderManager::PixelShader::PS_GBUFFER);
			}

			//RTV/DSV
			{
				m_d3dContext->OMSetRenderTargets(1, &m_GBufferTarget->m_pRTV, m_DepthTarget->m_pDSV);
			}

			//BS
			{
				BlendStateManager::GetReference( ).ApplyState(BlendStateManager::BStates::BS_Default);
			}

			//DSS
			{
				DepthStencilStateManager::GetReference( ).ApplyState(DepthStencilStateManager::DSStates::DSS_Deferred_Lighting_GBuffer);
			}

			m_d3dContext->DrawIndexed(pModel->m_unIndexCount, 0, 0);
		}
	}

	//#pragma region Generate Mask
	//	// Now that we've filled the G-Buffer, we'll generate a stencil mask
	//	// that masks out all pixels where the individual sub-samples aren't
	//	// identical due to a triangle edge lying on that pixel. At this point the
	//	// stencil buffer already has a value of 1 for all pixels where geometry was
	//	// rendered, and this pass will increment that value for all edge pixels
	//	// leaving a value of 2.
	//
	//	//wipe the state to reset stuff
	//	m_d3dContext->ClearState( );
	//	CRenderStateManager::GetReference( ).ClearState( );
	//
	//	m_d3dContext->ClearRenderTargetView(m_d3dRenderTargetView, DirectX::Colors::MediumPurple);
	//
	//	{
	//		//input layout
	//		{
	//			InputLayoutManager::GetReference( ).ApplyState(InputLayoutManager::VertexFormat::eVERTEX_POSTEX);
	//		}
	//
	//		//index buffer
	//		{
	//			m_d3dContext->IASetIndexBuffer(m_d3dScreenQuadIndex, DXGI_FORMAT_R32_UINT, 0);
	//		}
	//
	//		//vertex buffer
	//		{
	//			UINT stride = sizeof(VertexPositionTexture), offset = 0;
	//			m_d3dContext->IASetVertexBuffers(0, 1, &m_d3dScreenQuadVertex, &stride, &offset);
	//		}
	//
	//		//topology
	//		{
	//			CPrimitiveTopologyManager::GetReference( ).ApplyTopology(CPrimitiveTopologyManager::TTopology::TOPOLOGY_TRIANGLELIST);
	//		}
	//
	//		//vertex shader
	//		{
	//			CShaderManager::GetReference( ).ApplyVertexShader(CShaderManager::VertexShader::VS_MASK);
	//		}
	//
	//		//rasterizer
	//		{
	//			RasterizerStateManager::GetReference( ).ApplyState(
	//				RasterizerStateManager::RasterStates::RS_NoCull_MSAA);
	//		}
	//
	//		//viewport
	//		{
	//			m_d3dContext->RSSetViewports(1, &m_tScreenViewport);
	//		}
	//
	//		//PSSRV
	//		{
	//			m_d3dContext->PSSetShaderResources(0, 1, &m_GBufferTarget->m_pSRV);
	//		}
	//
	//		//pixel shader
	//		{
	//			CShaderManager::GetReference( ).ApplyPixelShader(CShaderManager::PixelShader::PS_MASK);
	//		}
	//
	//		//RTV/DSV
	//		{
	//			ID3D11RenderTargetView* null = NULL;
	//			m_d3dContext->OMSetRenderTargets(1, &null, m_DepthTarget->m_pDSV);
	//		}
	//
	//		//DSS
	//		{
	//			DepthStencilStateManager::GetReference( ).ApplyState(
	//				DepthStencilStateManager::DSStates::DSS_Deferred_Lighting_Mask);
	//		}
	//
	//		SetColorData(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	//
	//		m_d3dContext->DrawIndexed(6, 0, 0);
	//	}
	//#pragma endregion

	EndEvent( );
}
void CRenderer::ProcessLighting(const CScene* _pScene)
{
	BeginEvent(L"Lighting");

	//wipe the state to reset stuff
	m_d3dContext->ClearState( );
	CRenderStateManager::GetReference( ).ClearState( );

	// Clear the render target
	float clearColours[ ] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_d3dContext->ClearRenderTargetView(m_LightTarget->m_pRTV, clearColours);

	//Draw
	for (size_t i = 0; i < 3; i++)
	{
		std::vector<const LightParams> lights = const_cast<CScene*>(_pScene)->GetLightByType(i);

		if (lights.size( ))
		{
			//input layout
			{
				InputLayoutManager::GetReference( ).ApplyState(InputLayoutManager::VertexFormat::eVERTEX_LIGHT);
			}

			//un-bind an index buffer
			{
				ID3D11Buffer* null = NULL;
				m_d3dContext->IASetIndexBuffer(null, DXGI_FORMAT_R32_UINT, NULL);
			}

			//bind the vertex buffer
			{
				UINT stride = sizeof(LightParams), offset = 0;
				//unmap the vertex buffer
				D3D11_MAPPED_SUBRESOURCE edit;
				m_d3dContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
				memcpy(edit.pData, &lights[0], sizeof(LightParams) * lights.size( ));
				m_d3dContext->Unmap(m_pVertexBuffer, 0);

				m_d3dContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
			}

			//primitive topology [Geometry Shader]
			{
				CPrimitiveTopologyManager::GetReference( ).ApplyTopology(CPrimitiveTopologyManager::TTopology::TOPOLOGY_POINTLIST);
			}

			//vertex shader constant buffer
			{
				CICamera* pCamera = CScene::GetCamera( );
				SetPerCameraData(pCamera);
				m_d3dContext->VSSetConstantBuffers(0/*TPerCamera::register_slot*/, 1, &m_d3dPerCameraCBuffer);
			}

			//vertex shader
			{
				int vShader = CShaderManager::VertexShader::VS_POINT_LIGHTING + i;
				CShaderManager::GetReference( ).ApplyVertexShader((CShaderManager::VertexShader)vShader);
			}

			//geometry shader constant buffer
			{
				CICamera* pCamera = CScene::GetCamera( );
				SetPerCameraData(pCamera);
				m_d3dContext->GSSetConstantBuffers(0/*TPerCamera::register_slot*/, 1, &m_d3dPerCameraCBuffer);
			}

			//geometry shader
			{
				int gShader = CShaderManager::GeometryShader::GS_POINT_LIGHTING + i;
				CShaderManager::GetReference( ).ApplyGeometryShader((CShaderManager::GeometryShader)gShader);
			}

			//rasterizer state
			{
				RasterizerStateManager::GetReference( ).ApplyState(
					RasterizerStateManager::RasterStates::RS_Default_MSAA_CW);
			}

			//viewport
			{
				m_d3dContext->RSSetViewports(1, &m_tScreenViewport);
			}

			//pixel shader constant buffer
			{
				CICamera* pCamera = CScene::GetCamera( );
				SetPerCameraData(pCamera);
				m_d3dContext->PSSetConstantBuffers(0/*TPerCamera::register_slot*/, 1, &m_d3dPerCameraCBuffer);
			}

			//shader resource views
			{
				m_d3dContext->PSSetShaderResources(0, 1, &m_GBufferTarget->m_pSRV);
				m_d3dContext->PSSetShaderResources(1, 1, &m_DepthTarget->m_pSRV);
			}

			//pixel shader
			{
				int pShader = CShaderManager::PixelShader::PS_POINT_LIGHTING + i;
				CShaderManager::GetReference( ).ApplyPixelShader((CShaderManager::PixelShader)pShader);
			}

			//render target/depth stencil view
			{
				m_d3dContext->OMSetRenderTargets(1, &m_LightTarget->m_pRTV, m_ReadOnlyDepthTarget->m_pDSV);
			}

			//blend state
			{
				BlendStateManager::GetReference( ).ApplyState(BlendStateManager::BStates::BS_Lighting_Alpha);
			}

			//depth stencil state
			{
				DepthStencilStateManager::GetReference( ).ApplyState(DepthStencilStateManager::DSStates::DSS_Deferred_Lighting_GreaterThanEqual_1);
			}

			if (i == LightType::Directional)
			{
				DepthStencilStateManager::GetReference( ).ApplyState(DepthStencilStateManager::DSStates::DSS_Deferred_Lighting_Disabled_1);
			}

			m_d3dContext->Draw(lights.size( ), NULL);

			////depth stencil state
			//{
			//	DepthStencilStateManager::GetReference( ).ApplyState(DepthStencilStateManager::DSStates::DSS_Deferred_Lighting_GreaterThanEqual_2);
			//}
			//
			//if (i == LightType::Directional)
			//{
			//	DepthStencilStateManager::GetReference( ).ApplyState(DepthStencilStateManager::DSStates::DSS_Deferred_Lighting_Disabled_2);
			//}
			//
			//m_d3dContext->Draw(lights.size( ), NULL);
		}
	}

	EndEvent( );
}
void CRenderer::ProcessMaterials(const CScene* _pScene)
{
	BeginEvent(L"Final");

	//wipe the state to reset stuff
	m_d3dContext->ClearState( );
	CRenderStateManager::GetReference( ).ClearState( );

	float clearColours[ ] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_d3dContext->ClearRenderTargetView(m_FinalTarget->m_pRTV, clearColours);

	for (int i = 0; i < 2; i++)
	{
		std::vector<CRObject*> vRenderables = _pScene->GetRenderable((CRObject::RObjectType)i);

		if (!vRenderables.size( ))
		{
			continue;
		}

		// Run through the graph and render each of the entities
		for (auto& CRObject : vRenderables)
		{
			CAssetManager::TModel* pModel = m_pAssetManager->GetModelByTag(CRObject->GetTag( ));
			CAssetManager::TAnimation* pAnimation = NULL;

			if (NULL == pModel)
			{
				pAnimation = m_pAssetManager->GetAnimByTag(CRObject->GetTag( ));
				if (NULL == pAnimation)
				{
					continue;
				}

				pModel = pAnimation->m_pModel;
			}

			//input layout
			switch (CRObject->GetRObjectType( ))
			{
				case CRObject::RObjectType::ROBJECT_STATIC:
				{
					InputLayoutManager::GetReference( ).ApplyState(InputLayoutManager::VertexFormat::eVERTEX_POSNORMTEXTANBIN);
				}
				break;

				case CRObject::RObjectType::ROBJECT_ANIMATED:
				{
					if (NULL == pAnimation)
					{
						InputLayoutManager::GetReference( ).ApplyState(InputLayoutManager::VertexFormat::eVERTEX_POSNORMTEXTANBIN);
						break;
					}
					InputLayoutManager::GetReference( ).ApplyState(InputLayoutManager::VertexFormat::eVERTEX_POSNORMTEXTANBINSKINNED);
				}
				break;
			}

			//index buffer
			{
				if (!pModel->m_pIndexBuffer)
				{
					DebugPrint(std::string(CRObject->GetTag( ) + "No Index Buffer Bound!\n").c_str( ), ConsoleColor::DarkBlue);
					continue;
				}

				m_d3dContext->IASetIndexBuffer(pModel->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			}

			//vertex buffer
			switch (CRObject->GetRObjectType( ))
			{
				case CRObject::RObjectType::ROBJECT_STATIC:
				{
					UINT stride = sizeof(VertexPositionNormalTextureTangentBiNormal) + 4, offset = 0;
					m_d3dContext->IASetVertexBuffers(0, 1, &pModel->m_pVertexBuffer, &stride, &offset);
				}
				break;

				case CRObject::RObjectType::ROBJECT_ANIMATED:
				{
					if (NULL == pAnimation)
					{
						UINT stride = sizeof(VertexPositionNormalTextureTangentBiNormal) + 4, offset = 0;
						m_d3dContext->IASetVertexBuffers(0, 1, &pModel->m_pVertexBuffer, &stride, &offset);
						break;
					}
					UINT stride = sizeof(VertexPositionNormalTextureTangentBiNormalSkinned), offset = 0;
					m_d3dContext->IASetVertexBuffers(0, 1, &pModel->m_pVertexBuffer, &stride, &offset);
				}
				break;
			}

			//topology
			{
				CPrimitiveTopologyManager::GetReference( ).ApplyTopology(CPrimitiveTopologyManager::TTopology::TOPOLOGY_TRIANGLELIST);
			}

			//vs cbuffer
			for (int i_hate_fucking_final_project = 0; i_hate_fucking_final_project < 2; i_hate_fucking_final_project++)
			{
				// we don't need to/i don't fucking want to outline static shit
				//	so fuck static shit. easy >= pie
				if (i == CRObject::RObjectType::ROBJECT_STATIC && !i_hate_fucking_final_project)
					continue;

				float scaling = 1.01f;

				CICamera* pCamera = CScene::GetCamera( );

				DirectX::XMMATRIX m, mv, mvp;

				m = DirectX::XMLoadFloat4x4(&CRObject->GetModelWorld( ));
				mv = m * DirectX::XMLoadFloat4x4(&CScene::GetCamera( )->View( ));
				mvp = mv * DirectX::XMLoadFloat4x4(&CScene::GetCamera( )->Projection( ));

				switch (CRObject->GetRObjectType( ))
				{
					case CRObject::RObjectType::ROBJECT_STATIC:
					{
						SetPerObjectData(m, mv, mvp);
					}
					break;

					case CRObject::RObjectType::ROBJECT_ANIMATED:
					{
						if (NULL == pAnimation)
						{
							SetPerObjectData(m, mv, mvp);
							break;
						}
						CLivingEntities* pLEntity = static_cast<CLivingEntities*>(CRObject);

						DirectX::XMFLOAT4X4 mat[NUM_BONES];
						memset(mat, 0, sizeof(DirectX::XMFLOAT4X4) * NUM_BONES);
						memcpy(mat, &pLEntity->GetDeformer( ).GetSkinnedMatrices( )[0],
							   sizeof(DirectX::XMFLOAT4X4) * (pLEntity->GetDeformer( ).GetSkinnedMatrices( ).size( )));

						float e_scale = 1.0f * pLEntity->GetScaling( );
						switch (i_hate_fucking_final_project)
						{
							case 0:
								e_scale *= scaling;
								break;
						}
						SetPerObjectData(m, mv, mvp, mat, e_scale);
					}
					break;
				}

				//vs
				switch (CRObject->GetRObjectType( ))
				{
					case CRObject::RObjectType::ROBJECT_STATIC:
					{
						CShaderManager::GetReference( ).ApplyVertexShader(CShaderManager::VertexShader::VS_FINAL);
					}
					break;

					case CRObject::RObjectType::ROBJECT_ANIMATED:
					{
						if (NULL == pAnimation)
						{
							CShaderManager::GetReference( ).ApplyVertexShader(CShaderManager::VertexShader::VS_FINAL);
							break;
						}
						CShaderManager::GetReference( ).ApplyVertexShader(CShaderManager::VertexShader::VS_FINAL_SKINNING);
					}
					break;
				}

				//rasterizer

				//TODO: switch-statement for rasterizer
				switch (i_hate_fucking_final_project)
				{
					case 0:
						RasterizerStateManager::GetReference( ).ApplyState(
							RasterizerStateManager::RasterStates::RS_Default_MSAA_CW);
						break;
					case 1:
						RasterizerStateManager::GetReference( ).ApplyState(
							RasterizerStateManager::RasterStates::RS_Default_MSAA_CCW);
						break;
				}

				//viewport
				{
					m_d3dContext->RSSetViewports(1, &m_tScreenViewport);
				}

				//shader resource view
				{
					m_d3dContext->PSSetShaderResources(0, 1, &pModel->m_tTex.m_pDiffuseTexture[0].p);
					m_d3dContext->PSSetShaderResources(1, 1, &m_LightTarget->m_pSRV);
				}

				DirectX::XMFLOAT4 c = { 1, 1, 1, 1 };
				switch (i_hate_fucking_final_project)
				{
					case 0:
					{
						//outline color
						//	technically of course i already know
						//		this is gonna be an animated nigga
						// also, LOLHAX
						CBaseEnemy* pLEntity = dynamic_cast<CBaseEnemy*>(CRObject);
						if (pLEntity)
						{
							::CRObject* pI = pLEntity->GetVariantIndicator( );
							if (pI)
							{
								c = pI->GetColor( );
								c.x /= 2.0f;								c.y /= 2.0f;								c.z /= 2.0f;

							}
						}
					}
					break;

					case 1:
					{
						c = CRObject->GetColor( );
					}
					break;
				}
				SetColorData(c);

				//PSSS
				{
					CSamplerStateManager::GetReference( ).ApplyState(CSamplerStateManager::SamplerState::SS_ANISOTROPIC);
				}

				//PS

				//TODO: switch-statement for single-color
				switch (i_hate_fucking_final_project)
				{
					case 0:
						CShaderManager::GetReference( ).ApplyPixelShader(CShaderManager::PixelShader::PS_DEBUG_LINE);
						break;
					case 1:
						CShaderManager::GetReference( ).ApplyPixelShader(CShaderManager::PixelShader::PS_FINAL);
						break;
				}

				//render target/depth stencil view
				{
					m_d3dContext->OMSetRenderTargets(1, &m_FinalTarget->m_pRTV, m_ReadOnlyDepthTarget->m_pDSV);
				}

				//BS
				{
					BlendStateManager::GetReference( ).ApplyState(BlendStateManager::BStates::BS_Default);
				}

				//DSS
				{
					DepthStencilStateManager::GetReference( ).ApplyState(DepthStencilStateManager::DSStates::DSS_Deferred_Lighting_Final);
				}

				m_d3dContext->DrawIndexed(pModel->m_unIndexCount, 0, 0);
			}
		}
	}
	EndEvent( );

	BeginEvent(L"Skybox");
	if (CScene::GetSkybox( ))
	{
		CScene::GetSkybox( )->Render( );
	}
	EndEvent( );

	BeginEvent(L"Particles");
	CParticleManager::GetInstance( )->Render( );
	EndEvent( );

	CDebugRenderer::GetReference( ).Execute( );

}

void CRenderer::ProcessTransparency(const std::vector<CRObject*>& _vRenderables)
{
	using namespace MathHelper;

	BeginEvent(L"Transparency");

	//m_d3dContext->OMSetRenderTargets(1, &m_d3dRenderTargetView, m_d3dDepthStencilView);

	UINT stride = sizeof(VertexPositionNormalTextureTangentBiNormal) + 4;
	UINT offset = 0;

	for (auto& renderable : _vRenderables)
	{
		CAssetManager::TModel* modelT = m_pAssetManager->GetModelByTag(renderable->GetTag( ));

		if (!modelT)
		{
			continue;
		}

		InputLayoutManager::GetReference( ).ApplyState(InputLayoutManager::VertexFormat::eVERTEX_POSNORMTEXTANBIN);

		m_d3dContext->IASetVertexBuffers(0, 1, &modelT->m_pVertexBuffer, &stride, &offset);
		m_d3dContext->IASetIndexBuffer(modelT->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, NULL);

		CRenderer::GetReference( ).SetPerObjectData(DirectX::XMLoadFloat4x4(&renderable->GetModelWorld( )),
													DirectX::XMLoadFloat4x4(&renderable->GetModelWorld( )) * DirectX::XMLoadFloat4x4(&CScene::GetCamera( )->View( )),
													DirectX::XMLoadFloat4x4(&renderable->GetModelWorld( )) * DirectX::XMLoadFloat4x4(&CScene::GetCamera( )->ViewProjection( )));

		CComPtr<ID3D11ShaderResourceView> SRVs[ ] = {
			modelT->m_tTex.m_pDiffuseTexture[0],
			modelT->m_tTex.m_pDiffuseTexture[1],
			modelT->m_tTex.m_pDiffuseTexture[2],
		};

		for (int i = 0; i < ARRAYSIZE(SRVs); i++)
		{
			if (SRVs[i])
			{
				m_d3dContext->PSSetShaderResources(i, 1, &SRVs[i].p);
			}
			else
			{
				CComPtr<ID3D11ShaderResourceView> null = NULL;
				m_d3dContext->PSSetShaderResources(i, 1, &null.p);
			}
		}

		DirectX::XMFLOAT3 full_color = { 30.0f, 30.0f, 30.0f };
		full_color = full_color / gAMBIENCE;

		DirectX::XMFLOAT4 color = renderable->GetColor( );
		SetColorData(DirectX::XMFLOAT4(color.x / full_color.x, color.y / full_color.y, color.z / full_color.z, color.w));

		bool noDepth = false;
		if (renderable->GetTag( ).find("Crosshair") != std::string::npos)
			noDepth = true;

		CRenderStateManager::GetReference( ).ApplyRenderState(
			CRenderStateManager::eRenderState(CRenderStateManager::eRenderState::BACKFACE_TRANSPARENT));

		if (noDepth)
			DepthStencilStateManager::GetReference( ).ApplyState(DepthStencilStateManager::DSStates::DSS_NoDepth);

		m_d3dContext->DrawIndexed(modelT->m_unIndexCount, 0, 0);

		CRenderStateManager::GetReference( ).ApplyRenderState(
			CRenderStateManager::eRenderState(CRenderStateManager::eRenderState::FRONTFACE_TRANSPARENT));

		if (noDepth)
			DepthStencilStateManager::GetReference( ).ApplyState(DepthStencilStateManager::DSStates::DSS_NoDepth);

		m_d3dContext->DrawIndexed(modelT->m_unIndexCount, 0, 0);

		ID3D11ShaderResourceView* null[ ] = { NULL, NULL, NULL, };
		m_d3dContext->PSSetShaderResources(0, _countof(null), null);
	}

	EndEvent( );
}

void CRenderer::ResizeBuffers(UINT width,
							  UINT height)
{
	if (width == 0 || height == 0)
	{
		//DebugPrint("Jesus Sweet Tits Christ\n", ConsoleColor::DarkRed);
		return;
	}

	//std::cout << "Resizing Window...\n";

	m_unWidth = width;
	m_unHeight = height;

	//Now this method is the Meat and Potatoes of the initialization.
	//	Since a [WM_SIZE] message is sent upon window creation, we can call
	//	this function almost immediately to finish initializing DirectX11.
	//		Mainly, this function will be initializing a Render Target View,
	//		the Depth Stencil Buffer & View, and the Back Buffer.
	//		It will also create the Swap Chain [if it doesn't already exist]

	//so first we have to clear the previous size-specific stuff [reference to the swap chain]
	ID3D11RenderTargetView* null[ ] = { nullptr, };
	m_d3dContext->OMSetRenderTargets(_countof(null), null, nullptr);

	SafeRelease(m_d3dBackBuffer);
	SafeRelease(m_d3dDepthStencilView);
	SafeRelease(m_d3dRenderTargetView);

	//clearing the context wipes the current pipeline,
	//	and flush destroys deferred destroyed stuff [shouldn't actually need to do the Flush]
	m_d3dContext->ClearState( );
	m_d3dContext->Flush( );

	CRenderStateManager::GetReference( ).ClearState( );

	//if the Swap Chain already exists, there's no reason to make a new one
	//	calling ResizeBuffers should take care of it.
	if (m_d3dSwapChain)
	{
		KillRenderTargets( );

		HRESULT hr = m_d3dSwapChain->ResizeBuffers(0, m_unWidth, m_unHeight, DXGI_FORMAT_UNKNOWN, 0/*DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH*/);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			//if we get here, an uh-oh happened. we have to destroy it all *lol*
			//	but we can recreate new stuff so that's cool.
			return;
		}
		else
		{
			HR(hr);
		}
	}
	else
	{
		// First, retrieve the underlying DXGI Device from the D3D Device
		IDXGIDevice* pDXGIDevice;
		HR(m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice));

		// Identify the physical adapter (GPU or card) this device is running on.
		IDXGIAdapter* pDXGIAdapter;
		HR(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter));

		// And obtain the factory object that created it.
		IDXGIFactory* pIDXGIFactory;
		HR(pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory));

		// Creating just the swap chain, with the Render_Target_Output so it can be bound as an SRV
		DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
		swapChainDesc.BufferCount = 2;
		swapChainDesc.BufferDesc.Width = m_unWidth;
		swapChainDesc.BufferDesc.Height = m_unHeight;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = m_window;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = TRUE;
		//swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HR(pIDXGIFactory->CreateSwapChain(m_d3dDevice, &swapChainDesc, &m_d3dSwapChain));
		//SetD3DName(m_d3dSwapChain, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

		SafeRelease(pIDXGIFactory);
		SafeRelease(pDXGIAdapter);
		SafeRelease(pDXGIDevice);

#if TEST_MACHINE_BUILD
		m_d3dSwapChain->SetFullscreenState(TRUE, NULL);

		SafeRelease(m_d3dBackBuffer);
		SafeRelease(m_d3dDepthStencilView);
		SafeRelease(m_d3dRenderTargetView);

		KillRenderTargets( );
#endif
	}

	// so, now that the swap chain is resized or created, we can re/make everything.

	// obtain the backbuffer
	HR(m_d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_d3dBackBuffer));
	//SetD3DName(m_d3dBackBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	// create a render target view
	HR(m_d3dDevice->CreateRenderTargetView(m_d3dBackBuffer, nullptr, &m_d3dRenderTargetView));
	//SetD3DName(m_d3dRenderTargetView, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	BuildRenderTargets( );

	// create the depth stencil view
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	m_d3dSwapChain->GetDesc(&swapChainDesc);

	CD3D11_TEXTURE2D_DESC depthBufferDesc(DXGI_FORMAT_R24G8_TYPELESS, swapChainDesc.BufferDesc.Width, swapChainDesc.BufferDesc.Height);
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	depthBufferDesc.SampleDesc.Count = 4;
	depthBufferDesc.SampleDesc.Quality = 0;

	depthBufferDesc.MipLevels = 1;

	CComPtr<ID3D11Texture2D> pDepthBuffer;
	m_d3dDevice->CreateTexture2D(&depthBufferDesc, nullptr, &pDepthBuffer);
	//SetD3DName(m_d3dDepthStencilBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2DMS);
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	m_d3dDevice->CreateDepthStencilView(pDepthBuffer, &depthStencilViewDesc, &m_d3dDepthStencilView);


	// TODO: Initialize windows-size dependent objects here
}

void CRenderer::SetResolution(UINT unWidth,
							  UINT unHeight)
{
	m_unWidth = unWidth;
	m_unHeight = unHeight;

	ResizeBuffers(m_unWidth, m_unHeight);
}

void CRenderer::Shutdown( )
{
	m_d3dSwapChain->SetFullscreenState(FALSE, NULL);

	m_d3dContext->ClearState( );
	m_d3dContext->Flush( );

	SafeRelease(m_d3dDevice);
	SafeRelease(m_d3dContext);
	SafeRelease(m_d3dSwapChain);

	SafeRelease(m_d3dRenderTargetView);
	SafeRelease(m_d3dDepthStencilView);
	SafeRelease(m_d3dBackBuffer);

	SafeRelease(m_d3dPerObjectCBuffer);
	SafeRelease(m_d3dPerCameraCBuffer);
	SafeRelease(m_d3dPerSkinnedObjectCBuffer);

	SafeRelease(m_d3dColorCBuffer);
	SafeRelease(m_d3dTimeCBuffer);
	SafeRelease(m_d3dSizeCBuffer);

	SafeRelease(m_d3dScreenQuadVertex);
	SafeRelease(m_d3dScreenQuadIndex);
	SafeRelease(m_pVertexBuffer);

	SafeRelease(m_d3dUserDefinedAnnotation);

	KillRenderTargets( );

	CBitmapFont::DeleteInstance( );
	CDebugRenderer::DeleteInstance( );
	InputLayoutManager::DeleteInstance( );
	CRenderStateManager::DeleteInstance( );

#ifdef _DEBUG
	//m_d3dDebugDev->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif
}

void CRenderer::GetDefaultSize(UINT& unWidth,
							   UINT& unHeight)
{
	unWidth = 1024;
	unHeight = 768;
}

void CRenderer::Present( )
{
	HRESULT hr = m_d3dSwapChain->Present(0, 0);

	// If the device was reset we must completely reinitialize the renderer.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
	}
	else
	{
		HR(hr);
	}
}

void CRenderer::Clear( )
{
	//m_d3dContext->ClearRenderTargetView(m_d3dRenderTargetView, DirectX::Colors::MediumPurple);
	//m_d3dContext->ClearDepthStencilView(m_d3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//m_d3dContext->OMSetRenderTargets(1, &m_d3dRenderTargetView, m_d3dDepthStencilView);
	//m_d3dContext->RSSetViewports(1, &m_tScreenViewport);
}

void CRenderer::SetFullscreenState(BOOL _bFullscreen, float _fGamma)
{
	m_fGamma = _fGamma;

	if (m_bFullScreen == _bFullscreen)
		return;

	m_bFullScreen = _bFullscreen;
	m_d3dSwapChain->SetFullscreenState(_bFullscreen, NULL);

	if (!m_bFullScreen)
		return;
}

CRenderer* CRenderer::instancePtr;
