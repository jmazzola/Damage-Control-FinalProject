/***********************************************
* Filename:  		Module.cpp
* Date:      		07/08/2015
* Mod. Date:
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This file contains the
*			important classes/structs for the mod system
************************************************/
#include "ModSystem.h"
#include "Bullet.h"
#include "../Core/stdafx.h"
#include "../Renderer/Texture2D.h"
#include "../Asset Manager/AssetManager.h"
#include "../TinyXML2/tinyxml2.h"
#include "IWeapon.h"
#include "MachineGun.h"
#include "Pistol.h"
#include "ShotGun.h"
#include "DroneBuddy.h"
#include <vector>
#include "ModBank.h"

CModSystem::~CModSystem()
{
	ClearModData();
}

CModSystem* CModSystem::GetInstance(void)
{
	//Lazy
	static CModSystem s_cModSystem;
	return &s_cModSystem;
}

//Load the textures for m_umModData
bool CModSystem::LoadModIcon(unsigned int uModID)
{
	if (IsIDValid(uModID) && m_umModInfo[uModID].m_pIcon == nullptr)
	{
		m_umModInfo[uModID].m_pIcon = new TTexture2D();
		CAssetManager::LoadTexture(std::string("Assets\\Images\\Mods\\" + m_umModInfo[uModID].m_szIconFileName).c_str(), &m_umModInfo[uModID].m_pIcon->m_d3dSRV);
		return true;
	}
	return false;
}
bool CModSystem::LoadModDescPanel(unsigned int uModID)
{
	if (IsIDValid(uModID) && m_umModInfo[uModID].m_pDescPanel == nullptr)
	{
		m_umModInfo[uModID].m_pDescPanel = new TTexture2D();
		CAssetManager::LoadTexture(std::string("Assets\\Images\\Mods\\" + m_umModInfo[uModID].m_szDescFileName).c_str(), &m_umModInfo[uModID].m_pDescPanel->m_d3dSRV);
		return true;
	}
	return false;
}

//Loads in all modules from a file
bool CModSystem::LoadModules(const char* szFileName)
{
	//Only allow for loading one mod doc
	if (m_bLoaded)
	{
		return false;
	}
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(szFileName);
	if (!xmlDoc.Error())
	{
		//Read the module contents here!
		tinyxml2::XMLElement* xmlRoot = xmlDoc.RootElement();
		tinyxml2::XMLElement* xmlHead = nullptr;
		tinyxml2::XMLElement* xmlShoulder = nullptr;
		tinyxml2::XMLElement* xmlKnee = nullptr;

		//Blank module
		{
			xmlHead = xmlRoot->FirstChildElement("Blank");
			TModData tData;
			ZeroMemory(&tData.m_tTraitLevels, sizeof(TWeaponSpecials));
			ZeroMemory(&tData.m_fStatBoosts, sizeof(float)* EBOOST_COUNT);
			//ZeroMemory(&tData, sizeof(tData));
			tData.m_fStressValue = (float)atof(xmlHead->Attribute("Stress"));
			tData.m_nScrapValue = (int)atoi(xmlHead->Attribute("Scrap"));
			//Get the name
			xmlKnee = xmlHead->FirstChildElement("Name");
			tData.m_szName = xmlKnee->GetText();
			//Get the icon file name
			xmlKnee = xmlHead->FirstChildElement("Icon");
			tData.m_szIconFileName = xmlKnee->GetText();
			//Get the panel file name
			xmlKnee = xmlHead->FirstChildElement("DescPanel");
			tData.m_szDescFileName = xmlKnee->GetText();
			tData.m_pIcon = nullptr;
			tData.m_pDescPanel = nullptr;
			AddModule(tData, EMOD_NULL);
		}

		//Go through the weapons
		xmlHead = xmlRoot->FirstChildElement("Weapons");
		xmlShoulder = xmlHead->FirstChildElement("Module");

		//Go through all the modules
		while (xmlShoulder)
		{
			TModData tData;
			ZeroMemory(&tData.m_tTraitLevels, sizeof(TWeaponSpecials));
			ZeroMemory(&tData.m_fStatBoosts, sizeof(float)* EBOOST_COUNT);
			//ZeroMemory(&tData, sizeof(tData));
			tData.m_fStressValue = (float)atof(xmlShoulder->Attribute("Stress"));
			tData.m_nScrapValue = (int)atoi(xmlShoulder->Attribute("Scrap"));
			//Get the name
			xmlKnee = xmlShoulder->FirstChildElement("Name");
			tData.m_szName = xmlKnee->GetText();
			//Get the icon file name
			xmlKnee = xmlShoulder->FirstChildElement("Icon");
			tData.m_szIconFileName = xmlKnee->GetText();
			//Get the panel file name
			xmlKnee = xmlShoulder->FirstChildElement("DescPanel");
			tData.m_szDescFileName = xmlKnee->GetText();
			//Get the traits
			xmlKnee = xmlShoulder->FirstChildElement("Traits");
			if (xmlKnee)
			{
				if (xmlKnee->Attribute("Bounce"))
					tData.m_tTraitLevels.m_nBounceLevel = (signed char)atoi(xmlKnee->Attribute("Bounce"));
				else
					tData.m_tTraitLevels.m_nBounceLevel = 0;
				if (xmlKnee->Attribute("Chain"))
					tData.m_tTraitLevels.m_nChainLevel = (signed char)atoi(xmlKnee->Attribute("Chain"));
				else
					tData.m_tTraitLevels.m_nChainLevel = 0;
				if (xmlKnee->Attribute("Pierce"))
					tData.m_tTraitLevels.m_nPierceLevel = (signed char)atoi(xmlKnee->Attribute("Pierce"));
				else
					tData.m_tTraitLevels.m_nPierceLevel = 0;
				if (xmlKnee->Attribute("Explode"))
					tData.m_tTraitLevels.m_nExplodeLevel = (signed char)atoi(xmlKnee->Attribute("Explode"));
				else
					tData.m_tTraitLevels.m_nExplodeLevel = 0;
				if (xmlKnee->Attribute("Freeze"))
					tData.m_tTraitLevels.m_nFreezeLevel = (signed char)atoi(xmlKnee->Attribute("Freeze"));
				else
					tData.m_tTraitLevels.m_nFreezeLevel = 0;
				if (xmlKnee->Attribute("Poison"))
					tData.m_tTraitLevels.m_nPoisonLevel = (signed char)atoi(xmlKnee->Attribute("Poison"));
				else
					tData.m_tTraitLevels.m_nPoisonLevel = 0;
				if (xmlKnee->Attribute("Slow"))
					tData.m_tTraitLevels.m_nSlowLevel = (signed char)atoi(xmlKnee->Attribute("Slow"));
				else
					tData.m_tTraitLevels.m_nSlowLevel = 0;
				if (xmlKnee->Attribute("Secret"))
					tData.m_tTraitLevels.m_nSecretLevel = (signed char)atoi(xmlKnee->Attribute("Secret"));
				else
					tData.m_tTraitLevels.m_nSecretLevel = 0;
			}
			//Get the stat boosts
			xmlKnee = xmlShoulder->FirstChildElement("Stats");
			if (xmlKnee)
			{
				if (xmlKnee->Attribute("Damage"))
					tData.m_fStatBoosts[EBOOST_DAMAGE] = (float)atof(xmlKnee->Attribute("Damage"));
				else
					tData.m_fStatBoosts[EBOOST_DAMAGE] = 0;
				if (xmlKnee->Attribute("FireRate"))
					tData.m_fStatBoosts[EBOOST_FIRERATE] = (float)atof(xmlKnee->Attribute("FireRate"));
				else
					tData.m_fStatBoosts[EBOOST_FIRERATE] = 0;
				if (xmlKnee->Attribute("MagSize"))
					tData.m_fStatBoosts[EBOOST_MAGSIZE] = (float)atof(xmlKnee->Attribute("MagSize"));
				else
					tData.m_fStatBoosts[EBOOST_MAGSIZE] = 0;
				if (xmlKnee->Attribute("ReloadSpeed"))
					tData.m_fStatBoosts[EBOOST_RELOADSPEED] = (float)atof(xmlKnee->Attribute("ReloadSpeed"));
				else
					tData.m_fStatBoosts[EBOOST_RELOADSPEED] = 0;
			}
			tData.m_pIcon = nullptr;
			tData.m_pDescPanel = nullptr;

			//Store it in the map
			AddModule(tData);

			xmlShoulder = xmlShoulder->NextSiblingElement("Module");
		}

		xmlHead = xmlRoot->FirstChildElement("Gadgets");
		xmlShoulder = xmlHead->FirstChildElement("Module");
		//Go through all the modules
		while (xmlShoulder)
		{
			TModData tData;
			ZeroMemory(&tData.m_tTraitLevels, sizeof(TWeaponSpecials));
			ZeroMemory(&tData.m_fStatBoosts, sizeof(float)* EBOOST_COUNT);
			//ZeroMemory(&tData, sizeof(tData));
			tData.m_fStressValue = (float)atof(xmlShoulder->Attribute("Stress"));
			tData.m_nScrapValue = (int)atoi(xmlShoulder->Attribute("Scrap"));
			//Get the name
			xmlKnee = xmlShoulder->FirstChildElement("Name");
			tData.m_szName = xmlKnee->GetText();
			//Get the icon file name
			xmlKnee = xmlShoulder->FirstChildElement("Icon");
			tData.m_szIconFileName = xmlKnee->GetText();
			//Get the panel file name
			xmlKnee = xmlShoulder->FirstChildElement("DescPanel");
			tData.m_szDescFileName = xmlKnee->GetText();

			//Get the stat boosts
			xmlKnee = xmlShoulder->FirstChildElement("Stats");
			if (xmlKnee)
			{
				if (xmlKnee->Attribute("Cooldown"))
					tData.m_fStatBoosts[EBOOST_COOLDOWN] = (float)atof(xmlKnee->Attribute("Cooldown"));
				else
					tData.m_fStatBoosts[EBOOST_COOLDOWN] = 0;
				if (xmlKnee->Attribute("Duration"))
					tData.m_fStatBoosts[EBOOST_DURATION] = (float)atof(xmlKnee->Attribute("Duration"));
				else
					tData.m_fStatBoosts[EBOOST_DURATION] = 0;
				if (xmlKnee->Attribute("Repair"))
					tData.m_fStatBoosts[EBOOST_REPAIR] = (float)atof(xmlKnee->Attribute("Repair"));
				else
					tData.m_fStatBoosts[EBOOST_REPAIR] = 0;
				if (xmlKnee->Attribute("ExplosionRadius"))
					tData.m_fStatBoosts[EBOOST_EXP_RADIUS] = (float)atof(xmlKnee->Attribute("ExplosionRadius"));
				else
					tData.m_fStatBoosts[EBOOST_EXP_RADIUS] = 0;
				if (xmlKnee->Attribute("ExplosionDamage"))
					tData.m_fStatBoosts[EBOOST_EXP_DAMAGE] = (float)atof(xmlKnee->Attribute("ExplosionDamage"));
				else
					tData.m_fStatBoosts[EBOOST_EXP_DAMAGE] = 0;
			}
			tData.m_pIcon = nullptr;
			tData.m_pDescPanel = nullptr;

			//Store it in the map
			AddModule(tData, EMOD_GADGET);

			xmlShoulder = xmlShoulder->NextSiblingElement("Module");
		}

		//Go through the theoretical suit modules
		xmlHead = xmlRoot->FirstChildElement("Suit");

		//Go through the theoretical sub-system modules
		xmlHead = xmlRoot->FirstChildElement("SubSystem");

		m_bLoaded = true;
	}
	//We're done here
	xmlDoc.Clear();
	return m_bLoaded;
}

//Adds module to the map
void CModSystem::AddModule(TModData& tModule, MODULE_TRAITS eModType)
{
	//Generate an ID number
	unsigned int uModID = 0;
	uModID |= eModType;
	//Account for the special traits
	if (tModule.m_tTraitLevels.m_nBounceLevel > 0)
	{
		uModID |= EWEAPON_BOUNCE;
	}
	if (tModule.m_tTraitLevels.m_nChainLevel > 0)
	{
		uModID |= EWEAPON_CHAIN;
	}
	if (tModule.m_tTraitLevels.m_nPierceLevel > 0)
	{
		uModID |= EWEAPON_PIERCE;
	}
	if (tModule.m_tTraitLevels.m_nExplodeLevel > 0)
	{
		uModID |= EWEAPON_EXPLODE;
	}
	if (tModule.m_tTraitLevels.m_nFreezeLevel > 0)
	{
		uModID |= EWEAPON_FREEZE;
	}
	if (tModule.m_tTraitLevels.m_nPoisonLevel > 0)
	{
		uModID |= EWEAPON_POISON;
	}
	if (tModule.m_tTraitLevels.m_nSlowLevel > 0)
	{
		uModID |= EWEAPON_SLOW;
	}
	if (tModule.m_tTraitLevels.m_nSecretLevel > 0)
	{
		uModID |= EWEAPON_FUCKYOU;
	}
	//Account for the stats
	if (tModule.m_fStatBoosts[EBOOST_DAMAGE] > 0.0f)
	{
		uModID |= EWEAPON_DAMAGE;
	}
	if (tModule.m_fStatBoosts[EBOOST_MAGSIZE] > 0.0f)
	{
		uModID |= EWEAPON_MAGSIZE;
	}
	if (tModule.m_fStatBoosts[EBOOST_FIRERATE] > 0.0f)
	{
		uModID |= EWEAPON_FIRERATE;
	}
	if (tModule.m_fStatBoosts[EBOOST_RELOADSPEED] > 0.0f)
	{
		uModID |= EWEAPON_RELOADSPEED;
	}


	//See if we need a version number
	unsigned char* pVersion = (unsigned char*)&uModID;
	pVersion += 2;
	while (m_umModInfo.find(uModID) != m_umModInfo.end())
	{
		(*pVersion)++;
	}

	//Add to the thing
	m_umModInfo[uModID] = tModule;
}

//Adds the traits of applicable mods to the bullet
void CModSystem::ApplyMods(CModBank* pMods, CBullet* pBullet)
{
	if (pMods && pBullet)
	{
		TWeaponSpecials tSpecials;
		ZeroMemory(&tSpecials, sizeof(tSpecials));
		unsigned char uSpecialFlag = 0;
		/*
		float fStatBoosts[EBOOST_COUNT];
		ZeroMemory(fStatBoosts, sizeof(float)* EBOOST_COUNT);
		*/
		for (size_t i = 0; i < pMods->GetBankSize(); i++)
		{
			unsigned int uTraits = pMods->GetMod(i);
			//Make sure we're only getting weapon mods
			if (uTraits & EMOD_WEAPON)
			{
				//Apply specials
				tSpecials = tSpecials + GetModSpecials(uTraits);
				/*//We're applying stat changes directly to the weapons
				//Apply stat upgrades directly to the
				if (uTraits & EWEAPON_DAMAGE)
				{
					fStatBoosts[EBOOST_DAMAGE] += CModSystem::GetInstance()->GetStatBoost(uTraits, EBOOST_DAMAGE);
				}
				if (uTraits & EWEAPON_RANGE)
				{
					fStatBoosts[EBOOST_MAGSIZE] += GetInstance()->GetStatBoost(uTraits, EBOOST_MAGSIZE);
				}
				if (uTraits & EWEAPON_SPEED)
				{
					fStatBoosts[EBOOST_FIRERATE] += CModSystem::GetInstance()->GetStatBoost(uTraits, EBOOST_FIRERATE);
				}*/
				uSpecialFlag |= (unsigned char)uTraits;
			}
		}
		/*
		//Apply stat changes to bullets
		pBullet->SetDamage(pBullet->GetDamage() + fStatBoosts[EBOOST_DAMAGE]);
		pBullet->SetRange(pBullet->GetRange() + fStatBoosts[EBOOST_MAGSIZE]);
		pBullet->SetSpeed(pBullet->GetSpeed() + fStatBoosts[EBOOST_FIRERATE]);
		*/
		//Send the special traits for the bullets to handle
		pBullet->SetSpecial(tSpecials);
		pBullet->SetSpecialFlag(uSpecialFlag);
	}
}

/*
void CModSystem::ApplyMods(CModBank* pMods, CBullet* pBullets, int nBulletCount)
{
	if (nBulletCount > 0 && pMods && pBullets)
	{
		TWeaponSpecials tSpecials;
		ZeroMemory(&tSpecials, sizeof(tSpecials));
		for (size_t i = 0; i < pMods->GetBankSize(); i++)
		{
			unsigned int uTraits = pMods->GetMod(i);
			//Make sure we're only getting weapon mods
			if (uTraits & EMOD_WEAPON)
			{
				//Apply specials
				tSpecials = tSpecials + GetModSpecials(uTraits);
			}
		}
		for (int i = 0; i < nBulletCount; i++)
		{
			//Send the special traits for the bullets to handle
			pBullets[i].SetSpecial(tSpecials);
		}
	}
}
*/

void CModSystem::ApplyMods(CModBank* pMods, CIWeapon* pWeapon)
{
	if (pMods && pWeapon && pWeapon->GetGunType() != GUN_BASE)
	{
		if (pWeapon->GetGunType() == GUN_DRONE)
		{
			float fDurationBoost = 0;
			float fCooldownBoost = 0;
			float fExpRadius = 0;
			float fExpDamage = 0;
			float fRepairSpeed = 0;
			//Get the boosts
			for (size_t i = 0; i < pMods->GetBankSize(); i++)
			{
				fDurationBoost += GetStatBoost(pMods->GetMod(i), EBOOST_DURATION);
				fCooldownBoost += GetStatBoost(pMods->GetMod(i), EBOOST_COOLDOWN);
				fExpDamage += GetStatBoost(pMods->GetMod(i), EBOOST_EXP_DAMAGE);
				fExpRadius += GetStatBoost(pMods->GetMod(i), EBOOST_EXP_RADIUS);
				fRepairSpeed += GetStatBoost(pMods->GetMod(i), EBOOST_REPAIR);
			}
			CDroneBuddy* pDrone = static_cast<CDroneBuddy*>(pWeapon);
			pDrone->ResetDroneStats();
			pDrone->SetLifeMultiplier(1 + fDurationBoost);
			pDrone->SetCooldownSpeed(1 + fCooldownBoost);
			pDrone->SetExplodeDamage(pDrone->GetExplodeDamage() * (1 + fExpDamage));
			pDrone->SetExplodeRange(pDrone->GetExplodeRange() * (1 + fExpRadius));
			pDrone->SetRepairSpeed(pDrone->GetRepairSpeed() * (1 + fRepairSpeed));
		}
		else
		{
			//Percent Boosts
			float fDamageBoost = 0;
			float fFireRateBoost = 0;
			float fMagSizeBoost = 0;
			float fReloadBoost = 0;
			//Get the boosts
			for (size_t i = 0; i < pMods->GetBankSize(); i++)
			{
				fDamageBoost += GetStatBoost(pMods->GetMod(i), EBOOST_DAMAGE);
				fFireRateBoost += GetStatBoost(pMods->GetMod(i), EBOOST_FIRERATE);
				fMagSizeBoost += GetStatBoost(pMods->GetMod(i), EBOOST_MAGSIZE);
				fReloadBoost += GetStatBoost(pMods->GetMod(i), EBOOST_RELOADSPEED);
			}
			//Find out what kind of weapon we're using
			if (pWeapon->GetGunType() == EGunType::GUN_FULL_AUTO)
			{
				CMachineGun* pMachy = static_cast<CMachineGun*>(pWeapon);
				pMachy->SetDamage(pMachy->GetBaseDamage() * (1 + fDamageBoost));//%
				pMachy->SetFireRate(pMachy->GetBaseFireRate() * (1 + fFireRateBoost));//%
				pMachy->SetMagSize((int)(pMachy->GetBaseMagSize() * (1 + fMagSizeBoost)));//%
				pMachy->SetReloadSpeed(pMachy->GetBaseReloadSpeed() * (1 + fReloadBoost));//%
			}
			else if (pWeapon->GetGunType() == EGunType::GUN_SCATTER)
			{
				CShotGun* pShotty = static_cast<CShotGun*>(pWeapon);
				pShotty->SetDamage(pShotty->GetBaseDamage() * (1 + fDamageBoost));//%
				pShotty->SetFireRate(pShotty->GetBaseFireRate() * (1 + fFireRateBoost));//%
				pShotty->SetMagSize((int)(pShotty->GetBaseMagSize() * (1 + fMagSizeBoost)));//%
				pShotty->SetReloadSpeed(pShotty->GetBaseReloadSpeed() * (1 + fReloadBoost));//%
			}
			else if (pWeapon->GetGunType() == EGunType::GUN_SEMI_AUTO)
			{
				CPistol* pPisty = static_cast<CPistol*>(pWeapon);
				pPisty->SetDamage(pPisty->GetBaseDamage() * (1 + fDamageBoost));//%
				pPisty->SetFireRate(pPisty->GetBaseFireRate() * (1 + fFireRateBoost));//%
				pPisty->SetMagSize((int)(pPisty->GetBaseMagSize() * (1 + fMagSizeBoost)));//%
				pPisty->SetReloadSpeed(pPisty->GetBaseReloadSpeed() * (1 + fReloadBoost));//%
			}
			//We don't support yer kind here (yet)
			else
			{
				return;
			}
		}
	}
}

//Clears everything in m_umModInfo
void CModSystem::ClearModData(void)
{
	m_bLoaded = false;
	UnloadModData();
	m_umModInfo.clear();
}
//Unloads all textures from m_umModInfo, but keeps their filenames, so we can reload them
void CModSystem::UnloadModData(void)
{
	auto iMapIter = m_umModInfo.begin();
	while (iMapIter != m_umModInfo.end())
	{
		//Clear the icon
		if (iMapIter->second.m_pIcon)
		{
			SafeRelease(iMapIter->second.m_pIcon->m_d3dSRV);
			delete iMapIter->second.m_pIcon;
			iMapIter->second.m_pIcon = nullptr;
		}
		//Clear the desc panel
		if (iMapIter->second.m_pDescPanel)
		{
			SafeRelease(iMapIter->second.m_pDescPanel->m_d3dSRV);
			delete iMapIter->second.m_pDescPanel;
			iMapIter->second.m_pDescPanel = nullptr;
		}
		iMapIter++;
	}
}

const char* CModSystem::GetModName(unsigned int uModID)
{
	if (m_umModInfo.find(uModID) != m_umModInfo.end())
	{
		return m_umModInfo[uModID].m_szName.c_str();
	}
	else
	{
		return nullptr;
	}
}
//Get the textures from m_umModData
TTexture2D* CModSystem::GetModIcon(unsigned int uModID)
{
	if (IsIDValid(uModID))
	{
		if (m_umModInfo[uModID].m_pIcon == nullptr)
		{
			LoadModIcon(uModID);
		}
		return m_umModInfo[uModID].m_pIcon;
	}
	else
	{
		return nullptr;
	}
}
TTexture2D* CModSystem::GetModDescPanel(unsigned int uModID)
{
	if (IsIDValid(uModID))
	{
		if (m_umModInfo[uModID].m_pDescPanel == nullptr)
		{
			LoadModDescPanel(uModID);
		}
		return m_umModInfo[uModID].m_pDescPanel;
	}
	else
	{
		return nullptr;
	}
}

bool CModSystem::IsIDValid(unsigned int uModID) const
{
	if (m_umModInfo.find(uModID) != m_umModInfo.end())
	{
		return true;
	}
	return false;
}

unsigned int CModSystem::GetModByName(const char* szModName)
{
	string szSearchName = szModName;
	auto iter = m_umModInfo.begin();
	for (; iter != m_umModInfo.end(); iter++)
	{
		if (iter->second.m_szName == szSearchName)
		{
			return iter->first;
		}
	}
	//Otherwise return blank
	return EMOD_NULL;
}

