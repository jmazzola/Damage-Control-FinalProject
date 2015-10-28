/***********************************************
* Filename:			Pistol.cpp
* Date:      		05/15/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Brandon Lucier
* Purpose:			Pistol represents the properties
*				one of several weapons that the player
*				can use
************************************************/
#include "stdafx.h"
#include "Pistol.h"

#include "../Input Manager/InputManager.h"
#include "../Input Manager/KeyBindings.h"

#include "../State Machine/GameplayState.h"
#include "../Entity Manager/EntityManager.h"
#include "../Audio Manager/AudioSystemWwise.h"
#include "../TinyXML2/tinyxml2.h"
#include "ModBank.h"

#pragma region Pistol Upgrade Stats

#define PISTOL_UNLOCKED		false

#define PISTOL_DAMAGE		40.0f
#define PISTOL_FIRERATE		1.5f
#define PISTOL_RELOADTIME	1.0f
#define PISTOL_SPREAD		1.5f
#define PISTOL_BULLETLIFE	5.0f
#define PISTOL_MAGSIZE		24
#define PISTOL_BULLETCOST	0
#define PISTOL_WEAPONCOST	0
#define PISTOL_RELOADSPEED	1

#define PISTOL_MODLIMIT	3

#pragma endregion

CPistol::CPistol( ) : CIWeapon(GUN_SEMI_AUTO)
{
	m_szTag = "Pistol";

	//m_XMBarrel = { 0, 3, 17 };
	Reset( );
}

CPistol::~CPistol( )
{
}

void CPistol::Update(float dt)
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
		if (m_bReloading || m_nCurrentMagazine <= 0 || m_bToReload)
		{
			//Only activates the first frame of reloading
			if (!m_bReloading)
			{
				m_bReloading = true;
				AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_2D_PISTOL_RELOAD);
				//Full reload - the whole clip
				m_nCurrentMagazine = m_nMaxMagazine;
				CGameplayState::GetEntityManager( )->GetPlayer( )->SubScrap(GetReloadCost( ));
			}
			m_fReloadTimer += dt * m_fReloadSpeed;

			if (m_fReloadTimer >= m_fTimeToReload)
			{
				m_fReloadTimer = 0;
				m_bReloading = false;
				AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::STOP_2D_PISTOL_RELOAD);
			}
		}
	}

	if (m_bFiring && dt && !m_bReloading)
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

bool CPistol::Shoot( )
{
	if (CIWeapon::Shoot( ))
	{
		AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_2D_PISTOL_SHOOT);
		return true;
	}
	return false;
}

void CPistol::UpdateEnemyW(float dt, CEntity* owner)
{

}

void CPistol::Reset(void)
{
	//Load stats from on file, or use #defines
	if (s_fPresetDamage == 0)
	{
		LoadStats( );
	}

	m_bUnlocked = PISTOL_UNLOCKED;

	m_fBulletLifeTime = PISTOL_BULLETLIFE;
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

bool CPistol::LoadStats(void)
{
	tinyxml2::XMLDocument xmlDoc;

	//TODO: Load stats based on difficulty
	xmlDoc.LoadFile("Assets//XML//Balance//PistolStats.xml");

	//Did not open file properly
	if (xmlDoc.Error( ))
	{
		//Load in the default values
		s_nPresetBulletCost = PISTOL_BULLETCOST;
		s_nPresetMagSize = PISTOL_MAGSIZE;
		s_fPresetDamage = PISTOL_DAMAGE;
		s_fPresetFireRate = PISTOL_FIRERATE;
		s_fPresetSpread = PISTOL_SPREAD;
		s_fPresetReloadTime = PISTOL_RELOADTIME;
		s_fPresetReloadSpeed = PISTOL_RELOADSPEED;

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
			s_fPresetDamage = PISTOL_DAMAGE;
		if (xmlShoulder->Attribute("FireRate"))
			s_fPresetFireRate = (float)atof(xmlShoulder->Attribute("FireRate"));
		else
			s_fPresetFireRate = PISTOL_FIRERATE;
		if (xmlShoulder->Attribute("ReloadTime"))
			s_fPresetReloadTime = (float)atof(xmlShoulder->Attribute("ReloadTime"));
		else
			s_fPresetReloadTime = PISTOL_RELOADTIME;
		if (xmlShoulder->Attribute("ReloadSpeed"))
			s_fPresetReloadSpeed = (float)atof(xmlShoulder->Attribute("ReloadSpeed"));
		else
			s_fPresetReloadSpeed = PISTOL_RELOADSPEED;
		if (xmlShoulder->Attribute("Spread"))
			s_fPresetSpread = (float)atof(xmlShoulder->Attribute("Spread"));
		else
			s_fPresetSpread = PISTOL_SPREAD;
		if (xmlShoulder->Attribute("Magazine"))
			s_nPresetMagSize = (int)atoi(xmlShoulder->Attribute("Magazine"));
		else
			s_nPresetMagSize = PISTOL_MAGSIZE;
	}
	else
	{
		s_fPresetDamage = PISTOL_DAMAGE;
		s_fPresetFireRate = PISTOL_FIRERATE;
		s_fPresetReloadTime = PISTOL_RELOADTIME;
		s_fPresetSpread = PISTOL_SPREAD;
		s_nPresetMagSize = PISTOL_MAGSIZE;
	}
	//Price things
	xmlShoulder = xmlHead->FirstChildElement("Price");
	if (xmlShoulder)
	{
		if (xmlShoulder->Attribute("WeaponCost"))
			s_nPresetWeaponCost = (int)atoi(xmlShoulder->Attribute("WeaponCost"));
		else
			s_nPresetWeaponCost = PISTOL_WEAPONCOST;
		if (xmlShoulder->Attribute("BulletCost"))
			s_nPresetBulletCost = (int)atoi(xmlShoulder->Attribute("BulletCost"));
		else
			s_nPresetBulletCost = PISTOL_BULLETCOST;
	}
	else
	{
		s_nPresetWeaponCost = PISTOL_WEAPONCOST;
		s_nPresetBulletCost = PISTOL_BULLETCOST;
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
			s_nPresetModLimit = (PISTOL_MODLIMIT);
	}
	else
	{
		m_pModules->SetBankSize(PISTOL_MODLIMIT);
	}

	//We're done here
	xmlDoc.Clear( );
	return true;
}

float	CPistol::s_fPresetDamage;
float	CPistol::s_fPresetFireRate;
float	CPistol::s_fPresetReloadTime;
float	CPistol::s_fPresetReloadSpeed;
float	CPistol::s_fPresetSpread;
int		CPistol::s_nPresetMagSize;
int		CPistol::s_nPresetWeaponCost;
int		CPistol::s_nPresetBulletCost;
int		CPistol::s_nPresetModLimit;