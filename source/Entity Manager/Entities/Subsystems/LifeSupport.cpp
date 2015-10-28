/***********************************************
* Filename:			LifeSupport.cpp
* Date:      		05/22/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:
************************************************/

#include "LifeSupport.h"
#include <string>
#include "../../../Asset Manager/DDSTextureLoader.h"
#include "../../../Renderer/Renderer.h"
#include "../Player.h"
#include "../../../Audio Manager/AudioSystemWwise.h"
#include "../../../TinyXML2/tinyxml2.h"

using namespace MathHelper;
using namespace std;

/*****************************************************************
* CLifeSupport()
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
CLifeSupport::CLifeSupport(void)
{
	LoadStats();

	m_szTag = "LifeSupport";
	tCollider = new TAABB;
	tCollider->type = TCollider::eAABB;
	m_fHealthRegen = 5.0f;

	AudioSystemWwise::Get( )->RegisterEntity(this, "LifeSupport");

}

/*****************************************************************
* ~CLifeSupport() - Sets the position of the engines
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
/*virtual*/ CLifeSupport::~CLifeSupport(void)
{
	AudioSystemWwise::Get( )->UnRegisterEntity(this);

	delete tCollider;
	tCollider = nullptr;
}

/*****************************************************************
* SetPosition() - Sets the position of the engines
*
* Ins: position - the position to set the engines
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/28/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CLifeSupport::SetPosition(DirectX::XMFLOAT3& position) /*override*/
{
	CEntity::SetPosition(position);

	(tCollider)->vMin = position - DirectX::XMFLOAT3(60, 0, 60);
	(tCollider)->vMax = position + DirectX::XMFLOAT3(60, 120, 60);
}

/*****************************************************************
* Updates() - Updates the subsystem
*
* Ins: dt - the ammount of time that has elapsed
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/28/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CLifeSupport::Update(float dt) /*override*/
{
	//Heal the player and refill 02 over time
	if (GetAlive( ))
	{
		m_pPlayer->Heal(m_fHealthRegen * dt * PercentHealth( ));
		//Fill oxygen back up to 100
		if (m_pPlayer->GetOxygen( ) < 100)
		{
			m_pPlayer->SetOxygen(m_pPlayer->GetOxygen( ) + (dt * PercentHealth( ) * m_fOxygenRegen));
			if (m_pPlayer->GetOxygen( ) > 100)
			{
				m_pPlayer->SetOxygen(100);
			}
		}
	}
	m_fCurrentDSoundTime += dt;

#if PRINT_SUBSYSTEM_STATUS
	if (m_fPrintCounter >= 5.f)
	{
		//DebugPrint((std::string("Oxygen:") + to_string(m_pPlayer->GetOxygen()) + std::string("\n")).c_str(), ConsoleColor::Cyan);
		m_fPrintCounter = 0.f;
	}
#endif

	CSubSystem::Update(dt);
}

void CLifeSupport::TakeDamage(float fDamage)
{
	CLivingEntities::TakeDamage(fDamage);

	if (m_fCurrentDSoundTime > m_fDamageSoundTimer && GetIsDestroyed() == false)
	{
	
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_LSUPPORT_ATTACKED);

		if (nullptr != m_pAIDirector)
		{
			std::string message;
			message = "Life support system, under attack!";
			m_pAIDirector->SetSubtitle(message);
		}

		m_fCurrentDSoundTime = 0.0f;
	}
}


bool CLifeSupport::LoadStats(void)
{
	tinyxml2::XMLDocument xmlDoc;

	//TODO: Load stats based on difficulty
	xmlDoc.LoadFile("Assets//XML//Balance//LifeSupportStats.xml");

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
	m_fHealthRegen = (float)atof(xmlKnee->Attribute("HealthRegen"));
	m_fOxygenRegen = (float)atof(xmlKnee->Attribute("O2Regen"));

	//We're done here
	xmlDoc.Clear();
	return true;
}