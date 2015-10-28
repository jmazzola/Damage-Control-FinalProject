/***********************************************
* Filename:  		LogoState.cpp
* Date:      		08/06/2015
* Mod. Date:
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:   		This is the state that shows the studio logo and
team name
************************************************/

#include "LogoState.h"

#include "..\Renderer\Scene.h"
#include "..\Renderer\HUDRenderer.h"
#include "..\Renderer\Renderer.h"

#include "..\GUI Manager\GUIManager.h"
#include "..\GUI Manager\UIButton.h"

#include "..\Input Manager\InputManager.h"
#include "..\Audio Manager\AudioSystemWwise.h"

#include "../Renderer/BitmapFont.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define SET_BIT(var,pos) ((var) |= (1<<(pos)))

extern std::string pathA;

CLogoState::CLogoState() : CBaseState(STATE_LOGOS)
{
	if (PathFileExistsA((pathA + "Options.txt").c_str()))
	{
		m_bCanSkip = true;
	}
	else
	{
		m_bCanSkip = false;
	}

	m_fFadeIn = 0;
	m_fFadeOut = 0;

	m_pScene = new CScene();

	TRANSITION_TIME = 1.0f;
}


CLogoState::~CLogoState()
{
	delete m_pScene;
}

void CLogoState::Input(void)
{

	if (true == m_bCanSkip)
	{
		if (CInputManager::GetInstance()->IsKeyPressed(eControls::Spacebar) || 
			CInputManager::GetInstance()->IsKeyPressed(eControls::MouseLeft) || 
			CInputManager::GetInstance()->IsKeyPressed(eControls::Enter) ||
			CInputManager::GetInstance()->IsKeyPressed(eControls::ESCAPE))
		{
			QueueStateChange(STATE_CH_MAINMENU);
		}
	}
}

void CLogoState::Update(float fDT)
{
	m_fDT = fDT;
	m_fStateTimer += fDT;

	m_pGUI->Update(fDT);

	if (CHECK_BIT(m_nPhaseCheck, 2) == TRUE)
	{
		return;
	}

	if (m_fStateTimer > IMAGECHANGETIMER)
	{
		if (FadeToWhite(fDT, 1.0f) == TRUE)
		{
			if (FadeToNotBlack(fDT, 0.0f) == TRUE)
			{
				if (CHECK_BIT(m_nPhaseCheck, 0) == FALSE)
				{
					SET_BIT(m_nPhaseCheck, 0);
					m_fStateTimer = 0;
					m_pGPLogo->SetVisible(false);
					m_pStudioLogo->SetVisible(true);

					m_fFadeIn = 0;
					m_fFadeOut = 0;
				}
				else if (CHECK_BIT(m_nPhaseCheck, 1) == FALSE)
				{
					SET_BIT(m_nPhaseCheck, 1);
					m_fStateTimer = 0;
					m_pStudioLogo->SetVisible(false);
					m_pTeamLogo->SetVisible(true);

					m_fFadeIn = 0;
					m_fFadeOut = 0;
				}
				else if (CHECK_BIT(m_nPhaseCheck, 2) == FALSE)
				{
					SET_BIT(m_nPhaseCheck, 2);
					m_fStateTimer = 0;
					m_pTeamLogo->SetVisible(false);
					m_fFadeIn = 0;
					m_fFadeOut = 0;

					QueueStateChange(STATE_CH_MAINMENU);
				}
			}
		}
	}

	if (m_fFadeIn >= 0.0f && FadeToNotBlack(fDT, 0.0f) == FALSE)
	{
		return;
	}
}

void CLogoState::Render(CAssetManager* pAssetManager)
{
	CRenderer::GetReference().Render(m_pScene, pAssetManager);
	m_pGUI->Render();
}

bool CLogoState::FirstEnter(void)
{
	if (!CBaseState::FirstEnter())
	{
		return false;
	}
	//CRenderer::GetReference().SetColorData(DirectX::XMFLOAT4(1, 1, 1, 1));
	m_nPhaseCheck = 0;
	m_fFadeOut = 0.0f;
	m_fFadeIn = TRANSITION_TIME;
	m_pTeamLogo->SetVisible(false);
	m_pStudioLogo->SetVisible(false);
	return true;
}

bool CLogoState::ReEnter(void)
{
	return CBaseState::ReEnter();
}

bool CLogoState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded())
	{
		return true;
	}

	CBaseState::Load(pAssets);

	if (pLoadCommands)
	{
		pLoadCommands[STATE_MAINMENU] = STATE_COMMAND_LOAD;
		pLoadCommands[STATE_OPTIONS] = STATE_COMMAND_LOAD;
	}

	m_pGUI = new CGUIManager;

	CUIElement* pTemp = new CUIElement();

	pTemp->SetWidth(1000);
	pTemp->SetHeight(800);
	pTemp->SetPosition(vec3f(500, 400, 1));
	pTemp->SetColor(DirectX::XMFLOAT4{ 0, 0, 0, 1 });
	pTemp->SetVisible(true);

	pTemp->LoadTexture(L"Assets/Images/White.png");

	m_pGUI->AddElement(pTemp);
	//HACK in a white background so we can see the logos properly
	m_pGPLogo = new CUIElement();
	m_pGPLogo->SetWidth(512);
	m_pGPLogo->SetHeight(512);
	m_pGPLogo->SetPosition(vec3f(500, 400, 1));
	m_pGPLogo->SetVisible(true);
	m_pGPLogo->LoadTexture(L"Assets/Images/Logos/GPGames_Logo.png");
	m_pGUI->AddElement(m_pGPLogo);
	//Load in the logos
	m_pStudioLogo = new CUIElement();
	m_pStudioLogo->SetWidth(512);
	m_pStudioLogo->SetHeight(512);
	m_pStudioLogo->SetPosition(vec3f(500, 400, 1));
	m_pStudioLogo->SetVisible(false);
	m_pStudioLogo->LoadTexture(L"Assets/Images/Logos/TeamLogo.png");
	m_pGUI->AddElement(m_pStudioLogo);

	m_pTeamLogo = new CUIElement();
	m_pTeamLogo->SetWidth(512);
	m_pTeamLogo->SetHeight(512);
	m_pTeamLogo->SetPosition(vec3f(500, 400, 1));
	m_pTeamLogo->SetVisible(false);
	m_pTeamLogo->LoadTexture(L"Assets/Images/Logos/CorruptedWorks.png");
	m_pGUI->AddElement(m_pTeamLogo);

	m_pScreen = new CUIElement();
	//Add the screen element for fading in and out
	m_pScreen->SetWidth(1000);
	m_pScreen->SetHeight(800);
	m_pScreen->SetPosition(vec3f(500, 400, 1));
	m_pScreen->SetVisible(true);

	m_pScreen->LoadTexture(L"Assets/Images/White.png");
	m_pScreen->SetColor(DirectX::XMFLOAT4(0, 0, 0, 0));

	m_pGUI->AddElement(m_pScreen);

	return true;
}

void CLogoState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded())
	{
		return;
	}

	CBaseState::UnLoad();
}

bool CLogoState::Exit(void)
{
	CBaseState::Exit();
	return true;
}

eStateType CLogoState::CheckState(void)
{
	//If it stays this, we continue running
	eStateType eNextState = GetType();

	if (m_bStateChanges[STATE_CH_MAINMENU])
	{
		if (FadeToBlack(m_fDT, 1.0f) == FALSE)
		{
			return GetType();
		}
		m_bToPop = true;
		eNextState = STATE_MAINMENU;
	}

	if (CBaseState::CheckState() == STATE_NULL)
	{
		eNextState = STATE_NULL;
	}

	if (eNextState != GetType())
	{
		m_fFadeIn = 0;
		m_fFadeOut = 0;
	}

	return eNextState;
}

bool CLogoState::FadeToBlack(float _fDT, float _fAlpha)
{
	m_fFadeOut += _fDT;

	float c = m_fFadeOut / TRANSITION_TIME;
	m_pScreen->SetColor(DirectX::XMFLOAT4(0, 0, 0, c > _fAlpha ? _fAlpha : c));

	if (c >= TRANSITION_TIME)// + .5f)
	{
		return TRUE;
	}

	return FALSE;
}

bool CLogoState::FadeToNotBlack(float _fDT, float _fAlpha)
{
	m_fFadeIn -= _fDT;

	float c = m_fFadeIn / TRANSITION_TIME;
	m_pScreen->SetColor(DirectX::XMFLOAT4(1, 1, 1, c < _fAlpha ? _fAlpha : c));

	if (c <= 0.0f)// TRANSITION_TIME + .5f)
	{
		return TRUE;
	}

	return FALSE;
}

bool CLogoState::FadeToWhite(float _fDT, float _fAlpha)
{
	m_fFadeOut += _fDT;

	float c = m_fFadeOut / TRANSITION_TIME;
	m_pScreen->SetColor(DirectX::XMFLOAT4(1, 1, 1, c > _fAlpha ? _fAlpha : c));

	if (c >= TRANSITION_TIME)// + .5f)
	{
		return TRUE;
	}

	return FALSE;
}