#include "VertexStructures.h"

#include "ShaderManager.h"

//--------------------------------------------------------------------------------------
// Vertex struct holding light information
const D3D11_INPUT_ELEMENT_DESC VertexLight::InputElements[ ] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "RANGE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "DIRECTION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "SPOTANGLES", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

};
static_assert(sizeof(VertexLight) == 48, "Vertex struct/layout mismatch");

//--------------------------------------------------------------------------------------
// Vertex struct holding position, and texture mapping information.
const D3D11_INPUT_ELEMENT_DESC VertexPosition::InputElements[ ] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VertexPosition) == 12, "Vertex struct/layout mismatch");

//--------------------------------------------------------------------------------------
// Vertex struct holding position, and texture mapping information.
const D3D11_INPUT_ELEMENT_DESC VertexPositionTexture::InputElements[ ] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VertexPositionTexture) == 20, "Vertex struct/layout mismatch");

//--------------------------------------------------------------------------------------
// Vertex struct holding position, normal vector, and texture mapping information.
const D3D11_INPUT_ELEMENT_DESC VertexPositionNormalTexture::InputElements[ ] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "CONTROLPOINT", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

static_assert(sizeof(VertexPositionNormalTexture) == 32, "Vertex struct/layout mismatch");

//--------------------------------------------------------------------------------------
// Vertex struct holding position, normal vector, texture mapping & skinning information.
const D3D11_INPUT_ELEMENT_DESC VertexPositionNormalTextureSkinned::InputElements[ ] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BONE", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "CONTROLPOINT", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

static_assert(sizeof(VertexPositionNormalTextureSkinned) == 64, "Vertex struct/layout mismatch");

//--------------------------------------------------------------------------------------
// Vertex struct holding position, normal vector, texture mapping, tangents and binormals
const D3D11_INPUT_ELEMENT_DESC VertexPositionNormalTextureTangentBiNormal::InputElements[ ] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

static_assert(sizeof(VertexPositionNormalTextureTangentBiNormal) == 60, "Vertex struct/layout mismatch");

//--------------------------------------------------------------------------------------
// Vertex struct holding position, normal vector, texture mapping, tangents and binormals
const D3D11_INPUT_ELEMENT_DESC VertexPositionNormalTextureTangentBiNormalSkinned::InputElements[ ] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "CONTROLPOINT", 0, DXGI_FORMAT_R32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BONE", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

/*static*/ const D3D11_INPUT_ELEMENT_DESC VertexPositionTextureColor::InputElements[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

static_assert(sizeof(VertexPositionNormalTextureTangentBiNormalSkinned) == 96, "Vertex struct/layout mismatch");

#include "Renderer.h"

#include <fstream>

InputLayoutManager *InputLayoutManager::instancePtr = 0;

InputLayoutManager::InputLayoutManager(void)
{
	for (VertexFormat index = VertexFormat(0); index < eVERTEX_MAX; index = VertexFormat(index + 1))
		inputLayouts[index] = 0;
}

InputLayoutManager::~InputLayoutManager(void)
{
	for (int i = 0; i < VertexFormat::eVERTEX_MAX; i++)
	{
		inputLayouts[i] = nullptr;
	}
}

InputLayoutManager &InputLayoutManager::GetReference( )
{
	if (0 == instancePtr)
	{
		instancePtr = new InputLayoutManager;
		instancePtr->Initialize( );
	}
	return *instancePtr;
}
void InputLayoutManager::DeleteInstance( )
{
	delete instancePtr;
	instancePtr = 0;
}

bool InputLayoutManager::ApplyState(VertexFormat format)
{
	if (format >= eVERTEX_MAX)
	{
		currentFormat = format;
		return false;
	}

	if (format == currentFormat)
		return false;

	CRenderer::GetReference().GetContext( )->IASetInputLayout(inputLayouts[format]);
	currentFormat = format;
	return true;
}

void InputLayoutManager::ClearState( )
{
	ApplyState(VertexFormat::eVERTEX_MAX);
}

void InputLayoutManager::Initialize( )
{
	CreateLayout("Assets/Shader Output/Vertex_Lighting.cso",
				 VertexLight::InputElements,
				 VertexLight::InputElementCount,
				 VertexFormat::eVERTEX_LIGHT);

	CreateLayout("Assets/Shader Output/Vertex_Pos.cso",
				 //"VertexPosition",
				 VertexPosition::InputElements,
				 VertexPosition::InputElementCount,
				 VertexFormat::eVERTEX_POS);

	CreateLayout("Assets/Shader Output/Vertex_PosTex.cso",
				//"VertexPositionTexture",
				VertexPositionTexture::InputElements,
				VertexPositionTexture::InputElementCount,
				VertexFormat::eVERTEX_POSTEX);

	CreateLayout("Assets/Shader Output/Vertex_PosTexColor.cso",
				//"VertexPositionTexture",
				VertexPositionTextureColor::InputElements,
				VertexPositionTextureColor::InputElementCount,
				VertexFormat::eVERTEX_POSTEXCOLOR);

	CreateLayout("Assets/Shader Output/Vertex_PosNormTex.cso",
				 //"VertexPositionNormalTexture",
				 VertexPositionNormalTexture::InputElements,
				 VertexPositionNormalTexture::InputElementCount,
				 VertexFormat::eVERTEX_POSNORMTEX);

	CreateLayout("Assets/Shader Output/Vertex_PosNormTexSkinned.cso",
				 //"VertexPositionNormalTextureSkinned",
				 VertexPositionNormalTextureSkinned::InputElements,
				 VertexPositionNormalTextureSkinned::InputElementCount,
				 VertexFormat::eVERTEX_POSNORMTEXSKINNED);

	CreateLayout("Assets/Shader Output/Vertex_PosNormTexTanBit.cso",
				 //"VertexPositionNormalTextureTangentBiNormal",
				 VertexPositionNormalTextureTangentBiNormal::InputElements,
				 VertexPositionNormalTextureTangentBiNormal::InputElementCount,
				 VertexFormat::eVERTEX_POSNORMTEXTANBIN);

	CreateLayout("Assets/Shader Output/Vertex_PosNormTexTanBitSkinned.cso",
				 //"VertexPositionNormalTextureTangentBiNormalSkinned",
				 VertexPositionNormalTextureTangentBiNormalSkinned::InputElements,
				 VertexPositionNormalTextureTangentBiNormalSkinned::InputElementCount,
				 VertexFormat::eVERTEX_POSNORMTEXTANBINSKINNED);
}

void InputLayoutManager::CreateLayout(char* filename, const D3D11_INPUT_ELEMENT_DESC* inputElements, UINT inputElementsCount, VertexFormat format)
{
	if (format >= eVERTEX_MAX)
		return;

	char* byteCode = NULL;
	size_t byteCodeSize = 0;

	CShaderManager::LoadShader(&byteCode, byteCodeSize, filename);

	HRESULT hr = CRenderer::GetReference().GetDevice( )->CreateInputLayout(inputElements,
															inputElementsCount,
															byteCode, //pBlob->GetBufferPointer(),
															byteCodeSize, //pBlob->GetBufferSize(),
															&inputLayouts[format]);

	delete[ ] byteCode;

	HR(hr);

	//SetD3DName(inputLayouts[format], std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());
}
