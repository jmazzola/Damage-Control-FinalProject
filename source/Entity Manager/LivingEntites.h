/***********************************************
* Filename:			LivingEntities.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Brandon Lucier
* Purpose:			the base class for every entity
*					that is "alive".
************************************************/

#ifndef _CLIVINGENTIIES_H_
#define _CLIVINGENTIIES_H_

#define IS_IDLE 0x1
#define IS_HIT 0x2
#define IS_ATTACKING 0x4
#define IS_DEPLOYING 0x8

#include "Entity.h"
#include "../Animation Manager/Interpolator.h"
#include "../Animation Manager/Deformer.h"
#include "../Asset Manager/AssetManager.h"
#include "../Animation Manager/Animation State Machine/AnimStateMachine.h"
#include "../Animation Manager/AnimationBlender.h"

class CStatusBank;
class CParticleSystem;
enum EStatusEffect;

class CLivingEntities : public CEntity
{
public:
	CLivingEntities();
	virtual ~CLivingEntities();

	virtual void Update(float dt) override;

	virtual void TakeDamage(float fAmount);
	virtual void TakeDamageOverTime(float fAmount);
	virtual void Heal(float _fAmount);

	virtual void Disable(void) override;

	void AddStatusEffect(EStatusEffect eEffect, float fDuration, float fPotency);

	float PercentHealth(void) const { return m_fCurrentHealth / m_fMaxHealth; }
	/**************
	* Accessors
	**************/
	float GetMaxHealth() const { return m_fMaxHealth; }
	float GetCurrentHealth() const { return m_fCurrentHealth; }
	bool GetAlive() const { return m_bIsAlive; }
	void SetAlive(bool b) { m_bIsAlive = b; }

	CDeformer& GetDeformer() { return m_cDeformer; }
	bool		GetExplosionColliding()const { return m_bIsExplosionColliding; }

	CAnimationBlender<DirectX::XMFLOAT4X4>& GetAnimationBlender( )
	{
		return m_cAnimBlender;
	}
	CAnimStateMachine& GetAnimStateMachine() { return m_cAnimStateMachine; }

	float GetScaling( )
	{
		return m_fScaling;
	}

	int& GetActionBitfield()
	{
		return actionBitfield;
	}

	
	bool IsIdle() { return BitTwiddler::CheckBit(actionBitfield, IS_IDLE); }
	bool IsHit() { return BitTwiddler::CheckBit(actionBitfield, IS_HIT); }
	bool IsAttacking() { return BitTwiddler::CheckBit(actionBitfield, IS_ATTACKING); }
	bool IsDeploying() { return BitTwiddler::CheckBit(actionBitfield, IS_DEPLOYING); }

	BOOL IsWalkingForward() const { return (isWalkingForward == 1); }
	BOOL IsWalkingBackward() const { return (isWalkingForward == 0); }
	int  GetWalking() const { return isWalkingForward; }

	BOOL IsStrafingLeft() const { return (isStrafing == 1); }
	BOOL IsStrafingRight() const { return (isStrafing == 0); }
	int  GetStrafing()	const { return isStrafing; }

	/**************
	* Mutators
	**************/

	void SetExplosionColliding(bool YesNo) { m_bIsExplosionColliding = YesNo; m_fExplosionDamageTimer = 0.5f; }
	

protected:
	virtual bool LoadStats(void);

	// Sound* wwSound;
	float m_fMaxHealth;
	float m_fCurrentHealth;
	bool m_bIsAlive;
	float m_fTimer = -1;

	bool			m_bIsExplosionColliding = false;
	float			m_fExplosionDamageTimer;
	// Animation* aAnim;

	//Holds the status effects
	CStatusBank* m_pStatusEffects = nullptr;
	CParticleSystem* m_pSlowEffect = nullptr;

	CAssetManager::TAnimation* m_pAnimation;	// enemy animation
	CInterpolator<DirectX::XMFLOAT4X4> m_cInterpolator;	// interpolator
	CDeformer m_cDeformer;

	CAnimationBlender<DirectX::XMFLOAT4X4> m_cAnimBlender;
	CAnimStateMachine m_cAnimStateMachine;

	// Bitfield
	int actionBitfield;
	int isWalkingForward;
	int isStrafing;
	float m_fTakeDamageTimer;

	float m_fScaling;

private:

};
#endif

