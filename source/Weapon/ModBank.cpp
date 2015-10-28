/***********************************************
* Filename:  		ModBank.h
* Date:      		07/08/2015
* Mod. Date:
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		The modbank is the local repository
*				for storing and applying modules
************************************************/

#include "ModBank.h"
#include "Bullet.h"
#include "../Core/stdafx.h"
#include "../Renderer/Texture2D.h"
#include "../Asset Manager/AssetManager.h"
#include "../TinyXML2/tinyxml2.h"
#include <vector>
#include "ModSystem.h"

#define DEFAULT_BANK_SIZE 3

CModBank::CModBank(unsigned int uRequiredTraits)
{
	m_uRequiredTraits = uRequiredTraits;
	//Make 3 slots for mods
	SetBankSize(DEFAULT_BANK_SIZE);
	//Set them to empty
	Reset();
}

void CModBank::ApplyMods(CBullet* pBullet)
{
	CModSystem::GetInstance()->ApplyMods(this, pBullet);
}

void CModBank::ApplyMods(CIWeapon* pOwner)
{
	m_bChanged = false;
	CModSystem::GetInstance()->ApplyMods(this, pOwner);
}

//Inserts a mod at the first empty slot
bool CModBank::AddMod(unsigned int uModID)
{
	return AddMod(uModID, GetModCount());
}
//Inserts a mod in the designated slot
bool CModBank::AddMod(unsigned int uModID, unsigned int uSlot)
{
	if (uSlot >= m_vModules.size() || !IsModWelcome(uModID))
	{
		//Can't fit the new module
		return false;
	}
	//Set this mod to the target
	m_vModules[uSlot] = uModID;
	m_bChanged = true;
	return true;
}
//Switches the mods in two slots
void CModBank::SwapSlots(unsigned int uSlotA, unsigned int uSlotB)
{
	//Make sure we're working within our bounds
	if (uSlotA < m_vModules.size() && uSlotB < m_vModules.size() && uSlotA != uSlotB)
	{
		unsigned int temp = m_vModules[uSlotA];
		m_vModules[uSlotA] = m_vModules[uSlotB];
		m_vModules[uSlotB] = temp;
	}
}

//Resets all modules in the bank
void CModBank::Reset(void)
{
	for (size_t i = 0; i < m_vModules.size(); i++)
	{
		m_vModules[i] = EMOD_NULL;
	}
}

unsigned int CModBank::GetModCount(void) const
{
	unsigned int uModCount = 0;
	for (size_t i = 0; i < m_vModules.size(); i++)
	{
		if (m_vModules[i] != 0)
		{
			uModCount++;
		}
	}
	return uModCount;
}

void CModBank::SetBankSize(unsigned int uBankSize)
{
	if (uBankSize > 0)
	{
		m_vModules.resize(uBankSize);
	}
}

bool CModBank::IsModWelcome(unsigned int uModID) const
{
	return (uModID & m_uRequiredTraits) == m_uRequiredTraits || uModID == EMOD_NULL;//Always accept an empty slot
}

float CModBank::GetStressValue(void) const
{
	float fTotal = 0;
	for (size_t i = 0; i < GetBankSize(); i++)
	{
		fTotal += CModSystem::GetInstance()->GetModStress(m_vModules[i]);
	}
	return fTotal;
}
