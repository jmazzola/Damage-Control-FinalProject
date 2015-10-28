#include "../Core/stdafx.h"
#include "Bullet.h"

#include "../Particle Manager/ParticleSystem.h"
#include "../Particle Manager/ParticleManager.h"
#include "../Particle Manager/ParticleEmitter.h"

#include "../Audio Manager/AudioSystemWwise.h"
#include "../State Machine/GameplayState.h"
#include "../Entity Manager/EntityManager.h"
#include "../Entity Manager/LivingEntites.h"
#include "IWeapon.h"

#include "../Renderer/DebugRenderer.h"
#include "../Entity Manager/StatusEffects.h"
#include "ModBank.h"

using namespace MathHelper;

#define INITIAL_BULLET_SPEED 2100.0f

//Special numbers
#define PIERCE_DAMAGE_THRESHHOLD 0.5f
#define PIERCE_DAMAGE_PER_LEVEL 0.05f

#define POISON_DAMAGE_THRESHOLD 0.0f
#define POISON_DAMAGE_PER_LEVEL 1.0f
#define POISON_TIME_THRESHOLD 1.5f
#define POISON_TIME_PER_LEVEL 0.5f

#define SLOW_POTENCY_THRESHOLD 0.2f
#define SLOW_POTENCY_PER_LEVEL 0.05f
#define SLOW_TIME_THRESHOLD 1.5f
#define SLOW_TIME_PER_LEVEL 0.5f

CBullet::CBullet( )
{
	m_szTag = "Bullet";

	tCollider = new TSphere( );
	tCollider->type = TCollider::eSphere;
	tCollider->fRadius = 30.0f;

	m_fMaxLifeTime = 2.0f;

	m_XMColor = DirectX::XMFLOAT4(.99f, .99f, .99f, .99f);

	m_pBloodImpactEffect = CParticleSystem::LoadFrom("Assets/XML/Particles/BloodParticle.xml");
	CParticleManager::GetInstance( )->AddParticleSystem(m_pBloodImpactEffect);

	m_pLightParams = new LightParams( );
	m_pLightParams->Type = Point;
	m_pLightParams->Range = 75.0f;

	ZeroMemory(&m_tSpecialLevels, sizeof(m_tSpecialLevels));

	m_vPastMarks.resize(8);
}

CBullet::~CBullet( )
{
	delete tCollider;
	tCollider = nullptr;

	SafeDelete(m_pLightParams);
}

void CBullet::Update(float dt)
{
	m_fCurrentLifeTime -= dt;
	if (m_fCurrentLifeTime <= 0)
	{
		Disable( );
		return;
	}
	// TODO: Speed this up by making functions to get certain parts of these matricies
	DirectX::XMFLOAT3 position = { MyWorld.GetWorld( )._41, MyWorld.GetWorld( )._42, MyWorld.GetWorld( )._43 };

	//translate
	position = position + m_xmForward * m_fSpeed * dt;

	m_pLightParams->Position = XMFLOAT3(position.x, position.y / 4.0f, position.z);
	m_pLightParams->Color = (XMFLOAT3(m_XMColor.x, m_XMColor.y, m_XMColor.z) / 3) * 3.0f;

	(tCollider)->vCenter = DirectX::XMFLOAT3(position.x, position.y, position.z);

	MyWorld.Local._41 = position.x;
	MyWorld.Local._42 = position.y;
	MyWorld.Local._43 = position.z;

#ifdef _DEBUG
	CDebugRenderer::GetReference( ).CircleRenderer(tCollider->vCenter, DirectX::XMFLOAT2(tCollider->fRadius, tCollider->fRadius));
#endif

	//Specials
	if (m_uSpecialFlag != 0)
	{

		//Fuck all of you!
		if (m_uSpecialFlag & ESpecialBullets::BULLET_FUCKYOU)
		{
			DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(1 + dt * 10, 1 + dt * 10, 1 + dt * 10), DirectX::XMLoadFloat4x4(&MyWorld.Local)));
			tCollider->fRadius *= float(1 + dt * 10.0f);
		}
	}
}

#include <random>
void CBullet::Enable(DirectX::XMFLOAT3 spawnPoint, CIWeapon* Owner, CEntity::eEntityType myType)
{
	SetEntityType(myType);
	//Pass on the owner's info
	m_pOwner = Owner;
	if (m_pOwner)
	{
		if (m_pOwner->GetGunType( ) == ENEMY_FASTSMG || m_pOwner->GetGunType( ) == ENEMY_PISTOL)
		{
			m_bFriendly = false;
		}
		else
		{
			m_bFriendly = true;
		}
		m_fDamage = m_pOwner->GetDamage( );
		m_fSpeed = INITIAL_BULLET_SPEED;
		HandleColor( );
		m_fMaxLifeTime = m_pOwner->GetBulletLifeTime( );

		if (m_pOwner->GetModBank( ))
		{
			CModBank* pMods = m_pOwner->GetModBank( );
			//Clear the bits
			m_uSpecialFlag = 0;
			for (size_t i = 0; i < pMods->GetModCount( ); i++)
			{
				m_uSpecialFlag |= (unsigned char)(pMods->GetMod(i));
			}
			pMods->ApplyMods(this);
		}
	}
	auto randF = [&] (float lower_bound, float uppder_bound) -> float
	{
		return lower_bound + static_cast <float> (rand( )) / (static_cast <float> (RAND_MAX / (uppder_bound - lower_bound)));
	};

	(tCollider)->vCenter = DirectX::XMFLOAT3(spawnPoint.x, spawnPoint.y, spawnPoint.z);

	std::random_device rd;
	std::mt19937 gen(rd( ));

	// spread should be in radians here (not degrees which is what Unity uses)
	float spread = m_pOwner->GetSpread( );
	std::uniform_real_distribution<float> dis(-spread, spread);

	DirectX::XMFLOAT4X4 world = m_pOwner->GetWorldMatrix( );
	DirectX::XMStoreFloat4x4(&world, (DirectX::XMLoadFloat4x4(&world) * DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(randF(dis(gen), dis(gen))))));
	world._41 = m_pOwner->GetWorldMatrix( )._41;
	world._43 = m_pOwner->GetWorldMatrix( )._43;
	m_xmForward = { world.m[2][0], world.m[2][1], world.m[2][2] };


	//bullet matrix
	{
		MyWorld.Local = world;
		//MyWorld.Local._42 += 90;

		DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixTranslation(0, 0, GetZAxis().z * -85) * DirectX::XMLoadFloat4x4(&MyWorld.Local));
	}

	//Jesus has graced us
	//if (m_bFriendly && m_pOwner->GetWeaponTier( ) == MAX_WEAPON_LEVEL)
	{
		HyperBeam( );
	}
	m_vPastMarks.clear( );

	m_fCurrentLifeTime = m_fMaxLifeTime;

	m_bAlive = true;
}

void CBullet::Disable( )
{
	//CGameplayState::GetEntityManager()->CycleIntoDeadList(this);
	m_xmForward = DirectX::XMFLOAT3(0, 0, 0);
	m_pOwner = nullptr;
	m_bAlive = false;
	m_fDamage = 0.0f;
}

void CBullet::Collide(TCollider* pCollider, CEntity* pOther)
{
	if (!GetAlive( ))
	{
		return;
	}
	//Entity Collision
	if (pOther)
	{
		//Never hit a target twice
		if (m_uSpecialFlag && !(m_uSpecialFlag & BULLET_FUCKYOU))
		{
			//Find this guy in our vector
			for (size_t i = 0; i < m_vPastMarks.size( ); i++)
			{
				//He's here, so don't bother
				if (m_vPastMarks[i] == pOther)
				{
					return;
				}
			}
		}
		CLivingEntities* pMeatBag = dynamic_cast<CLivingEntities*>(pOther);
		if (pMeatBag)
		{
			if ((GetFriendly( ) && (pMeatBag->GetEntityType( ) == CEntity::eCREEP || pMeatBag->GetEntityType( ) == CEntity::eHUNTER || pMeatBag->GetEntityType( ) == CEntity::eSABOTEUR /*|| pMeatBag->GetEntityType() == CEntity::eSPAMMER*/))//Hurt the Enemies
				|| (!GetFriendly( ) && pOther->GetEntityType( ) == CEntity::ePLAYER))//Hurt the player
			{
				m_pBloodImpactEffect->SetPosition(GetPosition( ));
				m_pBloodImpactEffect->GetEmitter(0)->SetStartColor(GetColor( ));
				m_pBloodImpactEffect->GetEmitter(0)->Emit( );
				pMeatBag->TakeDamage(m_fDamage);

				//Do special things
				if (m_uSpecialFlag)
				{
					if (m_uSpecialFlag & BULLET_SLOW)
					{
						float fDuration = SLOW_TIME_THRESHOLD + m_tSpecialLevels.m_nSlowLevel * SLOW_TIME_PER_LEVEL;
						float fPotency = SLOW_POTENCY_THRESHOLD + m_tSpecialLevels.m_nSlowLevel * SLOW_POTENCY_PER_LEVEL;
						pMeatBag->AddStatusEffect(STAT_EFF_SLOW, fDuration, fPotency);
					}
					if (m_uSpecialFlag & BULLET_POISON)
					{
						float fDuration = POISON_TIME_THRESHOLD + m_tSpecialLevels.m_nPoisonLevel * POISON_TIME_PER_LEVEL;
						float fPotency = POISON_DAMAGE_THRESHOLD + m_tSpecialLevels.m_nPoisonLevel * POISON_DAMAGE_PER_LEVEL;
						pMeatBag->AddStatusEffect(STAT_EFF_DOT, fDuration, fPotency);
					}
					if (m_uSpecialFlag & BULLET_PIERCE)
					{
						m_vPastMarks.push_back(pOther);
						m_fDamage *= (PIERCE_DAMAGE_THRESHHOLD + m_tSpecialLevels.m_nPierceLevel * PIERCE_DAMAGE_PER_LEVEL);
						m_fSpeed *= 0.95f;
						if ((unsigned char)m_tSpecialLevels.m_nPierceLevel < m_vPastMarks.size( ))
						{
							Disable( );
						}
					}
					else
					{
						Disable( );
					}
				}
				else
				{
					Disable( );
				}
				return;
			}
			else if (pOther->GetEntityType( ) == CEntity::eSUBSYSTEM)
			{
				if (!(m_uSpecialFlag & BULLET_RICOCHET))
				{
					m_pBloodImpactEffect->SetPosition(GetPosition( ));
					m_pBloodImpactEffect->GetEmitter(0)->SetStartColor(GetColor( ));
					m_pBloodImpactEffect->GetEmitter(0)->Emit( );

					Disable( );
				}
				AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_3D_RICOCHET, GetPosition( ));
			}
		}
		return;
	}
	//Environmental Collision
	{
		if (((TAABB*)(pCollider))->vMax.y >= 95.0f)
		{
			if (!(m_uSpecialFlag & BULLET_RICOCHET))
			{
				m_pBloodImpactEffect->SetPosition(GetPosition( ));
				m_pBloodImpactEffect->GetEmitter(0)->SetStartColor(GetColor( ));
				m_pBloodImpactEffect->GetEmitter(0)->Emit( );

				Disable( );
			}
			AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_3D_RICOCHET, GetPosition( ));
		}
		return;
	}
}

void CBullet::HandleColor(void)
{/*
	//Player bullets
	if (m_bFriendly)
	{
	switch (m_pOwner->GetWeaponTier( ))
	{
	case 1:
	{
	//Red
	m_XMColor = { 0.988f, 0, 0, .95f };
	}
	break;

	case 2:
	{
	//Orange
	m_XMColor = { 0.996f, 0.490f, 0, .95f };
	}
	break;

	case 3:
	{
	//Yellow
	m_XMColor = { 0.992f, 0.929f, 0, .95f };
	}
	break;

	case 4:
	{
	//Green
	m_XMColor = { 0, .600f, .145f, .95f };
	}
	break;

	case 5:
	{
	//Blue
	m_XMColor = { 0, 0, 0.992f, .95f };
	}
	break;

	case 6:
	{
	//Indigo
	m_XMColor = { .290f, 0, 0.501f, .95f };
	}
	break;

	case 7:
	{
	//Violet
	m_XMColor = { 0.611f, 0, 0.992f, .95f };
	}
	break;

	default:
	{
	m_XMColor = { 1.0f, 1.0f, 1.0f, 0.95f };
	}
	break;
	}
	}
	//Enemies' bullets
	else
	{
	m_XMColor = { 0.0f, 0.0f, 0.0f, 0.95f };
	}
	*/
}

void CBullet::HyperBeam(void)
{
	//Enemies shoot black bullets
	if (m_pOwner->GetGunType( ) == ENEMY_PISTOL || m_pOwner->GetGunType( ) == ENEMY_FASTSMG)
	{
		m_XMColor = DirectX::XMFLOAT4(0.95f, 0, 0, 0.95f);
		return;
	}

	static int s_nColorIndex;

	if (++s_nColorIndex > 6)
	{
		s_nColorIndex = 0;
	}

	switch (s_nColorIndex)
	{
		case 0:
		{
			//RED
			m_XMColor = DirectX::XMFLOAT4(1, 0, 0, .95f);
			break;
		}
		case 1:
		{
			//ORANGE
			m_XMColor = DirectX::XMFLOAT4(1, 0.6471f, 0, .95f);
			break;
		}
		case 2:
		{
			//YELLOW
			m_XMColor = DirectX::XMFLOAT4(1, 1, 0, .95f);
			break;
		}
		case 3:
		{
			//GREEN
			m_XMColor = DirectX::XMFLOAT4(0, 1, 0, .95f);
			break;
		}
		case 4:
		{
			//BLUE
			m_XMColor = DirectX::XMFLOAT4(0, 0, 1, .95f);
			break;
		}
		case 5:
		{
			//INDIGO
			m_XMColor = DirectX::XMFLOAT4(0.2941f, 0, 0.51f, .95f);
			break;
		}
		case 6:
		{
			//RED
			m_XMColor = DirectX::XMFLOAT4(0.9333f, 0.51f, 0.9333f, .95f);
			break;
		}
		default:
		{
			break;
		}
	}

	m_XMColor.x *= 3.0f;
	m_XMColor.y *= 3.0f;
	m_XMColor.z *= 3.0f;

}

