/***********************************************
* Filename:  		Texture2D.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		This stores information related
*						to 2D images
************************************************/

#include "Texture2D.h"

#include "Renderer.h"

#include "../Asset Manager/AssetManager.h"

#include <algorithm>

CTexture2D::CTexture2D(float _fX,
					   float _fY,
					   float _fWidth,
					   float _fHeight,
					   std::wstring _szFile)
{
	m_XMColor = DirectX::XMFLOAT4(1, 1, 1, 1);
	m_tpTexture = new TTexture2D( );

	//m_tpTexture->UVs[0] = DirectX::XMFLOAT4(0, 0, 0, 0);
	//m_tpTexture->UVs[1] = DirectX::XMFLOAT4(1, 1, 0, 0);
	//m_tpTexture->UVs[2] = DirectX::XMFLOAT4(0, 1, 0, 0);
	//m_tpTexture->UVs[3] = DirectX::XMFLOAT4(1, 0, 0, 0);

	DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixIdentity( )/*scale * rotation * translation*/);

	SetScreenPosition(_fX, _fY);
	m_XMScaling.x = m_XMScaling.y = 1.f;
	m_XMSize.x = _fWidth;
	m_XMSize.y = _fHeight;

	CAssetManager::LoadTexture(_szFile.c_str( ), &m_tpTexture->m_d3dSRV);

	//CreateBuffers( );
}

CTexture2D::CTexture2D( )
{
	DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixIdentity( )/*scale * rotation * translation*/);
	m_XMScaling.x = m_XMScaling.y = 1.f;
}

CTexture2D::~CTexture2D( )
{
	if (m_tpTexture)
	{
		SafeRelease(m_tpTexture->m_d3dSRV);
		SafeDelete(m_tpTexture);
	}
}

void CTexture2D::CreateBuffers( )
{
	// Set up the description of the static vertex buffer.
	//D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	//vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//vertexBufferDesc.ByteWidth = sizeof(VertexPositionTexture) * 4;
	//vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	//HRESULT hr = CRenderer::GetReference().GetDevice( )->CreateBuffer(&vertexBufferDesc, nullptr, &m_tpTexture->m_d3dVertexBuffer);
	//HR(hr);
	//SetD3DName(m_tpTexture->m_d3dVertexBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	//UpdateBuffers( );
}

void CTexture2D::UpdateBuffers( )
{
	//float width = (float)CRenderer::GetReference().GetWidth( ) * .5f;
	//float height = (float)CRenderer::GetReference().GetHeight( ) * .5f;

	//float left = ((m_tpTexture->m_fPosX - (m_tpTexture->m_fTextureWidth * .5f)) / width) - 1;
	//float right = left + (m_tpTexture->m_fTextureWidth / width);
	//float top = ((m_tpTexture->m_fPosY + (m_tpTexture->m_fTextureHeight *.5f)) / height) + 1;
	//float bottom = top - (m_tpTexture->m_fTextureHeight / height);

	//VertexPositionTexture* vertices = new VertexPositionTexture[4];

	//vertices[0].position = DirectX::XMFLOAT3(left, top, 0.0f);  // Top left.
	//vertices[0].textureCoordinate = DirectX::XMFLOAT2(0.0f, 0.0f);
	//
	//vertices[1].position = DirectX::XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	//vertices[1].textureCoordinate = DirectX::XMFLOAT2(1.0f, 1.0f);
	//
	//vertices[2].position = DirectX::XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	//vertices[2].textureCoordinate = DirectX::XMFLOAT2(0.0f, 1.0f);
	//
	//vertices[3].position = DirectX::XMFLOAT3(right, top, 0.0f);  // Top right.
	//vertices[3].textureCoordinate = DirectX::XMFLOAT2(1.0f, 0.0f);

	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//CRenderer::GetReference().GetContext( )->Map(m_tpTexture->m_d3dVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//VertexPositionTexture* verticesPtr = (VertexPositionTexture*)mappedResource.pData;
	//memcpy(verticesPtr, (void*)vertices, (sizeof(VertexPositionTexture) * 4));
	//CRenderer::GetReference().GetContext( )->Unmap(m_tpTexture->m_d3dVertexBuffer, 0);

	//delete[ ] vertices;
	//vertices = 0;
}

void CTexture2D::LoadTexture(std::wstring _szFile)
{
	//std::wstring path = PathFindExtensionW(_szFile.c_str( ));
	//transform(path.begin( ), path.end( ), path.begin( ), toupper);
	//
	//ID3D11Resource* resource;
	//if (path.find(L".DDS") != std::string::npos)
	//{
	//	HRESULT hr = CreateDDSTextureFromFile(CRenderer::GetReference().GetDevice( ), _szFile.c_str( ), &resource, &m_tpTexture->m_d3dSRV);
	//}
	//else
	//{
	//	HRESULT hr = DirectX::CreateWICTextureFromFile(CRenderer::GetReference().GetDevice( ), _szFile.c_str( ), &resource, &m_tpTexture->m_d3dSRV);
	//}
	//
	//SafeRelease(resource);
}

void CTexture2D::SetScreenPosition(float _fX,
								   float _fY)
{
	//m_tpTexture->m_fPosX = _fX;
	////m_tpTexture->m_fPosY = _fY;

	//float fScreenWidth = (float)CRenderer::GetReference().GetWidth( );
	//float fScreenHeight = (float)CRenderer::GetReference().GetHeight( );

	//float left = (float)((fScreenWidth * .5f) * -1) + (float)_fX;
	//float top = (float)(fScreenHeight * .5f) - (float)_fY;

	MyWorld.Local._41 = _fX;
	MyWorld.Local._42 = _fY;
}

void CTexture2D::SetUVs(DirectX::XMFLOAT4 _pTexCoord[4])
{
	//memcpy(m_tpTexture->UVs, _pTexCoord, sizeof(DirectX::XMFLOAT4) * 4);
}
