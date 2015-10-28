/***********************************************
* Filename:			PickUp.h
* Date:      		06/3/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Gergar
* Purpose:			An entity that represents
*				objects that are destroyed when
*				colliding with the player that
*				bestow bonuses
************************************************/
#include "stdafx.h"
#include "PickUp.h"
#include "../../Renderer/DebugRenderer.h"

using namespace MathHelper;

#define SCRAP_MAGNET_RANGE 300.0f
//It will optimize itself
#define SCRAP_MAGNET_RANGE_SQ SCRAP_MAGNET_RANGE * SCRAP_MAGNET_RANGE
#define SCRAP_MAGNET_SPEED 8.0f

CPickUp::CPickUp(CEntity* pPlayer)
{
	//If other pick-ups are added, will have to change tag elsewhere
	SetTag("PickUp");
	tCollider = new TSphere( );
	tCollider->type = TCollider::eSphere;
	//Fine tune the collision
	((TSphere*)tCollider)->fRadius = 25.0f;
	((TSphere*)tCollider)->vCenter = DirectX::XMFLOAT3(0, 0, 0);
	//Default to disabled
	m_fLifeLived = m_fLifeTime;
	m_pPlayer = pPlayer;

	m_pLightParams = new LightParams({ 0.0f, 0.0f, 0.0f }, { 1, 1, 1 }, 175);
}


CPickUp::~CPickUp( )
{
	SafeDelete(m_pLightParams);
}

void CPickUp::Update(float fDt)
{
	m_fLifeLived += fDt;

#if _DEBUG
	CDebugRenderer::GetReference( ).CircleRenderer(GetPosition( ), { ((TSphere*)tCollider)->fRadius, ((TSphere*)tCollider)->fRadius }, DirectX::XMFLOAT4(1, 1, 0, 1));
#endif
	//Move towards the player
	DirectX::XMFLOAT3 toPlayer = m_pPlayer->GetPosition( ) - GetPosition( );

	m_pLightParams->Position = GetPosition( );
	m_pLightParams->Position.y += 50;

	m_pLightParams->Color = DirectX::XMFLOAT3(m_XMColor.x, m_XMColor.y, m_XMColor.z) * 5.0f;
	m_XMColor.w = 1.0f;

	DirectX::XMStoreFloat4x4(&MyWorld.Local,
							 DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(fDt * 150.0f)) * DirectX::XMLoadFloat4x4(&MyWorld.Local));

	DirectX::XMStoreFloat4x4(&MyWorld.Local,
							 DirectX::XMLoadFloat4x4(&MyWorld.Local) * (DirectX::XMMatrixTranslation(0, sin(m_fLifeLived) * fDt * 50.0f, 0)));

	DebugPrint(std::string(std::to_string(GetPosition( ).y) + "\n").c_str( ));

	if (SquareLength(toPlayer) <= SCRAP_MAGNET_RANGE_SQ)
	{
		m_fPercentSpeed += fDt * 3.5f;
		SetPosition(GetPosition( ) + toPlayer * fDt * m_fPercentSpeed * SCRAP_MAGNET_SPEED);
		((TSphere*)tCollider)->vCenter = GetPosition( );
	}
}

void CPickUp::Enable(DirectX::XMFLOAT3 xmSpawnPoint, eEnemyVariant variant)
{
	DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixRotationX(90));

	SetPosition(xmSpawnPoint);
	((TSphere*)tCollider)->vCenter = xmSpawnPoint;
	m_fLifeLived = 0.0f;
}

void CPickUp::Disable( )
{
	//Add set it above its lifetime, so it despawns
	m_fLifeLived = m_fLifeTime + 1;
	m_fPercentSpeed = 0.0f;
}