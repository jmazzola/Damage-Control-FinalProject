/***********************************************
* Filename:  		CCreditsState.h
* Date:      		08/10/2015
* Mod. Date:
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:   		Shows the credits
************************************************/

#include "stdafx.h"
#include "CreditsState.h"
#include "../TinyXML2/tinyxml2.h"

#include "..\Renderer\Scene.h"
#include "..\Renderer\HUDRenderer.h"
#include "..\Renderer\Renderer.h"

#include "..\GUI Manager\GUIManager.h"
#include "..\GUI Manager\UIButton.h"

#include "..\Input Manager\InputManager.h"
#include "..\Audio Manager\AudioSystemWwise.h"
#include "../Renderer/BitmapFont.h"

#include "../TinyXML2/tinyxml2.h"

#include <fstream>

CCreditsState::CCreditsState( ) : CBaseState(STATE_CREDITS)
{
	m_pScene = new CScene( );

}

CCreditsState::~CCreditsState( )
{
	SafeDelete(m_pScreen);
	SafeDelete(m_pScene);
}

void CCreditsState::Input(void)
{
	if (CInputManager::GetInstance( )->IsKeyPressed(eControls::Enter) || CInputManager::GetInstance( )->IsKeyPressed(eControls::ESCAPE)
		|| m_pBack->GetPressed())
	{
		QueueStateChange(STATE_CH_BACK);
	}

	//if (CInputManager::GetInstance()->IsKeyPressed(eControls::UpArrow) || CInputManager::GetInstance()->WasKeyDown(eControls::UpArrow))
	//{
	//	m_fTextPosition+= 0.01f;
	//}
}

void CCreditsState::Update(float fDT)
{
	m_pGUI->Update(fDT);

	m_fColorChange += fDT;
	if (m_fColorChange > CREDITS_COLOR_CHANGE)
	{
		m_fRed = 0.3f + (rand() % 70 + 1) / 100.0f;
		m_fGreen = 0.3f + (rand( ) % 70 + 1) / 100.0f;
		m_fBlue = 0.3f + (rand() % 70 + 1) / 100.0f;
		m_fColorChange = 0.0f;
	}

	m_fTextPosition += fDT / 9.0f;

	if (m_fTextPosition > CREDITS_TEXT_END_POSITION)
	{
		m_fTextPosition = CREDITS_TEXT_START_POSITION;
	}

	//screen fade stuff
	if (m_bFadeIn)
	{
		m_fFadeTimer += fDT;
		m_pScreen->SetColor({ 1, 1, 1, blend(1.0f, 0.0f, m_fFadeTimer * (1 / TRANSITION_TIME)) });
	}
	else if (m_bFadeOut)
	{
		m_fFadeTimer += fDT;
		m_pScreen->SetColor({ 1, 1, 1, blend(0.0f, 1.0f, m_fFadeTimer * (1 / TRANSITION_TIME)) });
	}

	if (m_pScreen->GetColor( ).w <= 0.0f)
	{
		m_fFadeTimer = 0.0f;
		m_bFadeIn = FALSE;
	}
	else if (m_pScreen->GetColor( ).w >= 1.0f)
	{
		m_fFadeTimer = 0.0f;
		m_bFadeOut = FALSE;
	}

}

void CCreditsState::Render(CAssetManager* pAssets)
{
	CRenderer::GetReference( ).Render(m_pScene, pAssets);
	m_pGUI->Render( );
	CBitmapFont::GetInstance( )->DrawString(m_sCredits.c_str( ), -0.8f, m_fTextPosition, DirectX::XMFLOAT4 { m_fRed, m_fGreen, m_fBlue, 1 }, 1.2f);

	CBaseState::DrawNavigationInfo();

	if (m_pScreen)
	{
		m_pScreen->Render();
	}
}

bool CCreditsState::FirstEnter(void)
{
	if (!CBaseState::FirstEnter( ))
	{
		return false;
	}

	ShowCursor(true);

	m_bFadeIn = true, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	m_fColorChange = 0.0f;

	m_fTextPosition = CREDITS_TEXT_START_POSITION;
	AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_CREDITS_LOOP);
	return true;
}

bool CCreditsState::ReEnter(void)
{
	m_bFadeIn = true, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	return CBaseState::ReEnter( );
}

bool CCreditsState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded( ))
	{
		return true;
	}

	CBaseState::Load(pAssets);


	m_pGUI = new CGUIManager;

	m_pScreen = new CUIElement();

	m_pScreen->SetWidth(10000);
	m_pScreen->SetHeight(8000);
	m_pScreen->SetPosition(vec3f(500, 400, 1));
	m_pScreen->SetVisible(true);

	m_pScreen->LoadTexture(L"Assets/Images/Black.png");
	//m_pGUI->AddElement(m_pScreen);

	m_pBackground = new CUIElement( );
	m_pBackground->SetWidth(1000);
	m_pBackground->SetHeight(800);
	m_pBackground->SetPosition(vec3f(500, 400, 1));
	m_pBackground->SetVisible(true);

	m_pBackground->LoadTexture(L"Assets/Images/Credits_Background.jpg");
	m_pBackground->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));

	m_pGUI->AddElement(m_pBackground);

	m_pBack = new CUIButton(std::wstring(L"Assets//Images//Menus//Back.png"), 75, 750, 150, 60, std::string("Void"));
	m_pBack->SetRenderText(false);
	m_pGUI->AddElement(m_pBack);

	//load credits

	std::string helper = std::string(TEXTFILESPATH + std::string("Creditos.txt"));

	std::fstream file(helper.c_str( ), std::ios_base::in);
	if (file.is_open( ) && file.good( ))
	{
		file.seekg(0, std::ios::end);
		size_t size = (size_t)file.tellg( );
		m_sCredits.resize(size);
		file.seekg(0);
		file.read(&m_sCredits[0], size);
	}

	return true;
}

void CCreditsState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded( ))
	{
		return;
	}

	CBaseState::UnLoad( );
}

bool CCreditsState::Exit(void)
{
	if (!GetEntered( ))
	{
		return false;
	}

	AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::STOP_CREDITS_LOOP);
	CBaseState::Exit( );
	return false;
}

eStateType CCreditsState::CheckState(void)
{
	eStateType eNextState = GetType( );

	if (m_bStateChanges[STATE_CH_BACK])
	{
		if (m_pScreen->GetColor( ).w < 1.0f || m_pScreen->GetColor( ).w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}

		eNextState = STATE_MAINMENU;
	}

	if (CBaseState::CheckState( ) == STATE_NULL)
	{
		eNextState = STATE_NULL;
	}

	return eNextState;
}

