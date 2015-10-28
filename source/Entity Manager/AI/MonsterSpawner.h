/***********************************************
* Filename:			MonsterSpawner.h
* Date:      		05/23/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Brandon Lucier
* Purpose:			define the entity who handles the
					enemies activation. 
************************************************/

#ifndef _MONSTERSPAWNER_H_
#define _MONSTERSPAWNER_H_

#include "..\..\Core\stdafx.h"
#include "..\..\Entity Manager\Entity.h"

class CEntityManager;

class CMonsterSpawner : public CEntity
{
public:
	/*****************************************************************
	* CMonsterSpawner(): default constructor.
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:			05/23/2015
	* Mod. Initials:		BL
	*****************************************************************/
	CMonsterSpawner(float fSpawnDelay, float fBasePoints, int nPointModifier, bool bisConstant);
	
	/*****************************************************************
	* ~CMonsterSpawner(): default destructor.
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:			05/23/2015
	* Mod. Initials:		BL
	*****************************************************************/
	~CMonsterSpawner();

	/*****************************************************************
	* Update(): Updates the spawner timer every frame and if this is 
	*			greater than the cooldown time it activates this spawner.
	*
	* Ins:		float
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:			05/23/2015
	* Mod. Initials:		BL
	*****************************************************************/
	void Update(float dt);

	/*****************************************************************
	* SpawnMonster(): Creates an amount of monsters using current points and then 
	*				desactivates itself
	*
	* Ins:	void
	*
	* Outs:	void
	*
	* Returns:
	*
	* Mod. Date:			05/23/2015
	* Mod. Initials:		BL
	*****************************************************************/
	void SpawnMonster();

	/*****************************************************************
	* ActivateMe(): activates the spawner reseting the values such as
	*				current points, enemy type chances and base points
	*
	* Ins:	void
	*
	* Outs:	void
	*
	* Returns:
	*
	* Mod. Date:			05/23/2015
	* Mod. Initials:		BL
	*****************************************************************/
	void ActivateMe();

	/*****************************************************************
	* DeactivateMe(): desactivates this spawner not allowing spawn more monsters.
	*
	* Ins:	void
	*
	* Outs:	void
	*
	* Returns:	void
	*
	* Mod. Date:			05/23/2015
	* Mod. Initials:		BL
	*****************************************************************/
	void DeactivateMe() { m_bIsActive = false; }

	/*****************************************************************
	* Reset(): Sets the spawner to its defaults, used when entering gamestate
	*
	* Ins:	void
	*
	* Outs:	void
	*
	* Returns:	void
	*
	* Mod. Date:			6/1/2015
	* Mod. Initials:		MJG
	*****************************************************************/
	void Reset();

	/*********************
	* Mutators
	*********************/
	void SetMaxBigChance(float fChance) { m_fMaxBigEnemyChance = fChance; }
	void SetMaxMediumChance(float fChance) { m_fMaxMediumEnemyChance = fChance; }
	void SetEntityManager(CEntityManager* _pEntityManager) { m_pEntityManager = _pEntityManager; }
	void SetSpawnStart(bool start){ m_bSpawnerStarted = start; }

private:
	float						m_fSpawnRadius;
	float						m_fBasePoints;
	float						m_fCurrentPoints;
	float						m_fCooldown;
	float						m_fWaitedTime; // to cooldown
	float						m_fSmallEnemyCost;
	float						m_fMediumEnemyCost;
	float						m_fBigEnemyCost;
	float						m_fMediumChance;
	float						m_fMaxMediumEnemyChance;
	float						m_fBigChance;
	float						m_fMaxBigEnemyChance;

	float						m_fChanceModifierBigEnemy;
	float						m_fChanceModifierMediumEnemy;

	bool						m_bIsActive = true;
	bool						m_bControllerCreated = false;
	bool						m_bFirstTime = true;

	int							m_nMaxPackNumber;
	int							m_nCurrentPack;
	int							m_nBasePointIncrement;

	bool						m_bForcedDeactivation = false;

	bool						m_bIsConstant = false;

	//Tells the entity manager to spawn a new guy
	CEntityManager*				m_pEntityManager = nullptr;

	bool						m_bSpawnerStarted;
};

#endif