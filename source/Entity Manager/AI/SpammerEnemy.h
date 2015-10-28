/***********************************************
* Filename:			SpammerEnemy.h
* Date:      		08/14/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Justin Mazzola
* Purpose:			shoots at the player
************************************************/

#ifndef _CSPAMMERENEMY_H_
#define _CSPAMMERENEMY_H_

#include "BaseEnemy.h"

class CIWeapon;

class CSpammerEnemy : 
	public CBaseEnemy
{

public:

	CSpammerEnemy();
	~CSpammerEnemy();

	virtual void Update(float fDT) override;
	virtual void UpdateObjective(float fDT) override;
	virtual void UpdatePatrol(float fDT) override;
	virtual void UpdateAggro(float fDT) override;

	virtual void EvaluateState() override;

	virtual void Enable(DirectX::XMFLOAT3 xmSpawnPoint, eEnemyVariant eVar = eEnemyVariant::eNoVariant) override;
	virtual void Disable() override;

	void TakeDamage(float fDamage);
	
	void Attack(float fDamage);

	bool RegisterAnimation();

	/***************
	*  Accessors
	***************/
	TSphere* GetCollider() override { return m_tCollider; }

	//Get the static presets
	static float GetPresetHealth(int nTier) { return s_fPresetHealth[nTier]; }
	static float GetPresetArmor(int nTier) { return s_fPresetArmor[nTier]; }
	static float GetPresetDamage(int nTier) { return s_fPresetDamage[nTier]; }
	static float GetPresetAttackSpeed(int nTier) { return s_fPresetAttackSpeed[nTier]; }
	static float GetPresetRange(int nTier) { return s_fPresetAttackRange[nTier]; }
	static float GetPresetMoveSpeed(int nTier) { return s_fPresetMoveSpeed[nTier]; }
	static float GetPresetTurnSpeed(int nTier) { return s_fPresetTurnSpeed[nTier]; }
	static int GetPresetScrap(int nTier) { return s_nPresetScrap[nTier]; }
	static float GetPresetColliderRadius(int nTier) { return s_fPresetColliderRadius[nTier]; }
	static float GetPresetScale(int nTier) { return s_fPresetScale[nTier]; }
	static float GetPresetMass(int nTier) { return s_fPresetMass[nTier]; }

	//CRObject* GetAttackEffect() { return m_pAttackEffect; }

protected:
	virtual bool LoadStats(void) override;
	virtual void ApplyVariant(eEnemyVariant eTier) override;

private:

	TSphere*	m_tCollider;
	CIWeapon*	m_pSppitlePistol;

	//Store the base stats for spammers
	static float s_fPresetHealth[eEnemyVariant::eVariantCount];
	static float s_fPresetArmor[eEnemyVariant::eVariantCount];
	static float s_fPresetDamage[eEnemyVariant::eVariantCount];
	static float s_fPresetAttackSpeed[eEnemyVariant::eVariantCount];
	static float s_fPresetAttackRange[eEnemyVariant::eVariantCount];
	static float s_fPresetMoveSpeed[eEnemyVariant::eVariantCount];
	static float s_fPresetTurnSpeed[eEnemyVariant::eVariantCount];
	static int	 s_nPresetScrap[eEnemyVariant::eVariantCount];
	static float s_fPresetColliderRadius[eEnemyVariant::eVariantCount];
	static float s_fPresetScale[eEnemyVariant::eVariantCount];//Will be applied to x, y, and z
	static float s_fPresetMass[eEnemyVariant::eVariantCount];

	//CRObject* m_pAttackEffect;
	CParticleSystem* m_pExplodeEffect;
};

#endif // _CSPAMMERENEMY_H_
