/***********************************************
* Filename:  		DroneBuddy.h
* Date:      		06/11/2015
* Mod. Date: 		06/11/2015
* Mod. Initials:	BL
* Author:    		Brandon Lucier
* Purpose:   		Class for our lovely drone buddy
*					which helps us kill stuff
************************************************/

#ifndef _CDRONEBUDDY_H_
#define _CDRONEBUDDY_H_

#include "IWeapon.h"
//#include "../Entity Manager/AI/BaseEnemy.h"
#include <vector>

class CBaseEnemy;
class CPlayer;
class CSubSystem;
class CParticleSystem;

class CDroneBuddy : public CIWeapon
{
public:
	CDroneBuddy(CPlayer* Player);
	~CDroneBuddy();

	void Update(float dt) override;
	void UpdateEnemyW(float dt, CEntity* owner) override;

	void Enable(); 
	void Disable();

	void FindTarget();
	void TurnToTarget();
	bool Shoot() override;

	void MergeLists();

	void Explode();

	void RepairSubSystem(float dt);

	//Sets all drone specific stats to their default. Use before applying mods
	void ResetDroneStats(void);

	/***************
	* Accessors
	***************/
	bool GetIsActive() { return isActive; }
	bool IsReady() { return m_bReady; }
	float GetCooldown() { return m_fCooldownTimer; }

	//Returns how long the drone will take to cooldown directly after use
	float GetCoolDownTime();
	//Returns how long the drone will live immediately after being deployed
	float GetLifeTime();

	float GetExplodeDamage(void) const { return m_fExplosionDamage; }
	float GetExplodeRange(void) const { return m_fExplosionRange; }
	float GetRepairSpeed(void) const { return m_fRepairSpeed; }

	/***************
	* Mutators
	***************/
	void SetActive(bool YesNo) { isActive = YesNo; }
	void SetCooldown(float fCooldown) { m_fCooldownTimer = fCooldown; m_bReady = fCooldown <= 0; }
	void SetLifeMultiplier(float fMultiplier) { m_fLifeMultiplier = fMultiplier; }
	void SetCooldownSpeed(float fCooldownSpeed) { m_fCooldownSpeed = fCooldownSpeed; }
	void SetExplodeDamage(float fExplodeDamage) { m_fExplosionDamage = fExplodeDamage; }
	void SetExplodeRange(float fExplodeRange) { m_fExplosionRange = fExplodeRange; }
	void SetRepairSpeed(float fRepairSpeed) { m_fRepairSpeed = fRepairSpeed; }

private:
	CBaseEnemy* m_pTarget;
	CPlayer* m_pPlayer;

	bool isActive;
	bool m_bReady;

	float m_fRange;
	float m_fAimTimer;
	float m_fCooldownTimer;
	float m_fLiveTimer;

	float m_fExplosionDamage = 50.0f;
	float m_fExplosionRange = 200.0f;
	float m_fRepairSpeed = 1.0f;

	//Scale delta time when we calculate cooldown/life time
	float m_fCooldownSpeed = 1.0f;
	float m_fLifeMultiplier = 1.0f;

	std::vector<CBaseEnemy*> m_lpAllEnemies;
	std::vector<CSubSystem*> m_lpSystems;

	CParticleSystem* m_pPoofEffect;
};

#endif