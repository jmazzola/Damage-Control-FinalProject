/***********************************************
* Filename:			EnemyPistol.h
* Date:      		06/17/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:			Silver hunter's ranged attack
************************************************/

#ifndef _CENEMY_PISTOL_H_
#define _CENEMY_PISTOL_H_

#include "IWeapon.h"

class CEntity;

class CEnemyPistol :
	public CIWeapon
{
public:
	CEnemyPistol();
	~CEnemyPistol();

	bool Shoot() override;

	void Update(float dt) override;

	void UpdateEnemyW(float dt, CEntity* owner) override;

	void UpgradeWeaponTier();

private:

};

#endif // _CENEMY_PISTOL_H_