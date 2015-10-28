/***********************************************
* Filename:  		HUDRenderer.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		This class is responsible
*						for drawing 2D
************************************************/

#include "HUDRenderer.h"

#include "RenderStateManager.h"

#include "BlendStateManager.h"
#include "DepthStencilStateManager.h"
#include "VertexStructures.h"
#include "PrimitiveTopologyManager.h"
#include "RasterizerStateManager.h"
#include "SamplerStateManager.h"
#include "ShaderManager.h"

#include "Scene.h"

#include "Renderer.h"

#include "Texture2D.h"

CHUDRenderer::CHUDRenderer( )
{
}

CHUDRenderer::~CHUDRenderer(void)
{
}

/*static*/ void CHUDRenderer::Initialize( )
{
	// Create the index array.
	unsigned int indices[ ] = { 0, 1, 2, 0, 3, 1, };

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc = { 0 };
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * _countof(indices);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA  data = { 0 };
	data.pSysMem = indices;

	HRESULT hr = CRenderer::GetReference().GetDevice( )->CreateBuffer(&indexBufferDesc, &data, &m_d3dIndexBuffer);
	HR(hr);
	//SetD3DName(m_d3dIndexBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	// Create the vertex array
	VertexPositionTexture vertices[ ] = {
		VertexPositionTexture(DirectX::XMFLOAT3(-1, 1, 1), DirectX::XMFLOAT2(0, 0)),
		VertexPositionTexture(DirectX::XMFLOAT3(1, -1, 1), DirectX::XMFLOAT2(1, 1)),
		VertexPositionTexture(DirectX::XMFLOAT3(-1, -1, 1), DirectX::XMFLOAT2(0, 1)),
		VertexPositionTexture(DirectX::XMFLOAT3(1, 1, 1), DirectX::XMFLOAT2(1, 0)),
	};

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexPositionTexture) * _countof(vertices);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;

	data = { 0 };
	data.pSysMem = vertices;

	hr = CRenderer::GetReference().GetDevice( )->CreateBuffer(&vertexBufferDesc, &data, &m_d3dVertexBuffer);
	HR(hr);
	//SetD3DName(m_tpTexture->m_d3dVertexBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	//// Set up the description of the texcoord buffer.
	//D3D11_BUFFER_DESC texCoordBufferDesc = { 0 };
	//texCoordBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//texCoordBufferDesc.ByteWidth = sizeof(TTexCoords);
	//texCoordBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//texCoordBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//
	//hr = CRenderer::GetReference().GetDevice( )->CreateBuffer(&texCoordBufferDesc, nullptr, &m_d3dTexCoordCBuffer);
	//HR(hr);
	////SetD3DName(m_tpTexture->texCoordBufferDesc, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	return;
}

/*static*/ void CHUDRenderer::Shutdown( )
{
	SafeRelease(m_d3dIndexBuffer);
	SafeRelease(m_d3dVertexBuffer);
	SafeRelease(m_d3dTexCoordCBuffer);
}

/*static*/ void CHUDRenderer::Render2D(CTexture2D* _pTexture2D)
{
	TTexture2D * pTexture = _pTexture2D->RetrieveThyTexture( );
	if (!pTexture)
	{
		return;
	}

	CRenderStateManager::GetReference( ).ApplyRenderState(
		CRenderStateManager::eRenderState::HUD);

	InputLayoutManager::GetReference( ).InputLayoutManager::ApplyState(
		pTexture->m_eFormat);

	ID3D11DeviceContext* iContext = CRenderer::GetReference().GetContext( );

	UINT stride = sizeof(VertexPositionTexture);
	UINT offset = { };

	CRenderer::GetReference().SetPerObjectData(DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ),
								DirectX::XMMatrixScaling(_pTexture2D->GetSize( ).x, _pTexture2D->GetSize( ).y, 1.f) *
								DirectX::XMMatrixScaling(_pTexture2D->GetScale( ).x, _pTexture2D->GetScale( ).y, 1.0f) *
								DirectX::XMMatrixRotationZ(_pTexture2D->GetRotation( )) *
								DirectX::XMLoadFloat4x4(&_pTexture2D->GetWorldMatrix( ))
								);

	//SetTexCoordData(pTexture->UVs);

	iContext->IASetVertexBuffers(0, 1, &m_d3dVertexBuffer, &stride, &offset);
	iContext->IASetIndexBuffer(m_d3dIndexBuffer, DXGI_FORMAT_R32_UINT, NULL);

	if (_pTexture2D->GetColor().w == 0.25f)
	{
		int dostuff = 0;
	}
	CRenderer::GetReference().SetColorData(_pTexture2D->GetColor( ));
	iContext->PSSetShaderResources(0, 1, &pTexture->m_d3dSRV);

	iContext->DrawIndexed(6, 0, 0);
}

/*static*/ void CHUDRenderer::SetTexCoordData(DirectX::XMFLOAT4 _pTexCoords[4])
{
	//TTexCoords texCoords;
	//memcpy(texCoords.UVs, _pTexCoords, sizeof(DirectX::XMFLOAT4) * 4);
	//
	//D3D11_MAPPED_SUBRESOURCE edit;
	//CRenderer::GetReference().GetContext( )->Map(m_d3dTexCoordCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
	//memcpy(edit.pData, &texCoords, sizeof(TTexCoords));
	//CRenderer::GetReference().GetContext( )->Unmap(m_d3dTexCoordCBuffer, 0);
	//
	//CRenderer::GetReference().GetContext( )->VSSetConstantBuffers(TTexCoords::register_slot, 1, &m_d3dTexCoordCBuffer);
}

ID3D11Buffer*			CHUDRenderer::m_d3dIndexBuffer;
ID3D11Buffer*			CHUDRenderer::m_d3dVertexBuffer;

ID3D11Buffer*			CHUDRenderer::m_d3dTexCoordCBuffer;
