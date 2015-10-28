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
#include "MachineGun.h"

#include "../Input Manager/InputManager.h"
#include "../Input Manager/KeyBindings.h"

#include "../State Machine/GameplayState.h"
#include "../Entity Manager/EntityManager.h"
#include "../Audio Manager/AudioSystemWwise.h"
#include "../TinyXML2/tinyxml2.h"
#include "ModBank.h"
#include "../Entity Manager/Entities/Player.h"

#pragma region Machine Gun Upgrade Stats

#define MACHGUN_UNLOCKED	false

#define MACHGUN_DAMAGE		16.0f
#define MACHGUN_FIRERATE	4.0f
#define MACHGUN_RELOADTIME	1.2f
#define MACHGUN_SPREAD		4.5f
#define MACHGUN_BULLETLIFE	5.0f
#define MACHGUN_MAGSIZE		60
#define MACHGUN_BULLETCOST	1
#define MACHGUN_WEAPONCOST	80
#define MACHGUN_RELOADSPEED 1

#define MACHGUN_MODLIMIT	3

#pragma endregion

CMachineGun::CMachineGun( ) : CIWeapon(GUN_FULL_AUTO)
{
	m_szTag = "MachineGun";

	//m_XMBarrel = { 0, 3.5, 42 };
	Reset( );
}


CMachineGun::~CMachineGun( )
{
}


void CMachineGun::Update(float dt)
{
	if (!CGameplayState::GetEntityManager()->GetPlayer()->GetAlive())
		return;

	m_fTotalTime += dt;

	if (m_bFiring == false && CInputManager::GetInstance( )->IsButtonDown(Keybindings::PrimaryFire))
	{
		BitTwiddler::TurnOnBit(CGameplayState::GetEntityManager()->GetPlayer()->GetActionBitfield(), IS_ATTACKING);
		m_bFiring = true;
	}
	if ((m_bFiring == true && CInputManager::GetInstance()->IsButtonUp(Keybindings::PrimaryFire)) || m_bReloading)
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
				AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_2D_MACHGUN_RELOAD);
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
				AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::STOP_2D_MACHGUN_RELOAD);
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

bool CMachineGun::Shoot( )
{
	if (CIWeapon::Shoot( ))
	{
		AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_2D_MACHGUN_SHOOT);
		return true;
	}
	return false;
}
/*
void CMachineGun::UpgradeWeaponTier()
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
m_fDamage = MACHGUN_LVL0_DAMAGE;
m_fFireRate = MACHGUN_LVL0_FIRERATE;
m_fReloadSpeed = MACHGUN_LVL0_RELOADTIME;
m_fSpread = MACHGUN_LVL0_SPREAD;
m_fBulletLifeTime = MACHGUN_LVL0_BULLETLIFE;
m_nCurrentMagazine = MACHGUN_LVL0_MAGSIZE;
m_nMaxMagazine = MACHGUN_LVL0_MAGSIZE;
m_uSpecialFlag = MACHGUN_LVL0_SPECIAL;
}
break;

case 1:
{
m_fDamage = MACHGUN_LVL1_DAMAGE;
m_fFireRate = MACHGUN_LVL1_FIRERATE;
m_fReloadSpeed = MACHGUN_LVL1_RELOADTIME;
m_fSpread = MACHGUN_LVL1_SPREAD;
m_fBulletLifeTime = MACHGUN_LVL1_BULLETLIFE;
m_nCurrentMagazine = MACHGUN_LVL1_MAGSIZE;
m_nMaxMagazine = MACHGUN_LVL1_MAGSIZE;
m_uSpecialFlag = MACHGUN_LVL1_SPECIAL;
}
break;

case 2:
{
m_fDamage = MACHGUN_LVL2_DAMAGE;
m_fFireRate = MACHGUN_LVL2_FIRERATE;
m_fReloadSpeed = MACHGUN_LVL2_RELOADTIME;
m_fSpread = MACHGUN_LVL2_SPREAD;
m_fBulletLifeTime = MACHGUN_LVL2_BULLETLIFE;
m_nCurrentMagazine = MACHGUN_LVL2_MAGSIZE;
m_nMaxMagazine = MACHGUN_LVL2_MAGSIZE;
m_uSpecialFlag = MACHGUN_LVL2_SPECIAL;
}
break;

case 3:
{
m_fDamage = MACHGUN_LVL3_DAMAGE;
m_fFireRate = MACHGUN_LVL3_FIRERATE;
m_fReloadSpeed = MACHGUN_LVL3_RELOADTIME;
m_fSpread = MACHGUN_LVL3_SPREAD;
m_fBulletLifeTime = MACHGUN_LVL3_BULLETLIFE;
m_nCurrentMagazine = MACHGUN_LVL3_MAGSIZE;
m_nMaxMagazine = MACHGUN_LVL3_MAGSIZE;
m_uSpecialFlag = MACHGUN_LVL3_SPECIAL;
}
break;

//case 4:
//{
//	m_fFireRate += 0.5f;
//	m_fSpread -= 1.0f;
//}
//break;
//
//case 5:
//{
//	m_fDamage += 5;
//}
//break;
//
//case 6:
//{
//	m_fFireRate += 0.5f;
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

}
break;
}
}
*/
void CMachineGun::UpdateEnemyW(float dt, CEntity* owner)
{

}

void CMachineGun::Reset(void)
{
	//Load stats from on file, or use #defines
	if (s_fPresetDamage == 0)
	{
		LoadStats( );
	}

	m_bUnlocked = MACHGUN_UNLOCKED;

	m_fBulletLifeTime = MACHGUN_BULLETLIFE;
	m_nBulletScrapCost = s_nPresetBulletCost;
	m_nMaxMagazine = s_nPresetMagSize;
	m_fDamage = s_fPresetDamage;
	m_fFireRate = s_fPresetFireRate;
	m_fSpread = s_fPresetSpread;
	m_fTimeToReload = s_fPresetReloadTime;
	m_fReloadSpeed = s_fPresetReloadSpeed;
	if (m_pModules == nullptr)
	{
		m_pModules = new CModBank(EMOD_WEAPON);
	}
	m_pModules->SetBankSize(s_nPresetModLimit);

	//Resets modules, reload clip
	CIWeapon::Reset( );
}


bool CMachineGun::LoadStats(void)
{
	tinyxml2::XMLDocument xmlDoc;

	//TODO: Load stats based on difficulty
	xmlDoc.LoadFile("Assets//XML//Balance//MachineGunStats.xml");

	//Did not open file properly
	if (xmlDoc.Error( ))
	{
		//Load in the default values
		s_nPresetBulletCost = MACHGUN_BULLETCOST;
		s_nPresetMagSize = MACHGUN_MAGSIZE;
		s_fPresetDamage = MACHGUN_DAMAGE;
		s_fPresetFireRate = MACHGUN_FIRERATE;
		s_fPresetSpread = MACHGUN_SPREAD;
		s_fPresetReloadTime = MACHGUN_RELOADTIME;
		s_fPresetReloadSpeed = MACHGUN_RELOADSPEED;

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
			s_fPresetDamage = MACHGUN_DAMAGE;
		if (xmlShoulder->Attribute("FireRate"))
			s_fPresetFireRate = (float)atof(xmlShoulder->Attribute("FireRate"));
		else
			s_fPresetFireRate = MACHGUN_FIRERATE;
		if (xmlShoulder->Attribute("ReloadTime"))
			s_fPresetReloadTime = (float)atof(xmlShoulder->Attribute("ReloadTime"));
		else
			s_fPresetReloadTime = MACHGUN_RELOADTIME;
		if (xmlShoulder->Attribute("ReloadSpeed"))
			s_fPresetReloadSpeed = (float)atof(xmlShoulder->Attribute("ReloadSpeed"));
		else
			s_fPresetReloadSpeed = MACHGUN_RELOADSPEED;
		if (xmlShoulder->Attribute("Spread"))
			s_fPresetSpread = (float)atof(xmlShoulder->Attribute("Spread"));
		else
			s_fPresetSpread = MACHGUN_SPREAD;
		if (xmlShoulder->Attribute("Magazine"))
			s_nPresetMagSize = (int)atoi(xmlShoulder->Attribute("Magazine"));
		else
			s_nPresetMagSize = MACHGUN_MAGSIZE;
	}
	else
	{
		s_fPresetDamage = MACHGUN_DAMAGE;
		s_fPresetFireRate = MACHGUN_FIRERATE;
		s_fPresetReloadTime = MACHGUN_RELOADTIME;
		s_fPresetSpread = MACHGUN_SPREAD;
		s_nPresetMagSize = MACHGUN_MAGSIZE;
	}
	//Price things
	xmlShoulder = xmlHead->FirstChildElement("Price");
	if (xmlShoulder)
	{
		if (xmlShoulder->Attribute("WeaponCost"))
			s_nPresetWeaponCost = (int)atoi(xmlShoulder->Attribute("WeaponCost"));
		else
			s_nPresetWeaponCost = MACHGUN_WEAPONCOST;
		if (xmlShoulder->Attribute("BulletCost"))
			s_nPresetBulletCost = (int)atoi(xmlShoulder->Attribute("BulletCost"));
		else
			s_nPresetBulletCost = MACHGUN_BULLETCOST;
	}
	else
	{
		s_nPresetWeaponCost = MACHGUN_WEAPONCOST;
		s_nPresetBulletCost = MACHGUN_BULLETCOST;
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
			s_nPresetModLimit = (MACHGUN_MODLIMIT);
	}
	else
	{
		m_pModules->SetBankSize(MACHGUN_MODLIMIT);
	}

	//We're done here
	xmlDoc.Clear( );
	return true;
}

float	CMachineGun::s_fPresetDamage;
float	CMachineGun::s_fPresetFireRate;
float	CMachineGun::s_fPresetReloadTime;
float	CMachineGun::s_fPresetReloadSpeed;
float	CMachineGun::s_fPresetSpread;
int		CMachineGun::s_nPresetMagSize;
int		CMachineGun::s_nPresetWeaponCost;
int		CMachineGun::s_nPresetBulletCost;
int		CMachineGun::s_nPresetModLimit;