/***********************************************
* Filename:  		RObject.h
* Date:				05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		Base Class for all Renderable
*						objects
************************************************/

#include "RObject.h"

#include "..\Core\StructureHolder.h"

#include "BlendStateManager.h"
#include "RasterizerStateManager.h"
#include "SamplerStateManager.h"
#include "VertexStructures.h"
#include "ShaderManager.h"

//this structure will hopefully provide a way to better configure
//	object-specific pipeline stuff.
//
//functionally, they're "overriders", meaning a "default" state is
//	configured, then, if necessary, these re-configure.
//
struct TRenderProperties
{
	TRenderProperties( )
	{
		//m_bInstance = false;
		//m_unStride = 0;
		//m_XMColor = { 1, 1, 1, 1 };
		//
		//m_eBlendState = EDRendererD3D::BlendStateManager::BStates::BS_Default;
		//m_eRasterizerState = EDRendererD3D::RasterizerStateManager::RasterStates::RS_CCW;
		//m_eSamplerState = CSamplerStateManager::SS_Default;
		//m_eVertexFormat = InputLayoutManager::VertexFormat::eVERTEX_MAX;
		//
		//m_ePixelShader = CShaderManager::PixelShader::PS_COUNT;
		//m_eVertexShader = CShaderManager::VertexShader::VS_COUNT;
	}

	//this keeps track of whether or not this
	//	particular object should be pushed
	//	for instancing.
	bool m_bInstance;

	//the byte-stride of the Vertex Buffer
	unsigned int m_unStride;

	//the base-color of the object [most typically white]
	DirectX::XMFLOAT4 m_XMColor;

	//I don't personally think this is necessary
	EDRendererD3D::BlendStateManager::BStates m_eBlendState;

	//we can use this to tell particular objects to change
	//	the Raster State is using [wireframe drawing]
	EDRendererD3D::RasterizerStateManager::RasterStates m_eRasterizerState;

	//this tells how this object needs to sample stuff
	CSamplerStateManager::SamplerState m_eSamplerState;

	//the vertex format [associated with the above stride]
	InputLayoutManager::VertexFormat m_eVertexFormat;

	//the vertex/pixel shaders this object needs to run through
	//	i'll do some checking to make sure the above states make sense
	//	with these [instancing mainly]
	CShaderManager::PixelShader m_ePixelShader;
	CShaderManager::VertexShader m_eVertexShader;

};

CRObject::CRObject( )
{
	m_XMColor = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixIdentity( ));

	//every object ever will start as a static object.
	//	player needs to change to animated
	//	enemies need to change to animated
	//	lighting will change in the base class
	//
	m_eRObjectType = CRObject::RObjectType::ROBJECT_STATIC;// CRObject::RObjectType::ROBJECT_COUNT;

	//m_tpRenderProperties = new TRenderProperties( );

	//null collider by default
	tCollider = NULL;
	m_XMBaseColor = DirectX::XMFLOAT4 { 1, 1, 1, 1 };
	m_pLightParams = NULL;
}

CRObject::~CRObject( )
{
	//delete m_tpRenderProperties;
	//m_tpRenderProperties = NULL;
	SafeDelete(m_pLightParams);
	SafeDelete(tCollider);
}

void CRObject::SetPosition(DirectX::XMFLOAT3& position)
{
	DirectX::XMFLOAT4X4& transform = MyWorld.Local;

	transform._41 = position.x;
	transform._42 = position.y;
	transform._43 = position.z;
}