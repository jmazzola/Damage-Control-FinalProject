/***********************************************
* Filename:			Subsystem.cpp
* Date:      		05/22/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:
************************************************/
#include "Subsystem.h"
#include "../../EntityManager.h"
#include "../../../State Machine/GameplayState.h"
#include "../../../Input Manager/InputManager.h"
#include "../../../Audio Manager/AudioSystemWwise.h"

#include "../../../Asset Manager/AssetManager.h"
#include "../../../Asset Manager/DDSTextureLoader.h"

#include "../../../Renderer/Renderer.h"
#include "../../../Renderer/DebugRenderer.h"

#include "../../../Particle Manager/ParticleSystem.h"
#include "../../../Particle Manager/ParticleEmitter.h"
#include "../../../Particle Manager/ParticleManager.h"

using namespace MathHelper;

#define SUBSYSTEMONLINEPERCENT 0.6f

/*static*/ CGameplayState* CSubSystem::g_pGamePlayState = nullptr;

//How often will ship wife say "(x) is under attack"
#define DAMAGE_TIMER 18.0f

CSubSystem::CSubSystem(void)
{
	m_fMaxHealth = 100;
	m_fCurrentHealth = m_fMaxHealth;

	//Get pointers to important objects
	m_pPlayer = CGameplayState::GetEntityManager( )->GetPlayer( );
	m_pAssets = CGameplayState::GetAssetManager( );

	m_pRepairEffect = CParticleSystem::LoadFrom("Assets/XML/Particles/WeldParticle.xml");
	m_pSmokeEffect = CParticleSystem::LoadFrom("Assets/XML/Particles/SmokeParticle.xml");

	CParticleManager::GetInstance( )->AddParticleSystem(m_pSmokeEffect);
	CParticleManager::GetInstance( )->AddParticleSystem(m_pRepairEffect);

	m_fCurrentDSoundTime = m_fDamageSoundTimer = DAMAGE_TIMER;
}

CSubSystem::~CSubSystem(void)
{
	for (unsigned int i = 0; i < 5; i++)
	{
		//SafeRelease(m_healthTextures[i]);
	}
}

/*virtual*/ void CSubSystem::Update(float dt) /*override*/
{
	DirectX::XMFLOAT3 lengthToPlayer = m_pPlayer->GetPosition( ) - GetPosition( );
	//Repair the subsystem
	if ((m_fCurrentHealth < m_fMaxHealth || m_bIsAlive == false) && CInputManager::GetInstance()->IsButtonDown(eControls::E) && Length(lengthToPlayer) < m_fRepairRange && dt > 0)
	{
		//If we've only just started
		if (!m_bRepairing)
		{
			AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_2DL_PLAYER_REPAIR, this);
		}
		DirectX::XMFLOAT3 effectPos = m_pPlayer->GetPosition();
		effectPos.y += 90.0f;
		effectPos.z += m_pPlayer->GetZAxis().z * 75.0f;
		m_pRepairEffect->SetPosition(effectPos);
		m_pRepairEffect->GetEmitter(0)->Emit( );
		m_bRepairing = true;
		Heal(12.f * dt);
	}
	else
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PAUSE_2DL_PLAYER_REPAIR, this);
		m_bRepairing = false;
	}

	if (m_fCurrentHealth < 25.f)
	{
		XMFLOAT3 pos = GetPosition( );
		m_pSmokeEffect->SetPosition(pos);
		m_pSmokeEffect->GetEmitter(0)->Emit( );
	}
	////Degrade subsystem slowly over time
	//else if (GetAlive( ))
	//{
	//	TakeDamageOverTime(float(rand( ) % 2) * dt * 0.5f);
	//}

	if (m_fCurrentHealth >= m_fMaxHealth * SUBSYSTEMONLINEPERCENT)
	{
		m_bIsAlive = true;
	}

	CAssetManager::TTexture& pTexture = m_pAssets->GetModelByTag(m_szTag)->m_tTex;

	if (m_fCurrentHealth <= 0)
	{
		m_fCurrentHealth = 0;
		m_bIsAlive = false;
	}
	//pTexture.m_pDiffuseTexture[0] = m_healthTextures[unsigned int(min(m_fCurrentHealth, 99.0f) / 20.f)];

	if (!m_bIsAlive)
	{
		m_fRecurringWarningTimer -= dt;
	}

	m_fPrintCounter += dt;
	CLivingEntities::Update(dt);

#if _DEBUG
	//CDebugRenderer::GetReference( ).SquareRenderer(GetPosition( ), { 100, 100 }, DirectX::XMFLOAT4(1, 0, 1, 1));
#endif
}

void CSubSystem::Reset(void)
{
	m_fCurrentHealth = m_fMaxHealth;
	m_bIsAlive = true;
}

void CSubSystem::Kill(void)
{
	m_fCurrentHealth = 0.0f;
	m_bIsAlive = false;
}

void CSubSystem::Heal(float fAmount)
{
	CLivingEntities::Heal(fAmount);
	//Restore functionality
	if (GetIsDestroyed( ) && PercentHealth( ) > 0.75f)
	{
		m_bIsAlive = true;
		//DebugPrint("[Sub-System] Restored to base functionality\n");
	}
}
