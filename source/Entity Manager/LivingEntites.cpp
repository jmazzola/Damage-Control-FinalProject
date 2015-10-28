#include "../Core/stdafx.h"
#include "LivingEntites.h"

#include "../Renderer/DebugRenderer.h"
#include "StatusEffects.h"
#include "../Particle Manager/ParticleSystem.h"
#include "../Particle Manager/ParticleManager.h"
#include "../Particle Manager/ParticleEmitter.h"

#define TAKE_DAMAGE_EPSILON 0.25f

CLivingEntities::CLivingEntities()
{
	m_pSlowEffect = CParticleSystem::LoadFrom("Assets\\XML\\Particles\\SlowParticle.xml");
	CParticleManager::GetInstance()->AddParticleSystem(m_pSlowEffect);

	m_fScaling = 1.0f;
}

CLivingEntities::~CLivingEntities()
{
	SafeDelete(m_pStatusEffects);
}

void CLivingEntities::Update(float dt)
{
	if (m_bIsExplosionColliding && m_fExplosionDamageTimer > 0)
		m_fExplosionDamageTimer -= dt;
	if (m_fExplosionDamageTimer <= 0)
	{
		m_fExplosionDamageTimer = 0.0f;
		m_bIsExplosionColliding = false;
	}

	if (m_fTakeDamageTimer < TAKE_DAMAGE_EPSILON)
	{
		m_fTakeDamageTimer += dt;
	}

	if (m_fTimer >= 0)
	{
		m_fTimer += dt;
		if (m_fTimer > .5f)
		{
			m_fTimer = -1;
			m_XMColor = m_XMBaseColor;
		}
	}
	if(m_fTimer < 0)
		BitTwiddler::TurnOffBit(actionBitfield, IS_HIT);

	if (m_pStatusEffects)
	{
		m_pStatusEffects->Update(dt);
		m_pStatusEffects->ApplyOverTimeEffects(dt, this);
		if (m_pStatusEffects->GetEffectPotency(EStatusEffect::STAT_EFF_SLOW) > 0)
		{
			m_pSlowEffect->SetPosition(GetPosition());
			m_pSlowEffect->GetEmitter(0)->Emit();
		}
	}

	//#if _DEBUG
	//CDebugRenderer::GetReference( ).LineRenderer(GetPosition( ), DirectX::XMFLOAT3(0, 0, 0));
	//#endif
}

void CLivingEntities::TakeDamage(float fAmount)
{
	BitTwiddler::TurnOnBit(actionBitfield, IS_HIT);

	if (GetEntityType() == ePLAYER && m_fTakeDamageTimer < TAKE_DAMAGE_EPSILON)
	{
		return;
	}
	m_fTakeDamageTimer = 0.0f;

	TakeDamageOverTime(fAmount);

	m_XMColor = DirectX::XMFLOAT4(0.8f, 0.1f, 0.16f, 1.0f);
	m_fTimer = 0;
}

void CLivingEntities::TakeDamageOverTime(float fAmount)
{
	m_fCurrentHealth -= fAmount;

	if (m_fCurrentHealth <= 0 && GetEntityType() != eCREEP)
	{
		m_bIsAlive = false;
		m_fCurrentHealth = 0;
	}
}

void CLivingEntities::Heal(float _fAmount)
{
	//Restore entity's health
	m_fCurrentHealth += _fAmount;
	if (m_fCurrentHealth > m_fMaxHealth)
	{
		m_fCurrentHealth = m_fMaxHealth;
	}
}

/*virtual*/ bool CLivingEntities::LoadStats(void)
{
	//Children will load their stuff
	return false;
}

void CLivingEntities::Disable(void)
{
	if (m_pStatusEffects)
	{
		m_pStatusEffects->Dispel();
	}
	CEntity::Disable();
}

void CLivingEntities::AddStatusEffect(EStatusEffect eEffect, float fDuration, float fPotency)
{
	if (m_pStatusEffects)
	{
		m_pStatusEffects->AddEffect(eEffect, fDuration, fPotency);
	}
}
