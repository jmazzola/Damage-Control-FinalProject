/***********************************************
* Filename:  		Renderer.h
* Date:      		05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		Implementation for DirectX11
************************************************/

#ifndef _CRENDERER_H_
#define _CRENDERER_H_

#include "..\Core\stdafx.h"

#include "ConstantBufferStructures.h"

#if defined(DEBUG) | defined(_DEBUG)
#ifndef SetD3DName
#define SetD3DName(object, name)							\
{															\
	if(name && object)										\
		object->SetPrivateData(WKPDID_D3DDebugObjectName,	\
			(UINT)strnlen( name, 255 ), name );				\
}
#endif
#else
#ifndef SetD3DName
#define SetD3DName(object, name)
#endif
#endif

/*
switch (CRObject->GetRObjectType( ))
{
case CRObject::RObjectType::ROBJECT_STATIC:
{

}
break;

case CRObject::RObjectType::ROBJECT_ANIMATED:
{

}
break;
}
*/


class CAssetManager;

class CICamera;
class CScene;
class CRObject;

struct tTexture2dConfig;
struct tRenderTargetViewConfig;
struct tDepthStencilViewConfig;
struct tShaderResourceViewConfig;
struct tTarget;

class CRenderer
{
public:

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	~CRenderer( ) = default;

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	bool Initialize(HWND hWnd,
					UINT unWidth,
					UINT unHeight);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void Render(const CScene* pScene,
				CAssetManager* pAssetManager);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void ResizeBuffers(UINT width,
					   UINT height);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void SetResolution(UINT unWidth,
					   UINT unHeight);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void Shutdown( );

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void SetPerObjectData(DirectX::XMMATRIX &mM,
						  DirectX::XMMATRIX &mMV,
						  DirectX::XMMATRIX &mMVP);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void SetParticleData(DirectX::XMFLOAT4X4 world,
						 DirectX::XMFLOAT4X4 view,
						 DirectX::XMFLOAT4X4 proj,
						 DirectX::XMFLOAT2 size);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void SetPerCameraData(CICamera* _pCamera, bool _slot2 = false);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void SetPerObjectData(DirectX::XMMATRIX &mSkinnedM,
						  DirectX::XMMATRIX &mSkinnedMV,
						  DirectX::XMMATRIX &mSkinnedMVP,
						  DirectX::XMFLOAT4X4 SkinnedMatrices[NUM_BONES],
						  float scaling);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void SetColorData(DirectX::XMFLOAT4 _XMColor);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void SetTimeData(DirectX::XMFLOAT2 _totalTime);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void Clear( );

	void SetFullscreenState(BOOL _bFullscreen, float _fGamma);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void Present( );

	ID3D11Device* GetDevice( )
	{
		return CRenderer::GetReference( ).m_d3dDevice;
	}

	ID3D11DeviceContext* GetContext( )
	{
		return CRenderer::GetReference( ).m_d3dContext;
	}

	void GetDefaultSize(UINT& unWidth,
						UINT& unHeight);

	UINT GetWidth( )
	{
		return CRenderer::GetReference( ).m_unWidth;
	}

	UINT GetHeight( )
	{
		return CRenderer::GetReference( ).m_unHeight;
	}

	HWND GetWindow( )
	{
		return CRenderer::GetReference( ).m_window;
	}

	CD3D11_VIEWPORT GetViewport( )
	{
		return CRenderer::GetReference( ).m_tScreenViewport;
	}

	void BeginEvent(const wchar_t* _szEvent)
	{
		if (m_d3dUserDefinedAnnotation)
		{
			m_d3dUserDefinedAnnotation->BeginEvent(_szEvent);
		}
	}
	void EndEvent( )
	{
		if (m_d3dUserDefinedAnnotation)
		{
			m_d3dUserDefinedAnnotation->EndEvent( );
		}
	}

	static CRenderer &GetReference( );

	static void DeleteInstance( );

	BOOL m_bFullScreen;
private:
	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	CRenderer( ) = default;

	CRenderer(const CRenderer &);

	CRenderer &operator=(const CRenderer &)
	{
		return *this;
	}

	static CRenderer *instancePtr;

	void CreateBuffer(D3D11_BUFFER_DESC* _pBD, ID3D11Buffer** _pBuffer);

	tTarget* CreateTexture2D(tTexture2dConfig* pConfig, D3D11_SUBRESOURCE_DATA* pData,
							 tShaderResourceViewConfig* pSRVConfig = NULL,
							 tRenderTargetViewConfig* pRTVConfig = NULL,
							 tDepthStencilViewConfig* pDSVConfig = NULL);

	enum eRenderTargets
	{
		NORMALS,
		LIGHTING,
		MATERIALS,
		FINAL,
		COUNT
	};

	void BuildScreenQuad(ID3D11Buffer** _ppVertex, ID3D11Buffer** _ppIndex);

	void BuildRenderTargets( );
	void KillRenderTargets( );

	void BuildLighting( );

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void BuildPerObjectConstantBuffers( );

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void BuildPerCameraConstantBuffers( );

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void BuildMiscBuffers( );

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void ProcessGeometry(const CScene* _pScene);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void ProcessLighting(const CScene* _pScene);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void ProcessMaterials(const CScene* _pScene);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void ProcessTransparency(const std::vector<CRObject*>& _vRenderables);

	ID3D11Debug*				m_d3dDebugDev;
	ID3DUserDefinedAnnotation*	m_d3dUserDefinedAnnotation;

	D3D_FEATURE_LEVEL			m_tFeatureLevel;
	ID3D11Device*				m_d3dDevice;
	ID3D11DeviceContext*		m_d3dContext;
	IDXGISwapChain*				m_d3dSwapChain;

	ID3D11RenderTargetView*		m_d3dRenderTargetView;
	ID3D11Texture2D*			m_d3dBackBuffer;

	ID3D11DepthStencilView*		m_d3dDepthStencilView;

	CD3D11_VIEWPORT				m_tScreenViewport;

	ID3D11Buffer*				m_d3dPerObjectCBuffer;
	ID3D11Buffer*				m_d3dPerCameraCBuffer;
	ID3D11Buffer*				m_d3dPerSkinnedObjectCBuffer;
	ID3D11Buffer*				m_d3dColorCBuffer;
	ID3D11Buffer*				m_d3dTimeCBuffer;
	ID3D11Buffer*				m_d3dSizeCBuffer;

	ID3D11Buffer*				m_d3dScreenQuadVertex;
	ID3D11Buffer*				m_d3dScreenQuadIndex;

	ID3D11Buffer*				m_pVertexBuffer;

	UINT						m_unWidth;
	UINT						m_unHeight;
	HWND						m_window;

	CAssetManager*				m_pAssetManager;

	tTarget*					m_GBufferTarget;
	tTarget*					m_LightTarget;
	tTarget*					m_FinalTarget;

	tTarget*					m_DepthTarget;

	tTarget*					m_ReadOnlyDepthTarget;

	tTarget*					m_ResolveTarget;

	float m_fGamma;
};

#endif //_CRENDERER_H_
