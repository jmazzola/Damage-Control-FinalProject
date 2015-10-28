#include "../Core/stdafx.h"

#include "Skybox.h"

#include "../Asset Manager/AssetManager.h"

#include "Renderer.h"
#include "Scene.h"

#include "RenderStateManager.h"
#include "VertexStructures.h"

using namespace MathHelper;

CSkybox::CSkybox( )
{
	m_szTag = "Skybox";

	CreateSkybox( );

	m_fSpeedMod = 0.0f;
}

CSkybox::~CSkybox( )
{
	SafeRelease(m_d3dTexture);
	SafeRelease(m_d3dVertexBuffer);
	SafeRelease(m_d3dIndexBuffer);
}

void CSkybox::Update(float _fDT)
{
	static float t = 0;

	t += _fDT * .5f * m_fSpeedMod;
	//CRenderer::GetReference( ).SetTimeData({ t, t });

	if (t)
	{
		D3D11_MAPPED_SUBRESOURCE data = { 0 };
		ID3D11DeviceContext* dc = CRenderer::GetReference( ).GetContext( );

		dc->Map(m_d3dVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

		VertexPositionTexture verts[ ] =
		{
			{ DirectX::XMFLOAT3(0.0f - (PLANE_SIZE_X / 2.0f), -(0.0f - (PLANE_SIZE_Y / 2.0f)), 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f - t) },
			{ DirectX::XMFLOAT3(PLANE_SIZE_X - (PLANE_SIZE_X / 2.0f), -(0.0f - (PLANE_SIZE_Y / 2.0f)), 1.0f), DirectX::XMFLOAT2(TILES, 0.0f - t) },
			{ DirectX::XMFLOAT3(PLANE_SIZE_X - (PLANE_SIZE_X / 2.0f), -(PLANE_SIZE_Y - (PLANE_SIZE_Y / 2.0f)), 1.0f), DirectX::XMFLOAT2(TILES, TILES - t) },
			{ DirectX::XMFLOAT3(0.0f - (PLANE_SIZE_X / 2.0f), -(PLANE_SIZE_Y - (PLANE_SIZE_Y / 2.0f)), 1.0f), DirectX::XMFLOAT2(0.0f, TILES - t) }
		};

		memcpy(data.pData, &verts, sizeof(VertexPositionTexture) * _countof(verts));
		dc->Unmap(m_d3dVertexBuffer, 0);
	}

	//DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixIdentity( ));
	//
	//DirectX::XMMATRIX viewMatrix = DirectX::XMLoadFloat4x4(&CScene::GetCamera( )->View( ));
	//viewMatrix = DirectX::XMMatrixInverse(NULL, viewMatrix);
	//DirectX::XMVECTOR eyePosition = viewMatrix.r[3];
	//
	//DirectX::XMMATRIX scaling = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
	//DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(DirectX::XMVectorGetX(eyePosition), DirectX::XMVectorGetY(eyePosition), DirectX::XMVectorGetZ(eyePosition));
	//DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY((_fDT * 2.0f));
	//
	//auto matrix = scaling /** rotation*/ * translation;
	//
	//DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixMultiply(matrix, DirectX::XMLoadFloat4x4(&MyWorld.Local)));
}

void CSkybox::Render( )
{
	UINT stride = sizeof(VertexPositionTexture);
	UINT offset = 0;

	CRenderStateManager::GetReference( ).ApplyRenderState(
		CRenderStateManager::eRenderState::SKYBOX);

	InputLayoutManager::GetReference( ).ApplyState(InputLayoutManager::VertexFormat::eVERTEX_POSTEX);

	ID3D11DeviceContext* context = CRenderer::GetReference( ).GetContext( );

	context->IASetVertexBuffers(0, 1, &m_d3dVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(m_d3dIndexBuffer, DXGI_FORMAT_R32_UINT, NULL);

	CRenderer::GetReference( ).SetPerObjectData(DirectX::XMLoadFloat4x4(&MyWorld.GetWorld( )),
												DirectX::XMLoadFloat4x4(&MyWorld.GetWorld( )) * DirectX::XMLoadFloat4x4(&CScene::GetCamera( )->View( )),
												DirectX::XMLoadFloat4x4(&MyWorld.GetWorld( )) * DirectX::XMLoadFloat4x4(&CScene::GetCamera( )->ViewProjection( )));

	//CRenderer::GetReference( ).SetPerCameraData(CScene::GetCamera( ), true);

	context->PSSetShaderResources(0, 1, &m_d3dTexture);

	context->DrawIndexed(6, 0, 0);
}

/*
template<typename T>
void CSkybox::CreateBuffer(_In_ ID3D11Device* pDevice, T const& data, D3D11_BIND_FLAG bindFlags, _Outptr_ ID3D11Buffer** ppBuffer)
{
assert(ppBuffer != 0);

D3D11_BUFFER_DESC bufferDesc = { 0 };

bufferDesc.ByteWidth = (UINT)data.size( ) * sizeof(T::value_type);
bufferDesc.BindFlags = bindFlags;
bufferDesc.Usage = D3D11_USAGE_DEFAULT;

D3D11_SUBRESOURCE_DATA dataDesc = { 0 };

dataDesc.pSysMem = &data.front( );

HR(pDevice->CreateBuffer(&bufferDesc, &dataDesc, ppBuffer));

//SetD3DName((*ppBuffer), std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());
}

void CSkybox::CreateCube( )
{
// A cube has six faces, each one pointing in a different direction.
const int FaceCount = 6;

static const DirectX::XMVECTORF32 faceNormals[FaceCount] =
{
{ 0, 0, 1 },
{ 0, 0, -1 },
{ 1, 0, 0 },
{ -1, 0, 0 },
{ 0, 1, 0 },
{ 0, -1, 0 },
};

static const DirectX::XMVECTORF32 textureCoordinates[4] =
{
{ 1, 0 },
{ 1, 1 },
{ 0, 1 },
{ 0, 0 },
};

std::vector<VertexPosition> vertices;
std::vector<size_t> indices;

float size = 2;
size /= 2;

// Create each face in turn.
for (int i = 0; i < FaceCount; i++)
{
DirectX::XMFLOAT3 normal; DirectX::XMStoreFloat3(&normal, faceNormals[i]);

// Get two vectors perpendicular both to the face normal and to each other.
DirectX::XMFLOAT3 basis; DirectX::XMStoreFloat3(&basis, (i >= 4) ? DirectX::g_XMIdentityR2 : DirectX::g_XMIdentityR1);

DirectX::XMFLOAT3 side1; DirectX::XMStoreFloat3(&side1, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&normal), DirectX::XMLoadFloat3(&basis)));
DirectX::XMFLOAT3 side2; DirectX::XMStoreFloat3(&side2, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&normal), DirectX::XMLoadFloat3(&side1)));

// Six indices (two triangles) per face.
size_t vbase = vertices.size( );
indices.push_back(vbase + 0);
indices.push_back(vbase + 1);
indices.push_back(vbase + 2);

indices.push_back(vbase + 0);
indices.push_back(vbase + 2);
indices.push_back(vbase + 3);

// Four vertices per face.
vertices.push_back(VertexPosition((normal - side1 - side2) * size));
vertices.push_back(VertexPosition((normal - side1 + side2) * size));
vertices.push_back(VertexPosition((normal + side1 + side2) * size));
vertices.push_back(VertexPosition((normal + side1 - side2) * size));
}

CreateBuffer(CRenderer::GetReference( ).GetDevice( ), vertices, D3D11_BIND_VERTEX_BUFFER, &m_d3dVertexBuffer);
CreateBuffer(CRenderer::GetReference( ).GetDevice( ), indices, D3D11_BIND_INDEX_BUFFER, &m_d3dIndexBuffer);
}
*/

void CSkybox::CreatePlane( )
{
	D3D11_SUBRESOURCE_DATA data = { 0 };
	VertexPositionTexture verts[ ] =
	{
		{ DirectX::XMFLOAT3(0.0f - (PLANE_SIZE_X / 2.0f), -(0.0f - (PLANE_SIZE_Y / 2.0f)), 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
		{ DirectX::XMFLOAT3(PLANE_SIZE_X - (PLANE_SIZE_X / 2.0f), -(0.0f - (PLANE_SIZE_Y / 2.0f)), 1.0f), DirectX::XMFLOAT2(TILES, 0.0f) },
		{ DirectX::XMFLOAT3(PLANE_SIZE_X - (PLANE_SIZE_X / 2.0f), -(PLANE_SIZE_Y - (PLANE_SIZE_Y / 2.0f)), 1.0f), DirectX::XMFLOAT2(TILES, TILES) },
		{ DirectX::XMFLOAT3(0.0f - (PLANE_SIZE_X / 2.0f), -(PLANE_SIZE_Y - (PLANE_SIZE_Y / 2.0f)), 1.0f), DirectX::XMFLOAT2(0.0f, TILES) }
	};

	D3D11_BUFFER_DESC vBuffer = { 0 };
	vBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBuffer.ByteWidth = sizeof(VertexPositionTexture) * _countof(verts);
	vBuffer.Usage = D3D11_USAGE_DYNAMIC;
	vBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	data = { 0 };
	data.pSysMem = &verts;
	CRenderer::GetReference( ).GetDevice( )->CreateBuffer(&vBuffer, &data, &m_d3dVertexBuffer);

	size_t indices[ ] =
	{
		0, 1, 2,
		3, 0, 2
	};

	D3D11_BUFFER_DESC iBuffer = { 0 };
	iBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBuffer.ByteWidth = sizeof(size_t) * _countof(indices);
	iBuffer.Usage = D3D11_USAGE_DEFAULT;

	data = { 0 };
	data.pSysMem = &indices;
	CRenderer::GetReference( ).GetDevice( )->CreateBuffer(&iBuffer, &data, &m_d3dIndexBuffer);

	DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixTranslation(0, -1, 0));
}

void CSkybox::CreateSkybox( )
{
	//CreateCube( );
	CreatePlane( );

	CAssetManager::LoadTexture("Assets/Textures/stars.png", &m_d3dTexture);
}
