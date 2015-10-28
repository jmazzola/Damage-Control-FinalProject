/***********************************************
* Filename:  		BaseState.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This is the base class of
*			 all gameplay states
************************************************/

#include "BaseState.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/Texture2D.h"
#include "../GUI Manager/GUIManager.h"
#include "..\Renderer\BitmapFont.h"
#include "../GUI Manager/UIElement.h"

CBaseState::CBaseState(eStateType eType)
{
	m_eType = eType;
	m_bShowCursor = true;
}

/*****************************************************************
* Enter(): Called when switching to a state, initializes members
*                and allocates memory
*
* Ins:			  CBaseState* cPrevState
*
* Outs:		      void
*
* Returns:		  bool, true if successfully entered
*
* Mod. Date:		      05/12/2015
* Mod. Initials:	      MJG
*****************************************************************/
bool CBaseState::FirstEnter(void)
{
	if (m_pGUI)
	{
		m_pGUI->SetVisible(true);
		m_pGUI->SetRadarRender(true);
	}
	m_bEntered = true;
	m_bToPop = false;
	return true;
}

bool CBaseState::ReEnter(void)
{
	if (m_pGUI)
	{
		m_pGUI->SetVisible(true);
		m_pGUI->SetRadarRender(true);
	}

	return true;
}

//Does some standardized error checking
bool CBaseState::Enter(void)
{
	if (m_bLoaded == false)
	{
		return false;
	}
	//Is this the first time?
	if (m_bEntered)//No
	{
		return ReEnter( );
	}
	else//Yes
	{
		return FirstEnter( );
	}
}

/*****************************************************************
* Exit(): Called when you switch states and don't need to preserve
*				this one
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  void
*
* Mod. Date:		      05/13/2015
* Mod. Initials:	      MJG
*****************************************************************/
bool CBaseState::Exit(void)
{
	if (m_pGUI)
	{
		m_pGUI->SetVisible(false);
		m_pGUI->SetRadarRender(false);
	}
	m_bEntered = false;
	m_pPrevState = nullptr;

	CBitmapFont::GetInstance()->ClearBuffer();

	return false;
}

/*****************************************************************
* Load(): Called before switching to a state to ensure all
*				assets are loaded
*
* Ins:			  CAssetManager* pAssets
*
* Outs:		      void
*
* Returns:		  bool, true if all assets are loaded
*
* Mod. Date:		      05/13/2015
* Mod. Initials:	      MJG
*****************************************************************/
bool CBaseState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	m_bLoaded = true;

	//Load in the LMB
	m_pLMBSelect = new CUIElement( );
	vec3f temp(0.78f, -0.91f, 0);
	m_pLMBSelect->SetPosition(temp);
	m_pLMBSelect->LoadTexture(L"Assets\\Images\\LMB.png");
	m_pLMBSelect->SetHeight(0.21f);
	m_pLMBSelect->SetWidth(0.335f);

	return true;
}

/*****************************************************************
* UnLoad(): Called when you no longer need this state's assets
*
* Ins:			  CAssetManager* pAssets
*
* Outs:		      void
*
* Returns:		  bool, true if all assets are unloaded
*
* Mod. Date:		      05/13/2015
* Mod. Initials:	      MJG
*****************************************************************/
void CBaseState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	SafeDelete(m_pGUI);
	SafeDelete(m_pLMBSelect);
	m_bLoaded = false;
}

void CBaseState::HandlePrevState(CBaseState* pPrevState)
{
	m_pPrevState = pPrevState;
	if (m_pPrevState && m_pPrevState->GetGUI( ))
	{
		m_pPrevState->GetGUI( )->SetRadarRender(false);
		m_pPrevState->GetGUI( )->SetVisible(false);
	}
}

/*****************************************************************
* CheckState(): Called at the end of each StateMachine's Update
*				to see if the state should switch to another state
*				returns the state that it switches to, returns
*				nullptr if application is closing, this if
*				no change
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  CBaseState*
*
* Mod. Date:		      05/13/2015
* Mod. Initials:	      MJG
*****************************************************************/
eStateType CBaseState::CheckState(void)
{
	//We were told to close the application, let the state machine know
	if (m_bStateChanges[STATE_CH_EXIT])
	{
		return STATE_NULL;
	}
	ClearStateChanges( );
	//We have nothing else to worry about
	return GetType( );
}

void CBaseState::DrawNavigationInfo(void)
{
	//CBitmapFont::GetInstance()->DrawString("Use mouse to navigate", 0.34f, -0.97f, (DirectX::XMFLOAT4)DirectX::Colors::White, 0.7f);
	if (m_pLMBSelect)
	{
		m_pLMBSelect->Render( );
	}
}