/***********************************************
* Filename:			HunterEnemy.cpp
* Date:      		05/12/2015
* Mod. Date:		05/14/2015
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:			Enemy who tracks and attacks the player
************************************************/

#include "HunterEnemy.h"
#include "..\..\Core\stdafx.h"
#include "..\..\State Machine\GameplayState.h"
#include "..\..\Animation Manager\Animation State Machine\States\MoveAnimState.h"
#include "..\..\Animation Manager\Animation State Machine\States\IdleAnimState.h"
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
#include "../../AchieveTracker.h"
using namespace MathHelper;

#define STEP_DELAY 0.3f
#define PARTICLE_DELAY 0.55f
CHunterEnemy::CHunterEnemy( )
{
	m_nEntityType = CEntity::eHUNTER;

	//Check if the stats are loaded
	if (GetPresetHealth(eEnemyVariant::eBronze) == 0)
	{
		if (!LoadStats( ))
		{
			DebugPrint("[HUNTER CTOR] Failed to load base stats from HunterStats.xml\n", ConsoleColor::Red);
		}
	}
	//m_fCurrentHealth = m_fMaxHealth = HUNTER_BRONZE_HEALTH;
	//m_fDamage = HUNTER_BRONZE_DAMAGE;
	//m_nScrapAmount = HUNTER_BRONZE_SCRAP;
	m_bIsAlive = false;

	//m_nLevel = 1;
	m_eState = eAIStateType::eObjective;
	//m_fAggroRange = 0.0f; //not used by this enemy
	//m_fAttackElapsedTime = m_fAttackFrequency = HUNTER_ATTACK_FREQUENCY;
	//m_fAttackRange = 200.0f;

	m_pPlayer = (CEntity*)CGameplayState::GetEntityManager( )->GetPlayer( );
	m_pTarget = m_pPlayer;

	//TWEAK
	m_eMyType = eEnemyType::ENEM_HUNT;
	//m_fMass = 10.0f;
	//m_fTurningSpeed = 20.0f;

	m_tCollider = new TSphere( );
	m_tCollider->type = TCollider::eSphere;
	//m_tCollider->fRadius = 40.0f;
	//m_fMovementSpeed = 300.0f;

	m_szTag = "Hunter";
	ApplyVariant(eNoVariant);
	//m_eVariant = eEnemyVariant::eBronze;
	m_fExplosionDamageTimer = 0.0f;
	m_bIsExplosionColliding = false;
	m_bIamStealth = false;
	m_fHideTimer = 8.0f;
	m_fCurrentHideTimer = 0.0f;

	//m_pSppitlePistol = new CEnemyPistol( );

	m_pLightParams = new LightParams({ 0.0f, 0.0f, 0.0f }, { 1, 3, 1 }, 100);

	m_xOffset.x = 0.0f;
	m_xOffset.y = 220.0f;
	m_xOffset.z = 0.0f;

	m_pExplodeEffect = CParticleSystem::LoadFrom("Assets/XML/Particles/HunterExplodeParticle.xml");
	CParticleManager::GetInstance()->AddParticleSystem(m_pExplodeEffect);

	m_pStepEffects = CParticleSystem::LoadFrom("Assets/XML/Particles/HunterStep.xml");
	CParticleManager::GetInstance()->AddParticleSystem(m_pStepEffects);
}
CHunterEnemy::~CHunterEnemy( )
{
	SafeDelete(m_tCollider);
	//SafeDelete(m_pSppitlePistol);
	SafeDelete(m_pLightParams);
}

/*virtual*/ void	CHunterEnemy::EvaluateState(void) /*override*/
{
	//not used in this class
}

/*virtual*/ void	CHunterEnemy::Update(float fDt) /*override*/
{
	if (m_fCurrentHealth < 0)
	{
		Disable( );
		return;
	}

	/*OutputDebugString(m_szTag.c_str());
	OutputDebugString("\n");*/

	if (m_bIamStealth == true && m_eVariant == eEnemyVariant::eGold)
	{
		m_fParticleDelay += fDt;
		if (m_fParticleDelay > PARTICLE_DELAY)
		{
			XMFLOAT3 pos = GetPosition();
			pos.y = 5.0f;
			m_pStepEffects->SetPosition(pos);
			m_pStepEffects->GetEmitter(0)->Emit();
			m_fParticleDelay = 0;
		}
		m_tCollider->fRadius = 0.0f;
	}
	//else if (m_eVariant == eEnemyVariant::eSilver)
	//{
		//m_pSppitlePistol->UpdateEnemyW(fDt, this);
	//}

	DirectX::XMFLOAT3 position = GetPosition( );
	(m_tCollider)->vCenter = DirectX::XMFLOAT3(position.x, position.y, position.z);

	m_pLightParams->Position = GetPosition( );
	m_pLightParams->Position.y += 50;

	//if (m_eState == eAIStateType::eObjective)
	UpdateObjective(fDt);

	CBaseEnemy::Update(fDt);

	// Animate
	if (m_pAnimation)
	{
		GetAnimStateMachine( ).Update(fDt);
		GetAnimationBlender( ).Process(fDt);
		m_cDeformer.Process(GetAnimationBlender( ).GetPose( ));

		if (m_pAnimation->m_pModel->m_pVertexBuffer == NULL)
		{
			CGameplayState::GetAssetManager( )->CreateBuffer(CRenderer::GetReference( ).GetDevice( ), m_cDeformer.GetSkinVerts( ),
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
			AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_3D_HUNTER_STEP, this);
			m_fStepTimer = STEP_DELAY;
		}
	}
}

bool CHunterEnemy::RegisterAnimation(void)
{
	GetAnimStateMachine( ).SetEntity(this);
	GetAnimStateMachine( ).ChangeState(CIdleAnimState::GetInstance( ));

	m_pAnimation = CGameplayState::GetAssetManager( )->GetAnimByTag(m_szTag);
	if (m_pAnimation)
	{
		m_cDeformer.SetMesh(&m_pAnimation->m_pModel->m_cMesh);

		if (m_pAnimation->m_pModel->m_pVertexBuffer == NULL)
		{
			CGameplayState::GetAssetManager( )->CreateBuffer(CRenderer::GetReference( ).GetDevice( ), m_cDeformer.GetSkinVerts( ),
															 D3D11_BIND_VERTEX_BUFFER,
															 &m_pAnimation->m_pModel->m_pVertexBuffer);
		}
		return true;
	}
	return false;
}

/*virtual*/ void	CHunterEnemy::UpdateObjective(float fDt) /*override*/
{
	if (!m_fAttackElapsedTime)
	{
		BitTwiddler::TurnOffBit(actionBitfield, IS_ATTACKING);
	}

	m_fAttackElapsedTime += fDt;

	if (m_pTarget != nullptr) // this should be the player
	{
		m_vDestination = m_pTarget->GetPosition( );

		float distancetoTarget = Length((m_vDestination - GetPosition( )));
		bool attack = true;
		if (m_eVariant == eEnemyVariant::eGold)
		{
			GoldBehavior(fDt, attack);
		}


		if (distancetoTarget <= m_fAttackRange && attack == true)
		{

			BitTwiddler::TurnOnBit(actionBitfield, IS_IDLE);

			if (m_fAttackElapsedTime >= m_fAttackFrequency)
			{
				DirectX::XMFLOAT3 myVect = m_vDestination - GetPosition();
				TurnToPoint(fDt, myVect, m_fTurningSpeed);
				CBaseEnemy::Attack( );
				BitTwiddler::TurnOnBit(actionBitfield, IS_ATTACKING);
				m_fAttackElapsedTime = 0;

			}

		}
		else if(distancetoTarget > m_fAttackRange)
		{
			/*if (m_eVariant == eSilver && distancetoTarget > m_fAttackRange && distancetoTarget <= 900.0f)
			{
				m_pSppitlePistol->Shoot( );
			}*/

			BitTwiddler::TurnOffBit(actionBitfield, IS_IDLE);
			MakeMovement(fDt);
		}
	}
	//}
	//}
}

/*virtual*/ void	CHunterEnemy::UpdatePatrol(float fDt) /*override*/
{
	//not used in this class
}

/*virtual*/ void	CHunterEnemy::UpdateAggro(float fDt) /*override*/
{
	//this enemy is always in aggro mode
}

void CHunterEnemy::Enable(DirectX::XMFLOAT3 SpawnPoint, eEnemyVariant variant)
{
	ApplyVariant(variant);

	//DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixScaling(.75f, .75f, .75f));
	m_fScaling = .75f;
	m_fParticleDelay = 0;
	MyWorld.Local._41 = SpawnPoint.x;
	MyWorld.Local._42 = SpawnPoint.y;
	MyWorld.Local._43 = SpawnPoint.z;

	(m_tCollider)->vCenter = DirectX::XMFLOAT3(SpawnPoint.x, SpawnPoint.y, SpawnPoint.z);

	m_eState = eAIStateType::eObjective;

	m_bIsAlive = true;

	if (CGameplayState::GetEntityManager( ) != nullptr)
	{
		m_pTarget = (CEntity*)CGameplayState::GetEntityManager( )->GetPlayer( );
	}

	//m_eVariant = variant;
	m_fExplosionDamageTimer = 0.0f;
	m_bIsExplosionColliding = false;
}

void CHunterEnemy::Disable( )
{
	CAchieveTracker::GetInstance()->UnlockAchievement(CAchieveTracker::eKiller);
	CGameplayState::GetEntityManager()->GetPlayer()->GetGameStats()->m_nTotalEnemiesKilled++;

	m_bIsAlive = false;
	m_pTarget = nullptr;
	if (m_pStatusEffects)
	{
		m_pStatusEffects->Dispel( );
	}
	AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::STOP_3D_HUNTER, this);
	//ApplyVariant(eNoVariant);
}

void CHunterEnemy::TakeDamage(float fDamage)
{
	CGameplayState::GetEntityManager()->GetPlayer()->GetGameStats()->m_nTotalDamageGiven += (int)fDamage;

	CBaseEnemy::TakeDamage(fDamage);
	//Player Enemy-specific sounds
	//He's dead
	if (m_fCurrentHealth <= 0)
	{
		AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::STOP_3D_HUNTER, this);
		AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_3D_HUNTER_DEATH, GetPosition( ));
		XMFLOAT3 pos = GetPosition();
		m_pExplodeEffect->SetPosition(pos);
		m_pExplodeEffect->GetEmitter(0)->Emit();
	}
	//Just take damage
	else
	{
		AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_3D_HUNTER_HIT, this);
	}
}

void CHunterEnemy::Attack(float fDamage)
{
	AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_3D_HUNTER_ATTACK, this);
	DealDamage(fDamage);
}

void CHunterEnemy::GoldBehavior(float fDt, bool &attack)
{
	float distancetoTarget = Length((m_vDestination - GetPosition( )));

	if (m_bIamStealth == true)
	{
		if (m_XMColor.w > 0)
		{
			m_XMColor.w -= fDt;
			m_pVariantIndicator->SetColor(DirectX::XMFLOAT4{ 1.0f, 0.94f, 0.0f, m_pVariantIndicator->GetColor().w - fDt });
		}

		if (m_pVariantIndicator->GetColor().w < 0.01f)
		{
			m_pVariantIndicator->SetColor(DirectX::XMFLOAT4{ 1.0f, 0.94f, 0.0f, 0.01f });
		}

		if (m_XMColor.w <= 0.0f)
		{
			m_XMColor.w = 0;
			m_fCurrentHideTimer += fDt;
		}
		attack = false;

		if (distancetoTarget < 400 && (m_fAttackElapsedTime > m_fAttackFrequency && m_fCurrentHideTimer > m_fHideTimer))
		{
			m_bIamStealth = false;
			m_fCurrentHideTimer = 0.0f;
			
		}
	}
	else
	{
		/*m_pStepEffects->GetEmitter(0)->;*/
		if (m_XMColor.w < 1)
		{
			m_XMColor.w += fDt;
			m_pVariantIndicator->SetColor(DirectX::XMFLOAT4{ 1.0f, 0.94f, 0.0f, m_pVariantIndicator->GetColor().w + fDt });
		}

		if (m_pVariantIndicator->GetColor().w > 0.99f)
		{
			m_pVariantIndicator->SetColor(DirectX::XMFLOAT4{ 1.0f, 0.94f, 0.0f, 0.99f });
		}
		if (m_XMColor.w >= 1.0f)
		{
			m_XMColor.w = 1;
			m_fCurrentHideTimer += fDt;
		}
		if (m_fCurrentHideTimer > m_fHideTimer && m_fAttackElapsedTime < m_fAttackFrequency || distancetoTarget > 1000)
		{
			m_bIamStealth = true;
			m_fHideTimer = (float)(rand( ) % 15 + 1);
			m_fCurrentHideTimer = 0.0f;
			
			m_pVariantIndicator->SetColor(DirectX::XMFLOAT4{ 1.0f, 0.94f, 0.0f, 0.1f });
		}
	}

	if (m_XMColor.w < 0)
	{
		m_XMColor.w = 0;
		(m_tCollider)->fRadius = 0.0f;
	}
	else if (m_XMColor.w > 1)
	{
		m_XMColor.w = 1;
		(m_tCollider)->fRadius = 40.0f;
	}
}

void CHunterEnemy::ApplyVariant(eEnemyVariant eTier)
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
		DirectX::XMFLOAT3 pos = GetPosition( );
		DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixScaling(s_fPresetScale[eTier], s_fPresetScale[eTier], s_fPresetScale[eTier]));
		SetPosition(pos);
		m_fMass = s_fPresetMass[eTier];
	}
}

bool CHunterEnemy::LoadStats(void)
{
	tinyxml2::XMLDocument xmlDoc;

	//TODO: Load in stats based on difficulty
	xmlDoc.LoadFile("Assets//XML//Balance//HunterStats.xml");

	//Did not open file properly
	if (xmlDoc.Error( ))
	{
		xmlDoc.Clear( );
		return false;
	}
	tinyxml2::XMLElement* xmlHead = xmlDoc.RootElement( );
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

		xmlTier = xmlTier->NextSiblingElement( );
	}

	//The Special


	//We're done here
	xmlDoc.Clear( );
	return true;
}

//Declare the statics
float CHunterEnemy::s_fPresetHealth[eEnemyVariant::eVariantCount];
float CHunterEnemy::s_fPresetArmor[eEnemyVariant::eVariantCount];
float CHunterEnemy::s_fPresetDamage[eEnemyVariant::eVariantCount];
float CHunterEnemy::s_fPresetAttackSpeed[eEnemyVariant::eVariantCount];
float CHunterEnemy::s_fPresetAttackRange[eEnemyVariant::eVariantCount];
float CHunterEnemy::s_fPresetMoveSpeed[eEnemyVariant::eVariantCount];
float CHunterEnemy::s_fPresetTurnSpeed[eEnemyVariant::eVariantCount];
int CHunterEnemy::s_nPresetScrap[eEnemyVariant::eVariantCount];
float CHunterEnemy::s_fPresetColliderRadius[eEnemyVariant::eVariantCount];
float CHunterEnemy::s_fPresetScale[eEnemyVariant::eVariantCount];
float CHunterEnemy::s_fPresetMass[eEnemyVariant::eVariantCount];
