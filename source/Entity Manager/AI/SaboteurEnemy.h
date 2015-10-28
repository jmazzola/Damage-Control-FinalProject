/***********************************************
* Filename:			SaboteurEnemy.h
* Date:      		05/12/2015
* Mod. Date:		05/25/2015
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:			handles the enemy that targets the subsystems
************************************************/

#ifndef _CSABOTEURENEMY_H_
#define _CSABOTEURENEMY_H_

#include "BaseEnemy.h"

//#pragma region Saboteur Info
//
//#define SABO_BRONZE_HEALTH	100.0f
//#define SABO_SILVER_HEALTH	120.0f
//#define SABO_GOLD_HEALTH	150.0f
//
//#define SABO_BRONZE_SCRAP	50.0f
//#define SABO_SILVER_SCRAP	75.0f
//#define SABO_GOLD_SCRAP		100.0f
////NOTE: This is (kinda) DPS
//#define SABO_BRONZE_DAMAGE	12.0f
//#define SABO_SILVER_DAMAGE	16.0f
//#define SABO_GOLD_DAMAGE	20.0f
////NOTE: Lower = faster
//#define SABO_ATTACK_FREQUENCY 3.5f

#pragma endregion

class CSubSystem;
class CIWeapon;

class CSaboteurEnemy
	: public CBaseEnemy
{
public:

	/*****************************************************************
	* CSaboteurEnemy(): default constructor.
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
	CSaboteurEnemy();

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
	~CSaboteurEnemy();

	/*****************************************************************
	* Update():
	*
	* Ins: float fDt
	*
	* Outs: N/A
	*
	* Returns: N/A
	*
	* Mod. Date:		05/14/2015
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
	* Mod. Date:		05/14/2015
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
	* Mod. Date:		05/14/2015
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
	* Mod. Date:		05/14/2015
	* Mod. Initials:	JM
	*****************************************************************/
	virtual void	EvaluateState(void) override;

	/*****************************************************************
	* FindClosestsTarget(): calculates the euclidean distance and
	gets a targe
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		05/14/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void	FindClosestsTarget(void);

	/*****************************************************************
	* EvaluateState(): Evaluates and updates the current state.
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		05/14/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void	FindNextTarget(void);

	/*****************************************************************
	* EvaluateState(): Evaluates and updates the current state.
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		05/14/2015
	* Mod. Initials:	JM
	*****************************************************************/
	bool	CheckDestination(void);

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
	* GetCollider(): returns the collider of the agent
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: TSphere*
	*
	* Mod. Date:		05/25/2015
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
	* Mod. Date:		05/27/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void ManageMovement(float fDt);

	/*****************************************************************
	* GoldBehavior(): special gold variant behavior
	*
	* Ins: float delta time
	*
	* Outs: void
	*
	* Returns: bool to know if it has been moved
	*
	* Mod. Date:		06/17/2015
	* Mod. Initials:	JM
	*****************************************************************/
	//bool GoldBehavior(float fDt);

	void TakeDamage(float fDamage);

	void Attack(float fDamage);

	bool RegisterAnimation(void);

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

	//CRObject* GetAttackEffect( ) { return m_pAttackEffect; }
protected:
	virtual bool LoadStats(void) override;
	virtual void ApplyVariant(eEnemyVariant eTier) override;

private:
	int					m_nCurrentSystem = 0;
	TSphere*			m_tCollider;
	std::list<CSubSystem*>	m_lpSubSystemList;
	CSubSystem*			m_pCurrentSystem;
	float				m_fSubsystemAttackTimer;
	float				m_fSubsystemElapsed;
	float				m_fBaseDamage;

	//Store the base stats for saboteurs
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
	static float s_fPresetShootRange;

	//CRObject* m_pAttackEffect;
	CParticleSystem* m_pExplodeEffect;
	CParticleSystem* m_pAttackEffect;
};

#endif // _CSABOTEURENEMY_H_

