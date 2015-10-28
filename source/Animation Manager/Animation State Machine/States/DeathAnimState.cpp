#include "stdafx.h"
#include "DeathAnimState.h"
#include "../AnimStateMachine.h"
#include "../../../Entity Manager/LivingEntites.h"
#include "../../../Input Manager/InputManager.h"
#include "../../../State Machine/GameplayState.h"

#include "../../../Entity Manager/Entities/Player.h"
#include "IdleAnimState.h"

#define TAG_PLAYER "Player_Die"
#define TAG_HUNTER "Hunter_Die"

CDeathAnimState::CDeathAnimState()
{
}


CDeathAnimState::~CDeathAnimState()
{
}

CDeathAnimState* CDeathAnimState::GetInstance()
{
	static CDeathAnimState lazy;
	return &lazy;
}

void CDeathAnimState::Enter(CLivingEntities* pEntity)
{
	switch (pEntity->GetEntityType())
	{
		case CEntity::ePLAYER:
		{
			pEntity->SetTag(TAG_PLAYER);
			CAssetManager::TAnimation* next_animation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_PLAYER);


			reinterpret_cast<CPlayer*>(pEntity)->SetAnimation(next_animation);

			pEntity->GetAnimationBlender().TransitionTo(&next_animation->m_pAnim, 1.0f);
			break;
		}

		case CEntity::eHUNTER:
		{
			pEntity->SetTag(TAG_HUNTER);
			CAssetManager::TAnimation* next_animation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_HUNTER);

			reinterpret_cast<CPlayer*>(pEntity)->SetAnimation(next_animation);

			pEntity->GetAnimationBlender().TransitionTo(&next_animation->m_pAnim, 1.0f);
			break;
		}
	}
}

void CDeathAnimState::Run(CLivingEntities* pEntity, float fDt)
{
	if (pEntity->GetAnimationBlender().IsBlending())
		return;

	switch (pEntity->GetEntityType())
	{
		case CEntity::ePLAYER:
		case CEntity::eHUNTER:
		{
			if (pEntity->GetAlive())
			{
				pEntity->GetAnimStateMachine().ChangeState(CIdleAnimState::GetInstance());
			}
		}
	}
}

void CDeathAnimState::Exit(CLivingEntities* pEntity)
{
}