/***********************************************
* Filename:			FastSMG.cpp
* Date:      		06/17/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:			Saboteur enemy fast ranged attack
that deals small amount of damage
************************************************/

#include "stdafx.h"
#include "FastSMG.h"

#include "../State Machine/GameplayState.h"
#include "../Entity Manager/EntityManager.h"
#include "../Audio Manager/AudioSystemWwise.h"

using namespace MathHelper;

CFastSMG::CFastSMG() : CIWeapon(ENEMY_FASTSMG)
{
	m_szTag = "EnemySmg";

	m_nCurrentMagazine = m_nMaxMagazine = 15;
	m_fFireRate = 2.0f;
	m_fDamage = 5.5f;
	m_fSpread = 1.0f;
	m_fTimeToReload = 4.0f;
	m_fReloadTimer = 0;
	//m_nCurrentTier = 1;

	//Set the upgrade costs
	//for (size_t i = 0; i < 7; i++)
	//{
	//	m_nUpgradeCosts[i] = 25 * (i + 1);
	//}
}


CFastSMG::~CFastSMG()
{
}

void CFastSMG::Update(float dt)
{

}

void CFastSMG::UpdateEnemyW(float dt, CEntity* owner)
{
	MyWorld.Local = owner->GetWorldMatrix();

	MyWorld.Local._41 = ((GetZAxis() * 50) + GetPosition()).x;
	MyWorld.Local._42 = ((GetZAxis() * 50) + GetPosition()).y + 90;
	MyWorld.Local._43 = ((GetZAxis() * 50) + GetPosition()).z;

	m_fTotalTime += dt;

	if (m_nCurrentMagazine <= 0)
	{
		//if (!m_bReloading)
		//{
		//	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MACHGUN_RELOAD);
		//}

		m_fReloadTimer += dt;
		int CurrScrap = CGameplayState::GetEntityManager()->GetPlayer()->GetScrapCount();
		if (m_fReloadTimer >= m_fTimeToReload)
		{
			m_fReloadTimer = 0;
			m_nCurrentMagazine = m_nMaxMagazine;
			m_bReloading = false;
			//CGameplayState::GetEntityManager()->GetPlayer()->SetScrapCount(CurrScrap - m_nScrapCost);
		}
	}
}

bool CFastSMG::Shoot()
{
	if (m_nCurrentMagazine > 0)
	{
		if (m_fTotalTime > m_fLastFireTime + 1 / m_fFireRate)
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MACHGUN_SHOOT);

			if (CGameplayState::GetEntityManager()->CycleIntoLiveList(CEntity::eENEMYBULLET, DirectX::XMFLOAT3(0, 0, 0), this))
			{
				m_nCurrentMagazine--;
			}

			m_fLastFireTime = m_fTotalTime;
			return true;
		}
	}
	else
	{
		m_bReloading = true;
	}
	return false;
}

void CFastSMG::UpgradeWeaponTier()
{
//	//We've capped out
//	if (m_nCurrentTier == m_nMaxTier)
//	{
//		return;
//	}
//
//	m_nCurrentTier++;
//
//	//TODO: Add unique bonuses for each level
//	switch (m_nCurrentTier)
//	{
//		//Level 1 unlocks the weapon
//	case 1:
//	{
//		m_fDamage += 5;
//	}
//	break;
//
//	case 2:
//	{
//		m_fFireRate += 0.5f;
//	}
//	break;
//
//	case 3:
//	{
//		m_fDamage += 5;
//	}
//	break;
//
//	case 4:
//	{
//		m_fFireRate += 0.5f;
//	}
//	break;
//
//	case 5:
//	{
//		m_fDamage += 5;
//	}
//	break;
//
//	case 6:
//	{
//		m_fFireRate += 0.5f;
//	}
//	break;
//
//	case 7:
//	{
//		m_fDamage += 5;
//	}
//	break;
//
//	default:
//	{
//
//	}
//	break;
//	}
}
