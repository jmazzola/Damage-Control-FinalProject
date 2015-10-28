#include "stdafx.h"
#include "AttackAnimState.h"
#include "../AnimStateMachine.h"

#include "../../../Entity Manager/LivingEntites.h"
#include "../../../Input Manager/InputManager.h"
#include "../../../State Machine/GameplayState.h"

#include "../../../Entity Manager/Entities/Player.h"
#include "MoveAnimState.h"
#include "IdleAnimState.h"

//#define TAG_PLAYER "Player_Attack"
#define TAG_HUNTER "Hunter_Attack"
#define TAG_CREEP  "Creep_Attack"
#define TAG_PLAYER_P "Player_Shoot_P"
#define TAG_PLAYER_MG "Player_Shoot_MG"
#define TAG_PLAYER_S "Player_Shoot_S"

CAttackAnimState::CAttackAnimState()
{
}

CAttackAnimState::~CAttackAnimState()
{
}

CAttackAnimState* CAttackAnimState::GetInstance()
{
	static CAttackAnimState lazy;
	return &lazy;
}

void CAttackAnimState::Enter(CLivingEntities* pEntity)
{
	switch (pEntity->GetEntityType())
	{
		case CEntity::ePLAYER:
		{
			if (pEntity->IsIdle())
			{
				CIWeapon* weap = reinterpret_cast<CPlayer*>(pEntity)->GetCurrentWeapon();
				std::string tag;

				if (weap == reinterpret_cast<CPlayer*>(pEntity)->GetPistol())
				{
					tag = TAG_PLAYER_P;
				}
				else if (weap == reinterpret_cast<CPlayer*>(pEntity)->GetMachineGun())
				{
					tag = TAG_PLAYER_MG;
				}
				else if (weap == reinterpret_cast<CPlayer*>(pEntity)->GetShotgun())
				{
					tag = TAG_PLAYER_S;
				}
				else
				{
					break;
				}

				pEntity->SetTag(tag);
				CAssetManager::TAnimation* next_animation = CGameplayState::GetAssetManager()->GetAnimByTag(tag);
				reinterpret_cast<CPlayer*>(pEntity)->SetAnimation(next_animation);
				pEntity->GetAnimationBlender().SnapTo(&next_animation->m_pAnim);
				break;
			}
		}
		case CEntity::eHUNTER:
		{
			// Play attack animation
			pEntity->SetTag(TAG_HUNTER);
			CAssetManager::TAnimation* next_animation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_HUNTER);

			reinterpret_cast<CPlayer*>(pEntity)->SetAnimation(next_animation);

			pEntity->GetAnimationBlender().SnapTo(&next_animation->m_pAnim);
			break;
		}

		case CEntity::eCREEP:
		{
			// Play attack animation
			pEntity->SetTag(TAG_CREEP);
			CAssetManager::TAnimation* next_animation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_CREEP);

			reinterpret_cast<CPlayer*>(pEntity)->SetAnimation(next_animation);

			pEntity->GetAnimationBlender().SnapTo(&next_animation->m_pAnim);/*, 1.0f);*/
			break;
		}
	}
}

void CAttackAnimState::Run(CLivingEntities* pEntity, float fDt)
{
	if (pEntity->GetAnimationBlender().IsBlending())
		return;

	switch (pEntity->GetEntityType())
	{
		case CEntity::ePLAYER:
		{
			if (!pEntity->IsAttacking())
			{
				if (pEntity->IsIdle())
				{
					pEntity->GetAnimStateMachine().ChangeState(CIdleAnimState::GetInstance());
					break;
				}
				else
				{
					pEntity->GetAnimStateMachine().ChangeState(CMoveAnimState::GetInstance());
					break;
				}
			}
			else if (pEntity->IsAttacking() && !pEntity->IsIdle())
			{
				pEntity->GetAnimStateMachine().ChangeState(CMoveAnimState::GetInstance());
				break;
			}
		}
		case CEntity::eCREEP:
		case CEntity::eHUNTER:
		{
			if (!pEntity->IsAttacking())
			{
				if (pEntity->IsIdle())
				{
					pEntity->GetAnimStateMachine().ChangeState(CIdleAnimState::GetInstance());
					break;
				}
				else
				{
					pEntity->GetAnimStateMachine().ChangeState(CMoveAnimState::GetInstance());
					break;
				}
			}
		}
	}

}

void CAttackAnimState::Exit(CLivingEntities* pEntity)
{
}
