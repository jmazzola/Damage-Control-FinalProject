/***********************************************
* Filename:			ShotGun.h
* Date:      		05/15/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Brandon Lucier
* Purpose:			ShotGun represents the properties
*				one of several weapons that the player
*				can use
************************************************/
#ifndef _CSHOTGUN_H_
#define _CSHOTGUN_H_

#include "IWeapon.h"

class CShotGun : public CIWeapon
{
public:
	CShotGun();
	~CShotGun();

	virtual void Reset(void) override;

	bool Shoot() override;
	void Update(float dt) override;
	void UpdateEnemyW(float dt, CEntity* owner) override;

	int GetPelletCount(void) const { return m_nPelletCount; }

	//Get base stats
	float static GetBaseDamage(void) { return s_fPresetDamage; }
	float static GetBaseFireRate(void) { return s_fPresetFireRate; }
	float static GetBaseReloadTime(void) { return s_fPresetReloadTime; }
	float static GetBaseReloadSpeed(void) { return s_fPresetReloadSpeed; }
	float static GetBaseSpread(void) { return s_fPresetSpread; }
	int static GetBaseMagSize(void) { return s_nPresetMagSize; }
	int static GetBasePelletCount(void) { return s_nPresetPelletCount; }

	int static GetBaseWeaponCost(void) { return s_nPresetWeaponCost; }
	int static GetBaseBulletCost(void) { return s_nPresetBulletCost; }
	int static GetBaseModLimit(void) { return s_nPresetModLimit; }
	int static GetBaseMagCost(void) { return s_nPresetBulletCost * s_nPresetMagSize; }

protected:
	virtual bool LoadStats(void) override;

	//Preset to be loaded from xml
	static float s_fPresetDamage;
	static float s_fPresetFireRate;
	static float s_fPresetReloadTime;
	static float s_fPresetReloadSpeed;
	static float s_fPresetSpread;
	static int s_nPresetMagSize;
	static int s_nPresetPelletCount;

	static int s_nPresetWeaponCost;
	static int s_nPresetBulletCost;
	static int s_nPresetModLimit;

private:
	int m_nPelletCount = 0;
};

#endif