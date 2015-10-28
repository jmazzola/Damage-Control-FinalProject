/***********************************************
* Filename:			MachineGun.h
* Date:      		05/15/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Brandon Lucier
* Purpose:			MachineGun represents the properties
*				one of several weapons that the player
*				can use
************************************************/

#include "stdafx.h"
#include "ShotGun.h"

#include "../Input Manager/InputManager.h"
#include "../Input Manager/KeyBindings.h"

#include "../State Machine/GameplayState.h"
#include "../Entity Manager/EntityManager.h"
#include "../Audio Manager/AudioSystemWwise.h"
#include "../TinyXML2/tinyxml2.h"
#include "ModBank.h"

#pragma region Shotgun Upgrade Stats

#define SHOTGUN_UNLOCKED	false

#define SHOTGUN_DAMAGE		5.0f
#define SHOTGUN_FIRERATE	0.9f
#define SHOTGUN_RELOADTIME	2.0f
#define SHOTGUN_SPREAD		10.0f
#define SHOTGUN_BULLETLIFE	5.0f
#define SHOTGUN_MAGSIZE		6
#define SHOTGUN_PELLETCNT	14
#define SHOTGUN_BULLETCOST	5
#define SHOTGUN_WEAPONCOST	100
#define SHOTGUN_RELOADSPEED 1

#define SHOTGUN_MODLIMIT	3

#pragma endregion

CShotGun::CShotGun( ) : CIWeapon(GUN_SCATTER)
{
	m_szTag = "ShotGun";

	//m_XMBarrel = { 0, 3.5, 57 };
	Reset( );
}


CShotGun::~CShotGun( )
{
}


void CShotGun::Update(float dt)
{
	if (!CGameplayState::GetEntityManager()->GetPlayer()->GetAlive())
		return;

	m_fTotalTime += dt;

	if (m_bFiring == false && CInputManager::GetInstance( )->IsButtonDown(Keybindings::PrimaryFire))
	{
		BitTwiddler::TurnOnBit(CGameplayState::GetEntityManager()->GetPlayer()->GetActionBitfield(), IS_ATTACKING);
		m_bFiring = true;
	}
	if ( (m_bFiring == true && CInputManager::GetInstance()->IsButtonUp(Keybindings::PrimaryFire) ) || m_bReloading)
	{
		BitTwiddler::TurnOffBit(CGameplayState::GetEntityManager()->GetPlayer()->GetActionBitfield(), IS_ATTACKING);
		m_bFiring = false;
	}

	{
		int CurrScrap = CGameplayState::GetEntityManager( )->GetPlayer( )->GetScrapCount( );
		if (m_bReloading || ((m_nCurrentMagazine <= 0 || m_bToReload) && CurrScrap >= GetBulletCost( )))
		{
			//Only activates the first frame of reloading
			if (!m_bReloading)
			{
				m_bReloading = true;
				AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_2D_SHOTGUN_RELOAD);
				//Full reload - the whole clip
				if ((CurrScrap - GetReloadCost( )) >= 0)
				{
					CGameplayState::GetEntityManager( )->GetPlayer( )->SubScrap(GetReloadCost( ));
					m_nCurrentMagazine = m_nMaxMagazine;
				}
				//Can only afford a few bullets
				else
				{
					//Will never divide by zero, Calm down
					CGameplayState::GetEntityManager( )->GetPlayer( )->SetScrapCount(CurrScrap % GetBulletCost( ));
					m_nCurrentMagazine += CurrScrap / GetBulletCost( );
				}
			}
			m_fReloadTimer += dt * m_fReloadSpeed;

			if (m_fReloadTimer >= m_fTimeToReload)
			{
				m_fReloadTimer = 0;
				m_bReloading = false;
				AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::STOP_2D_SHOTGUN_RELOAD);
			}
		}
	}


	if (m_bFiring && dt && !m_bReloading && m_nCurrentMagazine)
	{
		Shoot( );
	}
	else
	{
		DirectX::XMFLOAT4 c = m_pFlash->GetColor( );
		m_pFlash->SetColor({ c.x, c.y, c.z, 0.0f });
		m_pLightParams->Range = 0;
	}

	CIWeapon::Update(dt);
}

bool CShotGun::Shoot( )
{
	if (m_fTotalTime > m_fLastFireTime + 1 / m_fFireRate)
	{
		AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_2D_SHOTGUN_SHOOT);

		DirectX::XMFLOAT4 c = m_pFlash->GetColor( );
		m_pFlash->SetColor({ c.x, c.y, c.z, .99f });
		m_pLightParams->Range = 225;

		for (int i = 0; i < m_nPelletCount; i++)
		{
			// TODO BULLET SPREAD
			CGameplayState::GetEntityManager( )->CycleIntoLiveList(CEntity::eBULLET, DirectX::XMFLOAT3(0, 0, 0), this);
		}
		m_nCurrentMagazine--;
		m_fLastFireTime = m_fTotalTime;
		return true;
	}

	DirectX::XMFLOAT4 c = m_pFlash->GetColor( );
	m_pFlash->SetColor({ c.x, c.y, c.z, 0.0f });
	m_pLightParams->Range = 0;
	return false;
}

/*
void CShotGun::UpgradeWeaponTier()
{
//We've capped out
if (m_nCurrentTier == MAX_WEAPON_LEVEL)
{
return;
}

m_nCurrentTier++;

//Add unique bonuses for each level
switch (m_nCurrentTier)
{
//Level 0 unlocks the weapon
case 0:
{
m_fDamage = SHOTGUN_LVL0_DAMAGE;
m_fFireRate = SHOTGUN_LVL0_FIRERATE;
m_fReloadSpeed = SHOTGUN_LVL0_RELOADTIME;
m_fSpread = SHOTGUN_LVL0_SPREAD;
m_fBulletLifeTime = SHOTGUN_LVL0_BULLETLIFE;
m_nCurrentMagazine = SHOTGUN_LVL0_MAGSIZE;
m_nMaxMagazine = SHOTGUN_LVL0_MAGSIZE;
m_nPelletCount = SHOTGUN_LVL0_PELLETCNT;
m_uSpecialFlag = SHOTGUN_LVL0_SPECIAL;
}
break;

case 1:
{
m_fDamage = SHOTGUN_LVL1_DAMAGE;
m_fFireRate = SHOTGUN_LVL1_FIRERATE;
m_fReloadSpeed = SHOTGUN_LVL1_RELOADTIME;
m_fSpread = SHOTGUN_LVL1_SPREAD;
m_fBulletLifeTime = SHOTGUN_LVL1_BULLETLIFE;
m_nCurrentMagazine = SHOTGUN_LVL1_MAGSIZE;
m_nMaxMagazine = SHOTGUN_LVL1_MAGSIZE;
m_nPelletCount = SHOTGUN_LVL1_PELLETCNT;
m_uSpecialFlag = SHOTGUN_LVL1_SPECIAL;
}
break;

case 2:
{
m_fDamage = SHOTGUN_LVL2_DAMAGE;
m_fFireRate = SHOTGUN_LVL2_FIRERATE;
m_fReloadSpeed = SHOTGUN_LVL2_RELOADTIME;
m_fSpread = SHOTGUN_LVL2_SPREAD;
m_fBulletLifeTime = SHOTGUN_LVL2_BULLETLIFE;
m_nCurrentMagazine = SHOTGUN_LVL2_MAGSIZE;
m_nMaxMagazine = SHOTGUN_LVL2_MAGSIZE;
m_nPelletCount = SHOTGUN_LVL2_PELLETCNT;
m_uSpecialFlag = SHOTGUN_LVL2_SPECIAL;
}
break;

case 3:
{
m_fDamage = SHOTGUN_LVL3_DAMAGE;
m_fFireRate = SHOTGUN_LVL3_FIRERATE;
m_fReloadSpeed = SHOTGUN_LVL3_RELOADTIME;
m_fSpread = SHOTGUN_LVL3_SPREAD;
m_fBulletLifeTime = SHOTGUN_LVL3_BULLETLIFE;
m_nCurrentMagazine = SHOTGUN_LVL3_MAGSIZE;
m_nMaxMagazine = SHOTGUN_LVL3_MAGSIZE;
m_nPelletCount = SHOTGUN_LVL3_PELLETCNT;
m_uSpecialFlag = SHOTGUN_LVL3_SPECIAL;
}
break;

//case 4:
//{
//	m_fDamage += 5;
//}
//break;
//
//case 5:
//{
//	m_fFireRate += 0.30f;
//}
//break;
//
//case 6:
//{
//	m_fDamage += 5;
//}
//break;
//
//case 7:
//{
//	m_fDamage += 5;
//}
//break;

default:
{
m_fDamage =
m_fFireRate =
m_fReloadSpeed =
m_fSpread =
m_fBulletLifeTime = 0;
m_nCurrentMagazine =
m_nMaxMagazine =
m_nPelletCount =
m_uSpecialFlag = 0;
}
break;
}
}
*/

void CShotGun::UpdateEnemyW(float dt, CEntity* owner)
{
}

void CShotGun::Reset(void)
{
	//Load stats from on file, or use #defines
	if (s_fPresetDamage == 0)
	{
		LoadStats( );
	}

	m_bUnlocked = SHOTGUN_UNLOCKED;

	m_fBulletLifeTime = SHOTGUN_BULLETLIFE;
	m_nBulletScrapCost = s_nPresetBulletCost;
	m_nMaxMagazine = s_nPresetMagSize;
	m_fDamage = s_fPresetDamage;
	m_fFireRate = s_fPresetFireRate;
	m_fSpread = s_fPresetSpread;
	m_fTimeToReload = s_fPresetReloadTime;
	m_fReloadSpeed = s_fPresetReloadSpeed;
	m_nPelletCount = s_nPresetPelletCount;
	if (m_pModules == nullptr)
	{
		m_pModules = new CModBank(EMOD_WEAPON);
	}
	m_pModules->SetBankSize(s_nPresetModLimit);

	//Resets modules, reload clip
	CIWeapon::Reset( );
}

bool CShotGun::LoadStats(void)
{
	tinyxml2::XMLDocument xmlDoc;

	//TODO: Load stats based on difficulty
	xmlDoc.LoadFile("Assets//XML//Balance//ShotgunStats.xml");

	//Did not open file properly
	if (xmlDoc.Error( ))
	{
		//Load in the default values
		s_nPresetBulletCost = SHOTGUN_BULLETCOST;
		s_nPresetMagSize = SHOTGUN_MAGSIZE;
		s_fPresetDamage = SHOTGUN_DAMAGE;
		s_fPresetFireRate = SHOTGUN_FIRERATE;
		s_fPresetSpread = SHOTGUN_SPREAD;
		s_fPresetReloadTime = SHOTGUN_RELOADTIME;
		s_fPresetReloadSpeed = SHOTGUN_RELOADSPEED;
		s_nPresetPelletCount = SHOTGUN_PELLETCNT;

		xmlDoc.Clear( );
		return false;
	}
	tinyxml2::XMLElement* xmlHead = xmlDoc.RootElement( );
	tinyxml2::XMLElement* xmlShoulder = nullptr;

	xmlShoulder = xmlHead->FirstChildElement("Stats");
	if (xmlShoulder)
	{
		if (xmlShoulder->Attribute("Damage"))
			s_fPresetDamage = (float)atof(xmlShoulder->Attribute("Damage"));
		else
			s_fPresetDamage = SHOTGUN_DAMAGE;
		if (xmlShoulder->Attribute("FireRate"))
			s_fPresetFireRate = (float)atof(xmlShoulder->Attribute("FireRate"));
		else
			s_fPresetFireRate = SHOTGUN_FIRERATE;
		if (xmlShoulder->Attribute("ReloadTime"))
			s_fPresetReloadTime = (float)atof(xmlShoulder->Attribute("ReloadTime"));
		else
			s_fPresetReloadTime = SHOTGUN_RELOADTIME;
		if (xmlShoulder->Attribute("ReloadSpeed"))
			s_fPresetReloadSpeed = (float)atof(xmlShoulder->Attribute("ReloadSpeed"));
		else
			s_fPresetReloadSpeed = SHOTGUN_RELOADSPEED;
		if (xmlShoulder->Attribute("Spread"))
			s_fPresetSpread = (float)atof(xmlShoulder->Attribute("Spread"));
		else
			s_fPresetSpread = SHOTGUN_SPREAD;
		if (xmlShoulder->Attribute("Magazine"))
			s_nPresetMagSize = (int)atoi(xmlShoulder->Attribute("Magazine"));
		else
			s_nPresetMagSize = SHOTGUN_MAGSIZE;
		if (xmlShoulder->Attribute("Pellets"))
			s_nPresetPelletCount = (int)atoi(xmlShoulder->Attribute("Pellets"));
		else
			s_nPresetPelletCount = SHOTGUN_PELLETCNT;
	}
	else
	{
		s_fPresetDamage = SHOTGUN_DAMAGE;
		s_fPresetFireRate = SHOTGUN_FIRERATE;
		s_fPresetReloadTime = SHOTGUN_RELOADTIME;
		s_fPresetSpread = SHOTGUN_SPREAD;
		s_nPresetMagSize = SHOTGUN_MAGSIZE;
		s_nPresetPelletCount = SHOTGUN_PELLETCNT;
	}
	//Price things
	xmlShoulder = xmlHead->FirstChildElement("Price");
	if (xmlShoulder)
	{
		if (xmlShoulder->Attribute("WeaponCost"))
			s_nPresetWeaponCost = (int)atoi(xmlShoulder->Attribute("WeaponCost"));
		else
			s_nPresetWeaponCost = SHOTGUN_WEAPONCOST;
		if (xmlShoulder->Attribute("BulletCost"))
			s_nPresetBulletCost = (int)atoi(xmlShoulder->Attribute("BulletCost"));
		else
			s_nPresetBulletCost = SHOTGUN_BULLETCOST;
	}
	else
	{
		s_nPresetWeaponCost = SHOTGUN_WEAPONCOST;
		s_nPresetBulletCost = SHOTGUN_BULLETCOST;
	}
	//Allocate a mod bank if there isn't one
	if (!m_pModules)
	{
		m_pModules = new CModBank(EMOD_WEAPON);
	}
	//Get module information
	xmlShoulder = xmlHead->FirstChildElement("Mods");
	if (xmlShoulder)
	{
		if (xmlShoulder->Attribute("ModLimit"))
			s_nPresetModLimit = (int)atoi(xmlShoulder->Attribute("ModLimit"));
		else
			s_nPresetModLimit = (SHOTGUN_MODLIMIT);
	}
	else
	{
		m_pModules->SetBankSize(SHOTGUN_MODLIMIT);
	}

	//We're done here
	xmlDoc.Clear( );
	return true;
}

float	CShotGun::s_fPresetDamage;
float	CShotGun::s_fPresetFireRate;
float	CShotGun::s_fPresetReloadTime;
float	CShotGun::s_fPresetReloadSpeed;
float	CShotGun::s_fPresetSpread;
int		CShotGun::s_nPresetMagSize;
int		CShotGun::s_nPresetPelletCount;
int		CShotGun::s_nPresetWeaponCost;
int		CShotGun::s_nPresetBulletCost;
int		CShotGun::s_nPresetModLimit;