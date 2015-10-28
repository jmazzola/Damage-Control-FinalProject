/***********************************************
* Filename:			SaboteurEnemy.cpp
* Date:      		05/12/2015
* Mod. Date:		05/20/2015
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:			Enemy who goes after the Subsystems and only
targets the player if is inside his aggro range
************************************************/

#include "SaboteurEnemy.h"
#include "..\EntityManager.h"
#include "..\..\State Machine\GameplayState.h"
#include "..\Entities\Subsystems\Subsystem.h"
#include "..\..\Audio Manager\AudioSystemWwise.h"
#include "../../Weapon/FastSMG.h"
#include "../../Renderer/DebugRenderer.h"
#include "../../Renderer/Renderer.h"

#include "..\..\Animation Manager\Animation State Machine\States\MoveAnimState.h"

#include "../../TinyXML2/tinyxml2.h"
#include "../StatusEffects.h"

#include "../../Particle Manager/ParticleSystem.h"
#include "../../Particle Manager/ParticleEmitter.h"
#include "../../Particle Manager/ParticleManager.h"
using namespace MathHelper;

#define STEP_DELAY 0.35f

CSaboteurEnemy::CSaboteurEnemy()
{
	m_nEntityType = CEntity::eSABOTEUR;

	//Check if the stats are loaded
	if (GetPresetHealth(eEnemyVariant::eBronze) == 0)
	{
		if (!LoadStats())
		{
			DebugPrint("[SABOTEUR CTOR] Failed to load base stats from SaboteurStats.xml\n", ConsoleColor::Red);
		}
	}
	//m_fCurrentHealth = m_fMaxHealth = SABO_BRONZE_HEALTH;
	//m_fBaseDamage = m_fDamage = SABO_BRONZE_DAMAGE;
	//m_nScrapAmount = SABO_BRONZE_SCRAP;

	m_bIsAlive = false;

	//m_nLevel = 1;
	m_eState = eAIStateType::eObjective;
	m_fAggroRange = 250.0f;
	//m_fAttackElapsedTime = m_fAttackFrequency = SABO_ATTACK_FREQUENCY;
	//m_fAttackRange = 300.0f;
	//m_fMovementSpeed = 200.0f;
	//m_fShootRange = 900.0f;

	//TWEAK
	m_eMyType = eEnemyType::ENEM_SABO;
	//m_fMass = 100.0f;
	//m_fTurningSpeed = 2.0f;

	m_pPlayer = (CEntity*)CGameplayState::GetEntityManager()->GetPlayer();
	m_lpSubSystemList = CGameplayState::GetEntityManager()->GetSubSystems();

	m_tCollider = new TSphere();
	m_tCollider->type = TCollider::eSphere;
	//m_tCollider->fRadius = 60.0f;

	m_szTag = "Saboteur";

	m_nCurrentSystem = (int)(rand() % 3);
	int i = -1;
	for (auto listCheck = m_lpSubSystemList.begin(); i != m_nCurrentSystem; ++listCheck)
	{
		m_pCurrentSystem = (*listCheck);
		m_pTarget = (CEntity*)(*listCheck);
		i++;
	}

	ApplyVariant(eNoVariant);
	//m_eVariant = eEnemyVariant::eBronze;
	m_fExplosionDamageTimer = 0.0f;
	m_bIsExplosionColliding = false;

	m_fSubsystemAttackTimer = 2.5f;
	m_fSubsystemElapsed = 0.0f;
	//m_pSMG = new CFastSMG();

	m_fAttackSoundTimer = 1.0f;
	m_fCurrentASoundTime = m_fAttackSoundTimer + 1.0f;

	m_pLightParams = new LightParams({ 0.0f, 0.0f, 0.0f }, { 1, 1, 3 }, 100);

	//m_pAttackEffect = new CRObject;
	//m_pAttackEffect->SetTag("Saboteur Attack Effect");
	//m_pAttackEffect->SetColor(DirectX::XMFLOAT4{ 1, 1, 1, 0.99f });

	m_xOffset.x = 0.0f;
	m_xOffset.y = 275.0f;
	m_xOffset.z = 0.0f;

	m_pExplodeEffect = CParticleSystem::LoadFrom("Assets/XML/Particles/SaboteurExplodeParticle.xml");
	CParticleManager::GetInstance()->AddParticleSystem(m_pExplodeEffect);
	m_pAttackEffect = CParticleSystem::LoadFrom("Assets/XML/Particles/SaboteurAttackPosion.xml");
	CParticleManager::GetInstance()->AddParticleSystem(m_pAttackEffect);

}

CSaboteurEnemy::~CSaboteurEnemy()
{
	SafeDelete(tCollider);
	SafeDelete(m_tCollider);
	//SafeDelete(m_pSMG);
	SafeDelete(m_pLightParams);

	m_lpSubSystemList.clear();

}

/*virtual*/ void	CSaboteurEnemy::EvaluateState(void) /*override*/
{
	float distanceToPlayer = Length(m_pPlayer->GetPosition() - GetPosition());

	bool allDestroyed = true;
	for (auto listCheck = m_lpSubSystemList.begin(); listCheck != m_lpSubSystemList.end(); ++listCheck)
	{
		if ((*listCheck)->GetIsDestroyed() == false)
		{
			allDestroyed = false;
			break;
		}
	}

	if (distanceToPlayer <= m_fAggroRange /*&& m_eVariant != eEnemyVariant::eGold*/) //check the agrorange first
	{
		m_eState = eAIStateType::eAggro;
		m_pTarget = m_pPlayer;
	}
	else
	{
		switch (m_eState)
		{
			case (eAIStateType::eObjective) :
			{

				//if all subsystems ARE destroyed change to patrol

				if (allDestroyed == true)
				{
					FindNextTarget();
					m_eState = eAIStateType::ePatrol;
					m_fAggroRange = m_fAttackRange * 3; //increase
				}
				break;
			}
			case (eAIStateType::ePatrol) :
			{
				//attack
				bool allDestroyed = true;
				for (auto listCheck = m_lpSubSystemList.begin(); listCheck != m_lpSubSystemList.end(); ++listCheck)
				{
					if ((*listCheck)->GetIsDestroyed() == false)
					{
						allDestroyed = false;
						break;
					}
				}
				//if any subsystem is back
				if (allDestroyed == false)
				{
					m_eState = eAIStateType::eObjective;
					FindClosestsTarget();
					m_fAggroRange = m_fAttackRange; //decrease
					//CallPathFinder(m_pTarget->GetPosition());

				}
				break;
			}
			case (eAIStateType::eAggro) :
			{
				m_eState = eAIStateType::eObjective;
				m_pTarget = (CEntity*)m_pCurrentSystem;
				//CallPathFinder(m_pTarget->GetPosition());
				break;
			}
		}
	}

	m_vDestination = m_pTarget->GetPosition();
	CPathSearch::GetInstance()->PointInTriangle(m_vDestination, m_nGoalTriangle);
}

bool CSaboteurEnemy::RegisterAnimation(void)
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

/*virtual*/ void	CSaboteurEnemy::Update(float fDt) /*override*/
{
	if (m_fCurrentHealth < 0)
	{
		Disable();
		return;
	}

	/*XMStoreFloat4x4(&m_pAttackEffect->GetLocalMatrix(),
					(DirectX::XMMatrixScaling(m_fAttackRange * .5f, 1, m_fAttackRange * .5f) * DirectX::XMMatrixRotationY(fDt) * DirectX::XMMatrixIdentity()) *
					XMLoadFloat4x4(&MyWorld.Local));*/

	m_fCurrentASoundTime += fDt;

	if (m_fAttackElapsedTime < m_fAttackFrequency)
		m_fAttackElapsedTime += fDt;

	DirectX::XMFLOAT3 position = GetPosition();
	(m_tCollider)->vCenter = DirectX::XMFLOAT3(position.x, position.y, position.z);

	m_pLightParams->Position = GetPosition();
	m_pLightParams->Position.y += 50;

	EvaluateState();
	switch (m_eState)
	{
		case (eAIStateType::eObjective) :
		{
			UpdateObjective(fDt);
			break;
		}
		case (eAIStateType::ePatrol) :
		{
			UpdatePatrol(fDt);
			break;
		}
		case (eAIStateType::eAggro) :
		{
			UpdateAggro(fDt);
			break;
		}
	}

	/*if (m_eVariant == eEnemyVariant::eGold)
	{
		m_pSMG->UpdateEnemyW(fDt, this);
	}*/
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
	if (m_fCurrentHealth > 0)
	{
		//Play idle sound, keep them out of sync
		if (m_fIdleDelay == 0)
		{
			//AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3DL_SABO_IDLE, this);
			m_fIdleDelay = -1;
		}
		//Play steps
		float fSpeed = MathHelper::Length(m_vVelocity);
		//Count down to the next step
		if (m_fStepTimer > 0)
		{
			m_fStepTimer -= fSpeed * fDt * 0.001f;
			if (m_fStepTimer < 0)
			{
				m_fStepTimer = 0;
			}
		}
		//Play step sounds
		else if (fSpeed > 0)
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_SABO_STEP, this);
			m_fStepTimer = STEP_DELAY;
		}
	}
}

/*virtual*/ void	CSaboteurEnemy::UpdateObjective(float fDt) /*override*/
{
	if (m_pCurrentSystem != nullptr && m_pTarget != nullptr)
	{
		//check if target is alive
		if (m_pCurrentSystem->GetIsDestroyed() == true)
			FindClosestsTarget();

		if (m_pCurrentSystem != nullptr && m_pTarget != nullptr)
		{
			//m_fPathCurrentTimer += fDt;
			ManageMovement(fDt);
		}
	}
}

/*virtual*/ void	CSaboteurEnemy::UpdatePatrol(float fDt) /*override*/
{
	if (CheckDestination())// check if we reached the destination
		//find next destination
		FindNextTarget();

	if (m_pTarget != nullptr)
	{
		//m_fPathCurrentTimer += fDt;
		ManageMovement(fDt);
	}
}

/*virtual*/ void	CSaboteurEnemy::UpdateAggro(float fDt) /*override*/
{
	if (m_pTarget != nullptr)
	{
		//m_fPathCurrentTimer += fDt;
		ManageMovement(fDt);
	}
}

void CSaboteurEnemy::FindClosestsTarget(void)
{
	float closests = FLT_MAX;
	m_pCurrentSystem = nullptr;

	int index = -1;

	for (auto listCheck = m_lpSubSystemList.begin(); listCheck != m_lpSubSystemList.end(); ++listCheck)
	{
		float distancetoTarget = Length((*listCheck)->GetPosition() - GetPosition());

		index++;
		if ((*listCheck)->GetIsDestroyed() == false && distancetoTarget < closests)
		{

			m_pCurrentSystem = (*listCheck);
			closests = distancetoTarget;
		}
	}

	//this sets the alive target
	if (m_pCurrentSystem != nullptr)
	{
		m_pTarget = (CEntity*)m_pCurrentSystem;
		m_nCurrentSystem = index;
	}
	else
	{
		//sets the nex dead target
		FindNextTarget();
		m_eState = eAIStateType::ePatrol;
	}
	m_vDestination = m_pTarget->GetPosition();
}

void CSaboteurEnemy::FindNextTarget(void)
{
	m_nCurrentSystem++;
	if (m_nCurrentSystem > 2)
		m_nCurrentSystem = 0;

	auto listCheck = m_lpSubSystemList.begin();
	for (int index = 0; index != m_nCurrentSystem; index++)
		listCheck++;

	m_pCurrentSystem = (*listCheck);
	m_pTarget = (CEntity*)m_pCurrentSystem;
	m_vDestination = m_pTarget->GetPosition();
}

bool CSaboteurEnemy::CheckDestination(void)
{
	float distance = Length(m_vDestination - GetPosition());
	if (distance < m_fAttackRange)
		return true;
	return false;
}

void CSaboteurEnemy::Enable(DirectX::XMFLOAT3 SpawnPoint, eEnemyVariant variant)
{
	ApplyVariant(variant);

	MyWorld.Local._41 = SpawnPoint.x;
	MyWorld.Local._42 = SpawnPoint.y;
	MyWorld.Local._43 = SpawnPoint.z;

	(m_tCollider)->vCenter = DirectX::XMFLOAT3(SpawnPoint.x, SpawnPoint.y, SpawnPoint.z);

	m_eState = eAIStateType::eObjective;

	m_bIsAlive = true;
	FindNextTarget();

	//m_eVariant = variant;
	m_fExplosionDamageTimer = 0.0f;
	m_bIsExplosionColliding = false;
	m_fSubsystemAttackTimer = 2.5f;
	m_fSubsystemElapsed = 0.0f;
	m_fCurrentASoundTime = m_fAttackSoundTimer + 1.0f;

}

#include "../../AchieveTracker.h"
void CSaboteurEnemy::Disable()
{
	CAchieveTracker::GetInstance()->UnlockAchievement(CAchieveTracker::eKiller);
	CGameplayState::GetEntityManager()->GetPlayer()->GetGameStats()->m_nTotalEnemiesKilled++;

	m_bIsAlive = false;
	if (m_pStatusEffects)
	{
		m_pStatusEffects->Dispel();
	}
	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_3D_SABO, this);
	//ApplyVariant(eNoVariant);
}

void CSaboteurEnemy::ManageMovement(float fDt)
{
	float distancetoTarget = Length(m_pTarget->GetPosition() - GetPosition());

	if (distancetoTarget > m_fAttackRange)
	{
		CGameplayState::GetEntityManager()->GetPlayer()->GetActionBitfield() &= ~IS_HIT;
	}

	if (m_pTarget != nullptr)
	{
		if (distancetoTarget <= m_fAttackRange)
		{
			//CDebugRenderer::GetReference( ).CircleRenderer(DirectX::XMFLOAT3 { GetPosition( ).x, 6.0f, GetPosition( ).z }, { m_fAttackRange, m_fAttackRange }, DirectX::XMFLOAT4(0, 1, 0.5f, 1));
			if (m_fAttackElapsedTime >= m_fAttackFrequency)
			{
				//m_pAttackEffect->SetColor(DirectX::XMFLOAT4{ 1, 1, 1, 0.99f });
				CDebugRenderer::GetReference().CircleRenderer(DirectX::XMFLOAT3{ GetPosition().x, 6.0f, GetPosition().z }, { m_fAttackRange - 40, m_fAttackRange -40 }, DirectX::XMFLOAT4(0, 0.8f, 0, 0.5f));
				if (m_fSubsystemElapsed < m_fSubsystemAttackTimer)
				{
					XMFLOAT3 pos = GetPosition();
					m_pAttackEffect->SetPosition(pos);
					m_pAttackEffect->GetEmitter(0)->Emit();
					
					DirectX::XMFLOAT3 myVect = m_vDestination - GetPosition();
					TurnToPoint(fDt, myVect, m_fTurningSpeed);
					Attack(m_fDamage * fDt);
					m_fSubsystemElapsed += fDt;
				}
				else
				{
					//TurnOffBit(CGameplayState::GetEntityManager()->GetPlayer()->GetActionBitfield(), IS_HIT);
					// This prevents the rocking out glitch.
					CGameplayState::GetEntityManager()->GetPlayer()->GetActionBitfield() &= ~IS_HIT;

					m_fAttackElapsedTime = 0.0f;
					m_fSubsystemElapsed = 0.0f;
				}
			}
			/*else
			{
				m_pAttackEffect->SetColor(DirectX::XMFLOAT4{ 1, 1, 1, 0.f });
			}*/
		}
		else if (distancetoTarget > m_fAttackRange)
		{
			MakeMovement(fDt);
		}
	}
}

void CSaboteurEnemy::TakeDamage(float fDamage)
{
	CGameplayState::GetEntityManager()->GetPlayer()->GetGameStats()->m_nTotalDamageGiven += (int)fDamage;

	CBaseEnemy::TakeDamage(fDamage);
	//Player Enemy-specific sounds
	//He's dead
	if (m_fCurrentHealth <= 0)
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_3D_SABO, this);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_SABO_DEATH, GetPosition());

		XMFLOAT3 pos = GetPosition();
		m_pExplodeEffect->SetPosition(pos);
		m_pExplodeEffect->GetEmitter(0)->Emit();
	}
	//Just take damage
	else
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_SABO_HIT, this);
	}
}

void CSaboteurEnemy::Attack(float fDamage)
{

	if (m_fCurrentASoundTime > m_fAttackSoundTimer)
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_SABO_ATTACK, this);
		m_fCurrentASoundTime = 0.0f;
	}
	DealDamage(fDamage);
}

/*bool CSaboteurEnemy::GoldBehavior(float fDt)
{
	DirectX::XMFLOAT3 destination = m_pTarget->GetPosition();
	float distancetoTarget = Length(destination - GetPosition());
	float distancetoPlayer = Length(m_pPlayer->GetPosition() - GetPosition());

	bool allDestroyed = true;
	for (auto listCheck = m_lpSubSystemList.begin(); listCheck != m_lpSubSystemList.end(); ++listCheck)
	{
		if ((*listCheck)->GetIsDestroyed() == false)
		{
			allDestroyed = false;
			break;
		}
	}
	if (allDestroyed == false && distancetoTarget > m_fAttackRange)
	{
		if (m_pCurrentSystem != nullptr && m_pPlayer != nullptr)
		{
			MakeEvasionMovement(fDt, m_pCurrentSystem->GetPosition(), m_pPlayer->GetPosition());
		}
	}
	else
	{
		if (distancetoPlayer > m_fShootRange && m_pTarget != nullptr)
		{
			MakeMovement(fDt);
			allDestroyed = false;
		}
	}

	if (distancetoPlayer <= m_fShootRange && distancetoPlayer > m_fAttackRange && m_fSubsystemElapsed == 0.0f)
	{
		m_pSMG->Shoot();
	}

	return allDestroyed;
}*/

void CSaboteurEnemy::ApplyVariant(eEnemyVariant eTier)
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

		//m_fShootRange = 0;
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

		//m_fShootRange = s_fPresetShootRange;
	}
}


bool CSaboteurEnemy::LoadStats(void)
{
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile("Assets//XML//Balance//SaboteurStats.xml");

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
	xmlElement = xmlTier->FirstChildElement("Shooting");
	s_fPresetShootRange = (float)atof(xmlElement->Attribute("Range"));

	//We're done here
	xmlDoc.Clear();
	return true;
}

//Declare the statics
float CSaboteurEnemy::s_fPresetHealth[eEnemyVariant::eVariantCount];
float CSaboteurEnemy::s_fPresetArmor[eEnemyVariant::eVariantCount];
float CSaboteurEnemy::s_fPresetDamage[eEnemyVariant::eVariantCount];
float CSaboteurEnemy::s_fPresetAttackSpeed[eEnemyVariant::eVariantCount];
float CSaboteurEnemy::s_fPresetAttackRange[eEnemyVariant::eVariantCount];
float CSaboteurEnemy::s_fPresetMoveSpeed[eEnemyVariant::eVariantCount];
float CSaboteurEnemy::s_fPresetTurnSpeed[eEnemyVariant::eVariantCount];
int CSaboteurEnemy::s_nPresetScrap[eEnemyVariant::eVariantCount];
float CSaboteurEnemy::s_fPresetColliderRadius[eEnemyVariant::eVariantCount];
float CSaboteurEnemy::s_fPresetScale[eEnemyVariant::eVariantCount];
float CSaboteurEnemy::s_fPresetMass[eEnemyVariant::eVariantCount];

float CSaboteurEnemy::s_fPresetShootRange;