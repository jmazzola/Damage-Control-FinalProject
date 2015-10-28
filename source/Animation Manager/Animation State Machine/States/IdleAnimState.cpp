#include "stdafx.h"
#include "IdleAnimState.h"
#include "../AnimStateMachine.h"
#include "../../../Entity Manager/LivingEntites.h"
#include "../../../State Machine/GameplayState.h"
#include "../../../Input Manager/InputManager.h"

#include "MoveAnimState.h"
#include "DeathAnimState.h"
#include "HitAnimState.h"
#include "AttackAnimState.h"
#include "DeployAnimState.h"

#define TAG_PLAYER "Player_Idle"
#define TAG_HUNTER "Hunter_Idle"


CIdleAnimState::CIdleAnimState()
{
}

CIdleAnimState::~CIdleAnimState()
{
}

CIdleAnimState* CIdleAnimState::GetInstance()
{
	static CIdleAnimState lazy;
	return &lazy;
}

void CIdleAnimState::Enter(CLivingEntities* pEntity)
{
	switch (pEntity->GetEntityType())
	{
		case CEntity::ePLAYER:
		{
			if (pEntity->GetTag() == "Player_Die")
			{
				m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_PLAYER);


				pEntity->GetAnimationBlender().SnapTo(&m_pAnimation->m_pAnim);
				break;
			}

			pEntity->SetTag(TAG_PLAYER);

			m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_PLAYER);

			//pEntity->GetAnimationBlender().TransitionTo(&m_pAnimation->m_pAnim, 1.0f);
			pEntity->GetAnimationBlender().SnapTo(&m_pAnimation->m_pAnim);
			break;
		}

		case CEntity::eHUNTER:
		{
			if (pEntity->GetTag() == "Hunter_Die")
			{
				m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_HUNTER);

				pEntity->GetAnimationBlender().SnapTo(&m_pAnimation->m_pAnim);
				break;
			}

			pEntity->SetTag(TAG_HUNTER);


			m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_HUNTER);

			pEntity->GetAnimationBlender().TransitionTo(&m_pAnimation->m_pAnim, 1.0f);
			break;
		}
	}
}

void CIdleAnimState::Run(CLivingEntities* pEntity, float fDt)
{
	if (pEntity->GetAnimationBlender().IsBlending())
		return;

	// Check if the entity is a player
	switch (pEntity->GetEntityType())
	{
		case CEntity::ePLAYER:
		{
			static bool run_once_nigga = true;
			// If we are no longer idle, move.
			if (!pEntity->IsIdle())
			{
				pEntity->GetAnimStateMachine().ChangeState(CMoveAnimState::GetInstance());
				break;
			}

			// If we're idle and we're hit
			if (pEntity->IsHit())
			{
				pEntity->GetAnimStateMachine().ChangeState(CHitAnimState::GetInstance());
				break;
			}

			// If we're shooting
			if (pEntity->IsAttacking())
			{
				pEntity->GetAnimStateMachine().ChangeState(CAttackAnimState::GetInstance());
				break;
			}

			// If we're dead
			if (!pEntity->GetAlive() && run_once_nigga)
			{
				run_once_nigga = false; //bitch
				pEntity->GetAnimStateMachine().ChangeState(CDeathAnimState::GetInstance());
				break;
			}

			if (pEntity->IsDeploying())
			{
				pEntity->GetAnimStateMachine().ChangeState(CDeployAnimState::GetInstance());
				break;
			}

			break;
		}
		case CEntity::eHUNTER:
		{
			// If we are no longer idle, blend to the moving animation
			if (!pEntity->IsIdle())
			{
				pEntity->GetAnimStateMachine().ChangeState(CMoveAnimState::GetInstance());
				break;
			}

			if (pEntity->IsHit())
			{
				pEntity->GetAnimStateMachine().ChangeState(CHitAnimState::GetInstance());
				break;
			}

			if (!pEntity->GetAlive())
			{
				pEntity->GetAnimStateMachine().ChangeState(CDeathAnimState::GetInstance());
				break;
			}

			if (pEntity->IsAttacking())
			{
				pEntity->GetAnimStateMachine().ChangeState(CAttackAnimState::GetInstance());
				break;
			}

			break;
		}
	}

}

void CIdleAnimState::Exit(CLivingEntities* pEntity)
{
}