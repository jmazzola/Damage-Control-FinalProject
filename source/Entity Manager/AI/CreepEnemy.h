/***********************************************
* Filename:			CreepEnemy.h
* Date:      		06/02/2015
* Mod. Date:		
* Mod. Initials:	
* Author:    		Jagoba Marcos
* Purpose:			handles the small enemy. Fast but weak.
************************************************/

#ifndef _CCREEP_ENEMY_H_
#define _CCREEP_ENEMY_H_

#include "BaseEnemy.h"

#pragma region Creep Info

#define CREEP_BRONZE_HEALTH	32.0f
#define CREEP_SILVER_HEALTH	35.0f
#define CREEP_GOLD_HEALTH	54.0f

#define CREEP_BRONZE_SCRAP	10.0f
#define CREEP_SILVER_SCRAP	15.0f
#define CREEP_GOLD_SCRAP	25.0f

#define CREEP_BRONZE_DAMAGE	5.0f
#define CREEP_SILVER_DAMAGE	10.0f
#define CREEP_GOLD_DAMAGE	12.5f

#define CREEP_KAMIKAZE_DAMAGE	40.0f
//NOTE: Lower = faster
#define CREEP_ATTACK_FREQUENCY 0.9f

#pragma endregion

class CParticleSystem;

class CCreepEnemy :
	public CBaseEnemy
{
public:

	/*****************************************************************
	* CCreeperEnemy(): default constructor.
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		06/02/2015
	* Mod. Initials:	JM
	*****************************************************************/
	CCreepEnemy();

	/*****************************************************************
	* ~CCreeperEnemy(): default destructor
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		06/02/2015
	* Mod. Initials:	JM
	*****************************************************************/
	~CCreepEnemy();

	/*****************************************************************
	* Update():
	*
	* Ins: float fDt
	*
	* Outs: N/A
	*
	* Returns: N/A
	*
	* Mod. Date:		06/02/2015
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
	* Mod. Date:		06/02/2015
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
	* Mod. Date:		06/02/2015
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
	* Mod. Date:		06/02/2015
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
	* Mod. Date:		06/02/2015
	* Mod. Initials:	JM
	*****************************************************************/
	virtual void	EvaluateState(void) override;

	/*****************************************************************
	* GenerateNextRandomPoint(): generates a random point in the map.
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		06/02/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void	GenerateNextRandomPoint(void);

	/*****************************************************************
	* EvaluateState(): Evaluates and updates the current state.
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		06/02/2015
	* Mod. Initials:	JM
	*****************************************************************/
	bool	CheckDestination(void);

	/*****************************************************************
	* Enable(): sets the enemy's position to the spawner and restes the path. After that
	*			it moves the enemy to the living list.
	*
	* Ins:		DirectX::XMFLOAT3 SpawnPoint
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		06/02/2015
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
	* Mod. Date:		06/02/2015
	* Mod. Initials:	JM
	*****************************************************************/
	virtual void Disable() override;

	/*****************************************************************
	* GetCollider(): returns the collider of the agent
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: TSphere*
	*
	* Mod. Date:		06/02/2015
	* Mod. Initials:	JM
	*****************************************************************/
	TSphere* GetCollider() override { return m_tCollider; }

	/*****************************************************************
	* ManageMovement(): Function to handle all movement for the agent
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		06/02/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void ManageMovement(float fDt);

	/*****************************************************************
	* ManageMovement(): Function to manage the Aggro movement
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		06/03/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void ManageAggroMovement(float fDt);

	void TakeDamage(float fDamage);

	void Attack(float fDamage);

	bool GetBombExploded()const { return m_bBombExploded; }

	bool RegisterAnimation(void);

	/*****************************************************************
	* ExplosionManagement(): silver variant's special power
	*
	* Ins: float fDt
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		06/11/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void ExplosionManagement(float fDt);

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

	static float GetPresetExplodeRadius(void) { return s_fPresetExplodeRadius; }
	static float GetPresetExplodeDamage(void) { return s_fPresetExplodeDamage; }
	static float GetPresetFuseTime(void) { return s_fPresetFuseTime; }

protected:
	virtual bool LoadStats(void) override;
	virtual void ApplyVariant(eEnemyVariant eTier) override;

private:

	TSphere*			m_tCollider;
	float				m_fBlinkTimer;
	bool				m_bBombActivated;
	bool				m_bMoving = false;
	float				m_fBombTimer;
	bool				m_bBombExploded;
	CParticleSystem* m_pExplodeEffect;
	CParticleSystem* m_pDeathEffect;

	//Store the base stats for creeps
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

	//SPECIAL
	static float s_fPresetExplodeRadius;
	static float s_fPresetExplodeDamage;
	static float s_fPresetFuseTime;
};

#endif // _CCREEP_ENEMY_H_
