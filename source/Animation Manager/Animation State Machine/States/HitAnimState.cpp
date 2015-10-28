#include "stdafx.h"
#include "HitAnimState.h"

#include "../AnimStateMachine.h"
#include "../../../Entity Manager/LivingEntites.h"
#include "../../../Input Manager/InputManager.h"
#include "../../../State Machine/GameplayState.h"

#include "../../../Entity Manager/Entities/Player.h"

#include "IdleAnimState.h"
#include "MoveAnimState.h"
#include "DeathAnimState.h"

#define TAG_PLAYER "Player_Hit"
#define TAG_HUNTER "Hunter_Hit"
#define TAG_SAB	   "Sab_Hit"

CHitAnimState::CHitAnimState()
{
}


CHitAnimState::~CHitAnimState()
{
}

CHitAnimState* CHitAnimState::GetInstance()
{
	static CHitAnimState lazy;
	return &lazy;
}

void CHitAnimState::Enter(CLivingEntities* pEntity)
{
	switch (pEntity->GetEntityType())
	{
		case CEntity::ePLAYER:
		{
			pEntity->SetTag(TAG_PLAYER);

			m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_PLAYER);

			reinterpret_cast<CPlayer*>(pEntity)->SetAnimation(m_pAnimation);
			//pEntity->GetAnimationBlender().TransitionTo(&m_pAnimation->m_pAnim, 1.0f);
			pEntity->GetAnimationBlender().SnapTo(&m_pAnimation->m_pAnim);

			break;
		}

		case CEntity::eHUNTER:
		{
			pEntity->SetTag(TAG_HUNTER);

			m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_HUNTER);


			reinterpret_cast<CPlayer*>(pEntity)->SetAnimation(m_pAnimation);
			//pEntity->GetAnimationBlender().TransitionTo(&m_pAnimation->m_pAnim, 1.0f);
			pEntity->GetAnimationBlender().SnapTo(&m_pAnimation->m_pAnim);

			break;
		}

		case CEntity::eSABOTEUR:
		{
			pEntity->SetTag(TAG_SAB);

			m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_SAB);

			reinterpret_cast<CPlayer*>(pEntity)->SetAnimation(m_pAnimation);
			//pEntity->GetAnimationBlender().TransitionTo(&m_pAnimation->m_pAnim, 1.0f);
			pEntity->GetAnimationBlender().SnapTo(&m_pAnimation->m_pAnim);
			break;
		}
	}
}

void CHitAnimState::Run(CLivingEntities* pEntity, float fDt)
{
	if (pEntity->GetAnimationBlender().IsBlending())
		return;

	// Check if the entity is a player
	switch (pEntity->GetEntityType())
	{
		case CEntity::ePLAYER:
		case CEntity::eHUNTER:
		{
			if (!pEntity->IsHit())
			{
				switch (pEntity->IsIdle())
				{
					case TRUE:
					{
						pEntity->GetAnimStateMachine().ChangeState(CIdleAnimState::GetInstance());
						break;
					}
					case FALSE:
					{
						pEntity->GetAnimStateMachine().ChangeState(CMoveAnimState::GetInstance());
						break;
					}
				}
			}

			if (!pEntity->GetAlive())
			{
				pEntity->GetAnimStateMachine().ChangeState(CDeathAnimState::GetInstance());
			}

			break;
		}

		case CEntity::eSABOTEUR:
		{
			if (!pEntity->IsHit())
			{
				pEntity->GetAnimStateMachine().ChangeState(CMoveAnimState::GetInstance());
				break;
			}
		}
		break;
	}
}

void CHitAnimState::Exit(CLivingEntities* pEntity)
{
}