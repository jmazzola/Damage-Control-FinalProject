#ifndef _CBULLET_H_
#define _CBULLET_H_

#include "../Core/stdafx.h"
#include "..\Entity Manager\Entity.h"

#include "../Core/StructureHolder.h"

class CIWeapon;
class CParticleSystem;

class CBullet : public CEntity
{
public:
	CBullet();
	~CBullet();

	virtual void Update(float dt);

	void Enable(DirectX::XMFLOAT3 spawnPoint, CIWeapon* Owner, CEntity::eEntityType myType = CEntity::eBULLET);
	void Disable() override;

	/*****************************************************************
	* Collide(): The bullet reacts to what it collides with
	*				if colliding with the map, pass in nullptr for pOther
	*							
	* Ins:						pCollider,
	*							pOther
	* Outs:						void
	* Returns:					void
	* Mod. Date:				06/19/2015
	* Mod. Initials:			MJG
	*****************************************************************/
	void Collide(TCollider* pCollider, CEntity* pOther);

	//-------Accessors-----------
	TSphere* GetCollider() override { return tCollider; }
	CIWeapon* GetOwner() { return m_pOwner; }
	bool GetAlive() const { return m_bAlive; }
	bool GetFriendly(void) const { return m_bFriendly; }
	float GetDamage(void) const { return m_fDamage; }
	float GetSpeed(void) const { return m_fSpeed; }
	float GetRange(void) const { return m_fMaxLifeTime; }
	//------Mutators------------
	void SetSpecial(TWeaponSpecials& tSpecials) { m_tSpecialLevels = tSpecials; }
	void SetSpecialFlag(unsigned char uSpecialFlag) { m_uSpecialFlag = uSpecialFlag; }
	void SetDamage(float fDamage) { m_fDamage = fDamage; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetRange(float fRange) { m_fMaxLifeTime = fRange; }

protected:
	/*****************************************************************
	* HandleColor(): Changes bullet's color based on its owner's properties
	*
	* Ins:						void
	* Outs:						void
	* Returns:					void
	* Mod. Date:				06/19/2015
	* Mod. Initials:			MJG
	*****************************************************************/
	void HandleColor(void);

	/*****************************************************************
	* HyperBeam(): Changes bullet's color to match the rainbow
	*				
	*
	* Ins:						void
	* Outs:						void
	* Returns:					void
	* Mod. Date:				06/19/2015
	* Mod. Initials:			MJG
	*****************************************************************/
	void HyperBeam(void);

private:
	CParticleSystem* m_pBloodImpactEffect = nullptr;
	//Mod particles
	CParticleSystem* m_pPierceTrail = nullptr;
	CParticleSystem* m_pSlowTrail = nullptr;

	DirectX::XMFLOAT4X4 world_matrix;
	TSphere* tCollider;
	DirectX::XMFLOAT3 m_xmForward;
	CIWeapon* m_pOwner;

	float m_fSpeed = 1800;
	float m_fMaxLifeTime, m_fCurrentLifeTime;
	float m_fDist = 10000;
	//How much damage do you deal?
	float m_fDamage = 0.0f;

	float m_fSpawnTime = 0.0f;

	bool m_bAlive;
	//TRUE: Player's bullet
	//FALSE: Enemies' bullet
	bool m_bFriendly = true;

	//Guys we've hit before
	std::vector<CEntity*> m_vPastMarks;

	//Kept for super secret things
	unsigned char m_uSpecialFlag = 0;
	//We need more than just flags now
	TWeaponSpecials m_tSpecialLevels;
};

#endif // _CBULLET_H_