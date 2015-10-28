/***********************************************
* Filename:			Workstation.h
* Date:      		06/14/2015
* Mod. Date:		06/14/2015
* Mod. Initials:	MJG
* Author:    		Michael Gergar
* Purpose:			Entity that when interacted
*				with will open a menu for the player
************************************************/
#include "../../Core/stdafx.h"
#include "Workstation.h"
#include "../../GUI Manager/UIButton.h"
#include "../../GUI Manager/GUIManager.h"
#include "../../Input Manager/InputManager.h"
#include "../../Audio Manager/AudioSystemWwise.h"

using namespace SceneryHelpers;

#define PRESS_BLANK_TO_INTERACT_X 400
#define PRESS_BLANK_TO_INTERACT_Y 400

#define GREETER_DELAY 18.0f

void WorkStationAction(CScenery* pThis)
{
	CWorkStation* pStation = dynamic_cast<CWorkStation*>(pThis);
	if (CInputManager::GetInstance()->IsKeyPressed(eControls::E))
	{
		pStation->GetOwner()->QueueStateChange(pStation->GetStateMessage());
	}
}

CWorkStation::~CWorkStation( )
{

}

CWorkStation::CWorkStation(CBaseState* pOwner, eStateChange eMessage, const char* szHUDMessage) : CScenery(PlayerSceneryTest, WorkStationAction)
{
	//Set our messenger capabilities
	m_pOwnerState = pOwner;
	m_eMessage = eMessage;
	SetEntityType(CEntity::eWORKSTATION);

	//Make our GUI element
	CUIButton* pHUD = new CUIButton(std::wstring(L""), PRESS_BLANK_TO_INTERACT_X, PRESS_BLANK_TO_INTERACT_X, 1, 1, std::string(szHUDMessage));
	pHUD->SetVisible(false);
	pHUD->SetEnabled(false);
	pHUD->SetRenderText(true);
	pHUD->SetTextColor((DirectX::XMFLOAT4)DirectX::Colors::White);
	m_pHUDElement = pHUD;
	m_pOwnerState->GetGUI()->AddElement(pHUD);
}

void CWorkStation::Update(float fDt)
{
	//Don't bother with this stuff if we're paused
	if (fDt > 0)
	{
		//Display the HUD element
		if (GetInteract())
		{
			m_pHUDElement->SetVisible(true);
		}
		//Hid the HUD element
		else
		{
			m_pHUDElement->SetVisible(false);
		}
		//Greeter stuff
		m_fGreeterDelay -= fDt;
		if (GetFirstFrame() && m_fGreeterDelay <= 0.0f)
		{
			m_fGreeterDelay = GREETER_DELAY;
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_STORE_GREETER, GetPosition());
		}
	}
	CScenery::Update(fDt);
}
