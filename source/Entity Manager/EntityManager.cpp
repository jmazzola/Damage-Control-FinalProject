/***********************************************
* Filename:			EntityManager.cpp
* Date:      		05/12/2015
* Mod. Date:		07/09/2015
* Mod. Initials:	JM
* Author:    		Brandon Lucier
* Purpose:			Manages all of the entities
************************************************/

#include "EntityManager.h"
#include "..\Math\Collision.h"

#include "..\Weapon\Bullet.h"
#include "..\Weapon\IWeapon.h"

#include "Entities\Subsystems\Subsystem.h"
#include "Entities\Subsystems\Reactor.h"
#include "Entities\Subsystems\LifeSupport.h"
#include "Entities\Subsystems\Engines.h"
#include "AI\MonsterSpawner.h"
#include "AI/BaseEnemy.h"

#include "../Audio Manager/AudioSystemWwise.h"

#include "../Particle Manager/ParticleSystem.h"
#include "../Particle Manager/ParticleEmitter.h"
#include "../Entity Manager/Entities/Subsystems/Subsystem.h"
//#include "../Entity Manager/AI/WaveSpawner.h"
#include "AI\AIDirector.h"

using namespace FloorGuides;
using namespace MathHelper;

#define INVISIWALL_CAPSULE_RADIUS	152.0f
#define INVISIWALL_MIN_ALPHA		0.25f
#define INVISIWALL_LERP_RATE		1.75f

#define CREEPSCORE 8
#define HUNTERSCORE 25
#define SABOTEURSCORE 20
#define SPAMMERSCORE 15

/*****************************************************************
* CEntityManager():
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
CEntityManager::CEntityManager( )
{
	/*matrix4f identity;
	identity = identity.make_identity();
	TFrustum temp;
	CCollision::BuildFrustum(50, 1, 100, 1, identity, temp);*/
	m_pPlayer = new CPlayer( );
	m_nControl = 0;
	m_fDistance = 100; //600

	m_pAIDirector = new CAIDirector( );
	m_pAIDirector->SetPlayer(m_pPlayer);
	m_pAIDirector->SetEntityManager(this);

	m_pAIDirector->SetTag("Shia_Labeouf_Director");

	m_pFloorGuide = new CFloorGuide();
	//m_pAIDirector->Reset();
}

/*****************************************************************
* ~CEntityManager():
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
CEntityManager::~CEntityManager( )
{
	SafeDelete(m_pPlayer);
	SafeDelete(m_pFloorGuide);

	// bullets
	auto& BulletIter = m_lpBulletList.begin( );
	for (BulletIter; BulletIter != m_lpBulletList.end( );)
	{
		delete (*BulletIter);
		(*BulletIter) = nullptr;
		BulletIter = m_lpBulletList.erase(BulletIter);
	}
	m_lpBulletList.clear( );

	// dead bullets
	auto& DeadBulletIter = m_lpDeadBullets.begin( );
	for (DeadBulletIter; DeadBulletIter != m_lpDeadBullets.end( );)
	{
		delete (*DeadBulletIter);
		(*DeadBulletIter) = nullptr;
		DeadBulletIter = m_lpDeadBullets.erase(DeadBulletIter);
	}
	m_lpDeadBullets.clear( );

	// collision boxes
	auto& BoxIter = m_lpCollisionBoxList.begin( );
	for (BoxIter; BoxIter != m_lpCollisionBoxList.end( );)
	{
		delete (*BoxIter);
		(*BoxIter) = nullptr;
		BoxIter = m_lpCollisionBoxList.erase(BoxIter);
	}
	m_lpCollisionBoxList.clear( );

	// Subsystems
	auto& SystemIter = m_lpSubSystemList.begin( );
	for (SystemIter; SystemIter != m_lpSubSystemList.end( );)
	{
		delete (*SystemIter);
		(*SystemIter) = nullptr;
		SystemIter = m_lpSubSystemList.erase(SystemIter);
	}
	m_lpSubSystemList.clear( );

	auto& ScenerIter = m_lpSceneryList.begin( );
	for (ScenerIter; ScenerIter != m_lpSceneryList.end( );)
	{
		delete (*ScenerIter);
		(*ScenerIter) = nullptr;
		ScenerIter = m_lpSceneryList.erase(ScenerIter);
	}
	m_lpSceneryList.clear( );

	// enviroment objects
	auto& EnviroIter = m_lpEnvrionmentList.begin( );
	for (EnviroIter; EnviroIter != m_lpEnvrionmentList.end( );)
	{
		delete (*EnviroIter);
		(*EnviroIter) = nullptr;
		EnviroIter = m_lpEnvrionmentList.erase(EnviroIter);
	}
	m_lpEnvrionmentList.clear( );

	// Creeps
	auto& CreepIter = m_lpCreepList.begin( );
	for (CreepIter; CreepIter != m_lpCreepList.end( );)
	{
		delete (*CreepIter);
		(*CreepIter) = nullptr;
		CreepIter = m_lpCreepList.erase(CreepIter);
	}
	m_lpCreepList.clear( );

	// Dead Creeps
	auto& DeadCreepIter = m_lpDeadCreepList.begin( );
	for (DeadCreepIter; DeadCreepIter != m_lpDeadCreepList.end( );)
	{
		delete (*DeadCreepIter);
		(*DeadCreepIter) = nullptr;
		DeadCreepIter = m_lpDeadCreepList.erase(DeadCreepIter);
	}
	m_lpDeadCreepList.clear( );

	// Hunters
	auto& HunterIter = m_lpHunterList.begin( );
	for (HunterIter; HunterIter != m_lpHunterList.end( );)
	{
		delete (*HunterIter);
		(*HunterIter) = nullptr;
		HunterIter = m_lpHunterList.erase(HunterIter);
	}
	m_lpHunterList.clear( );

	// dead hunters
	auto& DeadHunterIter = m_lpDeadHunters.begin( );
	for (DeadHunterIter; DeadHunterIter != m_lpDeadHunters.end( );)
	{
		delete (*DeadHunterIter);
		(*DeadHunterIter) = nullptr;
		DeadHunterIter = m_lpDeadHunters.erase(DeadHunterIter);
	}
	m_lpDeadHunters.clear( );

	// saboteurs
	auto& SaboteurIter = m_lpSaboteurList.begin( );
	for (SaboteurIter; SaboteurIter != m_lpSaboteurList.end( );)
	{
		delete (*SaboteurIter);
		(*SaboteurIter) = nullptr;
		SaboteurIter = m_lpSaboteurList.erase(SaboteurIter);
	}
	m_lpSaboteurList.clear( );

	// dead sabotuers
	auto& DeadSaboteurIter = m_lpDeadSaboteurs.begin( );
	for (DeadSaboteurIter; DeadSaboteurIter != m_lpDeadSaboteurs.end( );)
	{
		delete (*DeadSaboteurIter);
		(*DeadSaboteurIter) = nullptr;
		DeadSaboteurIter = m_lpDeadSaboteurs.erase(DeadSaboteurIter);
	}
	m_lpDeadSaboteurs.clear( );

	// spammers
	//auto& SpammerIter = m_lpSpammerList.begin();
	//for (SpammerIter; SpammerIter != m_lpSpammerList.end();)
	//{
	//	delete (*SpammerIter);
	//	(*SpammerIter) = nullptr;
	//	SpammerIter = m_lpSpammerList.erase(SpammerIter);
	//}
	//m_lpSpammerList.clear();

	//// dead spammers
	//auto& DeadSpammerIter = m_lpDeadSpammers.begin();
	//for (DeadSpammerIter; DeadSpammerIter != m_lpDeadSpammers.end();)
	//{
	//	delete (*DeadSpammerIter);
	//	(*DeadSpammerIter) = nullptr;
	//	DeadSpammerIter = m_lpDeadSpammers.erase(DeadSpammerIter);
	//}
	//m_lpDeadSpammers.clear();

	// pick-ups
	auto& PickUpIter = m_lpPickUpList.begin( );
	for (PickUpIter; PickUpIter != m_lpPickUpList.end( );)
	{
		delete (*PickUpIter);
		(*PickUpIter) = nullptr;
		PickUpIter = m_lpPickUpList.erase(PickUpIter);
	}
	m_lpPickUpList.clear( );

	// dead pick-ups
	auto& DeadPickUpIter = m_lpDeadPickUps.begin( );
	for (DeadPickUpIter; DeadPickUpIter != m_lpDeadPickUps.end( );)
	{
		delete (*DeadPickUpIter);
		(*DeadPickUpIter) = nullptr;
		DeadPickUpIter = m_lpDeadPickUps.erase(DeadPickUpIter);
	}
	m_lpDeadPickUps.clear( );

	for (size_t i = 0; i < NUM_BUCKETS; i++)
	{
		m_lpaEntityBuckets[i].clear( );
	}

	delete m_pAIDirector;
}

void CEntityManager::Clear( )
{
	delete m_pPlayer;

	// bullets
	auto& BulletIter = m_lpBulletList.begin( );
	for (BulletIter; BulletIter != m_lpBulletList.end( );)
	{
		/*(*BulletIter)->SetBucketNumber(-1, 0);
		(*BulletIter)->SetBucketNumber(-1, 1);
		(*BulletIter)->SetBucketNumber(-1, 2);
		(*BulletIter)->SetBucketNumber(-1, 3);*/
		delete (*BulletIter);
		(*BulletIter) = nullptr;
		BulletIter = m_lpBulletList.erase(BulletIter);
	}
	m_lpBulletList.clear( );

	// dead bullets
	auto& DeadBulletIter = m_lpDeadBullets.begin( );
	for (DeadBulletIter; DeadBulletIter != m_lpDeadBullets.end( );)
	{
		/*(*DeadBulletIter)->SetBucketNumber(-1, 0);
		(*DeadBulletIter)->SetBucketNumber(-1, 1);
		(*DeadBulletIter)->SetBucketNumber(-1, 2);
		(*DeadBulletIter)->SetBucketNumber(-1, 3);*/
		delete (*DeadBulletIter);
		(*DeadBulletIter) = nullptr;
		DeadBulletIter = m_lpDeadBullets.erase(DeadBulletIter);
	}
	m_lpDeadBullets.clear( );

	// collision boxes
	auto& BoxIter = m_lpCollisionBoxList.begin( );
	for (BoxIter; BoxIter != m_lpCollisionBoxList.end( );)
	{
		delete (*BoxIter);
		(*BoxIter) = nullptr;
		BoxIter = m_lpCollisionBoxList.erase(BoxIter);
	}
	m_lpCollisionBoxList.clear( );

	// Subsystems
	auto& SystemIter = m_lpSubSystemList.begin( );
	for (SystemIter; SystemIter != m_lpSubSystemList.end( );)
	{
		/*(*SystemIter)->SetBucketNumber(-1, 0);
		(*SystemIter)->SetBucketNumber(-1, 1);
		(*SystemIter)->SetBucketNumber(-1, 2);
		(*SystemIter)->SetBucketNumber(-1, 3);*/
		delete (*SystemIter);
		(*SystemIter) = nullptr;
		SystemIter = m_lpSubSystemList.erase(SystemIter);
	}
	m_lpSubSystemList.clear( );

	// enviroment objects
	auto& EnviroIter = m_lpEnvrionmentList.begin( );
	for (EnviroIter; EnviroIter != m_lpEnvrionmentList.end( );)
	{
		/*(*EnviroIter)->SetBucketNumber(-1, 0);
		(*EnviroIter)->SetBucketNumber(-1, 1);
		(*EnviroIter)->SetBucketNumber(-1, 2);
		(*EnviroIter)->SetBucketNumber(-1, 3);*/
		delete (*EnviroIter);
		(*EnviroIter) = nullptr;
		EnviroIter = m_lpEnvrionmentList.erase(EnviroIter);
	}
	m_lpEnvrionmentList.clear( );

	// Creeps
	auto& CreepIter = m_lpCreepList.begin( );
	for (CreepIter; CreepIter != m_lpCreepList.end( );)
	{
		/*(*CreepIter)->SetBucketNumber(-1, 0);
		(*CreepIter)->SetBucketNumber(-1, 1);
		(*CreepIter)->SetBucketNumber(-1, 2);
		(*CreepIter)->SetBucketNumber(-1, 3);*/
		delete (*CreepIter);
		(*CreepIter) = nullptr;
		CreepIter = m_lpCreepList.erase(CreepIter);
	}
	m_lpCreepList.clear( );

	// Dead Creeps
	auto& DeadCreepIter = m_lpDeadCreepList.begin( );
	for (DeadCreepIter; DeadCreepIter != m_lpDeadCreepList.end( );)
	{
		/*(*DeadCreepIter)->SetBucketNumber(-1, 0);
		(*DeadCreepIter)->SetBucketNumber(-1, 1);
		(*DeadCreepIter)->SetBucketNumber(-1, 2);
		(*DeadCreepIter)->SetBucketNumber(-1, 3);*/
		delete (*DeadCreepIter);
		(*DeadCreepIter) = nullptr;
		DeadCreepIter = m_lpDeadCreepList.erase(DeadCreepIter);
	}
	m_lpDeadCreepList.clear( );

	// Hunters
	auto& HunterIter = m_lpHunterList.begin( );
	for (HunterIter; HunterIter != m_lpHunterList.end( );)
	{
		/*(*HunterIter)->SetBucketNumber(-1, 0);
		(*HunterIter)->SetBucketNumber(-1, 1);
		(*HunterIter)->SetBucketNumber(-1, 2);
		(*HunterIter)->SetBucketNumber(-1, 3);*/
		delete (*HunterIter);
		(*HunterIter) = nullptr;
		HunterIter = m_lpHunterList.erase(HunterIter);
	}
	m_lpHunterList.clear( );

	// dead hunters
	auto& DeadHunterIter = m_lpDeadHunters.begin( );
	for (DeadHunterIter; DeadHunterIter != m_lpDeadHunters.end( );)
	{
		/*(*DeadHunterIter)->SetBucketNumber(-1, 0);
		(*DeadHunterIter)->SetBucketNumber(-1, 1);
		(*DeadHunterIter)->SetBucketNumber(-1, 2);
		(*DeadHunterIter)->SetBucketNumber(-1, 3);*/
		delete (*DeadHunterIter);
		(*DeadHunterIter) = nullptr;
		DeadHunterIter = m_lpDeadHunters.erase(DeadHunterIter);
	}
	m_lpDeadHunters.clear( );

	// saboteurs
	auto& SaboteurIter = m_lpSaboteurList.begin( );
	for (SaboteurIter; SaboteurIter != m_lpSaboteurList.end( );)
	{
		/*(*SaboteurIter)->SetBucketNumber(-1, 0);
		(*SaboteurIter)->SetBucketNumber(-1, 1);
		(*SaboteurIter)->SetBucketNumber(-1, 2);
		(*SaboteurIter)->SetBucketNumber(-1, 3);*/
		delete (*SaboteurIter);
		(*SaboteurIter) = nullptr;
		SaboteurIter = m_lpSaboteurList.erase(SaboteurIter);
	}
	m_lpSaboteurList.clear( );

	// dead sabotuers
	auto& DeadSaboteurIter = m_lpDeadSaboteurs.begin( );
	for (DeadSaboteurIter; DeadSaboteurIter != m_lpDeadSaboteurs.end( );)
	{
		/*(*DeadSaboteurIter)->SetBucketNumber(-1, 0);
		(*DeadSaboteurIter)->SetBucketNumber(-1, 1);
		(*DeadSaboteurIter)->SetBucketNumber(-1, 2);
		(*DeadSaboteurIter)->SetBucketNumber(-1, 3);*/
		delete (*DeadSaboteurIter);
		(*DeadSaboteurIter) = nullptr;
		DeadSaboteurIter = m_lpDeadSaboteurs.erase(DeadSaboteurIter);
	}
	m_lpDeadSaboteurs.clear( );

	// spammers
	//auto& SpammerIter = m_lpSpammerList.begin();
	//for (SpammerIter; SpammerIter != m_lpSpammerList.end();)
	//{
	//	/*(*SaboteurIter)->SetBucketNumber(-1, 0);
	//	(*SaboteurIter)->SetBucketNumber(-1, 1);
	//	(*SaboteurIter)->SetBucketNumber(-1, 2);
	//	(*SaboteurIter)->SetBucketNumber(-1, 3);*/
	//	delete (*SpammerIter);
	//	(*SpammerIter) = nullptr;
	//	SpammerIter = m_lpSpammerList.erase(SpammerIter);
	//}
	//m_lpSpammerList.clear();

	//// dead spammers
	//auto& DeadSpammerIter = m_lpDeadSpammers.begin();
	//for (DeadSpammerIter; DeadSpammerIter != m_lpDeadSpammers.end();)
	//{
	//	/*(*DeadSaboteurIter)->SetBucketNumber(-1, 0);
	//	(*DeadSaboteurIter)->SetBucketNumber(-1, 1);
	//	(*DeadSaboteurIter)->SetBucketNumber(-1, 2);
	//	(*DeadSaboteurIter)->SetBucketNumber(-1, 3);*/
	//	delete (*DeadSpammerIter);
	//	(*DeadSpammerIter) = nullptr;
	//	DeadSpammerIter = m_lpDeadSpammers.erase(DeadSpammerIter);
	//}
	//m_lpDeadSpammers.clear();

	for (size_t i = 0; i < NUM_BUCKETS; i++)
	{
		m_lpaEntityBuckets[i].clear( );
	}
}

void CEntityManager::AddToManager(CSubSystem* pSubSystem, int WhichOne)
{
	if (WhichOne == eREACTOR) // Reactor
	{
		pSubSystem->SetEntityType(CEntity::eSUBSYSTEM);
		pSubSystem->SetSystemType(eSubsystemtype::eSystemReactor);

		CReactor* reac = static_cast<CReactor*>(pSubSystem);
		m_lpSubSystemList.push_back(reac);

		m_pAIDirector->SetReactor(reac);
		reac->SetDirector(m_pAIDirector);
	}
	else if (WhichOne == eLIFESUPPORT) // LifeSupport
	{
		pSubSystem->SetEntityType(CEntity::eSUBSYSTEM);
		pSubSystem->SetSystemType(eSubsystemtype::eSystemLife);

		CLifeSupport* lifsu = static_cast<CLifeSupport*>(pSubSystem);
		m_lpSubSystemList.push_back(lifsu);

		m_pAIDirector->SetLifeSupport(lifsu);
		lifsu->SetDirector(m_pAIDirector);
	}
	else if (WhichOne == eENGINES) // Engines
	{
		pSubSystem->SetEntityType(CEntity::eSUBSYSTEM);
		pSubSystem->SetSystemType(eSubsystemtype::eSystemEngine);

		CEngines* eng = static_cast<CEngines*>(pSubSystem);
		m_lpSubSystemList.push_back(eng);

		m_pAIDirector->SetEngines(eng);
		eng->SetDirector(m_pAIDirector);

	}

	DirectX::XMFLOAT3 min = pSubSystem->GetCollider( )->vMin;
	DirectX::XMFLOAT3 max = pSubSystem->GetCollider( )->vMax;

	TCell cellPos1, cellPos2, cellPos3, cellPos4;
	cellPos1.x = (int)min.x / m_unCellSize;
	cellPos1.z = (int)min.z / m_unCellSize;

	cellPos2.x = (int)min.x / m_unCellSize;
	cellPos2.z = (int)(min.z + (max.z - min.z)) / m_unCellSize;

	cellPos3.x = (int)max.x / m_unCellSize;
	cellPos3.z = (int)max.z / m_unCellSize;

	cellPos4.x = (int)(min.x + (max.x - min.x)) / m_unCellSize;
	cellPos4.z = (int)min.z / m_unCellSize;

	unsigned int bucket[4];
	bucket[0] = ComputeHashBucketIndex(cellPos1);
	bucket[1] = ComputeHashBucketIndex(cellPos2);
	bucket[2] = ComputeHashBucketIndex(cellPos3);
	bucket[3] = ComputeHashBucketIndex(cellPos4);
	for (size_t i = 0; i < 4; i++)
	{
		if (bucket[i] != pSubSystem->GetBucketNumber( )[i])
		{
			auto& Ent = m_lpaEntityBuckets[pSubSystem->GetBucketNumber( )[i]].begin( );
			for (Ent; Ent != m_lpaEntityBuckets[pSubSystem->GetBucketNumber( )[i]].end( ); Ent++)
			{
				if ((*Ent) == pSubSystem)
				{
					m_lpaEntityBuckets[pSubSystem->GetBucketNumber( )[i]].erase(Ent);
					break;
				}
			}
			pSubSystem->SetBucketNumber(bucket[i], i);
			m_lpaEntityBuckets[bucket[i]].push_back(pSubSystem);
		}
	}

	if (m_pFloorGuide)
	{
		m_pFloorGuide->AddSubSystem(pSubSystem, (unsigned char)WhichOne);
	}
}

#include "../Weapon/DroneBuddy.h"
void CEntityManager::Update(float fDt, CScene*& pScene)
{
	// Collision fun
	CollisionWithMap( );

	//director
	m_pAIDirector->Update(fDt);

#pragma region Player Update
	if (m_pPlayer != nullptr)
	{
		m_pPlayer->Update(fDt);
		pScene->AddObject(static_cast<CRObject*>(m_pPlayer));
		if (m_pPlayer->GetCurrentWeapon( ) && m_pPlayer->GetAlive())
		{
			CIWeapon* pWeapon = m_pPlayer->GetCurrentWeapon( );
			pScene->AddObject(pWeapon->GetFlash( ));
			pScene->AddObject(pWeapon->GetCrosshair( ));
			//pScene->AddObject(pWeapon);
			if (pWeapon->HasLight( ))
			{
				pScene->AddLight((*pWeapon->GetLight( )));
			}
		}
		if (m_pPlayer->GetDroneBuddy( ))
		{
			if (m_pPlayer->GetDroneBuddy( )->GetIsActive( ))
			{
				pScene->AddObject(m_pPlayer->GetDrone( ));
			}
			if (m_pPlayer->GetDrone( )->HasLight( ))
			{
				pScene->AddLight((*m_pPlayer->GetDrone( )->GetLight( )));
			}
		}
		if (m_pPlayer->HasLight( ))
		{
			pScene->AddLight((*m_pPlayer->GetLight( )));
		}

		// below computes what cell the object is in
		TCell cellPos1, cellPos2, cellPos3, cellPos4;
		cellPos1.x = (int)(m_pPlayer->GetWorldMatrix( ).m[3][0] - m_pPlayer->GetCollider( )->fRadius) / m_unCellSize;
		cellPos1.z = (int)(m_pPlayer->GetWorldMatrix( ).m[3][2] + m_pPlayer->GetCollider( )->fRadius) / m_unCellSize;

		cellPos2.x = (int)(m_pPlayer->GetWorldMatrix( ).m[3][0] + m_pPlayer->GetCollider( )->fRadius) / m_unCellSize;
		cellPos2.z = (int)(m_pPlayer->GetWorldMatrix( ).m[3][2] + m_pPlayer->GetCollider( )->fRadius) / m_unCellSize;

		cellPos3.x = (int)(m_pPlayer->GetWorldMatrix( ).m[3][0] + m_pPlayer->GetCollider( )->fRadius) / m_unCellSize;
		cellPos3.z = (int)(m_pPlayer->GetWorldMatrix( ).m[3][2] - m_pPlayer->GetCollider( )->fRadius) / m_unCellSize;

		cellPos4.x = (int)(m_pPlayer->GetWorldMatrix( ).m[3][0] - m_pPlayer->GetCollider( )->fRadius) / m_unCellSize;
		cellPos4.z = (int)(m_pPlayer->GetWorldMatrix( ).m[3][2] - m_pPlayer->GetCollider( )->fRadius) / m_unCellSize;

		unsigned int bucket[4];
		bucket[0] = ComputeHashBucketIndex(cellPos1);
		bucket[1] = ComputeHashBucketIndex(cellPos2);
		bucket[2] = ComputeHashBucketIndex(cellPos3);
		bucket[3] = ComputeHashBucketIndex(cellPos4);
		for (size_t i = 0; i < 4; i++)
		{
			if (bucket[i] != m_pPlayer->GetBucketNumber( )[i])
			{
				auto& Ent = m_lpaEntityBuckets[m_pPlayer->GetBucketNumber( )[i]].begin( );
				for (Ent; Ent != m_lpaEntityBuckets[m_pPlayer->GetBucketNumber( )[i]].end( ); Ent++)
				{
					if ((*Ent) == m_pPlayer)
					{
						m_lpaEntityBuckets[m_pPlayer->GetBucketNumber( )[i]].erase(Ent);
						break;
					}
				}
				m_pPlayer->SetBucketNumber(bucket[i], i);
				m_lpaEntityBuckets[bucket[i]].push_back(m_pPlayer);
			}
		}
	}
#pragma endregion

#pragma region Bullet Update
	if (m_lpBulletList.size( ) > 0)
	{
		std::list<CBullet*>::iterator iter = m_lpBulletList.begin( );
		for (iter; iter != m_lpBulletList.end( );)
		{
			(*iter)->Update(fDt);
			if ((*iter)->GetAlive( ) == false)
			{
				m_lpDeadBullets.push_back((*iter));
				iter = m_lpBulletList.erase(iter);
				if (iter == m_lpBulletList.end( ))
				{
					break;
				}
				continue;
			}

			CRObject* pBullet = static_cast<CRObject*>((*iter));
			pScene->AddObject(pBullet);
			if (pBullet->HasLight( ))
			{
				pScene->AddLight((*pBullet->GetLight( )));
			}

			TCell cellPos1, cellPos2, cellPos3, cellPos4;
			cellPos1.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] - (*iter)->GetCollider( )->fRadius) / m_unCellSize;
			cellPos1.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] + (*iter)->GetCollider( )->fRadius) / m_unCellSize;

			cellPos2.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] + (*iter)->GetCollider( )->fRadius) / m_unCellSize;
			cellPos2.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] + (*iter)->GetCollider( )->fRadius) / m_unCellSize;

			cellPos3.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] + (*iter)->GetCollider( )->fRadius) / m_unCellSize;
			cellPos3.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] - (*iter)->GetCollider( )->fRadius) / m_unCellSize;

			cellPos4.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] - (*iter)->GetCollider( )->fRadius) / m_unCellSize;
			cellPos4.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] - (*iter)->GetCollider( )->fRadius) / m_unCellSize;

			unsigned int bucket[4];
			bucket[0] = ComputeHashBucketIndex(cellPos1);
			bucket[1] = ComputeHashBucketIndex(cellPos2);
			bucket[2] = ComputeHashBucketIndex(cellPos3);
			bucket[3] = ComputeHashBucketIndex(cellPos4);
			for (size_t i = 0; i < 4; i++)
			{
				if (bucket[i] != (*iter)->GetBucketNumber( )[i])
				{
					auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].begin( );
					for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].end( ); Ent++)
					{
						if ((*Ent) == (*iter))
						{
							m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].erase(Ent);
							break;
						}
					}
					(*iter)->SetBucketNumber(bucket[i], i);
					m_lpaEntityBuckets[bucket[i]].push_back((*iter));
				}
			}
			iter++;
		}
	}
#pragma endregion

#pragma region Scenery Update
	if (m_lpSceneryList.size( ) > 0)
	{
		std::list<CScenery*>::iterator iter = m_lpSceneryList.begin( );
		for (iter; iter != m_lpSceneryList.end( ); iter++)
		{
			(*iter)->Update(fDt);
			//TODO: Test against things other than player
			(*iter)->TestZoneCollision(m_pPlayer);
			pScene->AddObject(static_cast<CRObject*>((*iter)));

			DirectX::XMFLOAT3 min = ((TAABB*)((*iter)->GetCollider( )))->vMin;
			DirectX::XMFLOAT3 max = ((TAABB*)((*iter)->GetCollider( )))->vMax;

			TCell cellPos1, cellPos2, cellPos3, cellPos4;
			cellPos1.x = (int)min.x / m_unCellSize;
			cellPos1.z = (int)min.z / m_unCellSize;

			cellPos2.x = (int)min.x / m_unCellSize;
			cellPos2.z = (int)(min.z + (max.z - min.z)) / m_unCellSize;

			cellPos3.x = (int)max.x / m_unCellSize;
			cellPos3.z = (int)max.z / m_unCellSize;

			cellPos4.x = (int)(min.x + (max.x - min.x)) / m_unCellSize;
			cellPos4.z = (int)min.z / m_unCellSize;

			unsigned int bucket[4];
			bucket[0] = ComputeHashBucketIndex(cellPos1);
			bucket[1] = ComputeHashBucketIndex(cellPos2);
			bucket[2] = ComputeHashBucketIndex(cellPos3);
			bucket[3] = ComputeHashBucketIndex(cellPos4);
			for (size_t i = 0; i < 4; i++)
			{
				if (bucket[i] != (*iter)->GetBucketNumber( )[i])
				{
					auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].begin( );
					for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].end( ); Ent++)
					{
						if ((*Ent) == (*iter))
						{
							m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].erase(Ent);
							break;
						}
					}
					(*iter)->SetBucketNumber(bucket[i], i);
					m_lpaEntityBuckets[bucket[i]].push_back((*iter));
				}
			}
		}
	}
#pragma endregion

	//if (m_lWaveSpawners.size() > 0)
	//{
	//	std::list<CWaveSpawner*>::iterator iter = m_lWaveSpawners.begin();
	//	for (iter; iter != m_lWaveSpawners.end(); iter++)
	//	{
	//		(*iter)->Update(fDt);
	//		//pScene->AddObject(static_cast<CRObject*>((*iter)));
	//	}
	//}


#pragma region Environment Update
	//Invisiwalls!!!
	//Make our capsule
	TCapsule tCamToPlayer;
	tCamToPlayer.type = TCollider::eCapsule;
	tCamToPlayer.fRadius = INVISIWALL_CAPSULE_RADIUS;
	tCamToPlayer.tSegment.m_Start = pScene->GetCamera( )->Position( );
	tCamToPlayer.tSegment.m_End = m_pPlayer->GetPosition( );
	tCamToPlayer.tSegment.m_End.z -= 100;
	if (m_lpEnvrionmentList.size( ) > 0)
	{
		std::list<CEntity*>::iterator iter = m_lpEnvrionmentList.begin( );
		for (iter; iter != m_lpEnvrionmentList.end( ); iter++)
		{
			(*iter)->Update(fDt);
			pScene->AddObject(static_cast<CRObject*>((*iter)));
			HandleInvisiWall(fDt, &tCamToPlayer, (*iter));
		}
	}
#pragma endregion

#pragma region SubSystem Update
	if (m_lpSubSystemList.size( ) > 0)
	{
		//tCamToPlayer.tSegment.m_End.z += 50;
		tCamToPlayer.fRadius *= 0.75f;
		std::list<CSubSystem*>::iterator iter = m_lpSubSystemList.begin( );
		for (iter; iter != m_lpSubSystemList.end( ); iter++)
		{
			(*iter)->Update(fDt);
			pScene->AddObject(static_cast<CRObject*>((*iter)));
			if ((*iter)->GetLight())
			{
				pScene->AddLight(*(*iter)->GetLight());
			}

			if (dynamic_cast<CReactor*>(*iter))
			{
				CReactor* pReactor = static_cast<CReactor*>((*iter));
				pScene->AddObject(pReactor->GetCore());
				pScene->AddObject(pReactor->GetServos());
				pScene->AddObject(pReactor->GetBase());
				HandleInvisiWall(fDt, &tCamToPlayer, pReactor->GetCore());
				HandleInvisiWall(fDt, &tCamToPlayer, pReactor->GetServos());
				HandleInvisiWall(fDt, &tCamToPlayer, pReactor->GetBase());
				HandleInvisiWall(fDt, &tCamToPlayer, (*iter));
			}
			else if (dynamic_cast<CEngines*>(*iter))
			{
				CEngines* pEngine = static_cast<CEngines*>((*iter));
				pScene->AddObject(pEngine->GetHoloDisplay());
			}


			//if ((*iter)->GetSystemType() == eSubsystemtype::eSystemEngine)
			//{
			//	CEngines* eng = (CEngines*)(*iter);
			//	//if (eng->GetCheckReached() == true)
			//	//{
			//	float progress = eng->GetProgress();
			//	
			//	if (m_lpEnvrionmentList.size() > 0 && progress >= m_nControl * m_fDistance)
			//	{
			//		auto iter = m_lpEnvrionmentList.begin();
			//		for (iter; iter != m_lpEnvrionmentList.end(); iter++)
			//		{
			//			CWaveSpawner* myspawn = (CWaveSpawner*)(*iter);
			//			myspawn->SetActive(true);
			//		}
			//		m_nControl = eng->GetCurrentWave();
			//	}
			//	//}
			//}

			/*DirectX::XMFLOAT3 min = ((TAABB*)((*iter)->GetCollider()))->vMin;
			DirectX::XMFLOAT3 max = ((TAABB*)((*iter)->GetCollider()))->vMax;

			TCell cellPos1, cellPos2, cellPos3, cellPos4;
			cellPos1.x = (int)min.x / m_unCellSize;
			cellPos1.z = (int)min.z / m_unCellSize;

			cellPos2.x = (int)min.x / m_unCellSize;
			cellPos2.z = (int)(min.z + (max.z - min.z)) / m_unCellSize;

			cellPos3.x = (int)max.x / m_unCellSize;
			cellPos3.z = (int)max.z / m_unCellSize;

			cellPos4.x = (int)(min.x + (max.x - min.x)) / m_unCellSize;
			cellPos4.z = (int)min.z / m_unCellSize;

			unsigned int bucket[4];
			bucket[0] = ComputeHashBucketIndex(cellPos1);
			bucket[1] = ComputeHashBucketIndex(cellPos2);
			bucket[2] = ComputeHashBucketIndex(cellPos3);
			bucket[3] = ComputeHashBucketIndex(cellPos4);
			for (size_t i = 0; i < 4; i++)
			{
			if (bucket[i] != (*iter)->GetBucketNumber()[i])
			{
			auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].begin();
			for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].end(); Ent++)
			{
			if ((*Ent) == (*iter))
			{
			m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].erase(Ent);
			break;
			}
			}
			(*iter)->SetBucketNumber(bucket[i], i);
			m_lpaEntityBuckets[bucket[i]].push_back((*iter));
			}
			}*/
		}
	}
#pragma endregion

#pragma region Creep Update

	if (m_lpCreepList.size( ) > 0)
	{
		std::list<CCreepEnemy*>::iterator iter = m_lpCreepList.begin( );
		for (iter; iter != m_lpCreepList.end( );)
		{
			(*iter)->Update(fDt);
			if ((*iter)->GetAlive( ) == false)
			{
				m_lpDeadCreepList.push_back((*iter));
				m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[0]].remove((*iter));
				m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[1]].remove((*iter));
				m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[2]].remove((*iter));
				m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[3]].remove((*iter));

				eEnemyVariant myVar = (*iter)->GetVariant();
				int multiplier = 1;

				if (eEnemyVariant::eSilver == myVar)
				{
					multiplier = 2;
				}
				else if (eEnemyVariant::eGold == myVar)
				{
					multiplier = 3;
				}

				m_pAIDirector->UpdateScore(CREEPSCORE * multiplier);

				//Spawn a new PickUp
				DirectX::XMFLOAT3 xmTemp = (*iter)->GetPosition( );
				xmTemp.y += 10;
				CreateScrapPickUp(xmTemp, (*iter)->GetScrapAmount( ));

				iter = m_lpCreepList.erase(iter);
				if (iter == m_lpCreepList.end( ))
				{
					break;
				}
				continue;
			}

			CRObject* pCreep = static_cast<CRObject*>((*iter));
			pScene->AddObject(pCreep);

			if (nullptr != (*iter)->GetVariantIndicator( ))
			{
				pScene->AddObject((*iter)->GetVariantIndicator( ));
			}

			if (pCreep->HasLight( ))
			{
				pScene->AddLight((*pCreep->GetLight( )));
			}

			TCell cellPos1, cellPos2, cellPos3, cellPos4;
			cellPos1.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] - (*iter)->GetCollider( )->fRadius) / m_unCellSize;
			cellPos1.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] + (*iter)->GetCollider( )->fRadius) / m_unCellSize;

			cellPos2.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] + (*iter)->GetCollider( )->fRadius) / m_unCellSize;
			cellPos2.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] + (*iter)->GetCollider( )->fRadius) / m_unCellSize;

			cellPos3.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] + (*iter)->GetCollider( )->fRadius) / m_unCellSize;
			cellPos3.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] - (*iter)->GetCollider( )->fRadius) / m_unCellSize;

			cellPos4.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] - (*iter)->GetCollider( )->fRadius) / m_unCellSize;
			cellPos4.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] - (*iter)->GetCollider( )->fRadius) / m_unCellSize;

			unsigned int bucket[4];
			bucket[0] = ComputeHashBucketIndex(cellPos1);
			bucket[1] = ComputeHashBucketIndex(cellPos2);
			bucket[2] = ComputeHashBucketIndex(cellPos3);
			bucket[3] = ComputeHashBucketIndex(cellPos4);
			for (size_t i = 0; i < 4; i++)
			{
				if (bucket[i] != (*iter)->GetBucketNumber( )[i])
				{
					auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].begin( );
					for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].end( ); Ent++)
					{
						if ((*Ent) == (*iter))
						{
							m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].erase(Ent);
							break;
						}
					}
					(*iter)->SetBucketNumber(bucket[i], i);
					m_lpaEntityBuckets[bucket[i]].push_back((*iter));
				}
			}
			iter++;
		}
	}

#pragma endregion

#pragma region Hunter Update
	if (m_lpHunterList.size( ) > 0)
	{
		std::list<CHunterEnemy*>::iterator iter = m_lpHunterList.begin( );
		for (iter; iter != m_lpHunterList.end( );)
		{
			(*iter)->Update(fDt);
			//Kill the hunter
			if ((*iter)->GetAlive( ) == false)
			{
				m_lpDeadHunters.push_back((*iter));
				m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[0]].remove((*iter));
				m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[1]].remove((*iter));
				m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[2]].remove((*iter));
				m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[3]].remove((*iter));

				eEnemyVariant myVar = (*iter)->GetVariant();
				int multiplier = 1;

				if (eEnemyVariant::eSilver == myVar)
				{
					multiplier = 2;
				}
				else if (eEnemyVariant::eGold == myVar)
				{
					multiplier = 3;
				}

				m_pAIDirector->UpdateScore(HUNTERSCORE * multiplier);

				//Spawn a new PickUp
				DirectX::XMFLOAT3 xmTemp = (*iter)->GetPosition( );
				xmTemp.y += 10;
				CreateScrapPickUp(xmTemp, (*iter)->GetScrapAmount( ));

				iter = m_lpHunterList.erase(iter);
				if (iter == m_lpHunterList.end( ))
				{
					break;
				}
				continue;
			}

			CRObject* pHunter = static_cast<CRObject*>((*iter));
			pScene->AddObject(pHunter);

			if (nullptr != (*iter)->GetVariantIndicator( ))
			{
				pScene->AddObject((*iter)->GetVariantIndicator( ));
			}

			if (pHunter->HasLight( ))
			{
				pScene->AddLight((*pHunter->GetLight( )));
			}

			TCell cellPos1, cellPos2, cellPos3, cellPos4;
			cellPos1.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] - (*iter)->GetCollider( )->fRadius) / m_unCellSize;
			cellPos1.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] + (*iter)->GetCollider( )->fRadius) / m_unCellSize;

			cellPos2.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] + (*iter)->GetCollider( )->fRadius) / m_unCellSize;
			cellPos2.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] + (*iter)->GetCollider( )->fRadius) / m_unCellSize;

			cellPos3.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] + (*iter)->GetCollider( )->fRadius) / m_unCellSize;
			cellPos3.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] - (*iter)->GetCollider( )->fRadius) / m_unCellSize;

			cellPos4.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] - (*iter)->GetCollider( )->fRadius) / m_unCellSize;
			cellPos4.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] - (*iter)->GetCollider( )->fRadius) / m_unCellSize;

			unsigned int bucket[4];
			bucket[0] = ComputeHashBucketIndex(cellPos1);
			bucket[1] = ComputeHashBucketIndex(cellPos2);
			bucket[2] = ComputeHashBucketIndex(cellPos3);
			bucket[3] = ComputeHashBucketIndex(cellPos4);
			for (size_t i = 0; i < 4; i++)
			{
				if (bucket[i] != (*iter)->GetBucketNumber( )[i])
				{
					auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].begin( );
					for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].end( ); Ent++)
					{
						if ((*Ent) == (*iter))
						{
							m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].erase(Ent);
							break;
						}
					}
					(*iter)->SetBucketNumber(bucket[i], i);
					m_lpaEntityBuckets[bucket[i]].push_back((*iter));
				}
			}
			iter++;
		}
	}
#pragma endregion

#pragma region Saboteur Update
	if (m_lpSaboteurList.size( ) > 0)
	{
		std::list<CSaboteurEnemy*>::iterator iter = m_lpSaboteurList.begin( );
		for (iter; iter != m_lpSaboteurList.end( );)
		{
			CSaboteurEnemy* saboteur = (*iter);

			saboteur->Update(fDt);
			if (saboteur->GetAlive( ) == false)
			{
				m_lpDeadSaboteurs.push_back(saboteur);
				m_lpaEntityBuckets[saboteur->GetBucketNumber( )[0]].remove(saboteur);
				m_lpaEntityBuckets[saboteur->GetBucketNumber( )[1]].remove(saboteur);
				m_lpaEntityBuckets[saboteur->GetBucketNumber( )[2]].remove(saboteur);
				m_lpaEntityBuckets[saboteur->GetBucketNumber( )[3]].remove(saboteur);

				eEnemyVariant myVar = saboteur->GetVariant();
				int multiplier = 1;

				if (eEnemyVariant::eSilver == myVar)
				{
					multiplier = 2;
				}
				else if (eEnemyVariant::eGold == myVar)
				{
					multiplier = 3;
				}

				m_pAIDirector->UpdateScore(SABOTEURSCORE * multiplier);

				//Spawn a new PickUp
				DirectX::XMFLOAT3 xmTemp = saboteur->GetPosition( );
				xmTemp.y += 10;
				CreateScrapPickUp(xmTemp, saboteur->GetScrapAmount( ));

				iter = m_lpSaboteurList.erase(iter);
				if (iter == m_lpSaboteurList.end( ))
				{
					break;
				}
				continue;
			}

			CRObject* pSaboteur = static_cast<CRObject*>(saboteur);
			pScene->AddObject(pSaboteur);

			if (pSaboteur->HasLight( ))
			{
				pScene->AddLight((*pSaboteur->GetLight( )));
			}

			if (nullptr != saboteur->GetVariantIndicator( ))
			{
				pScene->AddObject(saboteur->GetVariantIndicator( ));
			}
			//if (nullptr != saboteur->GetAttackEffect( ))
			//{
			//	pScene->AddObject(saboteur->GetAttackEffect( ));
			//}

			TCell cellPos1, cellPos2, cellPos3, cellPos4;
			cellPos1.x = (int)(saboteur->GetWorldMatrix( ).m[3][0] - saboteur->GetCollider( )->fRadius) / m_unCellSize;
			cellPos1.z = (int)(saboteur->GetWorldMatrix( ).m[3][2] + saboteur->GetCollider( )->fRadius) / m_unCellSize;

			cellPos2.x = (int)(saboteur->GetWorldMatrix( ).m[3][0] + saboteur->GetCollider( )->fRadius) / m_unCellSize;
			cellPos2.z = (int)(saboteur->GetWorldMatrix( ).m[3][2] + saboteur->GetCollider( )->fRadius) / m_unCellSize;

			cellPos3.x = (int)(saboteur->GetWorldMatrix( ).m[3][0] + saboteur->GetCollider( )->fRadius) / m_unCellSize;
			cellPos3.z = (int)(saboteur->GetWorldMatrix( ).m[3][2] - saboteur->GetCollider( )->fRadius) / m_unCellSize;

			cellPos4.x = (int)(saboteur->GetWorldMatrix( ).m[3][0] - saboteur->GetCollider( )->fRadius) / m_unCellSize;
			cellPos4.z = (int)(saboteur->GetWorldMatrix( ).m[3][2] - saboteur->GetCollider( )->fRadius) / m_unCellSize;

			unsigned int bucket[4];
			bucket[0] = ComputeHashBucketIndex(cellPos1);
			bucket[1] = ComputeHashBucketIndex(cellPos2);
			bucket[2] = ComputeHashBucketIndex(cellPos3);
			bucket[3] = ComputeHashBucketIndex(cellPos4);
			for (size_t i = 0; i < 4; i++)
			{
				if (bucket[i] != saboteur->GetBucketNumber( )[i])
				{
					auto& Ent = m_lpaEntityBuckets[saboteur->GetBucketNumber( )[i]].begin( );
					for (Ent; Ent != m_lpaEntityBuckets[saboteur->GetBucketNumber( )[i]].end( ); Ent++)
					{
						if ((*Ent) == saboteur)
						{
							m_lpaEntityBuckets[saboteur->GetBucketNumber( )[i]].erase(Ent);
							break;
						}
					}
					saboteur->SetBucketNumber(bucket[i], i);
					m_lpaEntityBuckets[bucket[i]].push_back(saboteur);
				}
			}
			iter++;
		}
	}
#pragma endregion

//#pragma region Spammer Update
//	if (m_lpSpammerList.size() > 0)
//	{
//		std::list<CSpammerEnemy*>::iterator iter = m_lpSpammerList.begin();
//		for (iter; iter != m_lpSpammerList.end();)
//		{
//			(*iter)->Update(fDt);
//
//			//Kill the spammer
//			if ((*iter)->GetAlive() == false)
//			{
//				m_lpDeadSpammers.push_back((*iter));
//				m_lpaEntityBuckets[(*iter)->GetBucketNumber()[0]].remove((*iter));
//				m_lpaEntityBuckets[(*iter)->GetBucketNumber()[1]].remove((*iter));
//				m_lpaEntityBuckets[(*iter)->GetBucketNumber()[2]].remove((*iter));
//				m_lpaEntityBuckets[(*iter)->GetBucketNumber()[3]].remove((*iter));
//
//				eEnemyVariant myVar = (*iter)->GetVariant();
//				int multiplier = 1;
//
//				if (eEnemyVariant::eSilver == myVar)
//				{
//					multiplier = 2;
//				}
//				else if (eEnemyVariant::eGold == myVar)
//				{
//					multiplier = 3;
//				}
//
//				m_pAIDirector->UpdateScore(SPAMMERSCORE * multiplier);
//
//				//Spawn a new PickUp
//				DirectX::XMFLOAT3 xmTemp = (*iter)->GetPosition();
//				xmTemp.y += 10;
//				CreateScrapPickUp(xmTemp, (*iter)->GetScrapAmount());
//
//				iter = m_lpSpammerList.erase(iter);
//				if (iter == m_lpSpammerList.end())
//				{
//					break;
//				}
//				continue;
//			}
//
//			CRObject* pSpammer = static_cast<CRObject*>((*iter));
//			pScene->AddObject(pSpammer);
//
//			if (nullptr != (*iter)->GetVariantIndicator())
//			{
//				pScene->AddObject((*iter)->GetVariantIndicator());
//			}
//
//			if (pSpammer->HasLight())
//			{
//				pScene->AddLight((*pSpammer->GetLight()));
//			}
//
//			TCell cellPos1, cellPos2, cellPos3, cellPos4;
//			cellPos1.x = (int)((*iter)->GetWorldMatrix().m[3][0] - (*iter)->GetCollider()->fRadius) / m_unCellSize;
//			cellPos1.z = (int)((*iter)->GetWorldMatrix().m[3][2] + (*iter)->GetCollider()->fRadius) / m_unCellSize;
//
//			cellPos2.x = (int)((*iter)->GetWorldMatrix().m[3][0] + (*iter)->GetCollider()->fRadius) / m_unCellSize;
//			cellPos2.z = (int)((*iter)->GetWorldMatrix().m[3][2] + (*iter)->GetCollider()->fRadius) / m_unCellSize;
//
//			cellPos3.x = (int)((*iter)->GetWorldMatrix().m[3][0] + (*iter)->GetCollider()->fRadius) / m_unCellSize;
//			cellPos3.z = (int)((*iter)->GetWorldMatrix().m[3][2] - (*iter)->GetCollider()->fRadius) / m_unCellSize;
//
//			cellPos4.x = (int)((*iter)->GetWorldMatrix().m[3][0] - (*iter)->GetCollider()->fRadius) / m_unCellSize;
//			cellPos4.z = (int)((*iter)->GetWorldMatrix().m[3][2] - (*iter)->GetCollider()->fRadius) / m_unCellSize;
//
//			unsigned int bucket[4];
//			bucket[0] = ComputeHashBucketIndex(cellPos1);
//			bucket[1] = ComputeHashBucketIndex(cellPos2);
//			bucket[2] = ComputeHashBucketIndex(cellPos3);
//			bucket[3] = ComputeHashBucketIndex(cellPos4);
//			for (size_t i = 0; i < 4; i++)
//			{
//				if (bucket[i] != (*iter)->GetBucketNumber()[i])
//				{
//					auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].begin();
//					for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].end(); Ent++)
//					{
//						if ((*Ent) == (*iter))
//						{
//							m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].erase(Ent);
//							break;
//						}
//					}
//					(*iter)->SetBucketNumber(bucket[i], i);
//					m_lpaEntityBuckets[bucket[i]].push_back((*iter));
//				}
//			}
//			iter++;
//		}
//	}
//#pragma endregion

#pragma region PickUp Update
	if (m_lpPickUpList.size( ) > 0)
	{
		std::list<CPickUp*>::iterator iter = m_lpPickUpList.begin( );
		for (iter; iter != m_lpPickUpList.end( );)
		{
			(*iter)->Update(fDt);
			if ((*iter)->GetAlive( ) == false)
			{
				m_lpDeadPickUps.push_back((*iter));
				m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[0]].remove((*iter));
				m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[1]].remove((*iter));
				m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[2]].remove((*iter));
				m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[3]].remove((*iter));

				iter = m_lpPickUpList.erase(iter);
				if (iter == m_lpPickUpList.end( ))
				{
					break;
				}
				continue;
			}

			CRObject* pPickup = static_cast<CRObject*>((*iter));
			pScene->AddObject(pPickup);
			if (pPickup->HasLight( ))
			{
				pScene->AddLight((*pPickup->GetLight( )));
			}

			TCell cellPos1, cellPos2, cellPos3, cellPos4;
			cellPos1.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] - ((TSphere*)(*iter)->GetCollider( ))->fRadius) / m_unCellSize;
			cellPos1.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] + ((TSphere*)(*iter)->GetCollider( ))->fRadius) / m_unCellSize;

			cellPos2.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] + ((TSphere*)(*iter)->GetCollider( ))->fRadius) / m_unCellSize;
			cellPos2.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] + ((TSphere*)(*iter)->GetCollider( ))->fRadius) / m_unCellSize;

			cellPos3.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] + ((TSphere*)(*iter)->GetCollider( ))->fRadius) / m_unCellSize;
			cellPos3.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] - ((TSphere*)(*iter)->GetCollider( ))->fRadius) / m_unCellSize;

			cellPos4.x = (int)((*iter)->GetWorldMatrix( ).m[3][0] - ((TSphere*)(*iter)->GetCollider( ))->fRadius) / m_unCellSize;
			cellPos4.z = (int)((*iter)->GetWorldMatrix( ).m[3][2] - ((TSphere*)(*iter)->GetCollider( ))->fRadius) / m_unCellSize;

			unsigned int bucket[4];
			bucket[0] = ComputeHashBucketIndex(cellPos1);
			bucket[1] = ComputeHashBucketIndex(cellPos2);
			bucket[2] = ComputeHashBucketIndex(cellPos3);
			bucket[3] = ComputeHashBucketIndex(cellPos4);
			for (size_t i = 0; i < 4; i++)
			{
				if (bucket[i] != (*iter)->GetBucketNumber( )[i])
				{
					auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].begin( );
					for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].end( ); Ent++)
					{
						if ((*Ent) == (*iter))
						{
							m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].erase(Ent);
							break;
						}
					}
					(*iter)->SetBucketNumber(bucket[i], i);
					m_lpaEntityBuckets[bucket[i]].push_back((*iter));
				}
			}
			iter++;
		}
	}
#pragma endregion

	if (m_pFloorGuide)
	{
		m_pFloorGuide->Update(fDt);
		m_pFloorGuide->AddToScene(pScene);
	}

	// Collision Fun
	CheckCollisions(fDt);
}

unsigned int CEntityManager::ComputeHashBucketIndex(TCell vCellPos)
{
	const int h1 = 0x8da6b343;	// Arbitrary, large primes.
	const int h2 = 0xd8163841;	// Primes are popular for hash functions
	const int h3 = 0xcb1ab31f;	// for reducing the chance of hash collision.

	int n = h1 * vCellPos.x + h2 /** vCellPos.y*/ + h3 * vCellPos.z;

	n = n % NUM_BUCKETS;	// Wrap indices to stay in bucket range
	if (n < 0) n += NUM_BUCKETS;	// Keep indices in positive range
	return n;

}

void CEntityManager::CheckCollisions(float fDt)
{
	//bool YayNayDirs[4] = { true, true, true, true };
	for (size_t CurrBucket = 0; CurrBucket < NUM_BUCKETS; CurrBucket++)
	{
		if (m_lpaEntityBuckets[CurrBucket].size( ) > 1)
		{
			// TODO: Change the for loops to the other kind and uncomment the "help"

			auto CurrentIter = m_lpaEntityBuckets[CurrBucket].begin( );
			for (; CurrentIter != m_lpaEntityBuckets[CurrBucket].end( ); CurrentIter++)
			{
				auto CurrIter = (*CurrentIter);
				if (CurrIter->GetCollider( ) == NULL)
				{
					continue;
				}

				auto OtherIterator = m_lpaEntityBuckets[CurrBucket].begin( );
				for (; OtherIterator != m_lpaEntityBuckets[CurrBucket].end( ); OtherIterator++)
				{
					auto OtherIter = (*OtherIterator);

					if (CurrIter == OtherIter || OtherIter->GetCollider( ) == NULL)
					{
						continue;
					}
					if (EntityToEntity(CurrIter, OtherIter))
					{
						// if the above is true then the two things are colliding
						// so do shit...
						int CurrItersTag = CurrIter->GetEntityType( );
						int OtherItersTag = OtherIter->GetEntityType( );

						//Le pick-ups
						if (CurrItersTag == CEntity::ePICKUP)
						{
							CPickUp* pPickUp = static_cast<CPickUp*>(CurrIter);
							//Player picked it up
							if (OtherItersTag == CEntity::ePLAYER)
							{
								//Until we find more sounds
								AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_2D_PLAYER_PICKUP);

								//Add to the player's scrap
								((CPlayer*)OtherIter)->HandlePickUp(pPickUp);
							}
						}

						// Le bullets
						else if (CurrItersTag == CEntity::eBULLET)
						{
							//Handle this mess in the bullet
							static_cast<CBullet*>((CurrIter))->Collide((OtherIter)->GetCollider( ), (OtherIter));

							/*if (OtherItersTag == CEntity::eSABOTEUR)
							{
							CSaboteurEnemy* Sabo = static_cast<CSaboteurEnemy*>((OtherIter));
							if (Sabo->GetAlive() && bullet->GetAlive())
							{
							Sabo->TakeDamage(bullet->GetOwner()->GetDamage());
							//CycleIntoDeadList(temp);
							bullet->Disable();
							}
							}
							else if (OtherItersTag == CEntity::eHUNTER)
							{
							CHunterEnemy* Hunter = static_cast<CHunterEnemy*>((OtherIter));
							if (Hunter->GetAlive() && bullet->GetAlive())
							{
							Hunter->TakeDamage(bullet->GetOwner()->GetDamage());
							//CycleIntoDeadList(temp);
							bullet->Disable();
							}
							}
							else if (OtherItersTag  == CEntity::eCREEP)
							{
							CCreepEnemy* Creep = static_cast<CCreepEnemy*>((OtherIter));
							if (Creep->GetAlive() && bullet->GetAlive())
							{
							Creep->TakeDamage(bullet->GetOwner()->GetDamage());
							// emit blood particle effect
							CParticleSystem* pSys = bullet->m_pBloodImpactEffect;
							XMFLOAT3 pos = bullet->GetPosition();
							vec3f posV = vec3f { pos.x, pos.y, pos.z };
							pSys->SetPosition(posV);
							pSys->GetEmitter(0)->Emit();

							//CycleIntoDeadList(temp);
							bullet->Disable();
							}
							}
							else if (OtherItersTag == CEntity::eREACTOR || OtherItersTag == CEntity::eENGINES ||
							OtherItersTag == CEntity::eLIFESUPPORT || OtherItersTag == CEntity::eWORKSTATION)
							{
							bullet->Disable();
							}*/
						}
						else if (CurrItersTag == CEntity::eENEMYBULLET)
						{
							CBullet* bullet = static_cast<CBullet*>((CurrIter));
							if (OtherItersTag == CEntity::ePLAYER)
							{
								if (bullet->GetOwner( ) != nullptr)
								{
									m_pPlayer->TakeDamage(bullet->GetOwner( )->GetDamage( ));
									bullet->Disable( );
								}
							}
						}
						else if (CurrItersTag != CEntity::eBULLET &&
								 OtherItersTag != CEntity::eBULLET &&
								 CurrItersTag != CEntity::eENEMYBULLET &&
								 OtherItersTag != CEntity::eENEMYBULLET)
						{
							// if neither are bullets then they both are some form of living entity

							DirectX::XMFLOAT3 PositionOne;
							DirectX::XMFLOAT3 PositionTwo;
							DirectX::XMFLOAT3 ToOtherEntity;
							DirectX::XMFLOAT3 ToFirstEntity;

							PositionOne.x = CurrIter->GetWorldMatrix( ).m[3][0];
							PositionOne.y = CurrIter->GetWorldMatrix( ).m[3][1];
							PositionOne.z = CurrIter->GetWorldMatrix( ).m[3][2];

							PositionTwo.x = OtherIter->GetWorldMatrix( ).m[3][0];
							PositionTwo.y = OtherIter->GetWorldMatrix( ).m[3][1];
							PositionTwo.z = OtherIter->GetWorldMatrix( ).m[3][2];

							ToOtherEntity = PositionTwo - PositionOne;
							ToFirstEntity = PositionOne - PositionTwo;

							// normalizing both of the above vectors  
							ToOtherEntity = Normalize(ToOtherEntity);
							ToFirstEntity = Normalize(ToFirstEntity);

							if ((CurrItersTag != CEntity::ePLAYER && OtherItersTag != CEntity::ePLAYER))
							{
								if (CurrItersTag != CEntity::eSUBSYSTEM && CurrItersTag != CEntity::eWORKSTATION)
								{
									// TODO: Jakes function that is used below
									DirectX::XMFLOAT3 newPos = PositionOne + ToFirstEntity;
									int stuff;

									if (CPathSearch::GetInstance( )->PointInTriangle(newPos, stuff) == true)
									{
										CurrIter->GetLocalMatrix( ).m[3][0] = newPos.x;// (PositionOne.x + (ToFirstEntity.x));// * fDt));
										CurrIter->GetLocalMatrix( ).m[3][2] = newPos.z;// (PositionOne.z + (ToFirstEntity.z));// * fDt
									}
								}
							}
							if ((CurrItersTag == CEntity::ePLAYER))
							{
								if (OtherItersTag != CEntity::eSUBSYSTEM && OtherItersTag != CEntity::eWORKSTATION)
								{
									if (OtherItersTag == CEntity::eCREEP)
									{
										m_pPlayer->SetCreepColliding(true);
									}
									else if (OtherItersTag == CEntity::eHUNTER)
									{
										m_pPlayer->SetHunterColliding(true);
									}
									else if (OtherItersTag == CEntity::eSABOTEUR)
									{
										m_pPlayer->SetSabotuerColliding(true);
									}
									/*else if (OtherItersTag == CEntity::eSPAMMER)
									{
										m_pPlayer->SetSpammerColliding(true);
									}*/

									DirectX::XMFLOAT3 newPos = PositionTwo + ToOtherEntity;
									int stuff;

									if (CPathSearch::GetInstance( )->PointInTriangle(newPos, stuff) == true)
									{
										OtherIter->GetLocalMatrix( ).m[3][0] = newPos.x;// * fDt));
										OtherIter->GetLocalMatrix( ).m[3][2] = newPos.z;// * fDt));
									}
								}
								/*else if (OtherItersTag == CEntity::eWORKSTATION)
								{
								CurrIter->GetLocalMatrix().m[3][0] = (PositionOne.x + (ToFirstEntity.x));// * fDt));
								CurrIter->GetLocalMatrix().m[3][2] = (PositionOne.z + (ToFirstEntity.z));// * fDt));
								}*/
							}
						}
					}
				}
			}
		}
	}
	//m_pPlayer->SetAvailableMoveDirs(YayNayDirs);
}

void CEntityManager::CollisionWithMap( )
{
	bool YayNayDirs[4] = { true, true, true, true };
	auto& MapColliderIter = m_lpCollisionBoxList.begin( );
	for (MapColliderIter; MapColliderIter != m_lpCollisionBoxList.end( ); MapColliderIter++)
	{
		DirectX::XMFLOAT3 CollisionPoint = { 0, 0, 0 };
		if (CCollision::SphereToAABB(m_pPlayer->GetCollider( ), (*MapColliderIter), CollisionPoint))
		{
			DirectX::XMFLOAT3 PlayerPos = { m_pPlayer->GetWorldMatrix( ).m[3][0], 0, m_pPlayer->GetWorldMatrix( ).m[3][2] };

			DirectX::XMFLOAT3 ToCollPoint = CollisionPoint - PlayerPos; // vector from the player to the point
			if (ToCollPoint.z > ToCollPoint.x && ToCollPoint.z > 0) // the point is more up than right or left
			{
				YayNayDirs[0] = false;
			}
			else if (ToCollPoint.z < ToCollPoint.x && ToCollPoint.z < 0) // the point is more down than right or left
			{
				YayNayDirs[1] = false;
			}
			if (ToCollPoint.x > ToCollPoint.z && ToCollPoint.x > 0) // the point is more right than up or down
			{
				YayNayDirs[2] = false;
			}
			else if (ToCollPoint.x < ToCollPoint.z && ToCollPoint.x < 0) // the point is more left than up or down
			{
				YayNayDirs[3] = false;
			}

		}
		// Bullet stuffs
		auto& BulletIter = m_lpBulletList.begin( );
		for (BulletIter; BulletIter != m_lpBulletList.end( ); BulletIter++)
		{
			DirectX::XMFLOAT3 NotUsed;
			if (CCollision::SphereToAABB((*BulletIter)->GetCollider( ), (*MapColliderIter), NotUsed))
			{
				(*BulletIter)->Collide((*MapColliderIter), nullptr);
			}
		}
	}

	// SubSystem checks
	auto& SystemCollIter = m_lpSubSystemList.begin( );
	for (SystemCollIter; SystemCollIter != m_lpSubSystemList.end( ); SystemCollIter++)
	{
		DirectX::XMFLOAT3 CollisionPoint = { 0, 0, 0 };
		if (CCollision::SphereToAABB(m_pPlayer->GetCollider( ), (*SystemCollIter)->GetCollider( ), CollisionPoint))
		{
			DirectX::XMFLOAT3 PlayerPos = { m_pPlayer->GetWorldMatrix( ).m[3][0], 0, m_pPlayer->GetWorldMatrix( ).m[3][2] };

			DirectX::XMFLOAT3 ToCollPoint = CollisionPoint - PlayerPos; // vector from the player to the point
			if (ToCollPoint.z > ToCollPoint.x && ToCollPoint.z > 0) // the point is more up than right or left
			{
				YayNayDirs[0] = false;
			}
			else if (ToCollPoint.z < ToCollPoint.x && ToCollPoint.z < 0) // the point is more down than right or left
			{
				YayNayDirs[1] = false;
			}
			if (ToCollPoint.x > ToCollPoint.z && ToCollPoint.x > 0) // the point is more right than up or down
			{
				YayNayDirs[2] = false;
			}
			else if (ToCollPoint.x < ToCollPoint.z && ToCollPoint.x < 0) // the point is more left than up or down
			{
				YayNayDirs[3] = false;
			}
		}

		auto& BulletIter = m_lpBulletList.begin( );
		for (BulletIter; BulletIter != m_lpBulletList.end( ); BulletIter++)
		{
			DirectX::XMFLOAT3 NotUsed;
			if (CCollision::SphereToAABB((*BulletIter)->GetCollider( ), (*SystemCollIter)->GetCollider( ), NotUsed))
			{
				AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_3D_RICOCHET, (*BulletIter)->GetPosition( ));
				(*BulletIter)->Disable( );
			}
		}
	}

	auto& SceneCollIter = m_lpSceneryList.begin( );
	for (SceneCollIter; SceneCollIter != m_lpSceneryList.end( ); SceneCollIter++)
	{
		DirectX::XMFLOAT3 CollisionPoint = { 0, 0, 0 };
		if (CCollision::SphereToAABB(m_pPlayer->GetCollider( ), (TAABB*)(*SceneCollIter)->GetCollider( ), CollisionPoint))
		{
			DirectX::XMFLOAT3 PlayerPos = { m_pPlayer->GetWorldMatrix( ).m[3][0], 0, m_pPlayer->GetWorldMatrix( ).m[3][2] };

			DirectX::XMFLOAT3 ToCollPoint = CollisionPoint - PlayerPos; // vector from the player to the point
			if (ToCollPoint.z > ToCollPoint.x && ToCollPoint.z > 0) // the point is more up than right or left
			{
				YayNayDirs[0] = false;
			}
			else if (ToCollPoint.z < ToCollPoint.x && ToCollPoint.z < 0) // the point is more down than right or left
			{
				YayNayDirs[1] = false;
			}
			if (ToCollPoint.x > ToCollPoint.z && ToCollPoint.x > 0) // the point is more right than up or down
			{
				YayNayDirs[2] = false;
			}
			else if (ToCollPoint.x < ToCollPoint.z && ToCollPoint.x < 0) // the point is more left than up or down
			{
				YayNayDirs[3] = false;
			}
		}

		auto& BulletIter = m_lpBulletList.begin( );
		for (BulletIter; BulletIter != m_lpBulletList.end( ); BulletIter++)
		{
			DirectX::XMFLOAT3 NotUsed;
			if (CCollision::SphereToAABB((*BulletIter)->GetCollider( ), (TAABB*)(*SceneCollIter)->GetCollider( ), NotUsed))
			{
				AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_3D_RICOCHET, (*BulletIter)->GetPosition( ));
				(*BulletIter)->Disable( );
			}
		}
	}

	m_pPlayer->SetAvailableDirs(YayNayDirs);
}

bool CEntityManager::EntityToEntity(CEntity* cEnt_1, CEntity* cEnt_2)
{
	int Ent1Type = cEnt_1->GetEntityType( );
	int Ent2Type = cEnt_2->GetEntityType( );
	if ((Ent1Type == CEntity::eBULLET && Ent2Type == CEntity::eBULLET) ||
		(Ent1Type == CEntity::ePLAYER && Ent2Type == CEntity::eBULLET) ||
		(Ent1Type == CEntity::eBULLET && Ent2Type == CEntity::ePLAYER) ||
		(Ent1Type == CEntity::eMAP && Ent2Type == CEntity::eMAP) ||
		(Ent1Type == CEntity::ePICKUP && Ent2Type != CEntity::ePLAYER) ||
		//((Ent1Type == CEntity::eLIFESUPPORT || Ent1Type == CEntity::eREACTOR || Ent1Type == CEntity::eENGINES || Ent1Type == CEntity::eMAP) && Ent2Type != CEntity::eWORKSTATION) ||
		//((Ent2Type == CEntity::eLIFESUPPORT || Ent2Type == CEntity::eREACTOR || Ent2Type == CEntity::eENGINES || Ent2Type == CEntity::eMAP) && Ent1Type != CEntity::eWORKSTATION) ||
		(Ent2Type == CEntity::ePICKUP) ||
		(Ent1Type == CEntity::eENEMYBULLET &&
		(Ent2Type != CEntity::ePLAYER && Ent2Type != CEntity::eENEMYBULLET)) ||
		(Ent1Type == CEntity::eENEMYBULLET && Ent2Type == CEntity::eENEMYBULLET) ||
		(Ent2Type == CEntity::eENEMYBULLET &&
		(Ent1Type != CEntity::ePLAYER && Ent1Type != CEntity::eENEMYBULLET)))
	{
		return false;
	}

	return CCollision::ColliderToCollider(cEnt_1->GetCollider( ), cEnt_2->GetCollider( ));
}

bool CEntityManager::CycleIntoLiveList(int EntityType, DirectX::XMFLOAT3 fPosition, CIWeapon* Owner, eEnemyVariant variant)
{
	int Type = EntityType;
	if (Type == CEntity::eBULLET)
	{
		if (m_lpDeadBullets.size( ) > 0)
		{
			auto& BulletIter = m_lpDeadBullets.begin( );
			m_lpBulletList.push_back(*BulletIter);

			(*BulletIter)->Enable(fPosition, Owner, CEntity::eBULLET);
			m_lpDeadBullets.erase(BulletIter);
			return true;
		}
		/*else
		{
		CRObject* bullet = new CBullet();
		bullet->SetTag("Bullet");
		CBullet* temp = static_cast<CBullet*>(bullet);
		AddToManager(temp, true);
		temp->Enable(fPosition);
		return false;
		}*/
	}
	else if (Type == CEntity::eENEMYBULLET)
	{
		if (m_lpDeadBullets.size( ) > 0)
		{
			auto& BulletIter = m_lpDeadBullets.begin( );
			m_lpBulletList.push_back(*BulletIter);

			(*BulletIter)->Enable(fPosition, Owner, CEntity::eENEMYBULLET);
			m_lpDeadBullets.erase(BulletIter);
			return true;
		}
	}
	//This worksby default, but should use CreateScrapPickUp instead
	else if (Type == CEntity::ePICKUP)
	{
		CreateScrapPickUp(fPosition);
	}
	if (Type == CEntity::eCREEP)
	{
		if (m_lpDeadCreepList.size( ) > 0)
		{
			auto& CreepIter = m_lpDeadCreepList.begin( );
			(*CreepIter)->Enable(fPosition, variant);
			m_lpCreepList.push_back(*CreepIter);
			//AudioSystemWwise::Get()->RegisterEntity((*CreepIter), "Creep");
			m_lpDeadCreepList.erase(CreepIter);
			return true;
		}
		/*else
		{
		CCreepEnemy* creep = new CCreepEnemy();
		creep->SetTag("Creep");
		AudioSystemWwise::Get()->RegisterEntity(creep, "Creep");
		AddToManager(creep, true);
		creep->Enable(fPosition);
		return false;
		}*/
	}
	if (Type == CEntity::eHUNTER)
	{
		if (m_lpDeadHunters.size( ) > 0)
		{
			auto& HunterIter = m_lpDeadHunters.begin( );
			(*HunterIter)->Enable(fPosition, variant);
			m_lpHunterList.push_back(*HunterIter);
			//AudioSystemWwise::Get()->RegisterEntity((*HunterIter), "Hunter");
			m_lpDeadHunters.erase(HunterIter);
			return true;
		}
		/*else
		//{
		CHunterEnemy* hunter = new CHunterEnemy();
		hunter->SetTag("Hunter");
		//AudioSystemWwise::Get()->RegisterEntity(hunter, "Hunter");
		AddToManager(hunter, true);
		hunter->Enable(fPosition);
		return false;
		}*/
	}
	if (Type == CEntity::eSABOTEUR)
	{
		if (m_lpDeadSaboteurs.size( ) > 0)
		{
			auto& SaboteurIter = m_lpDeadSaboteurs.begin( );
			(*SaboteurIter)->Enable(fPosition, variant);
			//AudioSystemWwise::Get()->RegisterEntity((*SaboteurIter), "Saboteur");
			m_lpSaboteurList.push_back(*SaboteurIter);
			m_lpDeadSaboteurs.erase(SaboteurIter);
			return true;
		}
		/*else
		{
		CEntity* Sabo = new CSaboteurEnemy();
		Sabo->SetTag("Saboteur");
		//AudioSystemWwise::Get()->RegisterEntity(Sabo, "Saboteur");
		CSaboteurEnemy* temp = static_cast<CSaboteurEnemy*>(Sabo);
		AddToManager(temp, true);
		temp->Enable(fPosition);
		return false;
		}*/
	}
	//if (Type == CEntity::eSPAMMER)
	//{
	//	if (m_lpDeadSpammers.size() > 0)
	//	{
	//		auto& SpammerIter = m_lpDeadSpammers.begin();
	//		(*SpammerIter)->Enable(fPosition, variant);
	//		m_lpSpammerList.push_back(*SpammerIter);
	//		m_lpDeadSpammers.erase(SpammerIter);
	//		return true;
	//	}
	//}
	return false;
}

void CEntityManager::CreateScrapPickUp(DirectX::XMFLOAT3 xmSpawnPos, int nScrapAmount)
{
	if (m_lpDeadPickUps.size( ) > 0)
	{
		auto& PickUpIter = m_lpDeadPickUps.begin( );
		(*PickUpIter)->SetScrapValue(nScrapAmount);
		//Assign color based on value
		if (nScrapAmount <= 10)
		{
			(*PickUpIter)->SetColor(DirectX::XMFLOAT4(0.804f, 0.498f, 0.196f, 1.0f));
		}
		else if (nScrapAmount <= 25)
		{
			(*PickUpIter)->SetColor(DirectX::XMFLOAT4(0.753f, 0.753f, 0.753f, 1.0f));
		}
		else
		{
			(*PickUpIter)->SetColor(DirectX::XMFLOAT4(1.0f, 0.843f, 0.0f, 1.0f));
		}
		m_lpPickUpList.push_back(*PickUpIter);

		(*PickUpIter)->Enable(xmSpawnPos);
		m_lpDeadPickUps.erase(PickUpIter);
	}
}

void CEntityManager::KillAllEntities( )
{
	//std::list<CHunterEnemy*> m_lpDeadHunters;
	//std::list<CSaboteurEnemy*> m_lpDeadSaboteurs;
	//std::list<CBullet*> m_lpDeadBullets;

	if (m_lpBulletList.size( ) > 0)
	{
		std::list<CBullet*>::iterator iter = m_lpBulletList.begin( );
		for (iter; iter != m_lpBulletList.end( );)
		{
			for (size_t i = 0; i < 4; i++)
			{
				auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].begin( );
				for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].end( ); Ent++)
				{
					if ((*Ent) == (*iter))
					{
						m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].erase(Ent);
						break;
					}
				}
			}
			(*iter)->Disable( );
			m_lpDeadBullets.push_back((*iter));
			iter = m_lpBulletList.erase(iter);
			//Moves on to the next iteration
			if (iter == m_lpBulletList.end( ))
			{
				break;
			}
		}
	}

	if (m_lpPickUpList.size( ) > 0)
	{
		std::list<CPickUp*>::iterator iter = m_lpPickUpList.begin( );
		for (iter; iter != m_lpPickUpList.end( );)
		{
			for (size_t i = 0; i < 4; i++)
			{
				auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].begin( );
				for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].end( ); Ent++)
				{
					if ((*Ent) == (*iter))
					{
						m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].erase(Ent);
						break;
					}
				}
			}
			(*iter)->Disable( );
			m_lpDeadPickUps.push_back((*iter));
			iter = m_lpPickUpList.erase(iter);
			//Moves on to the next iteration
			if (iter == m_lpPickUpList.end( ))
			{
				break;
			}
		}
	}


	if (m_lpCreepList.size( ) > 0)
	{
		std::list<CCreepEnemy*>::iterator iter = m_lpCreepList.begin( );
		for (iter; iter != m_lpCreepList.end( );)
		{
			for (size_t i = 0; i < 4; i++)
			{
				auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].begin( );
				for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].end( ); Ent++)
				{
					if ((*Ent) == (*iter))
					{
						m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].erase(Ent);
						break;
					}
				}
			}
			(*iter)->Disable( );
			m_lpDeadCreepList.push_back((*iter));
			iter = m_lpCreepList.erase(iter);
			//Moves on to the next iteration
			if (iter == m_lpCreepList.end( ))
			{
				break;
			}
		}
	}

	if (m_lpHunterList.size( ) > 0)
	{
		std::list<CHunterEnemy*>::iterator iter = m_lpHunterList.begin( );
		for (iter; iter != m_lpHunterList.end( );)
		{
			for (size_t i = 0; i < 4; i++)
			{
				auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].begin( );
				for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].end( ); Ent++)
				{
					if ((*Ent) == (*iter))
					{
						m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].erase(Ent);
						break;
					}
				}
			}
			(*iter)->Disable( );
			m_lpDeadHunters.push_back((*iter));
			iter = m_lpHunterList.erase(iter);
			//Moves on to the next iteration
			if (iter == m_lpHunterList.end( ))
			{
				break;
			}
		}
	}

	if (m_lpSaboteurList.size( ) > 0)
	{
		std::list<CSaboteurEnemy*>::iterator iter = m_lpSaboteurList.begin( );
		for (iter; iter != m_lpSaboteurList.end( );)
		{
			for (size_t i = 0; i < 4; i++)
			{
				auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].begin( );
				for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].end( ); Ent++)
				{
					if ((*Ent) == (*iter))
					{
						m_lpaEntityBuckets[(*iter)->GetBucketNumber( )[i]].erase(Ent);
						break;
					}
				}
			}
			(*iter)->Disable( );
			m_lpDeadSaboteurs.push_back((*iter));
			iter = m_lpSaboteurList.erase(iter);
			//Moves on to the next iteration
			if (iter == m_lpSaboteurList.end( ))
			{
				break;
			}
		}
	}

	//if (m_lpSpammerList.size() > 0)
	//{
	//	std::list<CSpammerEnemy*>::iterator iter = m_lpSpammerList.begin();
	//	for (iter; iter != m_lpSpammerList.end();)
	//	{
	//		for (size_t i = 0; i < 4; i++)
	//		{
	//			auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].begin();
	//			for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].end(); Ent++)
	//			{
	//				if ((*Ent) == (*iter))
	//				{
	//					m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].erase(Ent);
	//					break;
	//				}
	//			}
	//		}
	//		(*iter)->Disable();
	//		m_lpDeadSpammers.push_back((*iter));
	//		iter = m_lpSpammerList.erase(iter);
	//		//Moves on to the next iteration
	//		if (iter == m_lpSpammerList.end())
	//		{
	//			break;
	//		}
	//	}
	//}

	if (m_lpEnvrionmentList.size( ) > 0)
	{
		std::list<CEntity*>::iterator iter = m_lpEnvrionmentList.begin( );
		for (iter; iter != m_lpEnvrionmentList.end( ); iter++)
		{
			if (dynamic_cast<CMonsterSpawner*>(*iter))
			{
				dynamic_cast<CMonsterSpawner*>(*iter)->DeactivateMe( );
			}
		}
	}

	/*
	//Gotta clear out the buckets too! <3 Gergar
	for (size_t i = 0; i < NUM_BUCKETS; i++)
	{
	auto iter = m_lpaEntityBuckets[i].begin();
	while (iter != m_lpaEntityBuckets[i].end())
	{
	iter = m_lpaEntityBuckets[i].erase(iter);
	}
	}
	*/
	if (m_pFloorGuide)
	{
		m_pFloorGuide->Reset();
	}
}

bool CEntityManager::HandleInvisiWall(float fDt, TCapsule* pCamToPlayer, CRObject* pWall)
{
	bool bCollides = false;
	//Handle their invisibillity
	DirectX::XMFLOAT4 currColor = pWall->GetColor( );
	if (pWall->GetCollider( ) && pWall->GetCollider( )->type == TCollider::eAABB &&
		CCollision::CapsuleToAABB3D(pCamToPlayer, ((TAABB*)pWall->GetCollider( ))))
	{
		if (currColor.w > INVISIWALL_MIN_ALPHA)
		{
			currColor.w -= fDt * INVISIWALL_LERP_RATE;
			if (currColor.w < INVISIWALL_MIN_ALPHA)
			{
				currColor.w = INVISIWALL_MIN_ALPHA;
			}
		}
		bCollides = true;
	}
	else if (currColor.w < 1)
	{
		currColor.w += fDt * INVISIWALL_LERP_RATE;
		if (currColor.w > 1)
		{
			currColor.w = 1;
		}
	}
	//Apply invisi-wall alpha change
	pWall->SetColor(currColor);

	return bCollides;
}

void CEntityManager::CheckExplosionCollision(TSphere* ExplosionSphere, float DamageAmount)
{
	// Check everything against the passed in sphere
	if (CCollision::SphereToSphere(ExplosionSphere, m_pPlayer->GetCollider( )))
	{
		m_pPlayer->TakeDamage(DamageAmount);
		//std::cout << "BOOM\n";
	}

	if (m_lpCreepList.size( ) > 0)
	{
		std::list<CCreepEnemy*>::iterator iter = m_lpCreepList.begin( );
		for (iter; iter != m_lpCreepList.end( ); iter++)
		{
			if (CCollision::SphereToSphere(ExplosionSphere, (*iter)->GetCollider( )))
			{
				(*iter)->TakeDamage(DamageAmount);
			}
		}
	}

	if (m_lpHunterList.size( ) > 0)
	{
		std::list<CHunterEnemy*>::iterator iter = m_lpHunterList.begin( );
		for (iter; iter != m_lpHunterList.end( ); iter++)
		{
			if (CCollision::SphereToSphere(ExplosionSphere, (*iter)->GetCollider( )))
			{
				(*iter)->TakeDamage(DamageAmount);
			}
		}
	}

	if (m_lpSaboteurList.size( ) > 0)
	{
		std::list<CSaboteurEnemy*>::iterator iter = m_lpSaboteurList.begin( );
		for (iter; iter != m_lpSaboteurList.end( ); iter++)
		{
			if (CCollision::SphereToSphere(ExplosionSphere, (*iter)->GetCollider( )))
			{
				(*iter)->TakeDamage(DamageAmount);
			}
		}
	}

	/*if (m_lpSpammerList.size() > 0)
	{
		std::list<CSpammerEnemy*>::iterator iter = m_lpSpammerList.begin();
		for (iter; iter != m_lpSpammerList.end(); iter++)
		{
			if (CCollision::SphereToSphere(ExplosionSphere, (*iter)->GetCollider()))
			{
				(*iter)->TakeDamage(DamageAmount);
			}
		}
	}*/

	if (m_lpSubSystemList.size( ) > 0)
	{
		std::list<CSubSystem*>::iterator iter = m_lpSubSystemList.begin( );
		for (iter; iter != m_lpSubSystemList.end( ); iter++)
		{
			DirectX::XMFLOAT3 useless = { 0, 0, 0 };
			if (CCollision::SphereToAABB(ExplosionSphere, (*iter)->GetCollider( ), useless))
			{
				(*iter)->TakeDamage(DamageAmount);
			}
		}
	}
}

void CEntityManager::KillAllEnemies()
{
	//std::list<CHunterEnemy*> m_lpDeadHunters;
	//std::list<CSaboteurEnemy*> m_lpDeadSaboteurs;
	//std::list<CBullet*> m_lpDeadBullets;

	if (m_lpCreepList.size() > 0)
	{
		std::list<CCreepEnemy*>::iterator iter = m_lpCreepList.begin();
		for (iter; iter != m_lpCreepList.end();)
		{
			for (size_t i = 0; i < 4; i++)
			{
				auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].begin();
				for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].end(); Ent++)
				{
					if ((*Ent) == (*iter))
					{
						m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].erase(Ent);
						break;
					}
				}
			}
			(*iter)->Disable();
			m_lpDeadCreepList.push_back((*iter));
			iter = m_lpCreepList.erase(iter);
			//Moves on to the next iteration
			if (iter == m_lpCreepList.end())
			{
				break;
			}
		}
	}

	if (m_lpHunterList.size() > 0)
	{
		std::list<CHunterEnemy*>::iterator iter = m_lpHunterList.begin();
		for (iter; iter != m_lpHunterList.end();)
		{
			for (size_t i = 0; i < 4; i++)
			{
				auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].begin();
				for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].end(); Ent++)
				{
					if ((*Ent) == (*iter))
					{
						m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].erase(Ent);
						break;
					}
				}
			}
			(*iter)->Disable();
			m_lpDeadHunters.push_back((*iter));
			iter = m_lpHunterList.erase(iter);
			//Moves on to the next iteration
			if (iter == m_lpHunterList.end())
			{
				break;
			}
		}
	}

	if (m_lpSaboteurList.size() > 0)
	{
		std::list<CSaboteurEnemy*>::iterator iter = m_lpSaboteurList.begin();
		for (iter; iter != m_lpSaboteurList.end();)
		{
			for (size_t i = 0; i < 4; i++)
			{
				auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].begin();
				for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].end(); Ent++)
				{
					if ((*Ent) == (*iter))
					{
						m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].erase(Ent);
						break;
					}
				}
			}
			(*iter)->Disable();
			m_lpDeadSaboteurs.push_back((*iter));
			iter = m_lpSaboteurList.erase(iter);
			//Moves on to the next iteration
			if (iter == m_lpSaboteurList.end())
			{
				break;
			}
		}
	}

	//if (m_lpSpammerList.size() > 0)
	//{
	//	std::list<CSpammerEnemy*>::iterator iter = m_lpSpammerList.begin();
	//	for (iter; iter != m_lpSpammerList.end();)
	//	{
	//		for (size_t i = 0; i < 4; i++)
	//		{
	//			auto& Ent = m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].begin();
	//			for (Ent; Ent != m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].end(); Ent++)
	//			{
	//				if ((*Ent) == (*iter))
	//				{
	//					m_lpaEntityBuckets[(*iter)->GetBucketNumber()[i]].erase(Ent);
	//					break;
	//				}
	//			}
	//		}
	//		(*iter)->Disable();
	//		m_lpDeadSpammers.push_back((*iter));
	//		iter = m_lpSpammerList.erase(iter);
	//		//Moves on to the next iteration
	//		if (iter == m_lpSpammerList.end())
	//		{
	//			break;
	//		}
	//	}
	//}
}

