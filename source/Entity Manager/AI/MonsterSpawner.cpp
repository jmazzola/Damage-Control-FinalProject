#include "stdafx.h"
#include "MonsterSpawner.h"
#include "../EntityManager.h"

CMonsterSpawner::CMonsterSpawner(float cooldown, float basePoints, int pointModifier, bool bisconstant)
{
	m_fCooldown = cooldown;
	m_fBasePoints = basePoints;
	m_fCurrentPoints = basePoints;
	m_nBasePointIncrement = pointModifier;
	m_bIsConstant = bisconstant;
	m_fWaitedTime = 0;
	m_bSpawnerStarted = true;

	m_fMediumChance = 5.0f;
	m_fBigChance = 5.0f;
	
	m_fChanceModifierBigEnemy = 8.5f;
	m_fChanceModifierMediumEnemy = 5.5f;

	m_fMaxMediumEnemyChance = 90.0f;
	m_fMaxBigEnemyChance = 82.0f;

	m_fSmallEnemyCost = 10.0f;
	m_fMediumEnemyCost = 20.0f;
	m_fBigEnemyCost = 37.0f;

	m_bIsActive = false;
	m_bControllerCreated = false;
	m_fSpawnRadius = 5.0f;

	m_bForcedDeactivation = false;

	//if (m_bIsConstant == true)
	//{
	//	//	m_bIsActive = true;
	//	m_fWaitedTime = m_fCooldown / 2 + 5;
	//}
}


CMonsterSpawner::~CMonsterSpawner()
{
}

void CMonsterSpawner::Update(float fDt)
{
	if (m_bSpawnerStarted == false)
		return;

	if (m_bIsActive == false && m_fWaitedTime > m_fCooldown)
	{
		ActivateMe();
	}
	if (m_bIsActive == true  && m_pEntityManager)
	{
		//show particles
		SpawnMonster();
	}
	else
		m_fWaitedTime += fDt;
}


void CMonsterSpawner::SpawnMonster()
{
	//Will spawn something between the min/max spawn count
	
	float fExtraChance = 0.0f;
	bool bSabote = true;
	bool bHunter = true;
	bool bCreep = true;
	//while (m_fCurrentPoints >= m_fMediumEnemyCost)//Will change to m_fSmallEnemyCost
	//{
		float myChance = (float)(rand() % 100) + 1;

		//Calculate the position now (When finished every loop is guarunteed to spawn something)
		DirectX::XMFLOAT3 spawnPos;
		spawnPos.x = GetPosition().x;
		spawnPos.y = GetPosition().y;
		spawnPos.z = GetPosition().z;
		if (0 == m_fSpawnRadius)
		{
			spawnPos.x += (rand() % 2) - m_fSpawnRadius;
			spawnPos.z += (rand() % 2) - m_fSpawnRadius;
		}
		else
		{
			spawnPos.x += (rand() % ((int)m_fSpawnRadius * 2)) - m_fSpawnRadius;
			spawnPos.z += (rand() % ((int)m_fSpawnRadius * 2)) - m_fSpawnRadius;
		}
		eEnemyVariant variant;

		float variantchance = (float)(rand() % 100) + 1;

		//needs tweaking
		if (variantchance < 45)
			variant = eEnemyVariant::eBronze;
		else if (variantchance >= 85)
			variant = eEnemyVariant::eGold;
		else
			variant = eEnemyVariant::eSilver;

		float multiplier;

		if (variant == eEnemyVariant::eGold)
			multiplier = 1.4f;
		if (variant == eEnemyVariant::eSilver)
			multiplier = 1.2f;
		else
			multiplier = 1.0f;

		if (bSabote == true && m_fCurrentPoints >= m_fBigEnemyCost *multiplier && m_fBigChance > 0 && myChance <= m_fBigChance + fExtraChance && m_bIsConstant == false)
		{
			variant = eEnemyVariant::eBronze;
			if (m_pEntityManager->CycleIntoLiveList(CEntity::eSABOTEUR, spawnPos, nullptr, variant) == false)
				bSabote = false;
			else
			{
					m_fCurrentPoints -= m_fBigEnemyCost;
				//m_fBigChance -= m_fChanceModifierBigEnemy;
				//if (m_fBigChance < 0.0f)
				//	m_fBigChance = 0.0f;
			}
			//Defcon
			//if (defCon != null)
			//increase defcon cost using this enemy's cost
		}
		else if (bHunter == true && m_fCurrentPoints >= m_fMediumEnemyCost*multiplier && m_fMediumChance > 0 && myChance <= m_fMediumChance + fExtraChance && m_bIsConstant == false)
		{
			if (m_pEntityManager->CycleIntoLiveList(CEntity::eHUNTER, spawnPos, nullptr, variant) == false)
				bHunter = false;
			else
			{
					m_fCurrentPoints -= m_fMediumEnemyCost;
				//increase chance etc
			}

			//Defcon
			//if (defCon != null)
			//increase defcon cost using this enemy's cost
		}
		/*  WE DON'T HAVE CREEPS YET*/
		else if (bCreep == true)//spawn creep
		{
		//flocking controller - Pack

		//if (m_bControllerCreated == false || m_nCurrentPack == m_nMaxPackNumber)
		//{
		//
		//	//if (m_nCurrentPack == m_nMaxPackNumber)
		//		//select room to go
		//	m_nCurrentPack = 0;
		//	vec3f spawnPose = GetPosition();
		//	//myController =
		//	//m_bControllerCreated = true;
		//}
			if (variant == eEnemyVariant::eSilver)
				variant = eEnemyVariant::eBronze;

			if (m_pEntityManager->CycleIntoLiveList(CEntity::eCREEP, spawnPos, nullptr, variant) == false)
				bCreep = false;
			else
			{

				
					m_fCurrentPoints -= m_fSmallEnemyCost;

				//increase chance etc
			}
		//create monster
		//add to flocking controller
		//set the small's controller to this pack's controller
		//m_nCurrentPack++;

		//Defcon
		//if (defCon != null)
		//increase defcon cost using this enemy's cost
		}

		//fExtraChance += 1.0f;
	//}
	//if (myController != null)
	//	generate random point to go

	//m_bControllerCreated = false;
	//m_nCurrentPack = 0;
		if (m_fCurrentPoints < m_fSmallEnemyCost) 
		{
			m_bIsActive = false;
			m_fWaitedTime = 0.0f;
			m_bForcedDeactivation = false;
		}

		if (bHunter == false && bSabote == false && bCreep == false) 
		//checks if the spawner had to deactivate because there weren't enough enemies in the deadlist
		{
			m_bForcedDeactivation = true;
		}

	//particleSystem.enableEmission = false;
}

void CMonsterSpawner::ActivateMe()
{
	m_bIsActive = true;
	//float distance = Engine*->GetProgress();

	

	if (m_bForcedDeactivation == false && m_bIsConstant == false) //&& Engine*->GetIsDestroyed() == false)
	{
		if (0 == m_nBasePointIncrement)
		{
			m_fCurrentPoints += m_fBasePoints + 10;
		}
		else
		{
			m_fCurrentPoints += m_fBasePoints + rand() % m_nBasePointIncrement;
		}

		
		//float initChance = 20;//distance / Engine*->GetGoalDistance();

		m_fMediumChance += m_fChanceModifierMediumEnemy;
		m_fBigChance += m_fChanceModifierBigEnemy;

		if (m_fMediumChance > m_fMaxMediumEnemyChance)
			m_fMediumChance = m_fMaxMediumEnemyChance;

		if (m_fBigChance > m_fMaxBigEnemyChance)
			m_fBigChance = m_fMaxBigEnemyChance;
	}
	//particleSystem.enableEmission = true;
	m_bFirstTime = false;
}

void CMonsterSpawner::Reset()
{
	m_fCurrentPoints = m_fBasePoints;
	m_fWaitedTime = 0;
	m_fMediumChance = 5.0f;
	m_fBigChance = 5.0f;
	m_bIsActive = false;
	m_bControllerCreated = false;

	m_bForcedDeactivation = false;

	//if (m_bIsConstant == true)
	//{
	//	m_bIsActive = true;
		//m_fWaitedTime = m_fCooldown/2 + 5;
	//}

}

