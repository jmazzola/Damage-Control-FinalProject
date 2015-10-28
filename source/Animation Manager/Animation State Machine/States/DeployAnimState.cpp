#include "stdafx.h"
#include "DeployAnimState.h"
#include "../AnimStateMachine.h"
#include "../../../Entity Manager/LivingEntites.h"
#include "../../../Input Manager/InputManager.h"
#include "../../../State Machine/GameplayState.h"

#include "../../../Entity Manager/Entities/Player.h"

#include "MoveAnimState.h"
#include "IdleAnimState.h"
#include "DeathAnimState.h"
#include "HitAnimState.h"
#include "AttackAnimState.h"

#define TAG_PLAYER "Player_Drone"

CDeployAnimState::CDeployAnimState()
{
}


CDeployAnimState::~CDeployAnimState()
{
}

CDeployAnimState* CDeployAnimState::GetInstance()
{
	static CDeployAnimState lazy;
	return &lazy;
}

void CDeployAnimState::Enter(CLivingEntities* pEntity)
{
	switch (pEntity->GetEntityType())
	{
		case CEntity::ePLAYER:
		{
			// Set it to our tag
			pEntity->SetTag(TAG_PLAYER);

			// Set the current animation and blend to it
			m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(TAG_PLAYER);

			reinterpret_cast<CPlayer*>(pEntity)->SetAnimation(m_pAnimation);
			pEntity->GetAnimationBlender().TransitionTo(&m_pAnimation->m_pAnim, 1.0f);

			break;
		}
	}
}

void CDeployAnimState::Run(CLivingEntities* pEntity, float fDt)
{
	if (pEntity->GetAnimationBlender().IsBlending())
		return;

	// Check if the entity is a player
	switch (pEntity->GetEntityType())
	{
		case CEntity::ePLAYER:
		{
			// If we're running and we're suddenly idle
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
	}
}

void CDeployAnimState::Exit(CLivingEntities* pEntity)
{
}