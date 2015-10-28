/***********************************************
* Filename:  		StatusEffects.h
* Date:      		07/14/2015
* Mod. Date:
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This file holds all information
*				for the status effect system
************************************************/
#ifndef _STATUSEFFECTS_H_
#define _STATUSEFFECTS_H_

//#include <vector>
#include <list>

class CLivingEntities;

//Uses bits, but let's not use multiples
enum EStatusEffect : int {
	STAT_EFF_SLOW		= 0, //Decreases move speed
	STAT_EFF_DOT		= 1, //Damages over time
	STAT_EFF_COUNT,
	STAT_EFF_NULL = STAT_EFF_COUNT, //Does nothing
};

//Contains all the info for a single status effect
struct TStatusEffect {
	//How long does it last
	float m_fDuration;
	//How powerful is it
	//Note: Stat (ex Speed) effects are percentage based (0 - 1),
	//	Over Time effects are this much damage per second
	float m_fPotency;
};

//Each LivingEntity should have one of these to help calculate its current status
class CStatusBank
{
public:
	CStatusBank();
	~CStatusBank();

	//Adds a new effect
	bool AddEffect(EStatusEffect eEffect, float fDuration, float fPotency);
	//Adds a random status effect
	bool AddRandomEffect(void);
	//Will sometimes add status effects together
	bool StackEffectChance(EStatusEffect eEffect);

	//Apply over time effects
	void ApplyOverTimeEffects(float fDt, CLivingEntities* pOwner);

	//Returns fStat modified by the active effect
	//	Returns 0 if not applicaple
	float GetEffectedStat(EStatusEffect eEffect, float fStat);

	//Clears out all status effects
	void Dispel(void);

	//Removes any effects that need to go
	void Update(float fDt);

	bool IsEffectActive(EStatusEffect eEffect) const { return m_tStatusEffects[eEffect].m_fDuration > 0; }

	//---------ACCESSORS-----------
	float GetEffectDuration(EStatusEffect eEffect) const { return m_tStatusEffects[eEffect].m_fDuration; }
	float GetEffectPotency(EStatusEffect eEffect) const { return m_tStatusEffects[eEffect].m_fPotency; }

	//---------Mutators-------------
	void SetEffectPotency(EStatusEffect eEffect, float fPotency) { m_tStatusEffects[eEffect].m_fPotency = fPotency; }
	void SetEffectDuration(EStatusEffect eEffect, float fDuration) { m_tStatusEffects[eEffect].m_fDuration = fDuration; }

private:
	//All current effects
	TStatusEffect m_tStatusEffects[STAT_EFF_COUNT];

};

#endif