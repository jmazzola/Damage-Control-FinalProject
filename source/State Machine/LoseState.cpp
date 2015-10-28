/***********************************************
* Filename:  		LooseState.cpp
* Date:      		05/23/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:   		This state is used to indicate the
*					player has lost the game
************************************************/

#include "stdafx.h"

#include "LoseState.h"
#include "../Core/Common.h"
#include "../Renderer/Renderer.h"
#include "MainMenuState.h"
#include "../Input Manager/InputManager.h"
#include "../State Machine/GameplayState.h"
#include "../Entity Manager/EntityManager.h"
#include "../Asset Manager/AssetManager.h"
#include "../Animation Manager/Animation.h"

#include "../Renderer/BitmapFont.h"
#include "..\GUI Manager\GUIManager.h"
#include "../GUI Manager/UIElement.h"
#include "../GUI Manager/UIButton.h"

#include "../Entity Manager/Entities/Player.h"
#include "../Audio Manager/AudioSystemWwise.h"

extern std::string pathA;

bool SortByScoresesDesc(TMsScores const& left, TMsScores const& right)
{
	return left.score > right.score;
}

//static float X = 0.0f;
//static float Y = 0.0f;
CLoseState::CLoseState() : CBaseState(STATE_LOSE), m_fTimer(0.0f)
{
	m_bTransparent = true;
}
/*****************************************************************
* Input()		Handles the input for this state
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:
*
* Mod. Date:		      05/23/2015
* Mod. Initials:	      MJG
*****************************************************************/
/*virtual*/ void CLoseState::Input(void)
{
	//Return to main menu
	if (CInputManager::GetInstance()->IsKeyPressed(eControls::Spacebar) || CInputManager::GetInstance()->IsKeyPressed(eControls::Enter))
	{
		QueueStateChange(eStateChange::STATE_CH_QUIT);
	}

	if (true == m_pBack->GetPressed())
	{
		QueueStateChange(eStateChange::STATE_CH_QUIT);
	}

	if (true == m_bShowButtons)
	{
		if (true == m_pCharOneUp->GetPressed())
		{
			m_cOne[0]++;
		}
		if (true == m_pCharOneDown->GetPressed())
		{
			m_cOne[0]--;
		}

		if (true == m_pCharTwoUp->GetPressed())
		{
			m_cTwo[0]++;
		}
		if (true == m_pCharTwoDown->GetPressed())
		{
			m_cTwo[0]--;
		}

		if (true == m_pCharThreeUp->GetPressed())
		{
			m_cThree[0]++;
		}
		if (true == m_pCharThreeDown->GetPressed())
		{
			m_cThree[0]--;
		}

		if (m_cOne[0] < '!')
		{
			m_cOne[0] = '~';
		}
		if (m_cOne[0] > '~')
		{
			m_cOne[0] = '!';
		}

		if (m_cTwo[0] < '!')
		{
			m_cTwo[0] = '~';
		}
		if (m_cTwo[0] > '~')
		{
			m_cTwo[0] = '!';
		}

		if (m_cThree[0] < '!')
		{
			m_cThree[0] = '~';
		}
		if (m_cThree[0] > '~')
		{
			m_cThree[0] = '!';
		}


		if (true == m_bPlaySound)
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_SELECT);
			m_bPlaySound = false;

		}
	}

	/*if (CInputManager::GetInstance()->IsButtonDown(eControls::H))
		X -= 0.01f;
		if (CInputManager::GetInstance()->IsButtonDown(eControls::K))
		X += 0.01f;
		if (CInputManager::GetInstance()->IsButtonDown(eControls::U))
		Y -= 0.01f;
		if (CInputManager::GetInstance()->IsButtonDown(eControls::J))
		Y += 0.01f;*/
}

/*****************************************************************
* Update()
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  CBaseState*
*
* Mod. Date:		      05/23/2015
* Mod. Initials:	      MJG
*****************************************************************/
/*virtual*/ void CLoseState::Update(float fDT)
{
	if (m_pPrevState)
	{
		m_pPrevState->Update(0);
	}

	if (false == m_bSoundPlayed)
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_ALL);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_GAMEOVER);
		//AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_);
		m_bSoundPlayed = true;
	}

	m_pGUI->Update(fDT);
}

/*****************************************************************
* Render()
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  CBaseState*
*
* Mod. Date:		      05/23/2015
* Mod. Initials:	      MJG
*****************************************************************/
/*virtual*/ void CLoseState::Render(CAssetManager* cRenderer)
{
	if (false == m_bEntered)
	{
		return;
	}
	if (m_pPrevState)
	{
		m_pPrevState->Render(cRenderer);
	}

	/*if (m_fTimer >= CGameplayState::GetAssetManager()->GetAnimByTag("Player_Die")->m_pAnim.GetDuration())*/
	{
		for (auto& pElement : m_vElements)
		{
			CHUDRenderer::Render2D(pElement);
		}
		CGameplayState* pGameplayState = (CGameplayState*)m_pPrevState;

		if (pGameplayState->GetEntityManager()->GetPlayer()->GetCurrentHealth() <= 0.f)
			CBitmapFont::GetInstance()->DrawString("You were killed by aliens!", -0.5f, -0.27f, DirectX::XMFLOAT4{ 1, 1, 1, 1 }, 1.5f);
		else if (pGameplayState->GetEntityManager()->GetPlayer()->GetOxygen() <= 0.f)
			CBitmapFont::GetInstance()->DrawString("Your head exploded!", -0.45f, -0.27f, DirectX::XMFLOAT4{ 1, 1, 1, 1 }, 1.5f);
		else
			CBitmapFont::GetInstance()->DrawString("Your reactor blew up!", -0.45f, -0.27f, DirectX::XMFLOAT4{ 1, 1, 1, 1 }, 1.5f);

		// Stats
		CBitmapFont::GetInstance()->DrawString(CGameplayState::GetEntityManager()->GetPlayer()->GetGameStats()->DumpedInfo().c_str(), -0.4f, 0.8f, DirectX::XMFLOAT4{ 1, 1, 1, 1 }, 1.5f);


		if (true == m_bShowButtons)
		{
			//enter name
			CBitmapFont::GetInstance()->DrawString("Enter your name:", -0.35f, -0.4f);
			CBitmapFont::GetInstance()->DrawString(m_cOne.c_str(), -0.215f, -0.66f);
			CBitmapFont::GetInstance()->DrawString(m_cTwo.c_str(), -0.055f, -0.66f);
			CBitmapFont::GetInstance()->DrawString(m_cThree.c_str(), 0.1f, -0.66f);
		}

		CBaseState::DrawNavigationInfo();

		m_pGUI->Render();
	}
}

/*****************************************************************
* Enter()
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  CBaseState*
*
* Mod. Date:		      05/23/2015
* Mod. Initials:	      MJG
*****************************************************************/
/*virtual*/ bool CLoseState::FirstEnter(void)
{
	//DebugPrint("Lose state entered...\n", ConsoleColor::Piss);
	//DebugPrint("\tPress [SPACE] or [ENTER] to return to MainMenu\n");

	ShowCursor(true);
	m_bSoundPlayed = false;
	//CRenderer::GetReference().m_clearColor = DirectX::Colors::Red;
	CBaseState::FirstEnter();

	//AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_ALL);
	m_fTimer = 0;
	m_cOne = "A";
	m_cTwo = "A";
	m_cThree = "A";
	m_bPlaySound = false;

	Load(nullptr, nullptr);

	return true;
}

bool CLoseState::ReEnter(void)
{
	return CBaseState::ReEnter();
}

/*****************************************************************
* Load() Loads the state
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  CBaseState*
*
* Mod. Date:		      05/23/2015
* Mod. Initials:	      MJG
*****************************************************************/
/*virtual*/ bool CLoseState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded())
	{
		return true;
	}
	CBaseState::Load(pAssets);

	if (STATE_CUTSCENE == m_pPrevState->GetType())
	{
		CGameplayState* myGame = (CGameplayState*)m_pPrevState->GetPrevState();
		m_bisInfinite = myGame->GetIsInfinite();
		m_nScore = CGameplayState::GetEntityManager()->GetPlayer()->GetGameStats()->m_nTotalScore;
	}

	m_bShowButtons = CheckScores();

	m_pGUI = new CGUIManager();
	if (true == m_bShowButtons)
	{
		m_pBack = new CUIButton(std::wstring(L"Assets//Images//Menus//Accept.png"), 200, 700, 200, 60, std::string("Void"));
	}
	else
	{
		m_pBack = new CUIButton(std::wstring(L"Assets//Images//Menus//Back.png"), 500, 700, 200, 60, std::string("Void"));
	}

	m_pBack->SetRenderText(false);
	m_pGUI->AddElement(m_pBack);


	m_vElements.push_back(new CTexture2D(0, -0.15f, 1.2f, 1.2f, L"Assets/Images/Defeat.png"));


	if (true == m_bShowButtons)
	{
		m_pCharOneUp = new CUIButton(400, 600, 60, 60);
		m_pCharOneUp->LoadTexture(L"Assets/Images/Menus/ArrowUp.png");
		m_pGUI->AddElement(m_pCharOneUp);

		m_pCharOneDown = new CUIButton(400, 700, 60, 60);
		m_pCharOneDown->LoadTexture(L"Assets/Images/Menus/ArrowDown.png");
		m_pGUI->AddElement(m_pCharOneDown);

		m_pCharTwoUp = new CUIButton(480, 600, 60, 60);
		m_pCharTwoUp->LoadTexture(L"Assets/Images/Menus/ArrowUp.png");
		m_pGUI->AddElement(m_pCharTwoUp);

		m_pCharTwoDown = new CUIButton(480, 700, 60, 60);
		m_pCharTwoDown->LoadTexture(L"Assets/Images/Menus/ArrowDown.png");
		m_pGUI->AddElement(m_pCharTwoDown);

		m_pCharThreeUp = new CUIButton(560, 600, 60, 60);
		m_pCharThreeUp->LoadTexture(L"Assets/Images/Menus/ArrowUp.png");
		m_pGUI->AddElement(m_pCharThreeUp);

		m_pCharThreeDown = new CUIButton(560, 700, 60, 60);
		m_pCharThreeDown->LoadTexture(L"Assets/Images/Menus/ArrowDown.png");
		m_pGUI->AddElement(m_pCharThreeDown);
	}

	return true;
}

/*****************************************************************
* Unload() Called to unload the state
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  CBaseState*
*
* Mod. Date:		      05/23/2015
* Mod. Initials:	      MJG
*****************************************************************/
/*virtual*/ void CLoseState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded())
	{
		return;
	}

	CBaseState::UnLoad();

	for (auto& pElement : m_vElements)
	{
		delete pElement;
		pElement = nullptr;
	}
	m_vElements.clear();
	m_bSoundPlayed = false;
}

/*****************************************************************
* Exit()
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  CBaseState*
*
* Mod. Date:		      05/23/2015
* Mod. Initials:	      MJG
*****************************************************************/
/*virtual*/ bool CLoseState::Exit(void)
{
	if (!GetEntered())
	{
		return false;
	}

	if (true == m_bShowButtons)
	{
		SaveScores();
	}

	CBaseState::Exit();
	m_bEntered = false;
	UnLoad(nullptr);

	
	return false;
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
* Mod. Date:		      05/23/2015
* Mod. Initials:	      MJG
*****************************************************************/
/*virtual*/ eStateType CLoseState::CheckState(void)
{
	eStateType eNextState = GetType();
	if (m_bStateChanges[STATE_CH_QUIT] || m_bStateChanges[STATE_CH_BACK])
	{
		eNextState = STATE_MAINMENU;
	}

	if (CBaseState::CheckState() == STATE_NULL)
	{
		eNextState = STATE_NULL;
	}

	return eNextState;
}

bool CLoseState::CheckScores()
{
	//m_nScore = 50000;
	if (0 == m_nScore)
	{
		return false;
	}


	m_vAllScores.clear();
	std::string filename;
	if (true == m_bisInfinite)
	{
		filename = pathA + std::string("HighScoresEndless.bin");
	}
	else
	{
		filename = pathA + std::string("HighScoresNormal.bin");
	}
	fstream loadfile;

	loadfile.open(filename, ios_base::in | ios_base::binary);
	if (loadfile.is_open())
	{
		loadfile.seekg(0, loadfile.end);

		int counter = (int)loadfile.tellg();
		counter /= sizeof(TMsScores);

		loadfile.seekg(0, loadfile.beg);
		for (int i = 0; i < counter; ++i)
		{
			TMsScores temp;

			loadfile.read((char*)&temp, sizeof(TMsScores));
			m_vAllScores.push_back(temp);
		}
		loadfile.close();
	}
	else
	{
		return false;
	}

	size_t size = m_vAllScores.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i == 10)
		{
			return false;
		}

		if (m_vAllScores[i].score < m_nScore)
		{
			return true;
		}
	}

	return true;
}

bool CLoseState::SaveScores()
{
	std::string filename;
	if (true == m_bisInfinite)
	{
		filename = pathA + std::string("HighScoresEndless.bin");
	}
	else
	{
		filename = pathA + std::string("HighScoresNormal.bin");
	}
	fstream savefile;

	TMsScores newEntry;
	newEntry.name[0] = m_cOne[0];
	newEntry.name[1] = m_cTwo[0];
	newEntry.name[2] = m_cThree[0];
	newEntry.name[3] = '\0';
	newEntry.score = m_nScore;

	savefile.open(filename, ios_base::out | ios_base::binary | ios_base::app);
	if (savefile.is_open())
	{
		savefile.seekg(0, savefile.end);
		savefile.write((char*)&newEntry, sizeof(TMsScores));
		savefile.close();
	}
	else
	{
		return false;
	}

	m_vAllScores.clear();

	//load all and save only the top 10
	fstream loadfile;
	vector<TMsScores> vecScores;
	loadfile.open(filename, ios_base::in | ios_base::binary);
	if (loadfile.is_open())
	{
		loadfile.seekg(0, loadfile.end);

		int counter = (int)loadfile.tellg();
		counter /= sizeof(TMsScores);

		loadfile.seekg(0, loadfile.beg);
		for (int i = 0; i < counter; ++i)
		{
			TMsScores temp;

			loadfile.read((char*)&temp.name, sizeof(TMsScores));
			m_vAllScores.push_back(temp);
		}
		loadfile.close();
	}
	else
	{
		return false;
	}

	list<TMsScores> scoreList(m_vAllScores.begin(), m_vAllScores.end());

	scoreList.sort(SortByScoresesDesc);

	size_t size = scoreList.size();
	int i = 0;
	savefile.seekg(0, savefile.end);
	savefile.open(filename, ios_base::out | ios_base::binary | ios_base::trunc);
	if (savefile.is_open())
	{
		for each (TMsScores score in scoreList)
		{
			if (10 == i)
			{
				break;
			}
			savefile.write((char*)&score, sizeof(TMsScores));
			i++;
		}

		savefile.close();
	}
	else
	{
		return false;
	}

	return true;
}

