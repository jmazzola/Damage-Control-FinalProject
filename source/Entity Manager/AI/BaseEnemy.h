/***********************************************
* Filename:			BaseEnemy.h
* Date:      		05/12/2015
* Mod. Date:		05/21/2015
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:			Pure virtual class to store enemy shared members
*					and functions.
************************************************/

#ifndef _CBASEENEMY_H_
#define _CBASEENEMY_H_

/*****************************************************************
* eAIStateType: Enumeration that defines the enemy's current state.
*
* Mod. Date:		05/13/2015
* Mod. Initials:	JM
*****************************************************************/
enum eAIStateType
{
	eObjective = 0,
	ePatrol,
	eAggro
};

#include "..\..\Core\stdafx.h"

#include "..\..\Math\vec3.h"
#include "..\LivingEntites.h"
#include "..\Pathfinding\PathSearch.h"
#include "..\..\Core\EnumerationHolder.h"
#include "..\..\Asset Manager\AssetManager.h"

DirectX::XMFLOAT3 Truncate(DirectX::XMFLOAT3 vector, float maxValue);

class CBaseEnemy : public CLivingEntities
{
public:

	/*****************************************************************
	* CBaseEnemy(): default constructor.
	*
	* Ins: N/A
	*
	* Outs: N/A
	*
	* Returns: N/A
	*
	* Mod. Date:		05/13/2015
	* Mod. Initials:	JM
	*****************************************************************/
	CBaseEnemy();


	/*****************************************************************
	* ~CBaseEnemy(): destructor to free dynamic memory and update entity reference count.
	*
	* Ins: N/A
	*
	* Outs: N/A
	*
	* Returns: N/A
	*
	* Mod. Date:		05/13/2015
	* Mod. Initials:	JM
	*****************************************************************/
	virtual ~CBaseEnemy();

	virtual void	Update(float fDt) override;
	virtual void	UpdateObjective(float fDt) = 0;
	virtual void	UpdatePatrol(float fDt) = 0;
	virtual void	UpdateAggro(float fDt) = 0;
	virtual void	EvaluateState(void) = 0;
	virtual void	TakeDamage(float fAmount);

	/*****************************************************************
	* ChangeTarget(): Checks if the current target is null to decrease the
	*				reference count. After checks if the new target is null
	*				to increase the new target's reference count.
	*
	* Ins: CEntity* pTarget
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		05/13/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void			ChangeTarget(CEntity* pTarget);

	/*****************************************************************
	* DealDamage(): Calls target's TakeDamage() function to decrease the target's health.
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		05/13/2015
	* Mod. Initials:	JM
	*****************************************************************/
	bool			DealDamage(void);
	bool			DealDamage(float fDamage);

	//============================== PATHFINDING =======================

	/*****************************************************************
	* LinetoLineCollision(): calculates if two lines collide
	*
	* Ins:	A,B vertices of line1
	C, D vertices of line 2
	*
	* Outs: XMFLOAT3 & - the intersection point
	*
	* Returns: bool
	*
	* Mod. Date:		06/12/2015
	* Mod. Initials:	JM
	*****************************************************************/
	bool LinetoLineCollision(const DirectX::XMFLOAT3 A,
		const DirectX::XMFLOAT3 B,
		const DirectX::XMFLOAT3 C,
		const DirectX::XMFLOAT3 D,
		DirectX::XMFLOAT3 &point);

	//from the orange collision book
	float Signed2DTriArea(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 c);

	/*****************************************************************
	* TurnToPoint(): Turns the AI agent towards the point
	*
	* Ins: float fDt : XMFLOAT3, float
	*
	* Outs: void
	*
	* Returns: bool
	*
	* Mod. Date:		06/12/2015
	* Mod. Initials:	JM
	*****************************************************************/
	bool TurnToPoint(float fDt,
		DirectX::XMFLOAT3 turnTo,
		float TurnSpeed);

	/*****************************************************************
	* SelectDirection(): Evaluate and select the direction to go
	*
	* Ins: float fDt : XMFLOAT3, float
	*
	* Outs: void
	*
	* Returns: DirectX::XMFLOAT3
	*
	* Mod. Date:		07/13/2015
	* Mod. Initials:	JM
	*****************************************************************/
	DirectX::XMFLOAT3 SelectDirection();

	/*****************************************************************
	* CallPathFinder(): Calls to the Pathfinder to calculate the path.
	*
	* Ins: DirectX::XMFLOAT3
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		05/21/2015
	* Mod. Initials:	JM
	*****************************************************************/
	//void			CallPathFinder(DirectX::XMFLOAT3 goalPosition);


	/*****************************************************************
	* MakeMovement(): Calls to the Pathfinder to calculate the path.
	*
	* Ins: float fDt : delta time
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		05/21/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void MakeMovement(float fDt);

	/*****************************************************************
	* Attack(): Handles any enemy-specific matters when attacking
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		06/10/2015
	* Mod. Initials:	MJG
	*****************************************************************/
	virtual void Attack(void);
	virtual void Attack(float fDamage) = 0;

	/*****************************************************************
	* DrainAttack(): Handles any enemy special draining attack that applies over time
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		06/17/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void DrainAttack(void);

	/*****************************************************************
	* MakeEvasionMovement(): the enemy faces to a target while he is moving to another location
	*
	* Ins:	float fDt,					- delta time
	*		DirectX::XMFLOAT3 goalposition,		- position to go
	*		DirectX::XMFLOAT3 turnToThis			- position to turn to
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:		06/17/2015
	* Mod. Initials:	JM
	*****************************************************************/
	//void MakeEvasionMovement(float fDt, DirectX::XMFLOAT3 goalposition, DirectX::XMFLOAT3 turnToThis);

	/***************
	*  Accessors
	***************/

	CEntity*		GetTarget()const		{ return m_pTarget; }
	float			GetDamage()const		{ return m_fDamage; }
	int				GetScrapAmount()const		{ return m_nScrapAmount; }
	float			GetAttackFrequency()const		{ return m_fAttackFrequency; }
	float			GetAttackRange()const		{ return m_fAttackRange; }
	float			GetAttackElapsed()const		{ return m_fAttackElapsedTime; }
	float			GetAggroRange()const		{ return m_fAggroRange; }
	//int				GetLevel()const		{ return m_nLevel; }
	eAIStateType	GetState()const		{ return m_eState; }
	CEntity*		GetPlayer()const		{ return m_pPlayer; }
	eEnemyVariant	GetVariant()const		{ return m_eVariant; }

	CRObject*		GetVariantIndicator()const { return m_pVariantIndicator; }

	DirectX::XMFLOAT4X4 GetModelWorld() override
	{
		return ModelTransform.GetWorld();
	}

	/***************
	* Mutators
	***************/

	void			SetDamage(float fDamage)				{ m_fDamage = fDamage; }
	void			SetScrapAmount(int nScrap)				{ m_nScrapAmount = nScrap; }
	void			SetAttackFrequency(float fFrequency)			{ m_fAttackFrequency = fFrequency; }
	void			SetAttackRange(float fRange)				{ m_fAttackRange = fRange; }
	void			SetAttackElapsed(float fElapsed)			{ m_fAttackElapsedTime = fElapsed; }
	void			SetAggroRange(float fAggro)				{ m_fAggroRange = fAggro; }
	//void			SetLevel(int nLevel)				{ m_nLevel = nLevel; }
	void			SetState(eAIStateType newState)		{ m_eState = newState; }
	void			SetPlayer(CEntity* pPlayer)			{ m_pPlayer = pPlayer; }
	void			SetVariant(eEnemyVariant variant)	{ m_eVariant = variant; }

protected:

	virtual void ApplyVariant(eEnemyVariant eTier);

	Transform					ModelTransform;

	CEntity*					m_pTarget;					//Current target
	float						m_fDamage;					//Dealed damage per hit
	int							m_nScrapAmount;				//Amount of droped scrap
	float						m_fAttackFrequency;			//Waited time between attacks
	float						m_fAttackRange;				//Max distance from target to attack
	float						m_fAttackElapsedTime;		//Elapsed time between attacks
	float						m_fAggroRange;				//Max distance to notice and target the player
	//int							m_nLevel;					//member to balance the health/damage
	eAIStateType				m_eState;					//Current state
	CEntity*					m_pPlayer;					//All enemies should know what player is to manage the attack and ranges
	float						m_fMovementSpeed;			//max velocity magnitude
	DirectX::XMFLOAT3			m_vDestination;

	eEnemyType					m_eMyType;
	int							m_nMyTriangle;
	int							m_nGoalTriangle;
	eEnemyVariant				m_eVariant;
	DirectX::XMFLOAT3			m_vVelocity;

	float						m_fTurningSpeed;
	float						m_fMass;

	float						m_fAttackSoundTimer;
	float						m_fCurrentASoundTime;
	float						m_fStepTimer;
	float						m_fIdleDelay;

	CRObject*					m_pVariantIndicator;
	DirectX::XMFLOAT3			m_xOffset;
};

#endif // _CBASEENEMY_H_

