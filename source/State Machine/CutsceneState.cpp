#include "stdafx.h"
#include "CutsceneState.h"
#include "GameplayState.h"
#include "PauseState.h"
#include "LoseState.h"

#include "../GUI Manager/GUIManager.h"

#include "../Audio Manager/AudioSystemWwise.h"

#include "../Input Manager/InputManager.h"

#include "../Renderer/Scene.h"
#include "../Renderer/FirstPersonCamera.h"

#include "../Particle Manager/ParticleManager.h"
#include "../Particle Manager/ParticleEmitter.h"
#include "../Particle Manager/ParticleSystem.h"

#include "../Entity Manager/EntityManager.h"
#include "../Entity Manager/Entities/Subsystems/Reactor.h"
#include "../Entity Manager/Entities/Subsystems/Engines.h"

#define DIST_THRESHOLD 350.0f
#define CAMERASPEED 500.f

CCutsceneState::CCutsceneState() : CBaseState(STATE_CUTSCENE)
{
	m_bTransparent = true;

}

/*virtual*/ void CCutsceneState::Input()
{
}

/*virtual*/ void CCutsceneState::Update(float fDT)
{
	if (m_pPrevState)
	{
		m_pPrevState->Update(fDT);
	}

	DirectX::XMFLOAT3 toVec = { 0, 0, 0 };
	if (m_iCurrentNode < (int)m_vNodes.size())
	{
		toVec = m_vNodes[m_iCurrentNode].m_xmPos - m_pFPSCam->Position();

		DirectX::XMFLOAT3 speed, myDir;
		myDir = Normalize(toVec);
		speed = myDir * CAMERASPEED * fDT;

		m_pFPSCam->UpdateWorld(speed);
	}

	switch (m_Reason)
	{
		case REASON_BLOWNUP:
		{
			// Hoh, smoke.
			for (int i = 0; i < ARRAYSIZE(m_pSmokeEffects); i++)
			{
				if (m_pSmokeEffects[i])
				{
					m_pSmokeEffects[i]->SetPosition(m_xmSmokePositions[i]);

					if (m_pSmokeEffects[i]->GetEmitter(0))
					{
						m_pSmokeEffects[i]->GetEmitter(0)->Emit();
					}
				}
			}

			// It's electric. Boogie woogie woogie. Goddamn, it's late.
			for (int i = 0; i < ARRAYSIZE(m_pSparkEffects); i++)
			{
				if (m_pSparkEffects[i])
				{
					m_pSparkEffects[i]->SetPosition(m_xmSparkPositions[i]);

					if (m_pSparkEffects[i]->GetEmitter(0))
					{
						m_pSparkEffects[i]->GetEmitter(0)->Emit();
					}
				}
			}

			break;
		}

		case REASON_EATEN:
		{
			// I've been murdered by an aliennnn!!....ughh.....
			for (int i = 0; i < ARRAYSIZE(m_pBloodEffects); i++)
			{
				if (m_pBloodEffects[i])
				{
					m_pBloodEffects[i]->SetPosition(m_xmBloodPositions[i]);

					if (m_pBloodEffects[i]->GetEmitter(0))
					{
						m_pBloodEffects[i]->GetEmitter(0)->SetStartColor(DirectX::XMFLOAT4(0.67f, 0.0f, 0.0f, 1.0f));
						m_pBloodEffects[i]->GetEmitter(0)->SetEndColor(DirectX::XMFLOAT4(0.96f, 0.0f, 0.0f, 0.0f));
						m_pBloodEffects[i]->GetEmitter(0)->Emit();
					}
				}
			}

			break;
		}

		case REASON_SUFFOCATE:
		{
			// Idk. Show a black vignette shit? I guess we'll just do the same thing as eaten

			// I've been murdered by an aliennnn!!....ughh.....
			for (int i = 0; i < ARRAYSIZE(m_pBloodEffects); i++)
			{
				if (m_pBloodEffects[i])
				{
					m_pBloodEffects[i]->SetPosition(m_xmBloodPositions[i]);

					if (m_pBloodEffects[i]->GetEmitter(0))
					{
						m_pBloodEffects[i]->GetEmitter(0)->SetStartColor(DirectX::XMFLOAT4(0.67f, 0.0f, 0.0f, 1.0f));
						m_pBloodEffects[i]->GetEmitter(0)->SetEndColor(DirectX::XMFLOAT4(0.96f, 0.0f, 0.0f, 0.0f));
						m_pBloodEffects[i]->GetEmitter(0)->Emit();
					}
				}
			}

			break;
		}

		case REASON_WIN:
		{

			break;
		}
	}

	//// If we're closing on the node, go to the next one
	if (abs(Length(toVec)) < FLT_EPSILON + DIST_THRESHOLD)
	{
		m_iCurrentNode++;

		if (m_iCurrentNode >= (int)m_vNodes.size())
		{
			QueueStateChange(eStateChange::STATE_CH_LOSE);

			//OutputDebugString("I wanna fucking go home");
			//OutputDebugString("\n");
		}
	}
}

/*virtual*/ void CCutsceneState::Render(CAssetManager* pRenderer)
{
	if (m_pPrevState)
	{
		m_pPrevState->Render(pRenderer);
	}
}

/*virtual*/ bool CCutsceneState::FirstEnter()
{
	CBaseState::FirstEnter();

	m_iCurrentNode = 0;

	for (int i = 0; i < ARRAYSIZE(m_pSmokeEffects); i++)
	{
		m_pSmokeEffects[i] = CParticleSystem::LoadFrom("Assets/XML/Particles/CutsceneSmokeParticle.xml");
		CParticleManager::GetInstance()->AddParticleSystem(m_pSmokeEffects[i]);
	}

	for (int i = 0; i < ARRAYSIZE(m_pBloodEffects); i++)
	{
		m_pBloodEffects[i] = CParticleSystem::LoadFrom("Assets/XML/Particles/CutsceneBloodParticle.xml");
		CParticleManager::GetInstance()->AddParticleSystem(m_pBloodEffects[i]);
	}

	for (int i = 0; i < ARRAYSIZE(m_pSparkEffects); i++)
	{
		m_pSparkEffects[i] = CParticleSystem::LoadFrom("Assets/XML/Particles/CutsceneSparkParticle.xml");
		CParticleManager::GetInstance()->AddParticleSystem(m_pSparkEffects[i]);
	}

	m_xmSmokePositions[0] = { -1013.06f, 0.0f, 121.1f };
	m_xmSmokePositions[1] = { -40.6f, 0.0f, -530.93f };
	m_xmSmokePositions[2] = { 406.8f, 0.0f, -1758.2f };
	m_xmSmokePositions[3] = { -25.5f, 0.0f, 958.19f };
	m_xmSmokePositions[4] = { 1116.8f, 0.0f, -46.56f };


	m_xmSparkPositions[0] = { -391.997375f, 0.000000f, 167.443451f };
	m_xmSparkPositions[1] = { 405.560913f, 0.000000f, 83.531586f };
	m_xmSparkPositions[2] = { -782.092957f, 0.000000f, -539.664795f };
	m_xmSparkPositions[3] = { -22.648188f, 0.000000f, -791.861938f };
	m_xmSparkPositions[4] = { 1.677990f, 0.000000f, -1787.809326f };
	m_xmSparkPositions[5] = { 16.612629f, 0.000000f, 1240.109131f };
	m_xmSparkPositions[6] = { 763.208679f, 0.000000f, 350.007111f };
	m_xmSparkPositions[7] = { 1109.401978f, 0.000000f, -137.396072f };
	m_xmSparkPositions[8] = { 1258.830688f, 0.000000f, -1916.795410f };
	m_xmSparkPositions[9] = { -16.915432f, 0.000000f, -2616.470215f };

	DetermineReason();

	m_pFPSCam = new CFirstPersonCamera();
	if (m_pPrevState)
	{
		m_pPrevState->GetScene()->AddCamera(m_pFPSCam);
	}

	m_pFPSCam->SetPosition(CGameplayState::GetEntityManager()->GetPlayer()->GetPosition());

	DirectX::XMFLOAT3 playerPos = CGameplayState::GetEntityManager()->GetPlayer()->GetPosition();
	for (int i = 0; i < ARRAYSIZE(m_xmBloodPositions); i++)
	{
		float x = ((float(rand()) / float(RAND_MAX)) * (5 + 5)) - 5;
		float y = ((float(rand()) / float(RAND_MAX)) * (10 - 1)) - 1;
		float z = ((float(rand()) / float(RAND_MAX)) * (10 + 10)) - 10;

		m_xmBloodPositions[i] = DirectX::XMFLOAT3(playerPos.x + x, playerPos.y + y, playerPos.z + z);
	}

	return true;
}

bool CCutsceneState::ReEnter(void)
{
	return CBaseState::ReEnter();
}

/*virtual*/ bool CCutsceneState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded())
	{
		return true;
	}

	CBaseState::Load(pAssets);

	m_iCurrentNode = 0;
	m_fTimer = 0.0f;

	//m_vNodes.push_back(TFrame(DirectX::XMFLOAT3(910.90f, 1506.43f, -201.4f), DirectX::XMFLOAT2(0.68f, -0.99f)));
	m_vNodes.push_back(TFrame(DirectX::XMFLOAT3(1073.75f, 1981.97f, -1282.61f), DirectX::XMFLOAT2(0.88f, -1.35f)));		// cool angle.

	//m_vNodes.push_back(TFrame(DirectX::XMFLOAT3(447.89f, 1632.11f, -1582.21f), DirectX::XMFLOAT2(0.8f, -1.00f)));
	//m_vNodes.push_back(TFrame(DirectX::XMFLOAT3(-9.9f, 1676.0f, -2378.04f), DirectX::XMFLOAT2(0.8f, -1.00f)));
	//m_vNodes.push_back(TFrame(DirectX::XMFLOAT3(-745.67f, 1610.3f, -2161.43f), DirectX::XMFLOAT2(0.9f, 0.5f)));
	//m_vNodes.push_back(TFrame(DirectX::XMFLOAT3(-1059.02f, 1610.3f, -1366.69f), DirectX::XMFLOAT2(0.9f, 1.43f)));
	//m_vNodes.push_back(TFrame(DirectX::XMFLOAT3(-884.4f, 1424.11f, -364.11f), DirectX::XMFLOAT2(1.0f, 2.4f)));

	return true;
}

/*virtual*/ void CCutsceneState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded())
	{
		return;
	}

	CBaseState::UnLoad();
}

/*virtual*/ bool CCutsceneState::Exit(void)
{
	if (!GetEntered())
	{
		return false;
	}

	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_ALL);
	CBaseState::Exit();

	return false;
}

/*virtual*/ eStateType CCutsceneState::CheckState(void)
{
	eStateType eNextState = GetType();

	if (m_bStateChanges[STATE_CH_LOSE] && m_iCurrentNode > (int)m_vNodes.size())
	{
		eNextState = STATE_LOSE;
	}

	if (CBaseState::CheckState() == STATE_NULL)
	{
		eNextState = STATE_NULL;
	}

	return eNextState;
}

void CCutsceneState::DetermineReason()
{
	// Engine, Reactor, LS
	std::list<CSubSystem*> subsystems = CGameplayState::GetEntityManager()->GetSubSystems();

	CReactor* reactor = NULL;
	CEngines* engine = NULL;

	for (auto iter = subsystems.begin(); iter != subsystems.end(); iter++)
	{
		if (dynamic_cast<CReactor*>(*iter))
			reactor = static_cast<CReactor*>(*iter);

		if (dynamic_cast<CEngines*>(*iter))
			engine = static_cast<CEngines*>(*iter);
	}

	if (reactor->GetMeltdownTimer() <= 0.0f)
	{
		m_Reason = REASON_BLOWNUP;
	}

	if (CGameplayState::GetEntityManager()->GetPlayer()->GetCurrentHealth() <= 0.0f)
	{
		m_Reason = REASON_EATEN;
	}
	else if (CGameplayState::GetEntityManager()->GetPlayer()->GetOxygen() <= 0.0f)
	{
		m_Reason = REASON_SUFFOCATE;
	}
	else if (engine->IsComplete())
	{
		m_Reason = REASON_WIN;
	}

}