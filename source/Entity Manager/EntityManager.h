/***********************************************
* Filename:			EntityManager.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Brandon Lucier
* Purpose:			Manages all the entities, calls
*					the updates for each and checks
*					collision, passes entities to
*					renderer for rendering.
************************************************/

#ifndef _CENTITYMANAGER_H_
#define _CENTITYMANAGER_H_

#define NUM_BUCKETS 128

#include "..\Core\stdafx.h"

#include <list>
#include "..\Entity Manager\Entity.h"
#include "..\Core\EnumerationHolder.h"
#include "..\Core\StructureHolder.h"

#include "AI\SaboteurEnemy.h"
#include "AI\HunterEnemy.h"
#include "AI\CreepEnemy.h"

#include "Entities\Player.h"
#include "..\Weapon\Bullet.h"
#include "Scenery\Scenery.h"
#include "Scenery\FloorGuide.h"

#include "../Renderer/Scene.h"

#include "Entities\Subsystems\Subsystem.h"
#include "Entities\PickUp.h"

class CWaveSpawner;
class CAIDirector;

class CEntityManager
{
public:

	struct TCell
	{
		int x;
		//int y;
		int z;
	};
	enum eSubsystem { eREACTOR, eLIFESUPPORT, eENGINES };
	CEntityManager();
	~CEntityManager();

	void Update(float fDt, CScene*& pScene);

	// This function will clear all the lists in the manager
	void Clear();

	/*****************************************************************
	* ComputeHashBucketIndex(): Finds out in what bucket the cell is and
	*							returns the index
	* Ins:						void
	* Outs:						void
	* Returns:					(int) index into for the buckets
	* Mod. Date:				05/18/2015
	* Mod. Initials:			BL
	*****************************************************************/
	unsigned int ComputeHashBucketIndex(TCell tCellPos);

	/****************************************************************
	* CheckCollisions():	After updating the entities we need to see
	*						if some of them are colliding
	* Ins:					void
	* outs:					void
	* Returns:				void
	* Mod. Date:			05/19/2015
	* Mod. Initials:		BL
	****************************************************************/
	void CheckCollisions(float fDt);

	/****************************************************************
	* EntityToEntity():		Looks to see what collider types the entities have
	*						then calls the apporiate collision detection function
	* Ins:					2 entities
	* outs:					void
	* Returns:				bool - true if colliding / false if not
	* Mod. Date:			05/19/2015
	* Mod. Initials:		BL
	****************************************************************/
	bool EntityToEntity(CEntity* cEnt_1, CEntity* cEnt_2);

	/****************************************************************
	* CollisionWithMap():	After updating we need to check the entities
	*						against the map as well.
	* Ins:					void
	* outs:					void
	* Mod. Date:			05/22/2015
	* Mod. Initials:		BL
	****************************************************************/
	void CollisionWithMap();

	/****************************************************************
	* CycleIntoLiveList():	Takes an amount of entities from their dead list
	*						and moves them into the live lists
	* Ins:					EntityType - The enum type from CEntity that we are
	*						nAmount - the number of entities to move over
	* outs:					void
	* Returns:				bool
	* Mod. Date:			06/03/2015
	* Mod. Initials:		MJG
	****************************************************************/
	bool CycleIntoLiveList(int EntityType, DirectX::XMFLOAT3 fPosition = DirectX::XMFLOAT3(0, 0, 0), CIWeapon* BulletOwner = nullptr, eEnemyVariant variant = eEnemyVariant::eNoVariant);

	/****************************************************************
	* CreateScrapPickUp():	Creates a scrap pick-up at the position
	* outs:					void
	* Returns:				void
	* Mod. Date:			06/03/2015
	* Mod. Initials:		MJG
	****************************************************************/
	void CreateScrapPickUp(DirectX::XMFLOAT3 fPosition = DirectX::XMFLOAT3(0, 0, 0), int nScrapAmount = 10);

	/****************************************************************
	* KillAllEntities():	Moves all entities from every live list to
	*						their dead lists.
	* Ins:					
	* outs:					void
	* Returns:				void
	* Mod. Date:			05/19/2015
	* Mod. Initials:		BL
	****************************************************************/
	void KillAllEntities();

	/****************************************************************
	* KillAllEnemies():	Moves all enemies from every live list to
	*						their dead lists.
	* Ins:
	* outs:					void
	* Returns:				void
	* Mod. Date:			08/21/2015
	* Mod. Initials:		JM
	****************************************************************/
	void KillAllEnemies();

	void CheckExplosionCollision(TSphere* ExplosionSphere, float DamageAmount);

	/************************************************************************
	* All of the AddToManager and RemoveFromManager overloads will add/remove
	* in the passed in thing to the entity manager to ensure that the object
	* gets updated. Overloads are: Player, Bullet, Environment, SubSystem,
	* Creep, Hunter, Saboteur, PickUp.
	*************************************************************************/
	void AddToManager(CPlayer* PlayerPointer) { PlayerPointer->SetEntityType(CEntity::ePLAYER);  m_pPlayer = PlayerPointer; }
	void AddToManager(CBullet* pBullet, bool isAlive) { pBullet->SetEntityType(CEntity::eBULLET); 
														isAlive ? m_lpBulletList.push_back(pBullet) : m_lpDeadBullets.push_back(pBullet); }
	void AddToManager(CEntity* pEnviromentObj) { m_lpEnvrionmentList.push_back(pEnviromentObj); }
	void AddToManager(TAABB* pCollisionBox) { m_lpCollisionBoxList.push_back(pCollisionBox); }
	// 0 == Reactor, 1 == LifeSupport, 2 == Engines
	void AddToManager(CSubSystem* pSubSystem, int WhichOne);

	void AddToManager(CCreepEnemy* pCreep, bool isAlive) { pCreep->SetEntityType(CEntity::eCREEP); 
															isAlive ? m_lpCreepList.push_back(pCreep) : m_lpDeadCreepList.push_back(pCreep); }
	void AddToManager(CHunterEnemy* pHunter, bool isAlive) { pHunter->SetEntityType(CEntity::eHUNTER); 
															isAlive ? m_lpHunterList.push_back(pHunter) : m_lpDeadHunters.push_back(pHunter); }
	void AddToManager(CSaboteurEnemy* pSaboteur, bool isAlive) { pSaboteur->SetEntityType(CEntity::eSABOTEUR);
																isAlive ? m_lpSaboteurList.push_back(pSaboteur) : m_lpDeadSaboteurs.push_back(pSaboteur); }

	//void AddToManager(CSpammerEnemy* pSpammer, bool isAlive) { pSpammer->SetEntityType(CEntity::eSPAMMER);
	//															isAlive ? m_lpSpammerList.push_back(pSpammer) : m_lpDeadSpammers.push_back(pSpammer); }

	void AddToManager(CPickUp* pPickUp, bool isAlive) { pPickUp->SetEntityType(CEntity::ePICKUP); 
														isAlive ? m_lpPickUpList.push_back(pPickUp) : m_lpDeadPickUps.push_back(pPickUp); }
	void AddToManager(CScenery* pScenery) { m_lpSceneryList.push_back(pScenery); }

	//void AddToManager(CWaveSpawner* pWSpawner) { m_lWaveSpawners.push_back(pWSpawner); }

	/********************
	* Accessors
	********************/
	CPlayer* GetPlayer() { return m_pPlayer; }
	std::list<CSubSystem*> GetSubSystems() { return m_lpSubSystemList; }
	std::list<CEntity*> GetAllEnviromentObjects() { return m_lpEnvrionmentList; }
	std::list<TAABB*> GetAllCollisionBoxes() { return m_lpCollisionBoxList; }
	std::list<CCreepEnemy*> GetAllCreeps() { return m_lpCreepList; }
//	std::list<CSpammerEnemy*> GetAllSpammers() { return m_lpSpammerList; }
	std::list<CHunterEnemy*> GetAllHunters() { return m_lpHunterList; }
	std::list<CSaboteurEnemy*> GetAllSaboteur() { return m_lpSaboteurList; }
	CAIDirector* GetDirector()const{ return m_pAIDirector; }
	FloorGuides::CFloorGuide* GetFloorGuide() const { return m_pFloorGuide; }
	//std::list<CWaveSpawner*> GetAllWaveSpawners() { return m_lWaveSpawners; }

private:
	/****************************************************************
	* HandleInvisiWall():	Sets transparency of one object based on
	*						its collision with CamToPlayer
	* Ins:					pCamToPlayer, pWall
	* outs:					void
	* Returns:				bool
	* Mod. Date:			06/11/2015
	* Mod. Initials:		MJG
	****************************************************************/
	static bool HandleInvisiWall(float fDt, TCapsule* pCamToPlayer, CRObject* pWall);

	/****************************************************************
	* HandleInvisiWall():	Sets transparency of all environmental
	*						entities based on collision with CamToPlayer
	* Ins:					pCamToPlayer
	* outs:					void
	* Returns:				void
	* Mod. Date:			06/11/2015
	* Mod. Initials:		MJG
	****************************************************************/
	//void HandleInvisiWalls(float fDt, TCapsule* pCamToPlayer);

	// Here is where all of our Live entities need to be
	CPlayer* m_pPlayer;
	std::list<CBullet*> m_lpBulletList;
	std::list<CEntity*> m_lpEnvrionmentList;
	std::list<TAABB*> m_lpCollisionBoxList;
	std::list<CSubSystem*> m_lpSubSystemList;
	std::list<CCreepEnemy*> m_lpCreepList;
	std::list<CHunterEnemy*> m_lpHunterList;
	std::list<CSaboteurEnemy*> m_lpSaboteurList;
//	std::list<CSpammerEnemy*> m_lpSpammerList;
	std::list<CPickUp*> m_lpPickUpList;
	std::list<CScenery*> m_lpSceneryList;

	// Here is where all of our Dead entities need to be
	std::list<CHunterEnemy*> m_lpDeadHunters;
	std::list<CSaboteurEnemy*> m_lpDeadSaboteurs;
//	std::list<CSpammerEnemy*> m_lpDeadSpammers;
	std::list<CCreepEnemy*> m_lpDeadCreepList;
	std::list<CBullet*> m_lpDeadBullets;
	std::list<CPickUp*> m_lpDeadPickUps;

	//std::list<CWaveSpawner*> m_lWaveSpawners;

	std::list<CEntity*> m_lpaEntityBuckets[NUM_BUCKETS];
	unsigned int m_unCellSize = 1000;
	int		m_nControl;
	float m_fDistance;


	CAIDirector* m_pAIDirector;

	FloorGuides::CFloorGuide* m_pFloorGuide = nullptr;
};

#endif // _CENTITYMANAGER_H_

