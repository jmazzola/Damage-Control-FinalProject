/***********************************************
* Filename:			Reactor.cpp
* Date:      		05/22/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:
************************************************/
#include "Reactor.h"
#include "../../../State Machine/GameplayState.h"
#include "../../../Asset Manager/DDSTextureLoader.h"
#include "../../../Renderer/Renderer.h"
#include "../../../Audio Manager/AudioSystemWwise.h"
#include "../../../TinyXML2/tinyxml2.h"

#define CORE_MIN_Y -18.0f
#define CORE_MAX_Y 18.0f
#define CORE_BOUNCE_SPEED 16.0f

#define CORE_MIN_SCALE 0.85f
#define CORE_MAX_SCALE 1.15f
#define CORE_HARDMAX_SCALE 1.45f
#define CORE_STRETCH_SPEED 0.05f

#define CORE_SPIN_SPEED 20.0f
#define BROKEN_CORE_SPIN_SPEED 1.0f
#define SERVO_SPIN_SPEED -3.8f
#define BASE_SPIN_SPEED 3.6f

#define LIGHT_MAX_RANGE 420.0f
#define LIGHT_MIN_RANGE 300.0f
#define LIGHT_SWITCH_SPEED 20.0f

#define CORE_OFFSET 136.0f

using namespace MathHelper;

namespace
{
	XMFLOAT3 GetAABBRadii(const TAABB* pCollider)
	{
		XMFLOAT3 xmReturn;
		xmReturn.x = fabs(pCollider->vMin.x - pCollider->vMax.x);
		xmReturn.y = fabs(pCollider->vMin.y - pCollider->vMax.y);
		xmReturn.z = fabs(pCollider->vMin.z - pCollider->vMax.z);
		return xmReturn;
	}
}
/*****************************************************************
* CReactor()
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/28/2015
* Mod. Initials:
*****************************************************************/
CReactor::CReactor() : m_fMeltdownTimer(m_fMaxMeltdownTime)
{
	LoadStats();

	//TODO: Move to tutorial level
	m_fCurrentHealth = 0;

	m_pLightParams = new LightParams(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 5, 0), 200.0f);

	m_szTag = "ReactorCap";
	tCollider = new TAABB;
	tCollider->type = TCollider::eAABB;
	//Set up the core's info
	m_pCore = new CRObject();
	m_pCore->SetTag("ReactorCore");
	//Set up the servos's stuff
	m_pServos = new CRObject();
	m_pServos->SetTag("ReactorServos");
	//Set up the base
	m_pBase = new CRObject();
	m_pBase->SetTag("ReactorBase");

	m_fCoreHeight = (CORE_MAX_Y + CORE_MIN_Y) * 0.5f;
	m_fCoreScale = (CORE_MAX_SCALE + CORE_MIN_SCALE) * 0.5f;

	AudioSystemWwise::Get( )->RegisterEntity(this, "Reactor");
}

/*****************************************************************
* ~CReactor()
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/28/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ CReactor::~CReactor(void)
{
	AudioSystemWwise::Get( )->UnRegisterEntity(this);
	delete tCollider;
	tCollider = nullptr;
	SafeDelete(m_pCore);
	SafeDelete(m_pServos);
	SafeDelete(m_pBase);
}

/*****************************************************************
* SetPosition() - sets the position of the reactor
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/28/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CReactor::SetPosition(DirectX::XMFLOAT3& position) /*override*/
{
	XMFLOAT3 xmMovement = position - GetPosition();
	(tCollider)->vMin = tCollider->vMin + xmMovement;
	(tCollider)->vMax = tCollider->vMax + xmMovement;
	if (m_pCore)
	{
		position.y += 136;
		m_pCore->SetPosition(position);
		m_pLightParams->Position = position;
		position.y -= 136;
		if (m_pCore->GetCollider() && m_pCore->GetCollider()->type == TCollider::eAABB)
		{
			TAABB* pCollider = static_cast<TAABB*>(m_pCore->GetCollider());
			(pCollider)->vMin = pCollider->vMin + xmMovement;
			(pCollider)->vMax = pCollider->vMax + xmMovement;
		}
	}
	if (m_pServos)
	{
		m_pServos->SetPosition(position);
		if (m_pServos->GetCollider() && m_pServos->GetCollider()->type == TCollider::eAABB)
		{
			TAABB* pCollider = static_cast<TAABB*>(m_pServos->GetCollider());
			(pCollider)->vMin = pCollider->vMin + xmMovement;
			(pCollider)->vMax = pCollider->vMax + xmMovement;
		}
	}
	if (m_pBase)
	{
		m_pBase->SetPosition(position);
		if (m_pBase->GetCollider() && m_pBase->GetCollider()->type == TCollider::eAABB)
		{
			TAABB* pCollider = static_cast<TAABB*>(m_pBase->GetCollider());
			(pCollider)->vMin = pCollider->vMin + xmMovement;
			(pCollider)->vMax = pCollider->vMax + xmMovement;
		}
	}

	CRObject::SetPosition(position);
}

/*virtual*/ void CReactor::SetCollider(TAABB* pCollider) /*override*/
{
	CSubSystem::SetCollider(pCollider);
	XMFLOAT3 xmRadii = GetAABBRadii(pCollider);
	if (m_pCore)
	{
		if (m_pCore->GetCollider())
		{
			delete m_pCore->GetCollider();
			m_pCore->SetCollider(nullptr);
		}
		////Give the core have the radius of the frame
		//TAABB* pCol = new TAABB();
		//pCol->type = TCollider::eAABB;
		//pCol->vMin = GetPosition() - XMFLOAT3(xmRadii.x * 0.5f, 0, xmRadii.z * 0.5f);
		//pCol->vMax = GetPosition() + XMFLOAT3(xmRadii.x * 0.5f, xmRadii.y * 0.6f, xmRadii.z * 0.5f);
		//m_pCore->SetCollider(pCol);
		TAABB* pCol = new TAABB();
		*pCol = *pCollider;
		m_pCore->SetCollider(pCol);
	}
	if (m_pServos)
	{
		if (m_pServos->GetCollider())
		{
			delete m_pServos->GetCollider();
			m_pServos->SetCollider(nullptr);
		}
		////Give the servos a shorter
		//TAABB* pCol = new TAABB();
		//pCol->type = TCollider::eAABB;
		//pCol->vMin = GetPosition() - XMFLOAT3(xmRadii.x * 0.5f, 0, xmRadii.z * 0.5f);
		//pCol->vMax = GetPosition() + XMFLOAT3(xmRadii.x * 0.5f, xmRadii.y * 0.6f, xmRadii.z * 0.5f);
		//m_pServos->SetCollider(pCol);
		TAABB* pCol = new TAABB();
		*pCol = *pCollider;
		m_pServos->SetCollider(pCol);
	}
	if (m_pBase)
	{
		if (m_pBase->GetCollider())
		{
			delete m_pBase->GetCollider();
			m_pBase->SetCollider(nullptr);
		}
		////Give the servos a shorter
		//TAABB* pCol = new TAABB();
		//pCol->type = TCollider::eAABB;
		//pCol->vMin = GetPosition() - XMFLOAT3(xmRadii.x * 0.5f, 0, xmRadii.z * 0.5f);
		//pCol->vMax = GetPosition() + XMFLOAT3(xmRadii.x * 0.5f, xmRadii.y * 0.6f, xmRadii.z * 0.5f);
		//m_pBase->SetCollider(pCol);
		TAABB* pCol = new TAABB();
		*pCol = *pCollider;
		m_pBase->SetCollider(pCol);
	}

}


/*****************************************************************
* Update() - Updates the reactor
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/28/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CReactor::Update(float dt) /*override*/
{
	if (GetIsDestroyed( ) && !m_bRepairing)
	{
		m_fMeltdownTimer -= dt;
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::RESUME_2D_REACTOR_MELTDOWN);
	}
	else if (GetIsDestroyed( ) && m_bRepairing)
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PAUSE_2D_REACTOR_MELTDOWN);
	}
	else if (GetAlive( ) && m_fMeltdownTimer != m_fMaxMeltdownTime)
	{
		m_fMeltdownTimer = m_fMaxMeltdownTime;
		AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::STOP_MUSIC_LOOP_ALARM);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_REACTOR_MELTDOWN);
	}
	//Play the count-down sound
	if (!m_bRepairing && m_fMeltdownTimer > 7.75f && m_fMeltdownTimer < 8.25f && m_fRecurringWarningTimer < 0)
	{
		m_fRecurringWarningTimer = 25.0f;
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_REACTOR_MELTDOWN);

		if (nullptr != m_pAIDirector)
		{
			std::string message;
			message = "Emergency! Emergency! reactor \nmeltdown in: 5... 4... 3... \n2... 1...";
			m_pAIDirector->SetSubtitle(message);
		}
	}
	else if (m_fMeltdownTimer <= 0.f)
	{
		//DebugPrint("Reactor Meltdown: Oh man, you're super dead!\n", ConsoleColor::Red);

		g_pGamePlayState->QueueStateChange(STATE_CH_LOSE);
	}

#if PRINT_SUBSYSTEM_STATUS

	else if (m_fPrintCounter >= m_fMeltdownTimer && GetIsDestroyed( ))
	{
		//DebugPrint((std::string("Reactor Meltdown:") + to_string(int(m_fMeltdownTimer)) + std::string(":") + to_string(int(m_fMeltdownTimer / 1000.f)) + std::string("\n")).c_str(), ConsoleColor::Piss);
		m_fPrintCounter = 0.f;
	}

#endif

	m_fCurrentDSoundTime += dt;

	Animate(dt);

	CSubSystem::Update(dt);
}

void CReactor::TakeDamage(float fDamage)
{
	CLivingEntities::TakeDamage(fDamage);

	if (m_fCurrentHealth <= 0 && !m_bIsAlive)
	{
		m_fRecurringWarningTimer = SUBSYSTEM_RECURRING_WARNING_TIMER;

		if (nullptr != m_pAIDirector)
		{
			if (false == m_pAIDirector->GetPlayerDead())
			{
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_REACTOR_DOWN);
			}

			std::string message;
			message = "Reactor offline... \nT minus 30 seconds for reactor \nmeltdown.";
			m_pAIDirector->SetSubtitle(message);
		}

		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_MUSIC_LOOP_ALARM);
	}
	if (m_fCurrentDSoundTime > m_fDamageSoundTimer)
	{
		if (nullptr != m_pAIDirector)
		{
			if (false == m_pAIDirector->GetPlayerDead())
			{
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_REACTOR_ATTACKED);
			}

			std::string message;
			message = "Reactor, under attack!";
			m_pAIDirector->SetSubtitle(message);
		}
		m_fCurrentDSoundTime = 0.0f;
	}
}

bool CReactor::LoadStats(void)
{
	tinyxml2::XMLDocument xmlDoc;

	//TODO: Load stats based on difficulty
	xmlDoc.LoadFile("Assets//XML//Balance//ReactorStats.xml");

	//Did not open file properly
	if (xmlDoc.Error())
	{
		xmlDoc.Clear();
		return false;
	}
	tinyxml2::XMLElement* xmlHead = xmlDoc.RootElement();
	tinyxml2::XMLElement* xmlShoulder = xmlHead->FirstChildElement("Default");
	tinyxml2::XMLElement* xmlKnee = nullptr;

	//Load in the actual stats
	xmlKnee = xmlShoulder->FirstChildElement("Defense");
	m_fMaxHealth = m_fCurrentHealth = (float)atof(xmlKnee->Attribute("Health"));
	//m_fArmor = (float)atof(xmlElement->Attribute("Armor"));

	xmlKnee = xmlShoulder->FirstChildElement("Utility");
	m_fMaxMeltdownTime = (float)atof(xmlKnee->Attribute("MeltdownTime"));

	//We're done here
	xmlDoc.Clear();
	return true;
}

void CReactor::Animate(float fDt)
{
	//How values change when it's alive
	if (m_bIsAlive)
	{
		m_fCoreScale += fDt * CORE_STRETCH_SPEED * m_cStretching;
		m_fCoreRotation += fDt * CORE_SPIN_SPEED;
		m_fServoRotation += fDt * SERVO_SPIN_SPEED;
		m_fBaseRotation += fDt * BASE_SPIN_SPEED;
		m_fCoreHeight += fDt * CORE_BOUNCE_SPEED * m_cRising;
		m_pLightParams->Range += fDt * LIGHT_SWITCH_SPEED * m_cBrightening;

		if (m_fCoreHeight >= CORE_MAX_Y)
		{
			m_fCoreHeight = CORE_MAX_Y;
			m_cRising = -1;
		}
		else if (m_fCoreHeight <= CORE_MIN_Y)
		{
			m_fCoreHeight = CORE_MIN_Y;
			m_cRising = 1;
		}
		if (m_fCoreScale >= CORE_MAX_SCALE)
		{
			if (m_fCoreScale >= CORE_HARDMAX_SCALE)
				m_fCoreScale = CORE_HARDMAX_SCALE;
			m_cStretching = -1;
		}
		else if (m_fCoreScale <= CORE_MIN_SCALE)
		{
			m_fCoreScale = CORE_MIN_SCALE;
			m_cStretching = 1;
		}
		if (m_pLightParams->Range >= LIGHT_MAX_RANGE)
		{
			if (m_pLightParams->Range >= LIGHT_MAX_RANGE * 2)
				m_pLightParams->Range = LIGHT_MAX_RANGE * 2;
			m_cBrightening = -1;
		}
		else if (m_pLightParams->Range <= LIGHT_MIN_RANGE)
		{
			m_pLightParams->Range = LIGHT_MIN_RANGE;
			m_cBrightening = 1;
		}
	}
	//How values change when it's dead
	else
	{
		m_fCoreScale += fDt * CORE_STRETCH_SPEED;
		m_fCoreRotation += fDt * BROKEN_CORE_SPIN_SPEED;
		m_fServoRotation += fDt * SERVO_SPIN_SPEED * -0.01f;
		m_fBaseRotation += fDt * BASE_SPIN_SPEED * -0.01f;
		m_fCoreHeight += fDt * CORE_BOUNCE_SPEED * m_cRising * 0.5f;
		m_pLightParams->Range += fDt * LIGHT_SWITCH_SPEED;

		if (m_fCoreHeight >= CORE_MAX_Y * 0.75f)
		{
			if (m_fCoreHeight >= CORE_MAX_Y)
			{
				m_fCoreHeight = CORE_MAX_Y;
			}
			m_cRising = -1;
		}
		else if (m_fCoreHeight <= CORE_MIN_Y * 0.75f)
		{
			if (m_fCoreHeight <= CORE_MIN_Y)
			{
				m_fCoreHeight = CORE_MIN_Y;
			}
			m_fCoreHeight = CORE_MIN_Y;
			m_cRising = 1;
		}
		if (m_fCoreScale >= CORE_HARDMAX_SCALE)
		{
			m_fCoreScale = CORE_HARDMAX_SCALE;
		}
		if (m_pLightParams->Range >= LIGHT_MAX_RANGE * 2)
		{
			m_pLightParams->Range = LIGHT_MAX_RANGE * 2;
			//m_cBrightening = -1;
		}
	}
	//Spin the core
	DirectX::XMFLOAT4X4 xmMat;
	DirectX::XMStoreFloat4x4(&xmMat, DirectX::XMMatrixIdentity());
	xmMat._41 = m_pCore->GetLocalMatrix()._41;
	xmMat._42 = CORE_OFFSET + m_fCoreHeight + GetLocalMatrix()._42;
	xmMat._43 = m_pCore->GetLocalMatrix()._43;
	DirectX::XMStoreFloat4x4(&xmMat, DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(m_fCoreRotation), DirectX::XMMatrixScaling(m_fCoreScale, m_fCoreScale, m_fCoreScale)), DirectX::XMLoadFloat4x4(&xmMat)));
	m_pCore->GetLocalMatrix() = xmMat;
	//Spin the servos
	DirectX::XMStoreFloat4x4(&xmMat, DirectX::XMMatrixIdentity());
	xmMat._41 = m_pServos->GetLocalMatrix()._41;
	xmMat._42 = m_pServos->GetLocalMatrix()._42;
	xmMat._43 = m_pServos->GetLocalMatrix()._43;
	DirectX::XMStoreFloat4x4(&xmMat, DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(m_fServoRotation), DirectX::XMLoadFloat4x4(&xmMat)));
	m_pServos->GetLocalMatrix() = xmMat;
	//Spin the base
	DirectX::XMStoreFloat4x4(&xmMat, DirectX::XMMatrixIdentity());
	xmMat._41 = m_pBase->GetLocalMatrix()._41;
	xmMat._42 = m_pBase->GetLocalMatrix()._42;
	xmMat._43 = m_pBase->GetLocalMatrix()._43;
	DirectX::XMStoreFloat4x4(&xmMat, DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(m_fBaseRotation), DirectX::XMLoadFloat4x4(&xmMat)));
	m_pBase->GetLocalMatrix() = xmMat;
}