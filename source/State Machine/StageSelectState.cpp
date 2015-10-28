/***********************************************
* Filename:  		StageSelectState.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This is the state used for
*			 pausing the Gameplay
************************************************/
#include "StageSelectState.h"
#include "../Core/Common.h"
#include "../Input Manager/InputManager.h"
#include "../Audio Manager/AudioSystemWwise.h"
#include "../GUI Manager/GUIManager.h"
#include "../GUI Manager/UIElement.h"
#include "../GUI Manager/UIButton.h"
#include "../Renderer/HUDRenderer.h"
#include "..\Renderer\Scene.h"
#include "../Renderer/Renderer.h"

/*****************************************************************
* Default Constructor
*****************************************************************/
CStageSelectState::CStageSelectState(void) :CBaseState(STATE_STAGESELECT)
{
	m_bTransparent = true;
}

/*****************************************************************
* Default Destructor
*****************************************************************/
CStageSelectState::~CStageSelectState(void)
{

}

void CStageSelectState::Input(void)
{
	//UnStageSelect the game
	if (CInputManager::GetInstance()->IsKeyPressed(eControls::ESCAPE) ||
		CInputManager::GetInstance()->IsKeyPressed(eControls::Backspace) ||
		m_pBack->GetPressed())
	{
		QueueStateChange(STATE_CH_BACK);
	}
	//Press enter to keep going
	if (CInputManager::GetInstance()->IsKeyPressed(eControls::Enter))
	{
		//Is a button being moused over
		if (m_pBack->GetMouseHover())
		{
			QueueStateChange(STATE_CH_BACK);
		}
		//If endless is highlighted
		else if (m_pEndless->GetMouseHover())
		{
			QueueStateChange(STATE_CH_PLAY);
			m_bEndlessMode = true;
		}
		//If nothing is selected or the campaign button is highlighted
		else
		{
			QueueStateChange(STATE_CH_PLAY);
			m_bEndlessMode = false;
		}
	}

	if (m_pCampaign->GetPressed())
	{
		m_bEndlessMode = false;
		QueueStateChange(STATE_CH_PLAY);
	}
	else if (m_pEndless->GetPressed())
	{
		m_bEndlessMode = true;
		QueueStateChange(STATE_CH_PLAY);
	}
}

void CStageSelectState::Update(float fDT)
{
	m_pGUI->Update(fDT);

	if (m_pPrevState)
	{
		m_pPrevState->Update(0);
	}
//	screen fade stuff
	if (m_bFadeIn)
	{
		m_fFadeTimer += fDT;
		m_pScreen->SetColor({ 1, 1, 1, blend(1.0f, 0.0f, m_fFadeTimer * (1 / TRANSITION_TIME)) });
	}
	else if (m_bFadeOut)
	{
		m_fFadeTimer += fDT;
		m_pScreen->SetColor({ 1, 1, 1, blend(0.0f, .5f, m_fFadeTimer * (1 / TRANSITION_TIME)) });
	}
	
	if (m_pScreen->GetColor( ).w <= 0.0f)
	{
		m_fFadeTimer = 0.0f;
		m_bFadeIn = FALSE;
	}
	else if (m_pScreen->GetColor( ).w >= .5f)
	{
		m_fFadeTimer = 0.0f;
		m_bFadeOut = FALSE;
	}
}

void CStageSelectState::Render(CAssetManager* pRenderer)
{
	if (m_pPrevState)
	{
		m_pPrevState->Render(pRenderer);
	}


	//TODO: Render StageSelect menu
	m_pGUI->Render();

	for (auto& pElement : m_vElements)
	{
		CHUDRenderer::Render2D(pElement);
	}


	DrawNavigationInfo();
}

bool CStageSelectState::FirstEnter(void)
{
	CBaseState::FirstEnter();

	//Set buttons to default state

	m_bFadeIn = false, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	return true;
}

bool CStageSelectState::ReEnter(void)
{
	//m_bFadeIn = false, m_bFadeOut = false;
	//m_fFadeTimer = 0.0f;

	return CBaseState::ReEnter();
}

bool CStageSelectState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded())
	{
		return true;
	}
	CBaseState::Load(pAssets);
	//TODO: Load in GUI for StageSelect menu
	//Load in the GUI


	m_pScene = new CScene();
	m_pGUI = new CGUIManager();

	m_pScreen = new CUIElement();

	m_pScreen->SetWidth(1000);
	m_pScreen->SetHeight(800);
	m_pScreen->SetColor({ 0, 0, 0, 0 });
	m_pScreen->SetPosition(vec3f(500, 400, 1));
	m_pScreen->SetVisible(false);
	m_pScreen->LoadTexture(L"Assets/Images/Black.png");
	m_pGUI->AddElement(m_pScreen);

	//m_vElements.push_back(new CTexture2D(0, 0, 1.2f, 1.2f, L"Assets/Images/Menus/Main_Menu_Border_v4.png"));

	m_pSemiTransparent = new CUIElement();

	m_pSemiTransparent->SetWidth(500);
	m_pSemiTransparent->SetHeight(800);
	m_pSemiTransparent->SetColor({ 1,1,1, 0.25f });
	m_pSemiTransparent->SetPosition(vec3f(500, 400, 1));
	m_pSemiTransparent->SetVisible(false);
	m_pSemiTransparent->LoadTexture(L"Assets/Images/Menus/Main_Menu_Border_v4.png");
	m_pGUI->AddElement(m_pSemiTransparent);
	
	//Load whole menu
	m_pBack = new CUIButton(std::wstring(L"Assets//Images//Menus//Back.png"), 500, 600, 150, 60, std::string("Void"));
	m_pBack->SetRenderText(false);
	m_pGUI->AddElement(m_pBack);

	m_pCampaign = new CUIButton(std::wstring(L"Assets//Images//Menus//Campaign.png"), 500, 400, 150, 60, std::string("Void"));
	m_pCampaign->SetRenderText(false);
	m_pGUI->AddElement(m_pCampaign);

	m_pEndless = new CUIButton(std::wstring(L"Assets//Images//Menus//Endless.png"), 500, 500, 150, 60, std::string("Void"));
	m_pEndless->SetRenderText(false);
	m_pGUI->AddElement(m_pEndless);

	return true;
}

void CStageSelectState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded())
	{
		return;
	}
	CBaseState::UnLoad();
	m_pBack = m_pCampaign = m_pEndless = nullptr;

	for (auto& pElement : m_vElements)
	{
		delete pElement;
		pElement = nullptr;
	}
	m_vElements.clear();
}

bool CStageSelectState::Exit(void)
{
	if (!GetEntered())
	{
		return false;
	}
	CBaseState::Exit();
	return false;
}

eStateType CStageSelectState::CheckState(void)
{
	eStateType eNextState = GetType();
	//Go back to gameplay
	if (m_bStateChanges[STATE_CH_BACK] || m_bStateChanges[STATE_CH_QUIT])
	{
		if (m_pScreen->GetColor().w < 1.0f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}
		m_bToPop = true;
		eNextState = m_pPrevState->GetType();
	}
	else if (m_bStateChanges[STATE_CH_PLAY])
	{
		if (m_pScreen->GetColor().w < 1.0f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}
		eNextState = STATE_GAMEPLAY;
	}
	else if (m_bStateChanges[STATE_CH_HIGHSCORE])
	{
		if (m_pScreen->GetColor().w < 1.0f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}
		eNextState = STATE_HIGHSCORE;
	}
	if (CBaseState::CheckState() == STATE_NULL)
	{
		eNextState = STATE_NULL;
	}

	//Tell the state machine what state we're switching to
	return eNextState;
}
