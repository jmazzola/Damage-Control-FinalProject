/***********************************************
* Filename:			Engines.cpp
* Date:      		05/22/2015
* Mod. Date:		06/17/2015
* Mod. Initials:	JM
* Author:    		Michael Mozdzierz
* Purpose:
************************************************/
#include "Engines.h"
#include "../../../State Machine/GameplayState.h"
#include "../../../Asset Manager/DDSTextureLoader.h"
#include "../../../Renderer/Renderer.h"
#include "../../../Input Manager/InputManager.h"
#include "../../../Audio Manager/AudioSystemWwise.h"
#include "../../../TinyXML2/tinyxml2.h"

using namespace MathHelper;

#define ENGINE_SPIN_SPEED 1.0f
#define LIGHT_SCALE 10.0f
#define LIGHT_THRESHOLD 2
#define MAX_LIGHT 10.0f
#define ORANGE DirectX::XMFLOAT3(1.0f, 0.6f, 0.0f)

#define NORMAL_COLOR DirectX::XMFLOAT4(0, 1, 10.0f, 1)
#define DEAD_COLOR1 DirectX::XMFLOAT4(10, 6, 0, 1)
#define DEAD_COLOR2 DirectX::XMFLOAT4(10, 0, 0, 1)
#define COLOR_G_DIF 4.0f
#define BLINK_SPEED 10.0f

/*****************************************************************
* CEngines()
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
CEngines::CEngines(void)
{
	LoadStats();

	m_szTag = "Engine";
	tCollider = new TAABB;
	tCollider->type = TCollider::eAABB;

	AudioSystemWwise::Get( )->RegisterEntity(this, "Engine");

	m_fCurrentHealth = m_fMaxHealth;
	m_bIsAlive = false;
	m_fProgress = m_fStartDistance;

	m_pLightParams = new LightParams(GetPosition(), { 10, 10, 10 }, 300);
	m_pHoloDisplay = new CRObject();
	m_pHoloDisplay->SetTag("HoloDisplay");
	m_pHoloDisplay->SetRObjectType(RObjectType::ROBJECT_TRANSPARENT);
	m_pHoloDisplay->SetColor(NORMAL_COLOR);
}

/*****************************************************************
* ~CEngines() - Sets the position of the engines
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
/*virtual*/ CEngines::~CEngines(void)
{
	AudioSystemWwise::Get( )->UnRegisterEntity(this);

	delete tCollider;
	tCollider = nullptr;

	SafeDelete(m_pHoloDisplay);
}

/*****************************************************************
* SetPosition() - Sets the position of the engines
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
/*virtual*/ void CEngines::SetPosition(DirectX::XMFLOAT3& position) /*override*/
{
	CEntity::SetPosition(position);

	(tCollider)->vMin = position - DirectX::XMFLOAT3(60, 0, 60);
	(tCollider)->vMax = position + DirectX::XMFLOAT3(60, 120, 60);

	m_pLightParams->Position = position;
	m_pLightParams->Position.z -= 195;
	m_pLightParams->Position.y += 115;

	if (m_pHoloDisplay)
	{
		position.z += 200;
		//m_pHoloDisplay->SetPosition(position);
	}
}

/*****************************************************************
* Update(): Updates the engines
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
#include "../../../AchieveTracker.h"
/*virtual*/ void CEngines::Update(float dt) /*override*/
{
	if (GetAlive())
	{
		if (false == m_bStopped)
		{
			m_fProgress += (PercentHealth() * m_fSpeed) * dt;
		}
		m_pHoloDisplay->SetColor(NORMAL_COLOR);
		m_fBlinkProgress = 0;
	}
	else
	{
		DirectX::XMFLOAT4 xmColor = DEAD_COLOR1;
		xmColor.y += m_fBlinkProgress * COLOR_G_DIF;
		m_pHoloDisplay->SetColor(xmColor);
		if (m_bWaxing)
		{
			m_fBlinkProgress += dt * BLINK_SPEED;
			if (m_fBlinkProgress >= 1)
			{
				m_fBlinkProgress = 1;
				m_bWaxing = false;
			}
		}
		else
		{
			m_fBlinkProgress -= dt * BLINK_SPEED;
			if (m_fBlinkProgress <= 0)
			{
				m_fBlinkProgress = 0;
				m_bWaxing = true;
			}
		}
	}
	//How bright is the light
	{
		float fBrightness = LIGHT_THRESHOLD + PercentHealth() * LIGHT_SCALE + (rand() % 10) * 0.01f;
		m_pLightParams->Color = { 1.0f * fBrightness, 0.6f * fBrightness, 0 };
	}
	m_fCurrentDSoundTime += dt;

	// Debug for speeding up the win state
	if (CInputManager::GetInstance( )->IsButtonDown(eControls::Spacebar) && CInputManager::GetInstance( )->IsButtonDown(eControls::LeftControl))
		m_fProgress += m_fSpeed * 10 * dt;

#if PRINT_SUBSYSTEM_STATUS
	if (m_fPrintCounter >= 10.f)
	{
		//DebugPrint((std::string("Engine Progress:") + to_string(int(m_fProgress)) + std::string("\n")).c_str(), ConsoleColor::Yellow);
		m_fPrintCounter = 0.f;
	}
#endif
	if (IsComplete())
	{
		g_pGamePlayState->QueueStateChange(STATE_CH_WIN);
		CAchieveTracker::GetInstance()->UnlockAchievement(CAchieveTracker::eCampaigner);
	}
	CSubSystem::Update(dt);
}

//Engines are unique in that they start dead
void CEngines::Reset(void)
{
	m_fProgress = m_fStartDistance;
	Kill();
}

void CEngines::TakeDamage(float fDamage)
{
	CLivingEntities::TakeDamage(fDamage);
	if (m_fCurrentDSoundTime > m_fDamageSoundTimer)
	{
		if (false == GetIsDestroyed() && m_fCurrentDSoundTime > m_fDamageSoundTimer)
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_ENGINES_ATTACKED);

			if (nullptr != m_pAIDirector)
			{
				std::string message;
				message = "Engine system, under attack!";
				m_pAIDirector->SetSubtitle(message);
			}

		}
		m_fCurrentDSoundTime = 0.0f;
	}
}

bool CEngines::LoadStats(void)
{
	tinyxml2::XMLDocument xmlDoc;

	//TODO: Load stats based on difficulty
	xmlDoc.LoadFile("Assets//XML//Balance//EngineStats.xml");

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
	m_fSpeed = (float)atof(xmlKnee->Attribute("Speed"));
	m_fDestination = (float)atof(xmlKnee->Attribute("Distance"));
	m_fFinalEventDistance = (float)atof(xmlKnee->Attribute("FinalEventDistance"));
	m_fStartDistance = (float)atof(xmlKnee->Attribute("StartDistance"));

	//We're done here
	xmlDoc.Clear();
	return true;
}

float CEngines::GetGoalCheck() const 
{ 
	return m_fDestination - m_fFinalEventDistance; 
}