#include "stdafx.h"
#include "Radar.h"

#include "Renderer\Scene.h"
#include "Renderer\ICamera.h"
#include "Renderer\ThirdPersonCamera.h"
#include "Renderer\Renderer.h"
#include "Renderer\Texture2D.h"
#include "Renderer\HUDRenderer.h"
#include "GUI Manager\Rectangle.h"
#include "GUI Manager\GUIManager.h"
#include "State Machine\GameplayState.h"
#include "Entity Manager\EntityManager.h"

#define NDC_OFFSET_X 0.68f
#define NDC_OFFSET_Y 0.79f

#define BASE_BACKGROUND_ALPHA 0.75f
#define BASE_ICON_ALPHA 1.0f

CRadar::CRadar()
{
	m_pTexture = new CTexture2D(0, 0, 300, 300, L"Assets/Images/RadarBackground.png");
	m_pTexture->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.75f });
	
	// Radar Background Texture
	CRectangle rect = { 0.f, 210, 0.f, 230 };
	rect = CGUIManager::ComputeBounds(rect);
	m_pTexture->SetWidth(rect.ComputeWidth());
	m_pTexture->SetHeight(rect.ComputeHeight());
	m_pTexture->SetScreenPosition(-0.79f, -0.7f);

	// Creep Blimp Texture
	m_pCreepTexture = new CTexture2D(0, 0, 10, 10, L"Assets/Images/RadarCreep.png");
	rect = { 0.f, 12.5f, 0.f, 12.5f };
	rect = CGUIManager::ComputeBounds(rect);
	m_pCreepTexture->SetWidth(rect.ComputeWidth());
	m_pCreepTexture->SetHeight(rect.ComputeHeight());
	m_pCreepTexture->SetColor({ 1, 0, 0, 1 });

	// Hunter Blimp Texture
	m_pHuntTexture = new CTexture2D(0, 0, 10, 10, L"Assets/Images/RadarHunter.png");
	rect = { 0.f, 12.5f, 0.f, 12.5f };
	rect = CGUIManager::ComputeBounds(rect);
	m_pHuntTexture->SetWidth(rect.ComputeWidth());
	m_pHuntTexture->SetHeight(rect.ComputeHeight());
	m_pHuntTexture->SetColor({ 1, 0, 0, 1 });

	// Saboteur Blimp Texture
	m_pSaboTexture = new CTexture2D(0, 0, 10, 10, L"Assets/Images/RadarSaboteur.png");
	rect = { 0.f, 12.5f, 0.f, 12.5f };
	rect = CGUIManager::ComputeBounds(rect);
	m_pSaboTexture->SetWidth(rect.ComputeWidth());
	m_pSaboTexture->SetHeight(rect.ComputeHeight());
	m_pSaboTexture->SetColor({ 1, 0, 0, 1 });

	// LifeSupport Blimp Texture
	m_pLifeSupTexture = new CTexture2D(0, 0, 10, 10, L"Assets/Images/RadarLifeSupport.png");
	rect = { 0.f, 15.0f, 0.f, 15.0f };
	rect = CGUIManager::ComputeBounds(rect);
	m_pLifeSupTexture->SetWidth(rect.ComputeWidth());
	m_pLifeSupTexture->SetHeight(rect.ComputeHeight());
	m_pLifeSupTexture->SetColor({ 1, 1, 1, 1 });

	// Engines Blimp Texture
	m_pEnginesTexture = new CTexture2D(0, 0, 10, 10, L"Assets/Images/RadarEngines.png");
	rect = { 0.f, 15.0f, 0.f, 15.0f };
	rect = CGUIManager::ComputeBounds(rect);
	m_pEnginesTexture->SetWidth(rect.ComputeWidth());
	m_pEnginesTexture->SetHeight(rect.ComputeHeight());
	m_pEnginesTexture->SetColor({ 1, 1, 1, 1 });

	// Reactor Blimp Texture
	m_pReactorTexture = new CTexture2D(0, 0, 10, 10, L"Assets/Images/RadarReactor.png");
	rect = { 0.f, 15.0f, 0.f, 15.0f };
	rect = CGUIManager::ComputeBounds(rect);
	m_pReactorTexture->SetWidth(rect.ComputeWidth());
	m_pReactorTexture->SetHeight(rect.ComputeHeight());
	m_pReactorTexture->SetColor({ 1, 1, 1, 1 });

	m_bVisible = false;
	m_bHaveSystems = false;

	m_pPlayer = CGameplayState::GetEntityManager()->GetPlayer();
	m_fRange = 1500;
}


CRadar::~CRadar()
{
	SafeDelete(m_pTexture);

	SafeDelete(m_pCreepTexture);
	SafeDelete(m_pHuntTexture);
	SafeDelete(m_pSaboTexture);

	SafeDelete(m_pLifeSupTexture);
	SafeDelete(m_pReactorTexture);
	SafeDelete(m_pEnginesTexture);

}

void CRadar::Update(float dt)
{
	// First: Get all the enemies
	FillEnemyList();

	// Second Get all the subsystems once
	// NOTE: have to do it this way because the hud stuff is loaded before the subsystems are created
	if (m_bHaveSystems == false)
	{
		FillSubsystems();
		m_bHaveSystems = true;
	}

//	Render();
}

using namespace DirectX;
void CRadar::Render()
{
	if (m_pTexture && m_bVisible)
	{
		CHUDRenderer::Render2D(m_pTexture);

		for (size_t i = 0; i < m_lpEnemyList.size(); i++)
		{
			XMFLOAT3 temp = m_lpEnemyList[i]->GetPosition();// -m_pPlayer->GetPosition();
			//XMVECTOR tempVector = XMLoadFloat3(&temp);
			//tempVector = XMVector3Length(tempVector);
			//XMStoreFloat3(&temp, tempVector);
			//
			//float Distance = temp.x;

			float Distance = XMVectorGetX(XMVector3LengthSq(
				XMLoadFloat3(&(m_lpEnemyList[i]->GetPosition() - m_pPlayer->GetPosition()))));
			if (Distance <= (m_fRange * m_fRange))
			{
				// so at this point the lowest/highest that X and Z can be is -1000/+1000
				// the radar on the screen is obviously not 2000 units wide
				// for now i'm just gonna try and get something rendering on top of the enemies that are in range.
				
				D3D11_VIEWPORT viewport = CRenderer::GetReference().GetViewport();
				auto view = XMMatrixLookAtLH(XMLoadFloat3(&(m_pPlayer->GetPosition() + static_cast<CThirdPersonCamera*>(CScene::GetCamera())->GetPositionOffset())),
											 XMLoadFloat3(&m_pPlayer->GetPosition()),
											 g_XMIdentityR1);
				auto proj = XMMatrixPerspectiveFovLH(1.04f, viewport.Width / viewport.Height, viewport.MinDepth, viewport.MaxDepth);

				XMFLOAT4 WorldPos = XMFLOAT4(temp.x, 0, temp.z, 1.0f);
				XMVECTOR tempVector = XMVector4Transform(XMLoadFloat4(&WorldPos), view);
				tempVector = XMVector4Transform(tempVector, proj);

				XMFLOAT4 ClipSpacePos = { 0, 0, 0, 1 };
				XMStoreFloat4(&ClipSpacePos, tempVector);

				XMFLOAT3 ndcSpace = XMFLOAT3(ClipSpacePos.x / ClipSpacePos.w,
											 ClipSpacePos.y / ClipSpacePos.w,
											 ClipSpacePos.z / ClipSpacePos.w);

				if ((ndcSpace.y - NDC_OFFSET_Y) > -2.0f && (ndcSpace.x - NDC_OFFSET_X) < 0.9f &&
					(ndcSpace.y - NDC_OFFSET_Y) < 0.25f && (ndcSpace.x - NDC_OFFSET_X) > -2.25f)
				{
					if (m_lpEnemyList[i]->GetEntityType() == CEntity::eCREEP)
					{
						m_pCreepTexture->SetScreenPosition((ndcSpace.x * 0.100f) - NDC_OFFSET_Y,
														   (ndcSpace.y * 0.225f) - NDC_OFFSET_X);

						CHUDRenderer::Render2D(m_pCreepTexture);
					}
					else if (m_lpEnemyList[i]->GetEntityType() == CEntity::eHUNTER)
					{
						m_pHuntTexture->SetScreenPosition((ndcSpace.x * 0.100f) - NDC_OFFSET_Y,
														  (ndcSpace.y * 0.225f) - NDC_OFFSET_X);
						CHUDRenderer::Render2D(m_pHuntTexture);
					}
					else if (m_lpEnemyList[i]->GetEntityType() == CEntity::eSABOTEUR)
					{
						m_pSaboTexture->SetScreenPosition((ndcSpace.x * 0.100f) - NDC_OFFSET_Y,
														  (ndcSpace.y * 0.225f) - NDC_OFFSET_X);
						CHUDRenderer::Render2D(m_pSaboTexture);
					}
				}
				
			}
		}

		// Rendering the subsystem radar thingy
		SubSystemRadarRender(m_vLifeSup, 1);
		SubSystemRadarRender(m_vEngines, 2);
		SubSystemRadarRender(m_vReactor, 3);

	}

}

void CRadar::ClearRadar()
{
	m_lpEnemyList.clear();
	m_bHaveSystems = false;
}

#include <list>
void CRadar::FillEnemyList()
{
	m_lpEnemyList.clear();
	// Pointer to the EntityManager
	CEntityManager* PtrManager = CGameplayState::GetEntityManager();
	
	// Getting all the creeps into the list
	std::list<CCreepEnemy*> tempCreeps = PtrManager->GetAllCreeps();
	auto CreepIter = tempCreeps.begin();
	for (CreepIter; CreepIter != tempCreeps.end(); CreepIter++)
	{
		m_lpEnemyList.push_back((*CreepIter));
	}

	std::list<CHunterEnemy*> tempHunters = PtrManager->GetAllHunters();
	auto HunterIter = tempHunters.begin();
	for (HunterIter; HunterIter != tempHunters.end(); HunterIter++)
	{
		m_lpEnemyList.push_back((*HunterIter));
	}

	std::list<CSaboteurEnemy*> tempSabos = PtrManager->GetAllSaboteur();
	auto SaboIter = tempSabos.begin();
	for (SaboIter; SaboIter != tempSabos.end(); SaboIter++)
	{
		m_lpEnemyList.push_back((*SaboIter));
	}
}

void CRadar::FillSubsystems()
{
	CEntityManager* PtrManager = CGameplayState::GetEntityManager();
	
	std::list<CSubSystem*> tempSystems = PtrManager->GetSubSystems();
	auto SystemIter = tempSystems.begin();
	for (SystemIter; SystemIter != tempSystems.end(); SystemIter++)
	{
		if ((*SystemIter)->GetSystemType() == eSubsystemtype::eSystemLife)
		{
			m_vLifeSup = (*SystemIter)->GetPosition();
		}
		else if ((*SystemIter)->GetSystemType() == eSubsystemtype::eSystemReactor)
		{
			m_vReactor = (*SystemIter)->GetPosition();
		}
		else if ((*SystemIter)->GetSystemType() == eSubsystemtype::eSystemEngine)
		{
			m_vEngines = (*SystemIter)->GetPosition();
		}
	}
}

void CRadar::SubSystemRadarRender(XMFLOAT3 SubSysPos, int WhichOne)
{
	float Distance = XMVectorGetX(XMVector3LengthSq(
		XMLoadFloat3(&(SubSysPos - m_pPlayer->GetPosition()))));
	if (Distance <= (m_fRange * m_fRange))
	{
		D3D11_VIEWPORT viewport = CRenderer::GetReference().GetViewport();
		auto view = XMMatrixLookAtLH(XMLoadFloat3(&(m_pPlayer->GetPosition() + static_cast<CThirdPersonCamera*>(CScene::GetCamera())->GetPositionOffset())),
									 XMLoadFloat3(&m_pPlayer->GetPosition()),
									 g_XMIdentityR1);
		auto proj = XMMatrixPerspectiveFovLH(1.04f, viewport.Width / viewport.Height, viewport.MinDepth, viewport.MaxDepth);
		
		XMFLOAT4 WorldPos = XMFLOAT4(SubSysPos.x, 0, SubSysPos.z, 1.0f);
		XMVECTOR tempVector = XMVector4Transform(XMLoadFloat4(&WorldPos), view);
		tempVector = XMVector4Transform(tempVector, proj);

		XMFLOAT4 ClipSpacePos = { 0, 0, 0, 1 };
		XMStoreFloat4(&ClipSpacePos, tempVector);

		XMFLOAT3 ndcSpace = XMFLOAT3(ClipSpacePos.x / ClipSpacePos.w,
									 ClipSpacePos.y / ClipSpacePos.w,
									 ClipSpacePos.z / ClipSpacePos.w);

		if (ndcSpace.y - NDC_OFFSET_Y > -2.0f && (ndcSpace.x - NDC_OFFSET_X) < 0.9f && 
			(ndcSpace.y - NDC_OFFSET_Y) < 0.25f && (ndcSpace.x - NDC_OFFSET_X) > -2.25f)
		{
			if (WhichOne == 1) // Life Support
			{
				m_pLifeSupTexture->SetScreenPosition((ndcSpace.x * 0.100f) - NDC_OFFSET_Y,
													 (ndcSpace.y * 0.225f) - NDC_OFFSET_X);
				CHUDRenderer::Render2D(m_pLifeSupTexture);
			}
			else if (WhichOne == 2) // Engines
			{
				m_pEnginesTexture->SetScreenPosition((ndcSpace.x * 0.100f) - NDC_OFFSET_Y,
													 (ndcSpace.y * 0.225f) - NDC_OFFSET_X);
				CHUDRenderer::Render2D(m_pEnginesTexture);
			}
			else if (WhichOne == 3) // Reactor
			{
				m_pReactorTexture->SetScreenPosition((ndcSpace.x * 0.100f) - NDC_OFFSET_Y,
													 (ndcSpace.y * 0.225f) - NDC_OFFSET_X);
				CHUDRenderer::Render2D(m_pReactorTexture);
			}
		}
	}
}

void CRadar::SetTransparency(float fAlpha)
{
	fAlpha = fmin(fmax(fAlpha, 0.0f), 1.0f);
	DirectX::XMFLOAT4 xmColor;
	xmColor = m_pTexture->GetColor();
	xmColor.w = BASE_BACKGROUND_ALPHA * fAlpha;
	m_pTexture->SetColor(xmColor);
	//Icons
	xmColor = m_pCreepTexture->GetColor();
	xmColor.w = BASE_ICON_ALPHA * fAlpha;
	m_pCreepTexture->SetColor(xmColor);

	xmColor = m_pHuntTexture->GetColor();
	xmColor.w = BASE_ICON_ALPHA * fAlpha;
	m_pHuntTexture->SetColor(xmColor);

	xmColor = m_pSaboTexture->GetColor();
	xmColor.w = BASE_ICON_ALPHA * fAlpha;
	m_pSaboTexture->SetColor(xmColor);

	xmColor = m_pReactorTexture->GetColor();
	xmColor.w = BASE_ICON_ALPHA * fAlpha;
	m_pReactorTexture->SetColor(xmColor);

	xmColor = m_pLifeSupTexture->GetColor();
	xmColor.w = BASE_ICON_ALPHA * fAlpha;
	m_pLifeSupTexture->SetColor(xmColor);

	xmColor = m_pEnginesTexture->GetColor();
	xmColor.w = BASE_ICON_ALPHA * fAlpha;
	m_pEnginesTexture->SetColor(xmColor);


}