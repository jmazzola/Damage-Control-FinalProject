#include "../Core/stdafx.h"
#include "DebugRenderer.h"

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

CDebugRenderer *CDebugRenderer::instancePtr = 0;

CDebugRenderer::CDebugRenderer(void)
{
}

CDebugRenderer::~CDebugRenderer(void)
{
	SafeRelease(m_d3dLineVertexBuffer);
	SafeRelease(m_d3dSquareVertexBuffer);
	SafeRelease(m_d3dCircleVertexBuffer);
}

CDebugRenderer &CDebugRenderer::GetReference()
{
	if (!instancePtr)
	{
		instancePtr = new CDebugRenderer;
		instancePtr->CreateBuffers();
	}
	return *instancePtr;
}

void CDebugRenderer::DeleteInstance()
{
	delete instancePtr;
	instancePtr = nullptr;
}

void CDebugRenderer::CreateBuffers()
{
	m_nLine = 2;
	m_nSquare = 5;
	m_nCircle = 91;

	D3D11_BUFFER_DESC vertexBufferDesc;

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexPosition) * m_nLine;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HR(CRenderer::GetReference().GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &m_d3dLineVertexBuffer));
	//SetD3DName(m_tpTexture->m_d3dVertexBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexPosition) * m_nSquare;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HR(CRenderer::GetReference().GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &m_d3dSquareVertexBuffer));
	//SetD3DName(m_tpTexture->m_d3dVertexBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexPosition) * m_nCircle;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HR(CRenderer::GetReference().GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &m_d3dCircleVertexBuffer));
	//SetD3DName(m_tpTexture->m_d3dVertexBuffer, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());
}

void CDebugRenderer::LineRenderer(DirectX::XMFLOAT3 _XMStart,
	DirectX::XMFLOAT3 _XMEnd,
	DirectX::XMFLOAT4 _XMColor)
{
	if (!m_bExecute)
	{
		Line line;
		line._XMStart = _XMStart;
		line._XMEnd = _XMEnd;
		line._XMColor = _XMColor;
		m_vLines.push_back(line);
		return;
	}

	ApplyDebug();

	VertexPosition* vertices = new VertexPosition[m_nLine];
	vertices[0].position = _XMStart;
	vertices[1].position = _XMEnd;

	vertices[0].position.y += 10;
	vertices[1].position.y += 10;

	D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
	CRenderer::GetReference().GetContext()->Map(m_d3dLineVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VertexPosition* verticesPtr = (VertexPosition*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexPosition) * m_nLine));
	CRenderer::GetReference().GetContext()->Unmap(m_d3dLineVertexBuffer, 0);

	delete[] vertices;
	vertices = 0;

	CRenderer::GetReference().SetColorData(_XMColor);

	UINT stride = sizeof(VertexPosition);
	UINT offset = {};

	CRenderer::GetReference().GetContext()->IASetVertexBuffers(0, 1, &m_d3dLineVertexBuffer, &stride, &offset);
	CRenderer::GetReference().GetContext()->Draw(m_nLine, 0);
}

void CDebugRenderer::SquareRenderer(DirectX::XMFLOAT3 _XMPosition,
	DirectX::XMFLOAT2 _XMRadius,
	DirectX::XMFLOAT4 _XMColor)
{
	if (!m_bExecute)
	{
		Square square;
		square._XMPosition = _XMPosition;
		square._XMRadius = _XMRadius;
		square._XMColor = _XMColor;
		m_vSquares.push_back(square);
		return;
	}

	ApplyDebug();

	VertexPosition* vertices = new VertexPosition[m_nSquare];
	memset(vertices, 0, sizeof(VertexPosition) * m_nSquare);

	vertices[0].position =
		vertices[1].position =
		vertices[2].position =
		vertices[3].position = _XMPosition;

	vertices[0].position.x -= _XMRadius.x;
	vertices[0].position.z += _XMRadius.y;

	vertices[1].position.x += _XMRadius.x;
	vertices[1].position.z += _XMRadius.y;

	vertices[2].position.x += _XMRadius.x;
	vertices[2].position.z -= _XMRadius.y;

	vertices[3].position.x -= _XMRadius.x;
	vertices[3].position.z -= _XMRadius.y;

	vertices[4].position = vertices[0].position;

	D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
	CRenderer::GetReference().GetContext()->Map(m_d3dSquareVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VertexPosition* verticesPtr = (VertexPosition*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexPosition) * m_nSquare));
	CRenderer::GetReference().GetContext()->Unmap(m_d3dSquareVertexBuffer, 0);

	delete[] vertices;
	vertices = 0;

	CRenderer::GetReference().SetColorData(_XMColor);

	UINT stride = sizeof(VertexPosition);
	UINT offset = {};

	CRenderer::GetReference().GetContext()->IASetVertexBuffers(0, 1, &m_d3dSquareVertexBuffer, &stride, &offset);
	CRenderer::GetReference().GetContext()->Draw(m_nSquare, 0);
}

void CDebugRenderer::CircleRenderer(DirectX::XMFLOAT3 _XMPosition,
	DirectX::XMFLOAT2 _XMRadius,
	DirectX::XMFLOAT4 _XMColor)
{
	if (!m_bExecute)
	{
		Circle circle;
		circle._XMPosition = _XMPosition;
		circle._XMRadius = _XMRadius;
		circle._XMColor = _XMColor;
		m_vCircles.push_back(circle);
		return;
	}

	ApplyDebug();

	VertexPosition* vertices = new VertexPosition[m_nCircle];
	memset(vertices, 0, sizeof(VertexPosition) * m_nCircle);

	for (float i = 0; i < m_nCircle; i += 1)
	{
		vertices[(int)i].position = {
			_XMPosition.x + sin(DirectX::XMConvertToRadians(i * 4)) * _XMRadius.x,
			_XMPosition.y,
			_XMPosition.z + cos(DirectX::XMConvertToRadians(i * 4)) * _XMRadius.y };
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
	CRenderer::GetReference().GetContext()->Map(m_d3dCircleVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VertexPosition* verticesPtr = (VertexPosition*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexPosition) * m_nCircle));
	CRenderer::GetReference().GetContext()->Unmap(m_d3dCircleVertexBuffer, 0);

	delete[] vertices;
	vertices = 0;

	CRenderer::GetReference().SetColorData(_XMColor);

	UINT stride = sizeof(VertexPosition);
	UINT offset = {};

	CRenderer::GetReference().GetContext()->IASetVertexBuffers(0, 1, &m_d3dCircleVertexBuffer, &stride, &offset);
	CRenderer::GetReference().GetContext()->Draw(m_nCircle, 0);
}

void CDebugRenderer::ApplyDebug()
{
	CRenderStateManager::GetReference().ApplyRenderState(
		CRenderStateManager::eRenderState::DEBUG_LINE);

	InputLayoutManager::GetReference().InputLayoutManager::ApplyState(
		InputLayoutManager::VertexFormat::eVERTEX_POS);

	CICamera* pCamera = CScene::GetCamera();
	CRenderer::GetReference().SetPerCameraData(pCamera, true);
}

void CDebugRenderer::Execute()
{
	CRenderer& renderer = CRenderer::GetReference();

	renderer.BeginEvent(L"Begin Debug Rendering");

	m_bExecute = true;

	renderer.BeginEvent(L"LineRenderer");
	for (auto& line : m_vLines)
	{
		LineRenderer(line._XMStart, line._XMEnd, line._XMColor);
	}
	m_vLines.clear();
	renderer.EndEvent();

	renderer.BeginEvent(L"SquareRenderer");
	for (auto& square : m_vSquares)
	{
		SquareRenderer(square._XMPosition, square._XMRadius, square._XMColor);
	}
	m_vSquares.clear();
	renderer.EndEvent();

	renderer.BeginEvent(L"CircleRenderer");
	for (auto& circle : m_vCircles)
	{
		CircleRenderer(circle._XMPosition, circle._XMRadius, circle._XMColor);
	}
	m_vCircles.clear();
	renderer.EndEvent();

	m_bExecute = false;

	renderer.EndEvent();
}