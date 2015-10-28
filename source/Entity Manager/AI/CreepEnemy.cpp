/***********************************************
* Filename:			CreepEnemy.cpp
* Date:      		06/02/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:			handles the small enemy. Fast but weak.
************************************************/

#include "stdafx.h"
#include "CreepEnemy.h"
#include "..\EntityManager.h"
#include "..\..\State Machine\GameplayState.h"
#include "..\..\Audio Manager\AudioSystemWwise.h"

#include "../../Renderer/Renderer.h"
#include "../../Renderer/DebugRenderer.h"

#include "../../Particle Manager/ParticleSystem.h"
#include "../../Particle Manager/ParticleEmitter.h"
#include "../../Particle Manager/ParticleManager.h"

#include "../../Animation Manager/Animation State Machine/States/MoveAnimState.h"

#include "../../TinyXML2/tinyxml2.h"
#include "../StatusEffects.h"
using namespace MathHelper;

CCreepEnemy::CCreepEnemy()
{
	m_nEntityType = CEntity::eCREEP;

	//Check if the stats are loaded
	if (GetPresetHealth(eEnemyVariant::eBronze) == 0)
	{
		if (!LoadStats())
		{
			DebugPrint("[CREEP CTOR] Failed to load base stats from CreepStats.xml\n", ConsoleColor::Red);
		}
	}
	//m_fCurrentHealth = m_fMaxHealth = CREEP_BRONZE_HEALTH;
	//m_fDamage = CREEP_BRONZE_DAMAGE;
	//m_fScrapAmount = CREEP_BRONZE_SCRAP;

	m_bIsAlive = false;

	//m_nLevel = 1;
	m_eState = eAIStateType::eObjective;
	m_fAggroRange = 950.0f;
	//m_fAttackElapsedTime = m_fAttackFrequency = CREEP_ATTACK_FREQUENCY;
	//m_fAttackRange = 120.0f;
	//m_fMovementSpeed = 400.0f;

	//TWEAK
	m_eMyType = eEnemyType::ENEM_CREE;
	//m_fMass = 5.0f;
	//m_fTurningSpeed = 25.0f;

	m_pPlayer = (CEntity*)CGameplayState::GetEntityManager()->GetPlayer();

	m_tCollider = new TSphere();
	m_tCollider->type = TCollider::eSphere;
	//m_tCollider->fRadius = 30.0f;

	//m_eVariant = eEnemyVariant::eBronze;
	m_fBlinkTimer = 0.0f;
	m_bBombActivated = false;
	//m_fBombTimer = 2.0f;

	ApplyVariant(eEnemyVariant::eNoVariant);

	m_pExplodeEffect = CParticleSystem::LoadFrom("Assets/XML/Particles/AlienExplodeParticle.xml");
	CParticleManager::GetInstance()->AddParticleSystem(m_pExplodeEffect);

	m_pDeathEffect = CParticleSystem::LoadFrom("Assets/XML/Particles/CreepDeath.xml");
	CParticleManager::GetInstance()->AddParticleSystem(m_pDeathEffect);

	m_pLightParams = new LightParams({ 0.0f, 0.0f, 0.0f }, { 3, 1, 1 }, 150);

	m_xOffset.x = 0.0f;
	m_xOffset.y = 100.0f;
	m_xOffset.z = 0.0f;
}

CCreepEnemy::~CCreepEnemy()
{
	SafeDelete(m_tCollider);

	SafeDelete(m_pLightParams);
}

/*virtual*/ void	CCreepEnemy::EvaluateState(void) /*override*/
{
	if (m_bBombActivated == true)
		return;

	float distanceToPlayer = Length(m_pPlayer->GetPosition() - GetPosition());


	if (distanceToPlayer <= m_fAggroRange) //check the agrorange first
	{
		m_vDestination = m_pPlayer->GetPosition();
		m_eState = eAIStateType::eAggro;
		m_pTarget = m_pPlayer;
	}
	else
	{
		switch (m_eState)
		{
			case (eAIStateType::eObjective) :
			{
				if (CheckDestination())
				{
					GenerateNextRandomPoint();
				}
				break;
			}
											//case (eAIStateType::ePatrol) :
											//{
											//
											//	break;
											//}
			case (eAIStateType::eAggro) :
			{
				m_eState = eAIStateType::eObjective;
				m_pTarget = nullptr;
				GenerateNextRandomPoint();
				break;
			}
		}
	}
	CPathSearch::GetInstance()->PointInTriangle(m_vDestination, m_nGoalTriangle);
}

/*virtual*/ void	CCreepEnemy::Update(float fDt) /*override*/
{
	if (m_fCurrentHealth < 0)
	{
		if (true == m_bBombActivated)
		{
			ExplosionManagement(fDt);
		}
		else
		{
			XMFLOAT3 pos = GetPosition();
			m_pDeathEffect->SetPosition(pos);
			m_pDeathEffect->GetEmitter(0)->Emit();
			
			Disable();
			return;
		}
	}

	if (m_eVariant == eEnemyVariant::eGold)
	{
		m_fBlinkTimer += fDt;
		//blink
		if (m_fBlinkTimer < 0.4f)
			m_XMColor = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		else if (m_fBlinkTimer > 0.8f)
			m_fBlinkTimer = 0.0f;
		else
			m_XMColor = DirectX::XMFLOAT4(1.0f, 0.0f, 0.2f, 1.0f);
	}

	if (m_fAttackElapsedTime < m_fAttackFrequency)
		m_fAttackElapsedTime += fDt;

	DirectX::XMFLOAT3 position = GetPosition();
	(m_tCollider)->vCenter = DirectX::XMFLOAT3(position.x, position.y, position.z);

	m_pLightParams->Position = GetPosition();
	m_pLightParams->Position.y += 115;

	EvaluateState();
	switch (m_eState)
	{
		case (eAIStateType::eObjective) :
		{
			UpdateObjective(fDt);
			break;
		}
										/*case (eAIStateType::ePatrol) :
										{
										UpdatePatrol(fDt);
										break;
										}*/
		case (eAIStateType::eAggro) :
		{
			UpdateAggro(fDt);
			break;
		}
	}

	CBaseEnemy::Update(fDt);

	// Animate
	if (m_pAnimation)
	{
		GetAnimStateMachine().Update(fDt);
		GetAnimationBlender().Process(fDt);
		m_cDeformer.Process(GetAnimationBlender().GetPose());

		if (m_pAnimation->m_pModel->m_pVertexBuffer == NULL)
		{
			CGameplayState::GetAssetManager()->CreateBuffer(CRenderer::GetReference().GetDevice(), m_cDeformer.GetSkinVerts(),
															D3D11_BIND_VERTEX_BUFFER,
															&m_pAnimation->m_pModel->m_pVertexBuffer);
		}
	}

	//Play idle sound, keep them out of sync
	if (m_fIdleDelay == 0 && m_fCurrentHealth > 0)
	{
		//AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3DL_CREEP_IDLE, this);
		m_fIdleDelay = -1;
	}

	//Play scittering sound
	if (m_fCurrentHealth > 0 && SquareLength(m_vVelocity) > 0)
	{
		if (!m_bMoving)
		{
			m_bMoving = true;
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3DL_CREEP_MOVE, this);
		}
	}
	else
	{
		m_bMoving = false;
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_3DL_CREEP_MOVE, this);
	}
}

bool CCreepEnemy::RegisterAnimation(void)
{
	GetAnimStateMachine().SetEntity(this);
	GetAnimStateMachine().ChangeState(CMoveAnimState::GetInstance());

	m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(m_szTag);
	if (m_pAnimation)
	{
		m_cDeformer.SetMesh(&m_pAnimation->m_pModel->m_cMesh);

		if (m_pAnimation->m_pModel->m_pVertexBuffer == NULL)
		{
			CGameplayState::GetAssetManager()->CreateBuffer(CRenderer::GetReference().GetDevice(), m_cDeformer.GetSkinVerts(),
															D3D11_BIND_VERTEX_BUFFER,
															&m_pAnimation->m_pModel->m_pVertexBuffer);
		}
		return true;
	}
	return false;
}

/*virtual*/ void CCreepEnemy::UpdateObjective(float fDt) /*override*/
{
	if (CheckDestination() == false)
	{
		ManageMovement(fDt);
	}
}

/*virtual*/ void CCreepEnemy::UpdatePatrol(float fDt) /*override*/
{

}

/*virtual*/ void	CCreepEnemy::UpdateAggro(float fDt) /*override*/
{
	if (m_pTarget != nullptr)
	{
		//m_fPathCurrentTimer += fDt;
		ManageAggroMovement(fDt);
	}
}

bool CCreepEnemy::CheckDestination(void)
{
	float distance = Length(m_vDestination - GetPosition());
	if (distance < m_fAttackRange)
		return true;

	return false;
}

void CCreepEnemy::Enable(DirectX::XMFLOAT3 SpawnPoint, eEnemyVariant variant)
{
	ApplyVariant(variant);

	MyWorld.Local._41 = SpawnPoint.x;
	MyWorld.Local._42 = SpawnPoint.y;
	MyWorld.Local._43 = SpawnPoint.z;

	(m_tCollider)->fRadius = 30.0f;
	(m_tCollider)->vCenter = DirectX::XMFLOAT3(SpawnPoint.x, SpawnPoint.y, SpawnPoint.z);

	m_eState = eAIStateType::eObjective;

	m_bIsAlive = true;
	GenerateNextRandomPoint();

	//Play the idle sound
	//AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3DL_CREEP_IDLE, this);

	//m_eVariant = variant;
	m_fBlinkTimer = 0.0f;
	m_bBombActivated = false;
	//m_fBombTimer = 2.0f;
	m_fExplosionDamageTimer = 0.0f;
	m_bIsExplosionColliding = false;
	m_bMoving = false;

	m_fScaling = 1.2f;
}
#include "../../AchieveTracker.h"
void CCreepEnemy::Disable()
{
	CAchieveTracker::GetInstance()->UnlockAchievement(CAchieveTracker::eKiller);
	CGameplayState::GetEntityManager()->GetPlayer()->GetGameStats()->m_nTotalEnemiesKilled++;

	m_bIsAlive = false;
	m_bMoving = false;
	if (m_pStatusEffects)
	{
		m_pStatusEffects->Dispel();
	}
	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_3D_CREEP, this);
	//ApplyVariant(eNoVariant);
}

void CCreepEnemy::ManageMovement(float fDt)
{
	if (CheckDestination() == false)
	{
		MakeMovement(fDt);
	}
	else
	{
		GenerateNextRandomPoint();
	}
}

void CCreepEnemy::ManageAggroMovement(float fDt)
{
	float distancetoTarget = Length(m_vDestination - GetPosition());

	if (!m_fAttackElapsedTime || distancetoTarget > m_fAttackRange)
	{
		//DebugPrint("[CREEP] I AM NOT ATTACKING!\n", ConsoleColor::Red);
		BitTwiddler::TurnOffBit(actionBitfield, IS_ATTACKING);
	}

	if (m_pTarget != nullptr)
	{
		//MakeMovement(fDt, m_pTarget->GetPosition());

		switch (m_eVariant)
		{
			case(eEnemyVariant::eGold) :
			{
				ExplosionManagement(fDt);
				break;
			}
									   //gold
			default: //case(eEnemyVariant::eBronze) :
			{
				if (distancetoTarget <= m_fAttackRange)
				{
					//turn to face the player
					if (m_fAttackElapsedTime >= m_fAttackFrequency)
					{
						DirectX::XMFLOAT3 myVect = m_vDestination - GetPosition();
						TurnToPoint(fDt, myVect, m_fTurningSpeed);
						CBaseEnemy::Attack();
						//DebugPrint("[CREEP] I AM ATTACKING!\n", ConsoleColor::Red);
						BitTwiddler::TurnOnBit(actionBitfield, IS_ATTACKING);
						m_fAttackElapsedTime = 0;
					}
				}
				else if (distancetoTarget > m_fAttackRange)
				{
					MakeMovement(fDt);
				}
				break;
			}
		}
	}
}

void CCreepEnemy::GenerateNextRandomPoint(void)
{
	m_vDestination = CPathSearch::GetInstance()->RandomPoint();
}

void CCreepEnemy::TakeDamage(float fDamage)
{
	CGameplayState::GetEntityManager()->GetPlayer()->GetGameStats()->m_nTotalDamageGiven += (int)fDamage;

	CBaseEnemy::TakeDamage(fDamage);
	//Player Enemy-specific sounds
	//He's dead
	if (m_fCurrentHealth <= 0.0f)
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_3D_CREEP, this);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_CREEP_DEATH, GetPosition());
	}
	//Just take damage
	else
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_CREEP_HIT, this);
	}
}

void CCreepEnemy::Attack(float fDamage)
{
	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_CREEP_ATTACK, this);
	DealDamage(fDamage);
}

void CCreepEnemy::ExplosionManagement(float fDt)
{
	float distancetoTarget = Length(m_vDestination - GetPosition());
	if (distancetoTarget <= m_fAttackRange + 20.0f)
	{
		m_bBombActivated = true;
	}
	else if (m_bBombActivated == false)
	{
		MakeMovement(fDt);
	}

	if (m_bBombActivated == true)
	{
		CDebugRenderer::GetReference().CircleRenderer(DirectX::XMFLOAT3{ GetPosition().x, 6.0f, GetPosition().z }, { 200, 200 }, DirectX::XMFLOAT4(1, 0, 0, 1));
		m_fBombTimer -= fDt;

		if (m_fBombTimer < 0 && m_bBombExploded == false)
		{
			m_bBombExploded = true;
			m_fBombTimer = s_fPresetFuseTime;
			(m_tCollider)->fRadius = s_fPresetExplodeRadius;
			// TODO: Call the fancy ass new function in entitymanager
			CGameplayState::GetEntityManager()->CheckExplosionCollision(m_tCollider, CREEP_KAMIKAZE_DAMAGE);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_CREEP_KAMIKAZE, GetPosition());
			return;
			//restart to deal damage for 0.4 seconds
		}
		else
		{
			//m_fBombTimer -= fDt;
			if (m_fBombTimer < 0.0f)
			{

				Disable();

				XMFLOAT3 pos = GetPosition();
				m_pExplodeEffect->SetPosition(pos);
				m_pExplodeEffect->GetEmitter(0)->Emit();
			}
		}
	}
}

void CCreepEnemy::ApplyVariant(eEnemyVariant eTier)
{
	//Color and m_feVariant
	CBaseEnemy::ApplyVariant(eTier);

	//Zero out everything if invalid tier
	if (eTier == eNoVariant)
	{
		m_fCurrentHealth = m_fMaxHealth = 0;
		//m_fArmor = 0;
		m_fDamage = 0;
		m_fAttackFrequency = 0;
		m_fAttackRange = 0;
		m_fMovementSpeed = 0;
		m_fTurningSpeed = 0;
		m_nScrapAmount = 0;
		m_tCollider->fRadius = 0;
		m_fMass = 0;

		m_fBombTimer = 0;
	}
	//Fill out proper information
	else
	{
		m_fCurrentHealth = m_fMaxHealth = s_fPresetHealth[eTier];
		//m_fArmor = s_fPresetArmor;
		m_fDamage = s_fPresetDamage[eTier];
		m_fAttackFrequency = s_fPresetAttackSpeed[eTier];
		m_fAttackRange = s_fPresetAttackRange[eTier];
		m_fMovementSpeed = s_fPresetMoveSpeed[eTier];
		m_fTurningSpeed = s_fPresetTurnSpeed[eTier];
		m_nScrapAmount = s_nPresetScrap[eTier];
		m_tCollider->fRadius = s_fPresetColliderRadius[eTier];
		//Preserve the position
		DirectX::XMFLOAT3 pos = GetPosition();
		DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixScaling(s_fPresetScale[eTier], s_fPresetScale[eTier], s_fPresetScale[eTier]));
		SetPosition(pos);
		m_fMass = s_fPresetMass[eTier];

		m_fBombTimer = s_fPresetFuseTime;
	}
}

bool CCreepEnemy::LoadStats(void)
{
	tinyxml2::XMLDocument xmlDoc;

	//TODO: Load stats based on difficulty
	xmlDoc.LoadFile("Assets//XML//Balance//CreepStats.xml");

	//Did not open file properly
	if (xmlDoc.Error())
	{
		xmlDoc.Clear();
		return false;
	}
	tinyxml2::XMLElement* xmlHead = xmlDoc.RootElement();
	tinyxml2::XMLElement* xmlTier = xmlHead->FirstChildElement("Bronze");
	tinyxml2::XMLElement* xmlElement = nullptr;
	//Load in the actual stats
	for (size_t i = 0; i < eEnemyVariant::eVariantCount; i++)
	{
		xmlElement = xmlTier->FirstChildElement("Defense");
		s_fPresetHealth[i] = (float)atof(xmlElement->Attribute("Health"));
		s_fPresetArmor[i] = (float)atof(xmlElement->Attribute("Armor"));

		xmlElement = xmlTier->FirstChildElement("Offense");
		s_fPresetDamage[i] = (float)atof(xmlElement->Attribute("Damage"));
		s_fPresetAttackSpeed[i] = (float)atof(xmlElement->Attribute("AttackSpeed"));
		s_fPresetAttackRange[i] = (float)atof(xmlElement->Attribute("AttackRange"));

		xmlElement = xmlTier->FirstChildElement("Movement");
		s_fPresetMoveSpeed[i] = (float)atof(xmlElement->Attribute("MoveSpeed"));
		s_fPresetTurnSpeed[i] = (float)atof(xmlElement->Attribute("TurnSpeed"));

		xmlElement = xmlTier->FirstChildElement("Misc");
		s_nPresetScrap[i] = (int)atof(xmlElement->Attribute("Scrap"));
		s_fPresetColliderRadius[i] = (float)atof(xmlElement->Attribute("ColliderRadius"));
		s_fPresetScale[i] = (float)atof(xmlElement->Attribute("Scale"));
		s_fPresetMass[i] = (float)atof(xmlElement->Attribute("Mass"));

		xmlTier = xmlTier->NextSiblingElement();
	}

	//The Special
	xmlElement = xmlTier->FirstChildElement("Explosion");
	s_fPresetExplodeRadius = (float)atof(xmlElement->Attribute("Radius"));
	s_fPresetExplodeDamage = (float)atof(xmlElement->Attribute("Damage"));
	s_fPresetFuseTime = (float)atof(xmlElement->Attribute("Fuse"));

	//We're done here
	xmlDoc.Clear();
	return true;
}

//Declare the statics
float CCreepEnemy::s_fPresetHealth[eEnemyVariant::eVariantCount];
float CCreepEnemy::s_fPresetArmor[eEnemyVariant::eVariantCount];
float CCreepEnemy::s_fPresetDamage[eEnemyVariant::eVariantCount];
float CCreepEnemy::s_fPresetAttackSpeed[eEnemyVariant::eVariantCount];
float CCreepEnemy::s_fPresetAttackRange[eEnemyVariant::eVariantCount];
float CCreepEnemy::s_fPresetMoveSpeed[eEnemyVariant::eVariantCount];
float CCreepEnemy::s_fPresetTurnSpeed[eEnemyVariant::eVariantCount];
int CCreepEnemy::s_nPresetScrap[eEnemyVariant::eVariantCount];
float CCreepEnemy::s_fPresetColliderRadius[eEnemyVariant::eVariantCount];
float CCreepEnemy::s_fPresetScale[eEnemyVariant::eVariantCount];
float CCreepEnemy::s_fPresetMass[eEnemyVariant::eVariantCount];

float CCreepEnemy::s_fPresetExplodeRadius;
float CCreepEnemy::s_fPresetExplodeDamage;
float CCreepEnemy::s_fPresetFuseTime;
