/***********************************************
* Filename:			SpammerEnemy.cpp
* Date:      		08/14/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Justin Mazzola
* Purpose:			shoots at the player
************************************************/
#include "SpammerEnemy.h"

#include "..\..\Core\stdafx.h"
#include "..\..\State Machine\GameplayState.h"
#include "..\..\Animation Manager\Animation State Machine\States\MoveAnimState.h"
#include "..\EntityManager.h"
#include "..\..\Audio Manager\AudioSystemWwise.h"
#include "../../Renderer/DebugRenderer.h"
#include "../../Renderer/Renderer.h"
#include "../../Weapon/Pistol.h"
#include "..\..\Weapon\EnemyPistol.h"

#include "../../TinyXML2/tinyxml2.h"
#include "../StatusEffects.h"

#include "../../Particle Manager/ParticleSystem.h"
#include "../../Particle Manager/ParticleEmitter.h"
#include "../../Particle Manager/ParticleManager.h"

#define STEP_DELAY 0.3f

CSpammerEnemy::CSpammerEnemy()
{
	m_nEntityType = CEntity::eSPAMMER;

	//Check if the stats are loaded
	if (GetPresetHealth(eEnemyVariant::eBronze) == 0)
	{
		if (!LoadStats())
		{
			DebugPrint("[SPAMMER CTOR] Failed to load base stats from SpammerStats.xml\n", ConsoleColor::Red);
		}
	}

	m_bIsAlive = false;
	m_eState = eAIStateType::eObjective;

	m_pPlayer = (CEntity*)CGameplayState::GetEntityManager()->GetPlayer();
	m_pTarget = m_pPlayer;

	m_eMyType = eEnemyType::ENEM_HUNT;

	m_tCollider = new TSphere();
	m_tCollider->type = TCollider::eSphere;

	m_szTag = "Spammer";

	ApplyVariant(eNoVariant);

	m_fExplosionDamageTimer = 0.0f;
	m_bIsExplosionColliding = false;

	m_pSppitlePistol = new CEnemyPistol();
	m_pLightParams = new LightParams({ 0.0f, 0.0f, 0.0f }, { 1, 3, 1 }, 100);

	m_xOffset.x = 0.0f;
	m_xOffset.y = 220.0f;
	m_xOffset.z = 0.0f;

	m_pExplodeEffect = CParticleSystem::LoadFrom("Assets/XML/Particles/HunterExplodeParticle.xml");
	CParticleManager::GetInstance()->AddParticleSystem(m_pExplodeEffect);
}


CSpammerEnemy::~CSpammerEnemy()
{
	SafeDelete(m_tCollider);
	SafeDelete(m_pSppitlePistol);
	SafeDelete(m_pLightParams);
}

/*virtual*/ void CSpammerEnemy::EvaluateState(void) /*override*/
{
	//not used in this class
}

/*virtual*/ void CSpammerEnemy::Update(float fDt) /*override*/
{
	if (m_fCurrentHealth < 0)
	{
		Disable();
		return;
	}

	m_pSppitlePistol->UpdateEnemyW(fDt, this);

	DirectX::XMFLOAT3 position = GetPosition();
	(m_tCollider)->vCenter = DirectX::XMFLOAT3(position.x, position.y, position.z);

	m_pLightParams->Position = GetPosition();
	m_pLightParams->Position.y += 50;

	UpdateObjective(fDt);

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

	if (m_fCurrentHealth > 0)
	{
		//Play idle sound, keep them out of sync
		if (m_fIdleDelay == 0)
		{
			//AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3DL_HUNTER_IDLE, this);
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
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_HUNTER_STEP, this);
			m_fStepTimer = STEP_DELAY;
		}
	}
}

bool CSpammerEnemy::RegisterAnimation(void)
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

/*virtual*/ void CSpammerEnemy::UpdateObjective(float fDt) /*override*/
{
	if (!m_fAttackElapsedTime)
	{
		TurnOffBit(actionBitfield, IS_ATTACKING);
	}

	m_fAttackElapsedTime += fDt;

	if (m_pTarget != nullptr) // this should be the player
	{
		m_vDestination = m_pTarget->GetPosition();

		float distancetoTarget = Length((m_vDestination - GetPosition()));
		bool attack = true;
		if (distancetoTarget < m_fAttackRange)
		{
			DirectX::XMFLOAT3 myVect = m_vDestination - GetPosition();
			TurnToPoint(fDt, myVect, m_fTurningSpeed);
			m_pSppitlePistol->Shoot();
		}
		else
		{
			MakeMovement(fDt);
		}
	}
}

/*virtual*/ void CSpammerEnemy::UpdatePatrol(float fDt) /*override*/
{
	//not used is class
}

/*virtual*/ void CSpammerEnemy::UpdateAggro(float fDt) /*override*/
{
	//this enemy is always in aggro mode
}

void CSpammerEnemy::Enable(DirectX::XMFLOAT3 SpawnPoint, eEnemyVariant variant)
{
	ApplyVariant(variant);

	//DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixScaling(.75f, .75f, .75f));
	m_fScaling = .75f;

	MyWorld.Local._41 = SpawnPoint.x;
	MyWorld.Local._42 = SpawnPoint.y;
	MyWorld.Local._43 = SpawnPoint.z;

	(m_tCollider)->vCenter = DirectX::XMFLOAT3(SpawnPoint.x, SpawnPoint.y, SpawnPoint.z);

	m_eState = eAIStateType::eObjective;

	m_bIsAlive = true;

	if (CGameplayState::GetEntityManager() != nullptr)
	{
		m_pTarget = (CEntity*)CGameplayState::GetEntityManager()->GetPlayer();
	}

	//m_eVariant = variant;
	m_fExplosionDamageTimer = 0.0f;
	m_bIsExplosionColliding = false;
}

void CSpammerEnemy::Disable()
{
	CGameplayState::GetEntityManager()->GetPlayer()->GetGameStats()->m_nTotalEnemiesKilled++;

	m_bIsAlive = false;

	m_pTarget = nullptr;

	if (m_pStatusEffects)
	{
		m_pStatusEffects->Dispel();
	}

	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_3D_HUNTER, this);

}

void CSpammerEnemy::TakeDamage(float fDamage)
{
	CGameplayState::GetEntityManager()->GetPlayer()->GetGameStats()->m_nTotalDamageGiven += (int)fDamage;

	CBaseEnemy::TakeDamage(fDamage);
	//Player Enemy-specific sounds
	//He's dead
	if (m_fCurrentHealth <= 0)
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_3D_HUNTER, this);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_HUNTER_DEATH, GetPosition());
		XMFLOAT3 pos = GetPosition();
		m_pExplodeEffect->SetPosition(pos);
		m_pExplodeEffect->GetEmitter(0)->Emit();
	}
	//Just take damage
	else
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_HUNTER_HIT, this);
	}
}

void CSpammerEnemy::Attack(float fDamage)
{
	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_HUNTER_ATTACK, this);
	DealDamage(fDamage);
}

void CSpammerEnemy::ApplyVariant(eEnemyVariant eTier)
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
	}
}

bool CSpammerEnemy::LoadStats(void)
{
	tinyxml2::XMLDocument xmlDoc;

	//TODO: Load in stats based on difficulty
	xmlDoc.LoadFile("Assets//XML//Balance//SpammerStats.xml");

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


	//We're done here
	xmlDoc.Clear();
	return true;
}

//Declare the statics
float CSpammerEnemy::s_fPresetHealth[eEnemyVariant::eVariantCount];
float CSpammerEnemy::s_fPresetArmor[eEnemyVariant::eVariantCount];
float CSpammerEnemy::s_fPresetDamage[eEnemyVariant::eVariantCount];
float CSpammerEnemy::s_fPresetAttackSpeed[eEnemyVariant::eVariantCount];
float CSpammerEnemy::s_fPresetAttackRange[eEnemyVariant::eVariantCount];
float CSpammerEnemy::s_fPresetMoveSpeed[eEnemyVariant::eVariantCount];
float CSpammerEnemy::s_fPresetTurnSpeed[eEnemyVariant::eVariantCount];
int CSpammerEnemy::s_nPresetScrap[eEnemyVariant::eVariantCount];
float CSpammerEnemy::s_fPresetColliderRadius[eEnemyVariant::eVariantCount];
float CSpammerEnemy::s_fPresetScale[eEnemyVariant::eVariantCount];
float CSpammerEnemy::s_fPresetMass[eEnemyVariant::eVariantCount];

