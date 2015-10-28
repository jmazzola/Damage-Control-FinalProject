#include "stdafx.h"
#include "MoveAnimState.h"
#include "../AnimStateMachine.h"
#include "../../../Entity Manager/LivingEntites.h"
#include "../../../Input Manager/InputManager.h"
#include "../../../State Machine/GameplayState.h"

#include "../../../Entity Manager/Entities/Player.h"

#include "IdleAnimState.h"
#include "DeathAnimState.h"
#include "HitAnimState.h"
#include "AttackAnimState.h"

#define TAG_PLAYER		"Player_Run"
#define TAG_PLAYER_BW	"Player_Run_BW"
#define TAG_PLAYER_SL	"Player_Strafe_L"
#define TAG_PLAYER_SR	"Player_Strafe_R"
#define TAG_HUNTER		"Hunter_Run"
#define TAG_SAB			"Sab_Run"
#define TAG_CREEP		"Creep_Run"

CMoveAnimState::CMoveAnimState()
{
}


CMoveAnimState::~CMoveAnimState()
{
}

CMoveAnimState* CMoveAnimState::GetInstance()
{
	static CMoveAnimState lazy;
	return &lazy;
}

void CMoveAnimState::Enter(CLivingEntities* pEntity)
{
	switch (pEntity->GetEntityType())
	{
		case CEntity::ePLAYER:
		{
			std::string tag = "Player_Idle";

			if (pEntity->IsWalkingForward() && pEntity->GetStrafing() == -1)
			{
				tag = TAG_PLAYER;
			}
			else if (pEntity->IsWalkingBackward() && pEntity->GetStrafing() == -1)
			{
				tag = TAG_PLAYER_BW;
			}
			else if (pEntity->IsStrafingLeft() && pEntity->GetWalking() == -1)
			{
				tag = TAG_PLAYER_SL;
			}
			else if (pEntity->IsStrafingRight() && pEntity->GetWalking() == -1)
			{
				tag = TAG_PLAYER_SR;
			}

			// Set it to our tag
			pEntity->SetTag(tag);

			// Set the current animation and blend to it
			m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(tag);

			reinterpret_cast<CPlayer*>(pEntity)->SetAnimation(m_pAnimation);
			//pEntity->GetAnimationBlender().TransitionTo(&m_pAnimation->m_pAnim, 1.0f);
			pEntity->GetAnimationBlender().SnapTo(&m_pAnimation->m_pAnim);

			break;

		}

		case CEntity::eHUNTER:
		{
			// Set it to our tag
			pEntity->SetTag(TAG_HUNTER);

			// Set the current animation and blend to it
			m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_HUNTER);

			reinterpret_cast<CPlayer*>(pEntity)->SetAnimation(m_pAnimation);
			pEntity->GetAnimationBlender().SnapTo(&m_pAnimation->m_pAnim);

			break;
		}

		case CEntity::eSABOTEUR:
		{
			pEntity->SetTag(TAG_SAB);

			// Set the current animation and blend to it
			m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_SAB);

			reinterpret_cast<CPlayer*>(pEntity)->SetAnimation(m_pAnimation);
			pEntity->GetAnimationBlender().TransitionTo(&m_pAnimation->m_pAnim, 1.0f);

			break;
		}

		case CEntity::eCREEP:
		{
			pEntity->SetTag(TAG_CREEP);

			// Set the current animation and blend to it
			m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_CREEP);


			reinterpret_cast<CPlayer*>(pEntity)->SetAnimation(m_pAnimation);
			pEntity->GetAnimationBlender().TransitionTo(&m_pAnimation->m_pAnim, 1.0f);

			break;
		}
	}
}

void CMoveAnimState::Run(CLivingEntities* pEntity, float fDt)
{
	if (pEntity->GetAnimationBlender().IsBlending())
		return;

	// Check if the entity is a player
	switch (pEntity->GetEntityType())
	{
		case CEntity::ePLAYER:
		{
			// If we're running and we're suddenly idle
			if (pEntity->IsIdle())
			{
				pEntity->GetAnimStateMachine().ChangeState(CIdleAnimState::GetInstance());
			}

			// If we're running and we're hit
			if (pEntity->IsHit())
			{
				pEntity->GetAnimStateMachine().ChangeState(CHitAnimState::GetInstance());
			}

			// If we're running and we die
			if (!pEntity->GetAlive())
			{
				pEntity->GetAnimStateMachine().ChangeState(CDeathAnimState::GetInstance());
			}

			break;
		}
		case CEntity::eHUNTER:
		{
			if (pEntity->IsIdle())
			{
				pEntity->GetAnimStateMachine().ChangeState(CIdleAnimState::GetInstance());
			}

			if (pEntity->IsHit())
			{
				pEntity->GetAnimStateMachine().ChangeState(CHitAnimState::GetInstance());
			}

			if (pEntity->IsAttacking())
			{
				pEntity->GetAnimStateMachine().ChangeState(CAttackAnimState::GetInstance());
			}

			if (!pEntity->GetAlive())
			{
				pEntity->GetAnimStateMachine().ChangeState(CDeathAnimState::GetInstance());
			}

			break;
		}

		case CEntity::eSABOTEUR:
		{
			if (pEntity->IsHit())
			{
				pEntity->GetAnimStateMachine().ChangeState(CHitAnimState::GetInstance());
			}

			break;
		}

		case CEntity::eCREEP:
		{
			if (pEntity->IsAttacking())
			{
				pEntity->GetAnimStateMachine().ChangeState(CAttackAnimState::GetInstance());
			}
		}
	}
}

void CMoveAnimState::Exit(CLivingEntities* pEntity)
{
}