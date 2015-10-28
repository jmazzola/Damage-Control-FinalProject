/***********************************************
* Filename:			HunterEnemy.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:			defiens the enemy who tracks and follows the player
************************************************/

#ifndef _CHUNTERENEMY_H_
#define _CHUNTERENEMY_H_

#include "BaseEnemy.h"

//#pragma region Hunter Info
//
//#define HUNTER_BRONZE_HEALTH	60.0f
//#define HUNTER_SILVER_HEALTH	72.0f
//#define HUNTER_GOLD_HEALTH		90.0f
//
//#define HUNTER_BRONZE_SCRAP		25.0f
//#define HUNTER_SILVER_SCRAP		35.0f
//#define HUNTER_GOLD_SCRAP		45.0f
//
//#define HUNTER_BRONZE_DAMAGE	15.0f
//#define HUNTER_SILVER_DAMAGE	20.0f
//#define HUNTER_GOLD_DAMAGE		28.0f
////NOTE: Lower = faster
//#define HUNTER_ATTACK_FREQUENCY 2.0f
//
//#pragma endregion

class CIWeapon;

class CHunterEnemy 
	: public CBaseEnemy
{
public:

	/*****************************************************************
	* CHunterEnemy(): default constructor.
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:
	*****************************************************************/
	CHunterEnemy();

	/*****************************************************************
	* ~CHunterEnemy(): default destructor
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:
	*****************************************************************/
	~CHunterEnemy();

	/*****************************************************************
	* Update():
	*
	* Ins: float fDt
	*
	* Outs: N/A
	*
	* Returns: N/A
	*
	* Mod. Date:		05/13/2015
	* Mod. Initials:	JM
	*****************************************************************/
	virtual void	Update(float fDt) override;

	/*****************************************************************
	* UpdateObjective():
	*
	* Ins: float fDt
	*
	* Outs: N/A
	*
	* Returns: N/A
	*
	* Mod. Date:		05/13/2015
	* Mod. Initials:	JM
	*****************************************************************/
	virtual void	UpdateObjective(float fDt) override;

	/*****************************************************************
	* UpdatePatrol(): Checks if the agent has reached to the specific location
	*			if not, moves to the closest node.
	*			else changes the destination or prepares to attack stopping.
	*
	* Ins: float fDt
	*
	* Outs: N/A
	*
	* Returns: N/A
	*
	* Mod. Date:		05/13/2015
	* Mod. Initials:	JM
	*****************************************************************/
	virtual void	UpdatePatrol(float fDt) override;

	/*****************************************************************
	* UpdateAggro(): Update of the aggro state.
	*			updates the elapsed attack time and attacks
	*
	* Ins: float fDt
	*
	* Outs: N/A
	*
	* Returns: N/A
	*
	* Mod. Date:		05/13/2015
	* Mod. Initials:	JM
	*****************************************************************/
	virtual void	UpdateAggro(float fDt) override;

	/*****************************************************************
	* EvaluateState(): Evaluates and updates the current state.
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		05/12/2015
	* Mod. Initials:	JM
	*****************************************************************/
	virtual void	EvaluateState(void) override;

	/*****************************************************************
	* Enable(): sets the enemy's position to the spawner and restes the path. After that
	*			it moves the enemy to the living list.
	*
	* Ins:		DirectX::XMFLOAT3 SpawnPoint
	*			eEnemyVariant variant
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		05/27/2015
	* Mod. Initials:	JM
	*****************************************************************/
	virtual void Enable(DirectX::XMFLOAT3 SpawnPoint, 
						eEnemyVariant variant = eEnemyVariant::eNoVariant) override;

	/*****************************************************************
	* Disable(): sets active to false which means this entity will be moved to
				the dead list and don't be updated once there.
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		05/27/2015
	* Mod. Initials:	JM
	*****************************************************************/
	virtual void Disable() override;

	/*****************************************************************
	* GoldBehavior(): functiion to handle the gold behavior stealth mode
	*
	* Ins: float fDt - delta time
		  bool &attack - to handle the attack in stealth mode
	*
	* Outs: &attack
	*
	* Returns: void
	*
	* Mod. Date:		06/17/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void GoldBehavior(float fDt, 
					bool& attack);

	void TakeDamage(float fDamage);

	void Attack(float fDamage);

	bool RegisterAnimation(void);

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

protected:
	virtual bool LoadStats(void) override;
	virtual void ApplyVariant(eEnemyVariant eTier) override;

private:
	TSphere*	m_tCollider;
	float		m_fHideTimer;
	float		m_fCurrentHideTimer;
	bool		m_bIamStealth;
	//CIWeapon*	m_pSppitlePistol;

	//Store the base stats for hunters
	static float s_fPresetHealth[eEnemyVariant::eVariantCount];
	static float s_fPresetArmor[eEnemyVariant::eVariantCount];
	static float s_fPresetDamage[eEnemyVariant::eVariantCount];
	static float s_fPresetAttackSpeed[eEnemyVariant::eVariantCount];
	static float s_fPresetAttackRange[eEnemyVariant::eVariantCount];
	static float s_fPresetMoveSpeed[eEnemyVariant::eVariantCount];
	static float s_fPresetTurnSpeed[eEnemyVariant::eVariantCount];
	static int s_nPresetScrap[eEnemyVariant::eVariantCount];
	static float s_fPresetColliderRadius[eEnemyVariant::eVariantCount];
	static float s_fPresetScale[eEnemyVariant::eVariantCount];//Will be applied to x, y, and z
	static float s_fPresetMass[eEnemyVariant::eVariantCount];

	CParticleSystem* m_pExplodeEffect;
	CParticleSystem* m_pStepEffects;
	float m_fParticleDelay;
};

#endif // _CHUNTERENEMY_H_

