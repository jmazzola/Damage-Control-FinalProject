/***********************************************
* Filename:  		ShaderManager.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		Management class for shaders
************************************************/

#include "ShaderManager.h"

#include "Renderer.h"

#include <fstream>

CShaderManager *CShaderManager::instancePtr = 0;

CShaderManager::CShaderManager(void)
{
	currentPixelShader = PS_COUNT;
	currentVertexShader = VS_COUNT;
}

CShaderManager::~CShaderManager(void)
{
	for (int i = 0; i < PixelShader::PS_COUNT; i++)
	{
		SafeRelease(m_d3dPixelShader[i]);
	}
	for (int i = 0; i < VertexShader::VS_COUNT; i++)
	{
		SafeRelease(m_d3dVertexShader[i]);
	}
	for (int i = 0; i < GeometryShader::GS_COUNT; i++)
	{
		SafeRelease(m_d3dGeometryShader[i]);
	}
}

CShaderManager &CShaderManager::GetReference( )
{
	if (!instancePtr)
	{
		instancePtr = new CShaderManager;
		instancePtr->CreateShaders( );
	}
	return *instancePtr;
}

void CShaderManager::DeleteInstance( )
{
	delete instancePtr;
	instancePtr = nullptr;
}

bool CShaderManager::ApplyPixelShader(PixelShader shader)
{
	if (shader >= PS_COUNT)
	{
		currentPixelShader = shader;
		return false;
	}

	if (currentPixelShader == shader)
		return false;

	CRenderer::GetReference( ).GetContext( )->PSSetShader(m_d3dPixelShader[shader], NULL, NULL);
	currentPixelShader = shader;
	return true;
}

bool CShaderManager::ApplyVertexShader(VertexShader shader)
{
	if (shader >= VS_COUNT)
	{
		currentVertexShader = shader;
		return false;
	}

	if (currentVertexShader == shader)
		return false;

	CRenderer::GetReference( ).GetContext( )->VSSetShader(m_d3dVertexShader[shader], NULL, NULL);
	currentVertexShader = shader;
	return true;
}

bool CShaderManager::ApplyGeometryShader(GeometryShader shader)
{
	if (shader >= GS_COUNT)
	{
		currentGeometryShader = shader;
		return false;
	}

	if (currentGeometryShader == shader)
		return false;

	CRenderer::GetReference( ).GetContext( )->GSSetShader(m_d3dGeometryShader[shader], NULL, NULL);
	currentGeometryShader = shader;
	return true;
}

void CShaderManager::ClearPixelShader( )
{
	ApplyPixelShader(PixelShader::PS_COUNT);
}
void CShaderManager::ClearVertexShader( )
{
	ApplyVertexShader(VertexShader::VS_COUNT);
}
void CShaderManager::ClearGeometryShader( )
{
	ApplyGeometryShader(GeometryShader::GS_COUNT);
}

void CShaderManager::ClearPipeline( )
{
	ClearPixelShader( );
	ClearVertexShader( );
	ClearGeometryShader( );
}

void CShaderManager::CreateShaders( )
{
	//VERTEX SHADERS
	//
	m_d3dVertexShader[VertexShader::VS_TEST] = CreateVertexShader(SHADER_FILEPATH + std::string("OneTextureVS.cso"));
	m_d3dVertexShader[VertexShader::VS_HUD] = CreateVertexShader(SHADER_FILEPATH + std::string("HUDVS.cso"));
	m_d3dVertexShader[VertexShader::VS_SKYBOX] = CreateVertexShader(SHADER_FILEPATH + std::string("SkyboxVS.cso"));
	m_d3dVertexShader[VertexShader::VS_SKINNING] = CreateVertexShader(SHADER_FILEPATH + std::string("SkinningVS.cso"));
	m_d3dVertexShader[VertexShader::VS_DEBUG_LINE] = CreateVertexShader(SHADER_FILEPATH + std::string("LineVS.cso"));
	m_d3dVertexShader[VertexShader::VS_FONT] = CreateVertexShader(SHADER_FILEPATH + std::string("FontVS.cso"));
	m_d3dVertexShader[VertexShader::VS_PARTICLES] = CreateVertexShader(SHADER_FILEPATH + std::string("ParticleVS.cso"));
	m_d3dVertexShader[VertexShader::VS_GBUFFER] = CreateVertexShader(SHADER_FILEPATH + std::string("GBufferVS.cso"));
	m_d3dVertexShader[VertexShader::VS_GBUFFER_SKINNING] = CreateVertexShader(SHADER_FILEPATH + std::string("GBufferSkinningVS.cso"));
	//m_d3dVertexShader[VertexShader::VS_MASK] = CreateVertexShader(SHADER_FILEPATH + std::string("MaskVS.cso"));
	m_d3dVertexShader[VertexShader::VS_FINAL] = CreateVertexShader(SHADER_FILEPATH + std::string("FinalVS.cso"));
	m_d3dVertexShader[VertexShader::VS_FINAL_SKINNING] = CreateVertexShader(SHADER_FILEPATH + std::string("FinalSkinningVS.cso"));
	m_d3dVertexShader[VertexShader::VS_Screen] = CreateVertexShader(SHADER_FILEPATH + std::string("SpriteVS.cso"));

	m_d3dVertexShader[VertexShader::VS_POINT_LIGHTING] = CreateVertexShader(SHADER_FILEPATH + std::string("PointLightingVS.cso"));
	m_d3dVertexShader[VertexShader::VS_SPOT_LIGHTING] = CreateVertexShader(SHADER_FILEPATH + std::string("SpotLightingVS.cso"));
	m_d3dVertexShader[VertexShader::VS_DIRECTIONAL_LIGHTING] = CreateVertexShader(SHADER_FILEPATH + std::string("DirectionalLightingVS.cso"));


	//PIXEL SHADERS
	//
	m_d3dPixelShader[PixelShader::PS_TEXTURE] = CreatePixelShader(SHADER_FILEPATH + std::string("OneTexturePS.cso"));
	m_d3dPixelShader[PixelShader::PS_SKYBOX] = CreatePixelShader(SHADER_FILEPATH + std::string("SkyboxPS.cso"));
	m_d3dPixelShader[PixelShader::PS_DEBUG_LINE] = CreatePixelShader(SHADER_FILEPATH + std::string("LinePS.cso"));
	m_d3dPixelShader[PixelShader::PS_FONT] = CreatePixelShader(SHADER_FILEPATH + std::string("FontPS.cso"));
	m_d3dPixelShader[PixelShader::PS_PARTICLES] = CreatePixelShader(SHADER_FILEPATH + std::string("ParticlePS.cso"));
	m_d3dPixelShader[PixelShader::PS_GBUFFER] = CreatePixelShader(SHADER_FILEPATH + std::string("GBufferPS.cso"));
	//m_d3dPixelShader[PixelShader::PS_MASK] = CreatePixelShader(SHADER_FILEPATH + std::string("MaskPS.cso"));
	m_d3dPixelShader[PixelShader::PS_FINAL] = CreatePixelShader(SHADER_FILEPATH + std::string("FinalPS.cso"));

	m_d3dPixelShader[PixelShader::PS_POINT_LIGHTING] = CreatePixelShader(SHADER_FILEPATH + std::string("PointLightingPS.cso"));
	m_d3dPixelShader[PixelShader::PS_SPOT_LIGHTING] = CreatePixelShader(SHADER_FILEPATH + std::string("SpotLightingPS.cso"));
	m_d3dPixelShader[PixelShader::PS_DIRECTIONAL_LIGHTING] = CreatePixelShader(SHADER_FILEPATH + std::string("DirectionalLightingPS.cso"));

	//m_d3dPixelShader[PixelShader::PS_POINT_LIGHTING_MSAA] = CreatePixelShader(SHADER_FILEPATH + std::string("PointLightingPSMS.cso"));
	//m_d3dPixelShader[PixelShader::PS_SPOT_LIGHTING_MSAA] = CreatePixelShader(SHADER_FILEPATH + std::string("SpotLightingPSMS.cso"));
	//m_d3dPixelShader[PixelShader::PS_DIRECTIONAL_LIGHTING_MSAA] = CreatePixelShader(SHADER_FILEPATH + std::string("DirectionalLightingPSMS.cso"));


	//GEOMETRY SHADERS
	//
	m_d3dGeometryShader[GeometryShader::GS_PARTICLES] = CreateGeometryShader(SHADER_FILEPATH + std::string("ParticleGS.cso"));
	m_d3dGeometryShader[GeometryShader::GS_NONE] = nullptr;

	m_d3dGeometryShader[GeometryShader::GS_POINT_LIGHTING] = CreateGeometryShader(SHADER_FILEPATH + std::string("PointLightingGS.cso"));
	m_d3dGeometryShader[GeometryShader::GS_SPOT_LIGHTING] = CreateGeometryShader(SHADER_FILEPATH + std::string("SpotLightingGS.cso"));
	m_d3dGeometryShader[GeometryShader::GS_DIRECTIONAL_LIGHTING] = CreateGeometryShader(SHADER_FILEPATH + std::string("DirectionalLightingGS.cso"));

}

ID3D11PixelShader* CShaderManager::CreatePixelShader(std::string _szFileName)
{
	char *byteCode;
	size_t byteCodeSize;
	if (!LoadShader(&byteCode, byteCodeSize, _szFileName.c_str( )))
		return false;

	ID3D11PixelShader* pixelShader;

	HRESULT hr = CRenderer::GetReference( ).GetDevice( )->CreatePixelShader(byteCode,
																			byteCodeSize,
																			NULL,
																			&pixelShader);

	delete[ ] byteCode;

	if (FAILED(hr))
	{
		MessageBoxW(NULL, L"The pixel shader cannot be created.", L"Error", MB_OK);
		return NULL;
	}

	return pixelShader;
}

ID3D11GeometryShader* CShaderManager::CreateGeometryShader(std::string _szFileName)
{
	char *byteCode;
	size_t byteCodeSize;
	if (!LoadShader(&byteCode, byteCodeSize, _szFileName.c_str( )))
		return false;

	ID3D11GeometryShader* geometryShader;

	HRESULT hr = CRenderer::GetReference( ).GetDevice( )->CreateGeometryShader(byteCode,
																			   byteCodeSize,
																			   NULL,
																			   &geometryShader);

	delete[ ] byteCode;

	if (FAILED(hr))
	{
		MessageBoxW(NULL, L"The pixel shader cannot be created.", L"Error", MB_OK);
		return NULL;
	}

	return geometryShader;
}

ID3D11VertexShader* CShaderManager::CreateVertexShader(std::string _szFileName)
{
	char *byteCode;
	size_t byteCodeSize;
	if (!LoadShader(&byteCode, byteCodeSize, _szFileName.c_str( )))
		return false;

	ID3D11VertexShader* vertexShader;

	HRESULT hr = CRenderer::GetReference( ).GetDevice( )->CreateVertexShader(byteCode,
																			 byteCodeSize,
																			 NULL,
																			 &vertexShader);

	delete[ ] byteCode;

	if (FAILED(hr))
	{
		MessageBoxW(NULL, L"The vertex shader cannot be created.", L"Error", MB_OK);
		return NULL;
	}

	return vertexShader;
}

/*static*/ bool CShaderManager::LoadShader(char **_ppByteCode, size_t& _unSize, const char* _szFile)
{
	std::ifstream fin(_szFile, std::ifstream::in | std::ifstream::binary);

	if (!fin.is_open( ))
	{
		DebugPrint(std::string(std::string(_szFile) + " Failed to Open").c_str( ), ConsoleColor::Yellow);
		return false;
	}

	fin.seekg(0, fin.end);

	_unSize = size_t(fin.tellg( ));
	*_ppByteCode = new char[_unSize];
	memset(*_ppByteCode, 0, _unSize);

	fin.seekg(0, fin.beg);

	fin.read(*_ppByteCode, _unSize);

	fin.close( );

	return true;
}
