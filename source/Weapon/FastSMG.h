/***********************************************
* Filename:			FastSMG.h
* Date:      		06/17/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:			Saboteur enemy fast ranged attack 
					that deals small amount of damage
************************************************/

#ifndef _CFAST_SMG_H_
#define _CFAST_SMG_H_

#include "IWeapon.h"
#include "../Entity Manager/Entity.h"

class CEntity;

class CFastSMG :
	public CIWeapon
{
public:
	CFastSMG();
	~CFastSMG();

	bool Shoot() override;

	void Update(float dt) override;

	void UpdateEnemyW(float dt, CEntity* owner) override;

	void UpgradeWeaponTier();

private:

};

#endif // _CFAST_SMG_H_