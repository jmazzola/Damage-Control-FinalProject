#include "stdafx.h"
#include "DroneBuddy.h"
#include "../State Machine/GameplayState.h"
#include "../Entity Manager/EntityManager.h"
#include "../Entity Manager/Entities/Player.h"
#include "../Renderer/DebugRenderer.h"
#include "../Audio Manager/AudioSystemWwise.h"
#include "../Renderer/BitmapFont.h"
#include "ModBank.h"

#include "../Particle Manager/ParticleSystem.h"
#include "../Particle Manager/ParticleManager.h"
#include "../Particle Manager/ParticleEmitter.h"

using namespace MathHelper;

#define LIVETIMER 15.0f
#define COOLDOWN 20.0f
#define EXP_DAMAGE 50.0f
#define EXP_RANGE 200.0f
#define REPAIR_SPEED 1.0f
#define RANGE 600.0f

#define MODBANK_SIZE 3

CDroneBuddy::CDroneBuddy(CPlayer* Player) : CIWeapon(GUN_DRONE)
{
	m_szTag = "DroneBuddy";

	m_fDamage = 20.0f;
	m_fFireRate = 3.0f;
	m_fSpread = 0.0f;
	m_fAimTimer = 1.0f;
	m_fCooldownSpeed = 1.0f;
	m_fLifeMultiplier = 1.0f;
	m_fRange = RANGE;
	m_fCooldownTimer = COOLDOWN;
	m_bUnlocked = false;
	m_bReady = false;

	m_pTarget = nullptr;
	m_pPlayer = Player;

	m_pModules = new CModBank(EMOD_GADGET);
	m_pModules->SetBankSize(MODBANK_SIZE);

	if (!m_pLightParams)
	{
		m_pLightParams = new LightParams( );

		m_pLightParams->Color = { 1.0f, 1.0f, 1.0f };
		m_pLightParams->Direction = { 0, -1, 0 };
		m_pLightParams->Position = { 0, 0, 0 };
		m_pLightParams->Range = 300.0f;
		m_pLightParams->SpotInnerAngle = .98f;
		m_pLightParams->SpotOuterAngle = .7f;
		m_pLightParams->Type = LightType::Spot;
	}


	m_pPoofEffect = CParticleSystem::LoadFrom("Assets/XML/Particles/DroneExplode.xml");
	CParticleManager::GetInstance()->AddParticleSystem(m_pPoofEffect);

}


CDroneBuddy::~CDroneBuddy( )
{
	SafeDelete(m_pLightParams);
}

void CDroneBuddy::Update(float dt)
{
	m_fTotalTime += dt;

	// Cooldown time
	if (m_fCooldownTimer > 0)
	{
		m_fCooldownTimer -= dt * m_fCooldownSpeed;
		if (m_fCooldownTimer < 0)
		{
			m_fCooldownTimer = 0;
			m_bReady = true;
		}
		return;
	}

	if (m_fLiveTimer > 0)
	{
		CDebugRenderer::GetReference().CircleRenderer(DirectX::XMFLOAT3{ GetPosition().x, 6.0f, GetPosition().z }, { RANGE, RANGE });

		DirectX::XMStoreFloat4x4(&MyWorld.Local,
								 DirectX::XMLoadFloat4x4(&MyWorld.Local) * (DirectX::XMMatrixTranslation(0, sin(m_fLiveTimer) * dt * 25.0f, 0)));

		m_fLiveTimer -= dt;
		if (m_fLiveTimer < 0)
		{
			m_fLiveTimer = 0;
			Disable();
			return;
		}


		FindTarget();

		if (m_pTarget)
		{
			if (m_pTarget->GetAlive())
			{
				TurnToTarget();
				Shoot();
			}
		}

		RepairSubSystem(dt);
	}
	
}

void CDroneBuddy::Enable( )
{
	AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_2D_DRONE_DEPLOY);
	MyWorld.Local = m_pPlayer->GetWorldMatrix();
	MyWorld.Local._42 += 90;
	isActive = true;
	m_bReady = false;
	m_fLiveTimer = LIVETIMER * m_fLifeMultiplier;
	ResetDroneStats();

	m_pPoofEffect->SetPosition(GetPosition());
	m_pPoofEffect->GetEmitter(0)->SetStartColor(DirectX::XMFLOAT4(1, 1, 0, 1));
	m_pPoofEffect->GetEmitter(0)->SetEndColor(DirectX::XMFLOAT4(1, 0, 0, 1));
	m_pPoofEffect->GetEmitter(0)->Emit();
}

void CDroneBuddy::Disable( )
{
	if (isActive)
	{
		Explode();
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_DRONE_SHUTDOWN);

		m_pPoofEffect->SetPosition(GetPosition());
		m_pPoofEffect->GetEmitter(0)->SetStartColor(DirectX::XMFLOAT4(1, 0, 0, 1));
		m_pPoofEffect->GetEmitter(0)->SetEndColor(DirectX::XMFLOAT4(1, 1, 0, 1));
		m_pPoofEffect->GetEmitter(0)->Emit();
	}
	isActive = false;
	m_pTarget = nullptr;
	m_fCooldownTimer = COOLDOWN;

}

void CDroneBuddy::FindTarget( )
{
	MergeLists();
	int ListSize = m_lpAllEnemies.size();
	
	float ClosestEnemy = FLT_MAX;
	XMFLOAT3 myPos = GetPosition();

	if (ListSize > 1)
	{
		for (int i = 0; i < ListSize; i++)
		{
			// Destination - Source
			XMFLOAT3 ToEnemy = m_lpAllEnemies[i]->GetPosition() - myPos;
			XMFLOAT3 Distance;
			DirectX::XMStoreFloat3(&Distance, DirectX::XMVector3LengthSq(DirectX::XMLoadFloat3(&ToEnemy)));
			
			float Dist = Distance.x;

			if (Dist < ClosestEnemy && Dist < (m_fRange * m_fRange))
			{
				ClosestEnemy = Dist;
				m_pTarget = m_lpAllEnemies[i];
			}
		}
	}

}

void CDroneBuddy::TurnToTarget()
{
	DirectX::XMFLOAT3 eye = GetPosition();
	DirectX::XMFLOAT3 at = m_pTarget->GetPosition();

	DirectX::XMFLOAT3 x, y, z;
	DirectX::XMStoreFloat3(&z, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&(at - eye))));

	DirectX::XMStoreFloat3(&x, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 1, 0)), DirectX::XMLoadFloat3(&z)));
	DirectX::XMStoreFloat3(&x, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&x)));

	DirectX::XMStoreFloat3(&y, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&z), DirectX::XMLoadFloat3(&x)));
	DirectX::XMStoreFloat3(&y, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&y)));

	DirectX::XMMATRIX matrix(
		DirectX::XMLoadFloat3(&x),
		DirectX::XMLoadFloat3(&y),
		DirectX::XMLoadFloat3(&z),
		DirectX::XMLoadFloat3(&eye));

	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMStoreFloat4x4(&worldMatrix, matrix);
	worldMatrix._41 = MyWorld.GetWorld()._41;
	worldMatrix._42 = MyWorld.GetWorld()._42;
	worldMatrix._43 = MyWorld.GetWorld()._43;
	worldMatrix._44 = MyWorld.GetWorld()._44;

	MyWorld.Local = worldMatrix;
}

bool CDroneBuddy::Shoot( )
{
	if (m_fTotalTime > m_fLastFireTime + 1 / m_fFireRate)
	{
		//AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_PISTOL_SHOOT);

		if (CGameplayState::GetEntityManager( )->CycleIntoLiveList(CEntity::eBULLET, { 0, 0, 0 }, this))
		{
			//player->SubScrap(1);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_DRONE_SHOOT);
		}

		m_fLastFireTime = m_fTotalTime;
		return true;
	}
	return false;
}

#include "../Entity Manager/AI/CreepEnemy.h"
#include "../Entity Manager/AI/HunterEnemy.h"
#include "../Entity Manager/AI/SaboteurEnemy.h"

void CDroneBuddy::MergeLists( )
{
	m_lpAllEnemies.clear();

	std::list<CCreepEnemy*>Creeps = CGameplayState::GetEntityManager( )->GetAllCreeps( );
	std::list<CHunterEnemy*>Hunters = CGameplayState::GetEntityManager( )->GetAllHunters( );
	std::list<CSaboteurEnemy*>Sabos = CGameplayState::GetEntityManager( )->GetAllSaboteur( );

	if (Creeps.size( ) > 0)
	{
		std::list<CCreepEnemy*>::iterator iter = Creeps.begin( );
		for (iter; iter != Creeps.end( ); iter++)
		{
			m_lpAllEnemies.push_back(*iter);
		}
	}
	if (Hunters.size( ) > 0)
	{
		std::list<CHunterEnemy*>::iterator iter = Hunters.begin( );
		for (iter; iter != Hunters.end( ); iter++)
		{
			m_lpAllEnemies.push_back(*iter);
		}
	}
	if (Sabos.size( ) > 0)
	{
		std::list<CSaboteurEnemy*>::iterator iter = Sabos.begin( );
		for (iter; iter != Sabos.end( ); iter++)
		{
			m_lpAllEnemies.push_back(*iter);
		}
	}
}

void CDroneBuddy::UpdateEnemyW(float dt, CEntity* owner)
{
}

void CDroneBuddy::Explode()
{
	unsigned int size = m_lpAllEnemies.size();
	XMFLOAT3 myPos = GetPosition();

	for (size_t i = 0; i < size; i++)
	{
		XMFLOAT3 ToEnemy = m_lpAllEnemies[i]->GetPosition() - myPos;
		XMFLOAT3 Distance;
		DirectX::XMStoreFloat3(&Distance, DirectX::XMVector3LengthSq(DirectX::XMLoadFloat3(&ToEnemy)));

		float Dist = Distance.x;

		if (Dist < (m_fExplosionRange * m_fExplosionRange)) // Squared Distance
		{
			m_lpAllEnemies[i]->TakeDamage(m_fExplosionDamage);
		}

	}
}

#include "../Entity Manager/Entities/Subsystems/Subsystem.h"
void CDroneBuddy::RepairSubSystem(float dt)
{
	auto SystemList = CGameplayState::GetEntityManager()->GetSubSystems();
	auto iter = SystemList.begin();
	
	for (iter; iter != SystemList.end(); iter++)
	{
		XMFLOAT3 ToSystem = (*iter)->GetPosition() - GetPosition();
		XMFLOAT3 Distance;
		DirectX::XMStoreFloat3(&Distance, DirectX::XMVector3LengthSq(DirectX::XMLoadFloat3(&ToSystem)));

		float Dist = Distance.x;

		if (Dist < (m_fRange * m_fRange))
		{
			(*iter)->Heal(m_fRepairSpeed * dt);
		}
	}
}

//Returns how long the drone will take to cooldown directly after use
float CDroneBuddy::GetCoolDownTime()
{
	return COOLDOWN / m_fCooldownSpeed;
}
//Returns how long the drone will live immediately after being deployed
float CDroneBuddy::GetLifeTime()
{
	return LIVETIMER * m_fLifeMultiplier;
}

void CDroneBuddy::ResetDroneStats(void)
{
	m_fExplosionDamage = EXP_DAMAGE;
	m_fExplosionRange = EXP_RANGE;
	m_fRepairSpeed = REPAIR_SPEED;
}