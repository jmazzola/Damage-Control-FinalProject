/***********************************************
* Filename:  		MainMenuState.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This is the state used for
*					the main menu
************************************************/
#include "MainMenuState.h"

#include "GameplayState.h"
#include "OptionsState.h"

#include "..\Renderer\Scene.h"
#include "..\Renderer\HUDRenderer.h"
#include "..\Renderer\FirstPersonCamera.h"
#include "..\Renderer\Renderer.h"

#include "..\GUI Manager\GUIManager.h"
#include "..\GUI Manager\UIElement.h"
#include "..\GUI Manager\UIButton.h"
#include "..\GUI Manager\HealthBar.h"

#include "..\Input Manager\InputManager.h"
#include "..\Audio Manager\AudioSystemWwise.h"

#include "..\Entity Manager\Entity.h"

#include "..\Asset Manager\AssetManager.h"

#include "..\Renderer\structs.h"

#include "../Entity Manager/Entities/Player.h"

#include "../Cutscene Manager/Cutscene.h"

#include "../Renderer/BitmapFont.h"

#include "../Entity Manager/EntityManager.h"

extern std::string pathA;

/*****************************************************************
* Default Constructor
*****************************************************************/
CMainMenuState::CMainMenuState(void) : CBaseState(STATE_MAINMENU)
{

}

/*****************************************************************
* Default Destructor
*****************************************************************/
CMainMenuState::~CMainMenuState(void)
{
	SafeDelete(m_pScreen);
}

void CMainMenuState::Input(void)
{

	if (CInputManager::GetInstance()->IsKeyPressed(eControls::MouseLeft))
	{
		//CUIElement* mouseElement = m_pGUI->GetElementAt(CInputManager::GetInstance( )->GetMousePosition( ));
		std::vector<CUIElement*> elements;
		m_pGUI->GetElementsAt(CInputManager::GetInstance()->GetMousePosition(), elements);
		for (unsigned int i = 0; i < elements.size(); i++)
		{
			if (dynamic_cast<CUIButton*>(elements[i]))
			{
				CUIButton* button = (CUIButton*)elements[i];
				if (button->GetText() == "Start")
				{
					QueueStateChange(STATE_CH_MAINMENU);
				}
				else if (button->GetText() == "How to Play")
				{
					QueueStateChange(STATE_CH_CONTROLS);
				}
				else if (button->GetText() == "Credits")
				{
					QueueStateChange(STATE_CH_CREDITS);
				}
				else if (button->GetText() == "Options")
				{
					QueueStateChange(STATE_CH_OPTIONS);
				}
				else if (button->GetText() == "Achievements")
				{
					QueueStateChange(STATE_CH_ACHIEVEMENTS);
				}
				else if (button->GetText() == "Leaderboards")
				{
					QueueStateChange(STATE_CH_HIGHSCORE);
				}
				else if (button->GetText() == "Quit")
				{
					QueueStateChange(STATE_CH_EXIT);
				}
			}
		}
	}
	else if (CInputManager::GetInstance()->IsKeyPressed(eControls::Enter))
	{
		if (true == m_bSelectMode)
		{
			QueueStateChange(STATE_CH_PLAY);
			m_bEndlessMode = false;
			CPathSearch::GetInstance()->HandleLoading(!m_bEndlessMode);
		}
		else
		{
			QueueStateChange(STATE_CH_MAINMENU);
		}
	}

	if (true == m_bSelectMode)
	{
		if (true == m_pBack->GetPressed())
		{
			QueueStateChange(STATE_CH_MAINMENU);
		}
		if (true == m_pCampaign->GetPressed())
		{
			QueueStateChange(STATE_CH_PLAY);
			m_bEndlessMode = false;
			CPathSearch::GetInstance()->HandleLoading(!m_bEndlessMode);
		}
		if (true == m_pEndless->GetPressed())
		{
			QueueStateChange(STATE_CH_PLAY);
			m_bEndlessMode = true;
			CPathSearch::GetInstance()->HandleLoading(!m_bEndlessMode);
		}
	}
}

void CMainMenuState::Update(float fDT)
{
	m_pGUI->Update(fDT);

	/*m_pScene->Update(XMFLOAT3(0, 0, 0), fDT);

	std::vector<CRObject*> StaticCRObjects = m_pScene->GetRenderable(CRObject::RObjectType::ROBJECT_STATIC);
	if (StaticCRObjects.size())
	{
		DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(fDT * .2f);
		DirectX::XMMATRIX translate = DirectX::XMLoadFloat4x4(&StaticCRObjects[0]->GetLocalMatrix());
		XMStoreFloat4x4(&StaticCRObjects[0]->GetLocalMatrix(), rotation * translate);
	}*/

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

	if (m_pScreen->GetColor().w <= 0.0f)
	{
		m_fFadeTimer = 0.0f;
		m_bFadeIn = FALSE;
	}
	else if (m_pScreen->GetColor().w >= 1.0f)
	{
		m_fFadeTimer = 0.0f;
		m_bFadeOut = FALSE;
	}

}

void CMainMenuState::Render(CAssetManager* pAssetManager)
{
	CRenderer::GetReference().Render(m_pScene, pAssetManager);

	m_pGUI->Render();

	/*if (m_pScreen->GetColor().w == 0.0f)
	{*/
	DrawNavigationInfo();
	//}


	if (m_pScreen)
	{
		m_pScreen->Render();
	}
	/*CBitmapFont::GetInstance( )->DrawString(std::string(
	std::string("Do you believe in magic, in a young girl's heart? \n") +
	std::string("How the music can free her, whenever it starts \n") +
	std::string("And it's magic, if the music is groovy \n") +
	std::string("It makes you feel happy like an old - time movie \n") +
	std::string("I'll tell you about the magic and it'll free your soul \n") +
	std::string("But it's like tryin' to tell a stranger 'bout rock and roll \n") +
	std::string(" \n") +
	std::string("If you believe in magic, don't bother to choose \n") +
	std::string("If it's jug band music or rhythm and blues \n") +
	std::string("Just go and listen, it'll start with a smile \n") +
	std::string("It won't wipe off your face, no matter how hard you try \n") +
	std::string("Your feet start tapping and you can't seem to find \n") +
	std::string("How you got there, so just blow your mind \n") +
	std::string(" \n") +
	std::string("If you believe in magic, come along with me \n") +
	std::string("We'll dance until mornin' 'til there's just you and me \n") +
	std::string("And maybe, if the music is right \n") +
	std::string("I'll meet you tomorrow, sort of late at night \n") +
	std::string("And we'll go dancing, baby, then you'll see \n") +
	std::string("How the magic's in the music and the music's in me \n") +
	std::string(" \n") +
	std::string("Yeah, do you believe in magic ? \n") +
	std::string("Yeah, believe in the magic of a young girl's soul \n") +
	std::string("Believe in the magic of rock and roll \n") +
	std::string("Believe in the magic that can set you free \n") +
	std::string("Oh, talkin' 'bout magic \n") +
	std::string(" \n") +
	std::string("(Do you believe like I believe ? ) \n") +
	std::string("Do you believe in magic ? \n") +
	std::string("(Do you believe like I believe ? ) \n") +
	std::string("Do you believe, believer ? \n") +
	std::string("(Do you believe like I believe ? ) \n") +
	std::string("Do you believe in magic ? \n") +
	std::string("(Do you believe like I believe ? ) \n")
	).c_str( ),
	-1, .95f, { 1, 1, 1, 1 }, .5f);*/
}

bool CMainMenuState::FirstEnter(void)
{
	CBaseState::FirstEnter();

	m_pScene->AddCamera(new CFirstPersonCamera());
	m_pScene->AddSkybox(new CSkybox());
	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_ALL);
	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_MUSIC_LOOP_MENU);

	//for some damned reason the State Machine's Enter function doesn't also
	//	call it's ReEnter function.
	//		Think about it: stuff that needs to happen EVERY TIME should happen
	//			the FIRST TIME too...
	m_bFadeIn = true, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	m_bSelectMode = false;
	m_pBackPlate->SetVisible(m_bSelectMode);
	m_pBack->SetVisible(m_bSelectMode);
	m_pCampaign->SetVisible(m_bSelectMode);
	m_pEndless->SetVisible(m_bSelectMode);

	return true;
}

bool CMainMenuState::ReEnter(void)
{
	m_pScene->AddCamera(new CFirstPersonCamera());
	m_pScene->AddSkybox(new CSkybox());

	m_bFadeIn = true, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_ALL);
	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_MUSIC_LOOP_MENU);

	m_bSelectMode = false;
	m_pBackPlate->SetVisible(m_bSelectMode);
	m_pBack->SetVisible(m_bSelectMode);
	m_pCampaign->SetVisible(m_bSelectMode);
	m_pEndless->SetVisible(m_bSelectMode);

	return CBaseState::ReEnter();
}

bool CMainMenuState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded())
	{
		return true;
	}
	//DebugPrint("[MainMenu] Loading MainMenuState...\n");
	CBaseState::Load(pAssets);

	if (pLoadCommands)
	{
		pLoadCommands[STATE_CONTROLS] = STATE_COMMAND_LOAD;
		pLoadCommands[STATE_CREDITS] = STATE_COMMAND_LOAD;
		//		pLoadCommands[STATE_STAGESELECT] = STATE_COMMAND_LOAD;
	}

	/*std::ifstream fin(pathA + "Options.txt", std::ios_base::in);

	if (fin.is_open())
	{

	int _nMasterBank = -1;

	BOOL _nFullscreen, _waifu;

	fin >> _nMasterBank;
	fin >> _nMasterBank;
	fin >> _nMasterBank;

	fin >> _nMasterBank;
	fin >> _nMasterBank;
	fin >> _nFullscreen;

	fin >> _waifu;

	if (0 == _waifu)
	{
	m_bWaifuBander = false;
	}
	else
	{
	m_bWaifuBander = true;
	}
	}
	fin.close();*/

	m_pScene = new CScene();

	m_pGUI = new CGUIManager();//CGUIManager::Load(std::string("Assets/Xml/MainMenu.xml"));

	//m_pLogo = new CUIElement( );
	//
	//m_pLogo->SetWidth(256);
	//m_pLogo->SetHeight(256);
	//m_pLogo->SetPosition(vec3f(1000 - m_pLogo->GetBounds( ).ComputeWidth( ) / 2, m_pLogo->GetBounds( ).ComputeHeight( ) / 2, 1));
	//m_pLogo->SetVisible(true);
	//
	//m_pLogo->LoadTexture(L"Assets/Images/titleLogo.png");
	//
	//m_pGUI->AddElement(m_pLogo);

	//Background
	CUIElement* pBackground = new CUIElement();
	pBackground->SetHeight(GUI_HEIGHT);
	pBackground->SetWidth(GUI_WIDTH);
	pBackground->SetPosition(vec3f(GUI_WIDTH * 0.5f, GUI_HEIGHT * 0.5f, 0));
	pBackground->LoadTexture(L"Assets/Images/MainMenu/Main_Menu.png");
	m_pGUI->AddElement(pBackground);

	//Add buttons
	CUIButton* pButton = new CUIButton(std::wstring(L"Assets/Images/MainMenu/Start.png"), GUI_WIDTH * 0.708f, GUI_HEIGHT * 0.32f, 200, 60, std::string("Start"));
	m_pGUI->AddElement(pButton);
	pButton = new CUIButton(std::wstring(L"Assets/Images/MainMenu/Leaderboards.png"), GUI_WIDTH * 0.708f, GUI_HEIGHT * 0.4f, 200, 60, std::string("Leaderboards"));
	m_pGUI->AddElement(pButton);
	pButton = new CUIButton(std::wstring(L"Assets/Images/MainMenu/Achievements.png"), GUI_WIDTH * 0.708f, GUI_HEIGHT * 0.48f, 200, 60, std::string("Achievements"));
	m_pGUI->AddElement(pButton);
	pButton = new CUIButton(std::wstring(L"Assets/Images/MainMenu/How_To_Play.png"), GUI_WIDTH * 0.708f, GUI_HEIGHT * 0.56f, 200, 60, std::string("How to Play"));
	m_pGUI->AddElement(pButton);
	pButton = new CUIButton(std::wstring(L"Assets/Images/MainMenu/Options.png"), GUI_WIDTH * 0.708f, GUI_HEIGHT * 0.64f, 200, 60, std::string("Options"));
	m_pGUI->AddElement(pButton);
	pButton = new CUIButton(std::wstring(L"Assets/Images/MainMenu/Credits.png"), GUI_WIDTH * 0.708f, GUI_HEIGHT * 0.72f, 200, 60, std::string("Credits"));
	m_pGUI->AddElement(pButton);
	pButton = new CUIButton(std::wstring(L"Assets/Images/MainMenu/Exit.png"), GUI_WIDTH * 0.708f, GUI_HEIGHT * 0.80f, 200, 60, std::string("Quit"));
	m_pGUI->AddElement(pButton);

	m_pBackPlate = new CUIElement();

	m_pBackPlate->SetWidth(300);
	m_pBackPlate->SetHeight(300);
	m_pBackPlate->SetColor({ 1, 1, 1, 0.25f });
	m_pBackPlate->SetPosition(vec3f(300, 375, 1));
	m_pBackPlate->SetVisible(false);
	m_pBackPlate->LoadTexture(L"Assets/Images/Menus/Main_Menu_Border_v4.png");
	m_pGUI->AddElement(m_pBackPlate);

	//Load whole menu
	m_pBack = new CUIButton(std::wstring(L"Assets//Images//Menus//Back.png"), 300, 460, 150, 60, std::string("Void"));
	m_pBack->SetRenderText(false);
	m_pBack->SetVisible(false);
	m_pGUI->AddElement(m_pBack);


	m_pCampaign = new CUIButton(std::wstring(L"Assets//Images//Menus//Campaign.png"), 300, 300, 150, 60, std::string("Void"));
	m_pCampaign->SetRenderText(false);
	m_pCampaign->SetVisible(false);
	m_pGUI->AddElement(m_pCampaign);

	m_pEndless = new CUIButton(std::wstring(L"Assets//Images//Menus//Endless.png"), 300, 380, 150, 60, std::string("Void"));
	m_pEndless->SetRenderText(false);
	m_pEndless->SetVisible(false);
	m_pGUI->AddElement(m_pEndless);

	m_pScreen = new CUIElement();
	m_pScreen->SetWidth(10000);
	m_pScreen->SetHeight(8000);
	m_pScreen->SetPosition(vec3f(500, 400, 1));
	m_pScreen->SetVisible(true);

	m_pScreen->LoadTexture(L"Assets/Images/Black.png");
	//m_pGUI->AddElement(m_pScreen);

#ifdef SPINNING_SEXY
	//test object for Deferred
	{
		pAssets->LoadFBX("Assets//Models//Deferred.fbx", "Spinning Sexy");
		CRObject* bob = new CRObject();
		bob->SetTag("Spinning Sexy");
		m_pScene->AddObject(bob);
	}

	//set some light stuff
	{
		using namespace MathHelper;
		auto Lerp = [&](const DirectX::XMFLOAT3& x, const DirectX::XMFLOAT3& y, const DirectX::XMFLOAT3& s) -> DirectX::XMFLOAT3
		{
			return x + s * (y - x);
		};

		float amb = .45f;
		LightParams ambient({ amb, amb, amb }, { 0, -1, 0 });
		m_pScene->AddLight(ambient);

		LightParams light;
		light.Type = Point;
		light.Range = 2.0f;

		const int cubeSize = 3 + 0 * 2;
		const int cubeMin = -(cubeSize / 2);
		const int cubeMax = cubeSize / 2;

		const DirectX::XMFLOAT3 minExtents(-4.0f, 1.0f, -4.0f);
		const DirectX::XMFLOAT3 maxExtents(4.0f, 11.0f, 4.0f);
		const DirectX::XMFLOAT3 minColor(1.0f, 0.0f, 0.0f);
		const DirectX::XMFLOAT3 maxColor(0.0f, 1.0f, 1.0f);

		for (int x = cubeMin; x <= cubeMax; x++)
		{
			for (int y = cubeMin; y <= cubeMax; y++)
			{
				for (int z = cubeMin; z <= cubeMax; z++)
				{
					DirectX::XMFLOAT3 lerp;
					lerp.x = static_cast<float>(x - cubeMin) / (cubeSize - 1);
					lerp.y = static_cast<float>(y - cubeMin) / (cubeSize - 1);
					lerp.z = static_cast<float>(z - cubeMin) / (cubeSize - 1);

					light.Position = Lerp(minExtents, maxExtents, lerp);
					light.Color = Lerp(minColor, maxColor, lerp) * 1.5f * 1.5f;
					m_pScene->AddLight(light);
				}
			}
		}
	}
#endif
	static CEntity e;
	//Since this thing's only use us as a listener, I think I'll commandeer the render type and use it as a bool
	if (e.GetRObjectType() == CRObject::ROBJECT_STATIC)
	{
		AudioSystemWwise::Get()->RegisterListener(&e, NULL);
		e.SetRObjectType(CRObject::ROBJECT_TRANSPARENT);
	}

	return true;
}

void CMainMenuState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded())
	{
		return;
	}

	//DebugPrint("[MainMenu] Unloading MainMenuState assets...\n");
	CBaseState::UnLoad();

	if (m_pScene)
	{
		for (int i = 0; i < CRObject::RObjectType::ROBJECT_COUNT; i++)
		{
			for (auto& renderable : m_pScene->GetRenderable((CRObject::RObjectType)i))
				SafeDelete(renderable);
		}

		SafeDelete(m_pScene);
	}

	SafeDelete(m_pScreen);
}

bool CMainMenuState::Exit(void)
{
	if (!GetEntered())
	{
		return false;
	}

	if (CGameplayState::GetEntityManager())
	{
		CGameplayState::GetEntityManager()->GetPlayer()->GetGameStats()->Clear();
	}

	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_MUSIC_LOOP_MENU);
	CBaseState::Exit();

	return true;
}

eStateType CMainMenuState::CheckState(void)
{
	//If it stays this, we continue running
	eStateType eNextState = GetType();

	if (m_bStateChanges[STATE_CH_QUIT] || m_bStateChanges[STATE_CH_BACK])
	{
		eNextState = STATE_NULL;
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_QUIT);
	}
	else if (m_bStateChanges[STATE_CH_MAINMENU])
	{
		//if (m_pScreen->GetColor().w < 1.0f || m_pScreen->GetColor().w == 0.0f)
		//{
		//	m_bFadeOut = TRUE;
		//	return eNextState;
		//}
		////m_pScreen->SetVisible(false);
		//m_bFadeOut = FALSE;
		//eNextState = STATE_STAGESELECT;

		m_bSelectMode = !m_bSelectMode;
		m_pBackPlate->SetVisible(m_bSelectMode);
		m_pBack->SetVisible(m_bSelectMode);
		m_pCampaign->SetVisible(m_bSelectMode);
		m_pEndless->SetVisible(m_bSelectMode);

		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_SELECT);
	}
	else if (m_bStateChanges[STATE_CH_CREDITS])
	{
		//if (FadeToBlack(m_fDT, 1.0f) == FALSE)
		//{
		//	return GetType();
		//}

		if (m_pScreen->GetColor().w < 1.0f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}

		//DebugPrint("[MainMenuState] Switching to GameplayState...\n");

		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_SELECT);
		eNextState = STATE_CREDITS;
	}
	else if (m_bStateChanges[STATE_CH_PLAY])
	{
		if (m_pScreen->GetColor().w < 1.0f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}

		eNextState = STATE_GAMEPLAY;
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_SELECT);
	}
	else if (m_bStateChanges[STATE_CH_OPTIONS])
	{
		if (m_pScreen->GetColor().w < 1.0f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}

		eNextState = STATE_OPTIONS;
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_SELECT);
	}
	else if (m_bStateChanges[STATE_CH_CONTROLS])
	{
		if (m_pScreen->GetColor().w < 1.0f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}

		eNextState = STATE_CONTROLS;
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_SELECT);
	}
	else if (m_bStateChanges[STATE_CH_ACHIEVEMENTS])
	{
		if (m_pScreen->GetColor().w < 1.0f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}

		eNextState = STATE_ACHIEVEMENTS;
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_SELECT);
	}
	else if (m_bStateChanges[STATE_CH_HIGHSCORE])
	{
		if (m_pScreen->GetColor().w < 1.0f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}

		eNextState = STATE_HIGHSCORE;
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_SELECT);
	}


	if (CBaseState::CheckState() == STATE_NULL)
	{
		eNextState = STATE_NULL;
	}

	//Return the next state
	return eNextState;
}
