#ifndef _CIWEAPON_H_
#define _CIWEAPON_H_

#include "..\Renderer\RObject.h"

class CModBank;

enum EGunType : signed char
{
	GUN_BASE = -1, GUN_SEMI_AUTO = 0, GUN_FULL_AUTO, GUN_SCATTER, GUN_DRONE,
	ENEMY_PISTOL, ENEMY_FASTSMG, GUN_TYPE_COUNT
};

// | these guys together
enum ESpecialBullets : unsigned char
{
	BULLET_RICOCHET = 1,
	BULLET_CHAIN = 2,
	BULLET_PIERCE = 4,
	BULLET_EXPLODE = 8,
	BULLET_FREEZE = 16,
	BULLET_POISON = 32,
	BULLET_SLOW = 64,
	BULLET_FUCKYOU = 128//Ran out of ideas :(
};

class CEntity;
class CIWeapon : public CRObject
{
public:
	CIWeapon(EGunType eType);
	virtual ~CIWeapon( );

	virtual bool Shoot( );
	virtual void Update(float dt);
	virtual void UpdateEnemyW(float dt, CEntity* owner) = 0;

	bool Reload(void);

	//Set all stats to their default, reload clips, etc.
	virtual void Reset(void);

	/********************
	* Accessors
	********************/
	virtual float GetDamage( ) const
	{
		return m_fDamage;
	}
	virtual float GetSpread( ) const
	{
		return m_fSpread;
	}
	virtual float GetBulletLifeTime(void) const
	{
		return m_fBulletLifeTime;
	}
	bool GetUnlocked(void) const
	{
		return m_bUnlocked;
	}
	EGunType GetGunType(void) const
	{
		return m_eType;
	}
	bool GetReloading(void) const
	{
		return m_bReloading;
	}
	int GetCurrentMagazine(void) const
	{
		return m_nCurrentMagazine;
	}
	int GetMaxAmmo(void) const
	{
		return m_nMaxMagazine;
	}
	//How many bullets are missing from the clip
	int GetMissingBullets(void) const
	{
		return m_nMaxMagazine - m_nCurrentMagazine;
	}
	int GetBulletCost(void) const
	{
		return m_nBulletScrapCost;
	}
	//How much will it cost to reload
	int GetReloadCost(void) const
	{
		return m_nBulletScrapCost * (m_nMaxMagazine - m_nCurrentMagazine);
	}
	float GetReloadTime(void) const
	{
		return m_fTimeToReload;
	}
	float GetReloadSpeed(void) const
	{
		return m_fReloadSpeed;
	}
	int GetWeaponScrapCost(void) const
	{
		return m_nWeaponScrapCost;
	}
	CModBank* GetModBank(void) const
	{
		return m_pModules;
	}
	float GetFireRate(void) const
	{
		return m_fFireRate;
	}

	CRObject* GetFlash( ) const
	{
		return m_pFlash;
	}
	CRObject* GetCrosshair( ) const
	{
		return m_pCrosshair;
	}
	/********************
	* Mutators
	********************/
	void SetWeaponScrapCost(int nScrapCost)
	{
		m_nWeaponScrapCost = nScrapCost;
	}
	void SetUnlocked(bool bUnlocked)
	{
		m_bUnlocked = bUnlocked;
	}
	void SetDamage(float fDamage)
	{
		m_fDamage = fDamage;
	}
	void SetFireRate(float fFireRate)
	{
		m_fFireRate = fFireRate;
	}
	void SetMagSize(int nMagSize)
	{
		m_nMaxMagazine = nMagSize;
	}
	void SetReloadSpeed(float fReloadSpeed)
	{
		m_fReloadSpeed = fReloadSpeed;
	}
protected:
	virtual bool LoadStats(void)
	{
		return false;
	}

	//bool m_bUnlocked = false;
	bool m_bFiring = false;
	bool m_bReloading = false;
	//Our owner told us to reload
	bool m_bToReload = false;
	//Is the weapon availible for player use?
	bool m_bUnlocked = false;

	//What level is the weapon at?
	//	Lvl 0 means the weapon is unlocked
	//signed char m_nCurrentTier = -1;

	//The size of the magazine and the current bullets in the magazine
	int m_nMaxMagazine = 0, m_nCurrentMagazine = 0;
	//Scrap you pay to get a new clip of ammo
	int m_nBulletScrapCost = 0;
	//Weapon value
	int m_nWeaponScrapCost = 0;

	float m_fDamage = 0.0f;
	float m_fSpread = 0.0f;
	float m_fBulletLifeTime = 2.0f;

	float m_fFireRate = 0.0f;
	float m_fLastFireTime = 0.0f;

	float m_fReloadTimer = 0.0f;
	float m_fTimeToReload = 0.0f;
	float m_fReloadSpeed = 1.0f;

	DirectX::XMFLOAT3 m_d3dPosition;
	DirectX::XMFLOAT3 m_XMBarrel;

	//float m_fBulletSpeed = 2100.0f;

	//BIT FLAGS!
	//unsigned char m_uSpecialFlag = 0;

	//Holds and manages the modules for our weapons
	CModBank* m_pModules = nullptr;

	float m_fTotalTime = 0;

	CRObject* m_pFlash = NULL;
	CRObject* m_pCrosshair = NULL;

private:
	//Easy access to what type of gun it is
	EGunType m_eType = GUN_BASE;
};

#endif //_CIWEAPON_H_
