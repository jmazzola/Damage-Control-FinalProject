/***********************************************
* Filename:  		StatusEffects.cpp
* Date:      		07/14/2015
* Mod. Date:
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This file holds all information
*				for the status effect system
************************************************/
#include "StatusEffects.h"
#include "../Core/stdafx.h"
#include "../Entity Manager/LivingEntites.h"

#define DEFAULT_STATUSEFFECT_BANK_SIZE 32

CStatusBank::CStatusBank()
{
	ZeroMemory(m_tStatusEffects, sizeof(TStatusEffect)* STAT_EFF_COUNT);
}

CStatusBank::~CStatusBank()
{
}

//Adds a new effect
bool CStatusBank::AddEffect(EStatusEffect eEffect, float fDuration, float fPotency)
{
	//Make sure we can add
	if (fDuration <= 0)
	{
		DebugPrint("[StatusEffects] Cannot Add Effect: Duration Invalid!\n");
		return false;
	}
	else if (fPotency <= 0)
	{
		DebugPrint("[StatusEffects] Cannot Add Effect: Potency Invalid!\n");
		return false;
	}
	else if (eEffect == STAT_EFF_NULL)
	{
		DebugPrint("[StatusEffects] Cannot Add Effect: Effect Invalid!\n");
		return false;
	}
	bool bAlreadyActive = IsEffectActive(eEffect);
	//Add the guy to the thing
	m_tStatusEffects[eEffect].m_fDuration = max(m_tStatusEffects[eEffect].m_fDuration, fDuration);
	m_tStatusEffects[eEffect].m_fPotency = max(m_tStatusEffects[eEffect].m_fPotency, fPotency);

	//If the effect is already active, there's a chance that we add to the effect
	if (bAlreadyActive)
	{
		StackEffectChance(eEffect);
	}

	return true;
}

//Apply over time effects
void CStatusBank::ApplyOverTimeEffects(float fDt, CLivingEntities* pOwner)
{
	if (pOwner)
	{
		if (IsEffectActive(STAT_EFF_DOT))
		{
			pOwner->TakeDamageOverTime(fDt * m_tStatusEffects[STAT_EFF_DOT].m_fPotency);
		}
	}
}

//Clears out all status effects
void CStatusBank::Dispel(void)
{
	ZeroMemory(m_tStatusEffects, sizeof(TStatusEffect)* STAT_EFF_COUNT);
}

//Removes any effects that need to go
void CStatusBank::Update(float fDt)
{
	for (size_t i = 0; i < STAT_EFF_COUNT; i++)
	{
		if (IsEffectActive((EStatusEffect)i))
		{
			m_tStatusEffects[i].m_fDuration -= fDt;
			//Remove a status effect if its duration == 0
			if (m_tStatusEffects[i].m_fDuration <= 0)
			{
				m_tStatusEffects[i].m_fDuration = m_tStatusEffects[i].m_fPotency = 0;
			}
		}
	}
}

//Adds a random status effect
bool CStatusBank::AddRandomEffect(void)
{
	//Set the duration: max = 30 sec, min = 5 sec
	float fDuration = (float)(rand() % 5) * 5 + 5;
	//Set the potency: max = 2, min = 0.5
	float fPotency = ((float)(rand() & 39) * 0.5f) + 0.5f;
	//Set the ID
	int nEffect = 1;
	nEffect = nEffect << (rand() % 6);

	return AddEffect((EStatusEffect)nEffect, fDuration, fPotency);
}

float CStatusBank::GetEffectedStat(EStatusEffect eEffect, float fStat)
{
	if (eEffect == STAT_EFF_NULL || eEffect == STAT_EFF_DOT)
	{
		return 0;
	}

	//Decrease the stat by that much percent
	return fStat - (fStat * m_tStatusEffects[eEffect].m_fPotency);
}

bool CStatusBank::StackEffectChance(EStatusEffect eEffect)
{
	if ((rand() % 10) == 0 && IsEffectActive(eEffect))
	{
		m_tStatusEffects[eEffect].m_fPotency *= 1.1f;

		return true;
	}

	return false;
}