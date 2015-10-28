/***********************************************
* Filename:  		PauseState.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This is the state used for
*			 pausing the Gameplay
************************************************/
#include "PauseState.h"
#include "../Input Manager/InputManager.h"
#include "../Audio Manager/AudioSystemWwise.h"
#include "../GUI Manager/GUIManager.h"
#include "../GUI Manager/UIElement.h"
#include "../GUI Manager/UIButton.h"

#include "../Renderer/Renderer.h"
#include "../AchieveTracker.h"

#include "../Renderer/Scene.h"

#include "GameplayState.h"

/*****************************************************************
* Default Constructor
*****************************************************************/
CPauseState::CPauseState(void) :CBaseState(STATE_PAUSE)
{
	m_bTransparent = true;

	m_pScene = new CScene();
}

/*****************************************************************
* Default Destructor
*****************************************************************/
CPauseState::~CPauseState(void)
{
	SafeDelete(m_pScene);
}

void CPauseState::Input(void)
{
	//Unpause the game
	if (CInputManager::GetInstance( )->IsKeyPressed(eControls::ESCAPE) ||
		CInputManager::GetInstance( )->IsKeyPressed(eControls::Backspace) ||
		CInputManager::GetInstance( )->IsKeyPressed(eControls::P))
	{
		QueueStateChange(STATE_CH_BACK);
	}
	if (m_pResume->GetPressed( ))
	{
		QueueStateChange(STATE_CH_PLAY);
	}
	else if (m_pQuit->GetPressed( ))
	{
		QueueStateChange(STATE_CH_QUIT);
	}
	else if (m_pExit->GetPressed( ))
	{
		QueueStateChange(STATE_CH_EXIT);
	}
	else if (m_pOptions->GetPressed())
	{
		QueueStateChange(STATE_CH_OPTIONS);
	}
}

void CPauseState::Update(float fDT)
{
	m_pGUI->Update(fDT);

	m_pScene->Update({ 0, 0, 0 }, fDT);

	if (m_pGamePlay)
	{
		m_pGamePlay->Update(0);
	}

	//screen fade stuff
	//if (m_bFadeIn)
	//{
	//	m_fFadeTimer += fDT;
	//	m_pScreen->SetColor({ 1, 1, 1, blend(1.0f, 0.0f, m_fFadeTimer * (1 / TRANSITION_TIME)) });
	//}
	//else if (m_bFadeOut)
	//{
	//	m_fFadeTimer += fDT;
	//	m_pScreen->SetColor({ 1, 1, 1, blend(0.0f, .5f, m_fFadeTimer * (1 / TRANSITION_TIME)) });
	//}
	//
	//if (m_pScreen->GetColor( ).w <= 0.0f)
	//{
	//	m_fFadeTimer = 0.0f;
	//	m_bFadeIn = FALSE;
	//}
	//else if (m_pScreen->GetColor( ).w >= .5f)
	//{
	//	m_fFadeTimer = 0.0f;
	//	m_bFadeOut = FALSE;
	//}
}

void CPauseState::Render(CAssetManager* pRenderer)
{
	CRenderer::GetReference().Render(m_pScene, pRenderer);

	if (m_pGamePlay)
	{
		m_pGamePlay->Render(pRenderer);
	}

	//TODO: Render Pause menu
	m_pGUI->Render( );

	DrawNavigationInfo();
}

bool CPauseState::FirstEnter(void)
{
	CBaseState::FirstEnter();

	//Set buttons to default state
	ClearStateChanges();

	if (dynamic_cast<CGameplayState*>(m_pPrevState))
	{
		m_pGamePlay = m_pPrevState;
	}
	//m_bFadeIn = false, m_bFadeOut = false;
	//m_fFadeTimer = 0.0f;

	return true;
}

bool CPauseState::ReEnter(void)
{
	//m_bFadeIn = false, m_bFadeOut = false;
	//m_fFadeTimer = 0.0f;

	return CBaseState::ReEnter();
}

bool CPauseState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded( ))
	{
		return true;
	}
	CBaseState::Load(pAssets);
	//TODO: Load in GUI for Pause menu
	//Load in the GUI
	m_pGUI = new CGUIManager( );


	m_pScreen = new CUIElement();

	m_pScreen->SetWidth(1000);
	m_pScreen->SetHeight(800);
	m_pScreen->SetPosition(vec3f(500, 400, 1));
	m_pScreen->SetVisible(true);
	m_pScreen->LoadTexture(L"Assets/Images/Black.png");
	m_pScreen->SetColor({ 1, 1, 1, .5f });
	m_pGUI->AddElement(m_pScreen);

	//Load whole menu
	CUIElement* pTemp = new CUIElement( );
	pTemp->SetPosition(vec3f(520, 384, 0));
	pTemp->SetWidth(380);
	pTemp->SetHeight(500);
	pTemp->LoadTexture(L"Assets//Images//Pause//PauseState.png");
	m_pGUI->AddElement(pTemp);

	//Resume button
	std::wstring wzTemp = L"Assets//Images//Pause//Resume.png";
	std::string szTemp = "Void";
	m_pResume = new CUIButton(wzTemp, 520, 290, 150, 60, szTemp);
	m_pResume->SetRenderText(false);
	m_pGUI->AddElement(m_pResume);

	wzTemp = L"Assets//Images//MainMenu//Options.png";
	m_pOptions = new CUIButton(wzTemp, 520, 372, 150, 60, szTemp);
	m_pOptions->SetRenderText(false);
	m_pGUI->AddElement(m_pOptions);

	wzTemp = L"Assets//Images//Pause//ExitToMain.png";
	m_pQuit = new CUIButton(wzTemp, 520, 454, 150, 60, szTemp);
	m_pQuit->SetRenderText(false);
	m_pGUI->AddElement(m_pQuit);

	wzTemp = L"Assets//Images//Pause//ExitToDesktop.png";
	m_pExit = new CUIButton(wzTemp, 520, 536, 150, 60, szTemp);
	m_pExit->SetRenderText(false);
	m_pGUI->AddElement(m_pExit);

	return true;
}

void CPauseState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded( ))
	{
		return;
	}
	CBaseState::UnLoad();
	//m_pResume = m_pOptions = m_pQuit = m_pExit = nullptr;
}

bool CPauseState::Exit(void)
{
	if (!GetEntered( ))
	{
		return false;
	}
	//m_bEntered = false;
	CBaseState::Exit();
	return false;
}

eStateType CPauseState::CheckState(void)
{
	eStateType eNextState = GetType();
	//Are there any state changes that should be remembered for next frame
	if (m_bStateChanges[STATE_CH_EXIT])
	{
		if (m_bStateChanges[STATE_CH_YES])
		{
			CAchieveTracker::GetInstance()->SetIsExit(true);
			eNextState = STATE_NULL;
			m_bToPop = true;
		}
		else if (!m_bStateChanges[STATE_CH_NO])
		{
			eNextState = STATE_YESNO;
		}
	}
	else if (m_bStateChanges[STATE_CH_OPTIONS])
	{
		eNextState = STATE_OPTIONS;
	}
	//Go back to gameplay
	else if (m_bStateChanges[STATE_CH_BACK] || m_bStateChanges[STATE_CH_PLAY])
	{
		eNextState = STATE_GAMEPLAY;
		AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::RESUME_ALL);
	}
	//We were told to quit, so let the gameplay state do it for us
	else if (m_bStateChanges[STATE_CH_QUIT])
	{
		if (m_bStateChanges[STATE_CH_YES])
		{
			CAchieveTracker::GetInstance()->SetIsExit(true);
			eNextState = STATE_MAINMENU;
			m_bToPop = true;
		}
		else if (!m_bStateChanges[STATE_CH_NO])
		{
			eNextState = STATE_YESNO;
		}
	}
	if (eNextState != STATE_YESNO)
	{
		ClearStateChanges();
	}

	//Tell the state machine what state we're switching to
	return eNextState;
}
