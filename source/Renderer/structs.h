#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#include "../Core/stdafx.h"

struct tTexture2dConfig
{
	tTexture2dConfig( )
	{
		SetDefaults( );
	}

	void SetDefaults( )
	{
		// Set the state to the default configuration.  These are the D3D11 default
		// values as well.

		m_State.Width = 1;
		m_State.Height = 1;
		m_State.MipLevels = 1;
		m_State.ArraySize = 1;
		m_State.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_State.SampleDesc.Count = 1;
		m_State.SampleDesc.Quality = 0;
		m_State.Usage = D3D11_USAGE_DEFAULT;
		m_State.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		m_State.CPUAccessFlags = 0;
		m_State.MiscFlags = 0;
	}
	void SetDepthBuffer(UINT width, UINT height)
	{
		m_State.Width = width;
		m_State.Height = height;
		m_State.MipLevels = 1;
		m_State.ArraySize = 1;
		m_State.Format = DXGI_FORMAT_D32_FLOAT;
		m_State.SampleDesc.Count = 1;
		m_State.SampleDesc.Quality = 0;
		m_State.Usage = D3D11_USAGE_DEFAULT;
		m_State.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		m_State.CPUAccessFlags = 0;
		m_State.MiscFlags = 0;
	}
	void SetColorBuffer(UINT width, UINT height)
	{
		m_State.Width = width;
		m_State.Height = height;
		m_State.MipLevels = 1;
		m_State.ArraySize = 1;
		m_State.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		m_State.SampleDesc.Count = 1;
		m_State.SampleDesc.Quality = 0;
		m_State.Usage = D3D11_USAGE_DEFAULT;
		m_State.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		m_State.CPUAccessFlags = 0;
		m_State.MiscFlags = 0;
	}

	D3D11_TEXTURE2D_DESC 		m_State;
};

struct tRenderTargetViewConfig
{
	tRenderTargetViewConfig( )
	{
		SetDefaults( );
	}

	void SetDefaults( )
	{
		m_State.Format = DXGI_FORMAT_UNKNOWN;
		m_State.ViewDimension = D3D11_RTV_DIMENSION_UNKNOWN;
	}

	D3D11_RENDER_TARGET_VIEW_DESC m_State;
};

struct tDepthStencilViewConfig
{
	tDepthStencilViewConfig( )
	{
		SetDefaults( );
	}

	void SetDefaults( )
	{
		m_State.Flags = 0;
		m_State.Format = DXGI_FORMAT_UNKNOWN;
		m_State.ViewDimension = D3D11_DSV_DIMENSION_UNKNOWN;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC m_State;
};

struct tShaderResourceViewConfig
{
	tShaderResourceViewConfig( )
	{
		SetDefaults( );
	}

	void SetDefaults( )
	{
		m_State.Format = DXGI_FORMAT_UNKNOWN;
		m_State.ViewDimension = D3D11_SRV_DIMENSION_UNKNOWN;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC m_State;
};

struct tTarget
{
	tTarget( )
	{
		m_pSRV = NULL;
		m_pRTV = NULL;
		m_pDSV = NULL;

		m_pResource = NULL;
	}

	~tTarget( )
	{
		SafeRelease(m_pSRV);
		SafeRelease(m_pRTV);
		SafeRelease(m_pDSV);
		SafeRelease(m_pResource);
	}

	ID3D11ShaderResourceView* m_pSRV;
	ID3D11RenderTargetView*  m_pRTV;
	ID3D11DepthStencilView*  m_pDSV;

	ID3D11Texture2D* m_pResource;
};

#endif //_STRUCTS_H_
