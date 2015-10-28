/***********************************************
* Filename:  		ICamera.cpp
* Date:				05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		This serves as the base for
*						cameras. Can derive
*						for other behavior
************************************************/

#include "ICamera.h"

#include "Renderer.h"

using namespace DirectX;

CICamera::CICamera( ) : m_XMUp(DirectX::XMFLOAT3(0, 1, 0))
{
	m_fFOV = 60;

	m_fNear = 50.0f;
	m_fFar = 5000.0f;

	m_fWidth = (float)CRenderer::GetReference().GetWidth( );
	m_fHeight = (float)CRenderer::GetReference().GetHeight( );

	this->m_XMLookAt = DirectX::XMFLOAT3(0, 1, 0);
	UpdateProjection(m_fFOV, m_fWidth / m_fHeight, m_fNear, m_fFar);

	m_vPlanes.resize(6);
}

CICamera::~CICamera( )
{
}

void CICamera::UpdateView( )
{
	float rWidth = (float)CRenderer::GetReference().GetWidth( );
	float rHeight = (float)CRenderer::GetReference().GetHeight( );

	if (m_fWidth != rWidth || m_fHeight != rHeight)
	{
		m_fWidth = rWidth;
		m_fHeight = rHeight;

		UpdateProjection(m_fFOV, m_fWidth / m_fHeight, m_fNear, m_fFar);
	}

	DirectX::XMVECTOR vPosition = DirectX::XMLoadFloat3(&this->m_XMPosition);
	DirectX::XMVECTOR vLook = DirectX::XMLoadFloat3(&m_XMLookAt);
	DirectX::XMVECTOR vUp = DirectX::XMLoadFloat3(&this->m_XMUp);

	DirectX::XMStoreFloat4x4(&this->m_XMView, DirectX::XMMatrixLookAtLH(vPosition, vLook, vUp));

	DirectX::XMMATRIX view, projection, vp;
	view = DirectX::XMLoadFloat4x4(&this->m_XMView);
	projection = DirectX::XMLoadFloat4x4(&this->m_XMProjection);
	vp = view * projection;

	DirectX::XMStoreFloat4x4(&this->m_XMViewProjection, vp);

	DirectX::XMStoreFloat4x4(&this->m_XMInvView, DirectX::XMMatrixInverse(NULL, view));
	DirectX::XMStoreFloat4x4(&this->m_XMInvProjection, DirectX::XMMatrixInverse(NULL, projection));

	//CCollision::BuildFrustum(m_fFOV, m_fNear, m_fFar, m_fWidth / m_fHeight, this->m_XMView, m_tFrustum);

	ComputeFrustumPlanes(this->m_XMViewProjection);
}

void CICamera::UpdateProjection(float _fFieldOfView, float _fAspectRatio, float _fNearPlane, float _fFarPlane)
{
	float fovAngleY = _fFieldOfView * XM_PI / 180.0f;

	if (_fAspectRatio < 1.0f)
	{
		///
		/// portrait or snap m_XMView
		///
		this->m_XMUp = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		fovAngleY = 120.0f * XM_PI / 180.0f;
	}
	else
	{
		///
		/// landscape m_XMView
		///
		this->m_XMUp = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	}

	DirectX::XMStoreFloat4x4(&this->m_XMProjection, DirectX::XMMatrixPerspectiveFovLH(fovAngleY, _fAspectRatio, _fNearPlane, _fFarPlane));
	DirectX::XMStoreFloat4x4(&this->m_XMOrthoMatrix, DirectX::XMMatrixOrthographicLH(m_fWidth, m_fHeight, m_fNear, 100));// m_fFar));
}

void CICamera::ComputeFrustumPlanes(DirectX::XMFLOAT4X4& viewProj)
{
	// x, y, z, and w represent A, B, C and D in the plane equation
	// where ABC are the xyz of the planes normal, and D is the plane constant
	//std::vector<DirectX::XMFLOAT4> tempFrustumPlane(6);

	// Left Frustum Plane
	// Add first column of the matrix to the fourth column
	m_vPlanes[0].x = viewProj._14 + viewProj._11;
	m_vPlanes[0].y = viewProj._24 + viewProj._21;
	m_vPlanes[0].z = viewProj._34 + viewProj._31;
	m_vPlanes[0].w = viewProj._44 + viewProj._41;

	// Right Frustum Plane
	// Subtract first column of matrix from the fourth column
	m_vPlanes[1].x = viewProj._14 - viewProj._11;
	m_vPlanes[1].y = viewProj._24 - viewProj._21;
	m_vPlanes[1].z = viewProj._34 - viewProj._31;
	m_vPlanes[1].w = viewProj._44 - viewProj._41;

	// Top Frustum Plane
	// Subtract second column of matrix from the fourth column
	m_vPlanes[2].x = viewProj._14 - viewProj._12;
	m_vPlanes[2].y = viewProj._24 - viewProj._22;
	m_vPlanes[2].z = viewProj._34 - viewProj._32;
	m_vPlanes[2].w = viewProj._44 - viewProj._42;

	// Bottom Frustum Plane
	// Add second column of the matrix to the fourth column
	m_vPlanes[3].x = viewProj._14 + viewProj._12;
	m_vPlanes[3].y = viewProj._24 + viewProj._22;
	m_vPlanes[3].z = viewProj._34 + viewProj._32;
	m_vPlanes[3].w = viewProj._44 + viewProj._42;

	// Near Frustum Plane
	// We could add the third column to the fourth column to get the near plane,
	// but we don't have to do this because the third column IS the near plane
	m_vPlanes[4].x = viewProj._13;
	m_vPlanes[4].y = viewProj._23;
	m_vPlanes[4].z = viewProj._33;
	m_vPlanes[4].w = viewProj._43;

	// Far Frustum Plane
	// Subtract third column of matrix from the fourth column
	m_vPlanes[5].x = viewProj._14 - viewProj._13;
	m_vPlanes[5].y = viewProj._24 - viewProj._23;
	m_vPlanes[5].z = viewProj._34 - viewProj._33;
	m_vPlanes[5].w = viewProj._44 - viewProj._43;

	// Normalize plane normals (A, B and C (xyz))
	// Also take note that planes face inward
	for (int i = 0; i < 6; ++i)
	{
		float length = sqrt((m_vPlanes[i].x * m_vPlanes[i].x) + (m_vPlanes[i].y * m_vPlanes[i].y) + (m_vPlanes[i].z * m_vPlanes[i].z));
		m_vPlanes[i].x /= length;
		m_vPlanes[i].y /= length;
		m_vPlanes[i].z /= length;
		m_vPlanes[i].w /= length;
	}
}

DirectX::XMFLOAT4X4 CICamera::View( )
{
	return this->m_XMView;
}
DirectX::XMFLOAT4X4 CICamera::Projection( )
{
	return this->m_XMProjection;
}
DirectX::XMFLOAT4X4 CICamera::ViewProjection( )
{
	return this->m_XMViewProjection;
}

DirectX::XMFLOAT4X4 CICamera::InvView( )
{
	return this->m_XMInvView;
}
DirectX::XMFLOAT4X4 CICamera::InvProjection( )
{
	return this->m_XMInvProjection;
}

DirectX::XMFLOAT4X4 CICamera::Ortho( )
{
	return this->m_XMOrthoMatrix;
}

DirectX::XMFLOAT3 CICamera::Position( )
{
	return this->m_XMPosition;
}

std::vector<DirectX::XMFLOAT4>& CICamera::GetFrustumPlanes( )
{
	return m_vPlanes;
}
