/***********************************************
* Filename:  		Scene.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		Container class for objects
*						to eventually be drawn
************************************************/

#include "Scene.h"

//class includes
#include "ICamera.h"
#include "RObject.h"
#include "Magic.h"
#include "Skybox.h"

#include "structs.h"

#include "../Entity Manager/Entity.h"

//c++ includes
#include <algorithm>

#include "Renderer.h"

using namespace MathHelper;

CScene::CScene( )
{ }

CScene::~CScene( )
{
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}

	if (m_pSkybox)
	{
		delete m_pSkybox;
		m_pSkybox = nullptr;
	}

}

void CScene::AddCamera(CICamera* pCamera)
{
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = NULL;
	}

	m_pCamera = pCamera;
}
void CScene::AddSkybox(CSkybox* pSkybox)
{
	if (m_pSkybox)
	{
		delete m_pSkybox;
		m_pSkybox = NULL;
	}

	m_pSkybox = pSkybox;
}

bool sort_depth(/*const*/ CRObject* l, /*const*/ CRObject* r)
{
	DirectX::XMFLOAT3 cam = CScene::GetCamera( )->Position( );
	return (Length(l->GetPosition( ) - cam) > Length(r->GetPosition( ) - cam));
}

bool sort_transparent(/*const*/ CRObject* l, /*const*/ CRObject* r)
{
	DirectX::XMFLOAT3 cam = CScene::GetCamera( )->Position( );
	return (Length(l->GetPosition( ) - cam) < Length(r->GetPosition( ) - cam));
}

void CScene::AddObject(CRObject* pObject)
{
	TCollider* pCollider = pObject->GetCollider( );

	if (pCollider)
	{
		switch (pCollider->type)
		{
			case TCollider::eAABB:
			{
				TAABB* aabb = static_cast<TAABB*>(pCollider);

				DirectX::XMFLOAT3 center = (aabb->vMin + aabb->vMax) * .5f;
				DirectX::XMFLOAT3 extent = (aabb->vMax - aabb->vMin) * .5f;
				if (!CheckRectangle(center, extent))
				{
					return;
				}
			}
			break;

			case TCollider::eSphere:
			{
				TSphere* sphere = static_cast<TSphere*>(pCollider);

				if (!CheckSphere(sphere->vCenter, sphere->fRadius))
				{
					return;
				}
			}
			break;
		}
	}

	CRObject::RObjectType eObjectType = pObject->GetRObjectType( );

	//pre-empt the transparency
	if (pObject->GetColor( ).w < 1.0f)
	{
		eObjectType = CRObject::RObjectType::ROBJECT_TRANSPARENT;
	}

	m_vRenderable[eObjectType].push_back(pObject);

	switch (eObjectType)
	{
		case CRObject::RObjectType::ROBJECT_STATIC:
		{
			std::sort(m_vRenderable[eObjectType].begin( ), m_vRenderable[eObjectType].end( ), sort_depth);
		}
		break;

		case CRObject::RObjectType::ROBJECT_ANIMATED:
		{
			std::sort(m_vRenderable[eObjectType].begin( ), m_vRenderable[eObjectType].end( ), sort_depth);
			m_mInstanced[pObject->GetTag( )].push_back(pObject);
		}
		break;

		case CRObject::RObjectType::ROBJECT_TRANSPARENT:
		{
			std::sort(m_vRenderable[eObjectType].begin( ), m_vRenderable[eObjectType].end( ), sort_transparent);
		}
		break;
	}
}
void CScene::AddLight(const LightParams _pLight)
{
	m_vLights[_pLight.Type].push_back(_pLight);
}

void CScene::Update(DirectX::XMFLOAT3 bindPos, float dt)
{
	if (m_pCamera)
		m_pCamera->Update(bindPos, dt);

	if (m_pSkybox)
		m_pSkybox->Update(dt);

	//if (obj.size( ))
	//{
	//	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(dt * .2f);
	//	DirectX::XMMATRIX translate = DirectX::XMLoadFloat4x4(&obj[0]->GetLocalMatrix( ));
	//	XMStoreFloat4x4(&obj[0]->GetLocalMatrix( ), rotation * translate);
	//}

}

bool CScene::CheckSphere(DirectX::XMFLOAT3 _XMPos, float radius)
{
	const std::vector<DirectX::XMFLOAT4>& planes = m_pCamera->GetFrustumPlanes( );

	// Check if the radius of the sphere is inside the view frustum.
	for (int i = 0; i < 6; i++)
	{
		DirectX::XMFLOAT4 plane = planes[i];

		DirectX::XMVECTOR PlaneDotProduct = DirectX::XMPlaneDotCoord(DirectX::XMLoadFloat4(&plane), DirectX::XMLoadFloat3(&_XMPos));

		DirectX::XMFLOAT3 fPlaneDotProduct; DirectX::XMStoreFloat3(&fPlaneDotProduct, PlaneDotProduct);

		if (fPlaneDotProduct.x < -radius)
		{
			return false;
		}
	}

	return true;
}
bool CScene::CheckRectangle(DirectX::XMFLOAT3 _XMPos, DirectX::XMFLOAT3 _XMSize)
{
	const std::vector<DirectX::XMFLOAT4>& planes = m_pCamera->GetFrustumPlanes( );

	auto fp = [&] (DirectX::XMVECTOR P, DirectX::XMFLOAT3 _XMPoint) -> float
	{
		DirectX::XMFLOAT3 dot = DirectX::XMFLOAT3( );
		DirectX::XMVECTOR V = DirectX::XMLoadFloat3(&_XMPoint);
		DirectX::XMStoreFloat3(&dot, DirectX::XMPlaneDotCoord(P, V));

		return dot.x;
	};

	DirectX::XMFLOAT3 point;
	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for (int i = 0; i < 6; i++)
	{
		DirectX::XMVECTOR P = DirectX::XMLoadFloat4(&planes[i]);

		point = DirectX::XMFLOAT3((_XMPos.x - _XMSize.x), (_XMPos.y - _XMSize.y), (_XMPos.z - _XMSize.z));
		if (fp(P, point) >= 0.0f)
		{
			continue;
		}

		point = DirectX::XMFLOAT3((_XMPos.x + _XMSize.x), (_XMPos.y - _XMSize.y), (_XMPos.z - _XMSize.z));
		if (fp(P, point) >= 0.0f)
		{
			continue;
		}

		point = DirectX::XMFLOAT3((_XMPos.x - _XMSize.x), (_XMPos.y + _XMSize.y), (_XMPos.z - _XMSize.z));
		if (fp(P, point) >= 0.0f)
		{
			continue;
		}

		point = DirectX::XMFLOAT3((_XMPos.x - _XMSize.x), (_XMPos.y - _XMSize.y), (_XMPos.z + _XMSize.z));
		if (fp(P, point) >= 0.0f)
		{
			continue;
		}

		point = DirectX::XMFLOAT3((_XMPos.x + _XMSize.x), (_XMPos.y + _XMSize.y), (_XMPos.z - _XMSize.z));
		if (fp(P, point) >= 0.0f)
		{
			continue;
		}

		point = DirectX::XMFLOAT3((_XMPos.x + _XMSize.x), (_XMPos.y - _XMSize.y), (_XMPos.z + _XMSize.z));
		if (fp(P, point) >= 0.0f)
		{
			continue;
		}

		point = DirectX::XMFLOAT3((_XMPos.x - _XMSize.x), (_XMPos.y + _XMSize.y), (_XMPos.z + _XMSize.z));
		if (fp(P, point) >= 0.0f)
		{
			continue;
		}

		point = DirectX::XMFLOAT3((_XMPos.x + _XMSize.x), (_XMPos.y + _XMSize.y), (_XMPos.z + _XMSize.z));
		if (fp(P, point) >= 0.0f)
		{
			continue;
		}

		//if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((_XMPos.x - _XMSize.x), (_XMPos.y - _XMSize.y), (_XMPos.z - _XMSize.z))) >= 0.0f)
		//{
		//	continue;
		//}
		//
		//if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((_XMPos.x + _XMSize.x), (_XMPos.y - _XMSize.y), (_XMPos.z - _XMSize.z))) >= 0.0f)
		//{
		//	continue;
		//}
		//
		//if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((_XMPos.x - _XMSize.x), (_XMPos.y + _XMSize.y), (_XMPos.z - _XMSize.z))) >= 0.0f)
		//{
		//	continue;
		//}
		//
		//if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((_XMPos.x - _XMSize.x), (_XMPos.y - _XMSize.y), (_XMPos.z + _XMSize.z))) >= 0.0f)
		//{
		//	continue;
		//}
		//
		//if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((_XMPos.x + _XMSize.x), (_XMPos.y + _XMSize.y), (_XMPos.z - _XMSize.z))) >= 0.0f)
		//{
		//	continue;
		//}
		//
		//if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((_XMPos.x + _XMSize.x), (_XMPos.y - _XMSize.y), (_XMPos.z + _XMSize.z))) >= 0.0f)
		//{
		//	continue;
		//}
		//
		//if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((_XMPos.x - _XMSize.x), (_XMPos.y + _XMSize.y), (_XMPos.z + _XMSize.z))) >= 0.0f)
		//{
		//	continue;
		//}
		//
		//if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((_XMPos.x + _XMSize.x), (_XMPos.y + _XMSize.y), (_XMPos.z + _XMSize.z))) >= 0.0f)
		//{
		//	continue;
		//}

		return false;
	}

	return true;
}

/*static*/ CICamera* CScene::m_pCamera;
/*static*/ CSkybox* CScene::m_pSkybox;
