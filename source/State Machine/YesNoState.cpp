/***********************************************
* Filename:  		YesNoState.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This is the state used for
*			 confirming any actions
************************************************/
#include "YesNoState.h"
#include "../GUI Manager/UIButton.h"
#include "../GUI Manager/GUIManager.h"
#include "../Input Manager/InputManager.h"

#include "../Renderer/Scene.h"

#include "../Renderer/Renderer.h"

/*****************************************************************
* Default Constructor
*****************************************************************/
CYesNoState::CYesNoState(void) :CBaseState(STATE_YESNO)
{
	m_bTransparent = true;

	m_pScene = new CScene( );
}

/*****************************************************************
* Default Destructor
*****************************************************************/
CYesNoState::~CYesNoState(void)
{
	SafeDelete(m_pScene);
}

void CYesNoState::Input(void)
{
	if (!CInputManager::GetInstance()->IsButtonDown(eControls::MouseLeft))
	{
		if (m_pNo->GetPressed())
		{
			QueueStateChange(eStateChange::STATE_CH_NO);
		}
		else if (m_pYes->GetPressed())
		{
			QueueStateChange(eStateChange::STATE_CH_YES);
		}
	}
}

void CYesNoState::Update(float fDT)
{
	m_pScene->Update({ 0, 0, 0 }, fDT);

	if (m_pPrevState)
	{
		m_pPrevState->Update(0);

		if (m_pPrevState->GetPrevState())
		{
			m_pPrevState->GetPrevState()->Update(0);
		}
	}

	m_pGUI->Update(fDT);
}

void CYesNoState::Render(CAssetManager* pRenderer)
{
	CRenderer::GetReference( ).Render(m_pScene, pRenderer);

	if (m_pPrevState)
	{
		m_pPrevState->Render(pRenderer);
	}

	m_pGUI->Render();
}

bool CYesNoState::FirstEnter(void)
{
	CBaseState::FirstEnter();
	Load(nullptr, nullptr);
	ClearStateChanges();
	//m_pYes->SetPressed(false);
	//m_pNo->SetPressed(false);
	return true;
}

bool CYesNoState::ReEnter(void)
{
	return CBaseState::ReEnter();
	return true;
}

bool CYesNoState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded())
	{
		return true;
	}
	CBaseState::Load(pAssets);
	//Load in the GUI
	m_pGUI = new CGUIManager();

	//Load the Back plate

	CUIElement* m_pScreen = new CUIElement();

	m_pScreen->SetWidth(1000);
	m_pScreen->SetHeight(800);
	m_pScreen->SetPosition(vec3f(500, 400, 1));
	m_pScreen->SetVisible(true);
	m_pScreen->LoadTexture(L"Assets/Images/Black.png");
	m_pScreen->SetColor({ 1, 1, 1, .5f });
	m_pGUI->AddElement(m_pScreen);

	CUIElement* pTemp = new CUIElement();
	pTemp->SetPosition(vec3f(512, 384, 0));
	pTemp->SetWidth(250);
	pTemp->SetHeight(250);
	pTemp->LoadTexture(L"Assets//Images//YesNo//YouSure.png");
	m_pGUI->AddElement(pTemp);
	//Load the yes button
	m_pYes = new CUIButton(std::wstring(L"Assets//Images//YesNo//Yes.png"), 510, 360, 150, 60, std::string("Yes"));
	m_pYes->SetRenderText(false);
	m_pGUI->AddElement(m_pYes);
	//Load the no button
	m_pNo = new CUIButton(std::wstring(L"Assets//Images//YesNo//No.png"), 510, 440, 150, 60, std::string("No"));
	m_pNo->SetRenderText(false);
	m_pGUI->AddElement(m_pNo);
	return true;
}

void CYesNoState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded())
	{
		return;
	}
	CBaseState::UnLoad();
	//m_pNo = m_pYes = nullptr;
}

bool CYesNoState::Exit(void)
{
	if (!GetEntered())
	{
		return false;
	}

	CBaseState::Exit();

	UnLoad(nullptr);

	return true;
}

eStateType CYesNoState::CheckState(void)
{
	eStateType eNextState = GetType();
	if (m_pPrevState)
	{
		//Yes
		if (m_bStateChanges[STATE_CH_YES])
		{
			m_pPrevState->QueueStateChange(STATE_CH_YES);
			eNextState = m_pPrevState->CheckState();
			m_bToPop = true;
		}
		else if (m_bStateChanges[STATE_CH_NO])
		{
			m_pPrevState->QueueStateChange(STATE_CH_NO);
			eNextState = m_pPrevState->CheckState();
			m_bToPop = true;
		}
	}
	//How is YesNo the only state in the stack???
	else
	{
		m_bToPop = true;
	}
	//Still allow for the emergency exit
	if (CBaseState::CheckState() == STATE_NULL)
	{
		return eStateType::STATE_NULL;
	}
	//Keep going
	return eNextState;
}
