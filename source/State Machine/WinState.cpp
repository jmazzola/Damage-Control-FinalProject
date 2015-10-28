/***********************************************
* Filename:  		WinState.cpp
* Date:      		05/23/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:   		This state is used to indicate the
*					player has lost the game
************************************************/

#include "WinState.h"
#include "../Core/Common.h"
#include "../Renderer/Renderer.h"
#include "MainMenuState.h"
#include "../Input Manager/InputManager.h"
#include "../State Machine/GameplayState.h"
#include "../Entity Manager/Entities/Player.h"
#include "../Entity Manager/EntityManager.h"

#include "..\GUI Manager\GUIManager.h"
#include "..\GUI Manager\UIButton.h"

#include "../Renderer/HUDRenderer.h"
#include "..\Renderer\BitmapFont.h"
#include "../Audio Manager/AudioSystemWwise.h"

extern std::string pathA;

bool SortByScoresDesc(TMsScores const& left, TMsScores const& right)
{
	return left.score > right.score;
}

CWinState::CWinState() :CBaseState(STATE_WIN)
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
/*virtual*/ void CWinState::Input(void)
{
	//Return to main menu
	if (CInputManager::GetInstance()->IsKeyPressed(eControls::Enter))
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
/*virtual*/ void CWinState::Update(float fDT)
{
	if (m_pPrevState)
	{
		m_pPrevState->Update(0);
	}

	//m_pGUI->Update(fDT);

	if (false == m_bSoundPlayed)
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_ALL_EXCEPT_MUSIC);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU_ALLGOINGDOWN_LOOP);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_WINSOUND);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_LANDINGPROTOCOL);
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
/*virtual*/ void CWinState::Render(CAssetManager* cRenderer)
{
	if (m_pPrevState)
	{
		m_pPrevState->Render(cRenderer);
	}

	for (auto& element : m_vElements)
	{
		CHUDRenderer::Render2D(element);
	}

	CBitmapFont::GetInstance()->DrawString(CGameplayState::GetEntityManager()->GetPlayer()->GetGameStats()->DumpedInfo().c_str(), -0.4f, 0.8f, DirectX::XMFLOAT4{ 1, 1, 1, 1 }, 1.5f);


	CBitmapFont::GetInstance()->DrawString("You\'ve reached your home planet!", -0.65f, -0.25f, DirectX::XMFLOAT4{ 1, 1, 1, 1 }, 1.5f);

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
/*virtual*/ bool CWinState::FirstEnter(void)
{
	ShowCursor(true);
	m_bSoundPlayed = false;

	m_cOne = "A";
	m_cTwo = "A";
	m_cThree = "A";

	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_ALL);

	m_bPlaySound = false;

	Load(nullptr, nullptr);
	return CBaseState::FirstEnter();

	//CRenderer::GetReference().m_clearColor = DirectX::Colors::Green;
}

bool CWinState::ReEnter(void)
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
/*virtual*/ bool CWinState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded())
		return true;
	CBaseState::Load(pAssets);

	m_vElements.push_back(new CTexture2D(0.f, -0.15f, 1.2f, 1.2f, L"Assets/Images/Victory.png"));

	if (STATE_GAMEPLAY == m_pPrevState->GetType())
	{
		CGameplayState* myGame = (CGameplayState*)m_pPrevState;
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
/*virtual*/ void CWinState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded())
		return;

	CBaseState::UnLoad();

	for (auto& element : m_vElements)
	{
		delete element;
		element = nullptr;
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
/*virtual*/ bool CWinState::Exit(void)
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
/*virtual*/ eStateType CWinState::CheckState(void)
{
	eStateType eNextState = GetType();
	if (m_bStateChanges[STATE_CH_QUIT] || m_bStateChanges[STATE_CH_BACK])
	{
		eNextState = STATE_CREDITS;
	}

	if (CBaseState::CheckState() == STATE_NULL)
	{
		eNextState = STATE_NULL;
	}

	return eNextState;


}

bool CWinState::CheckScores()
{
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

bool CWinState::SaveScores()
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

		if (0 == counter)
		{

		}

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

	scoreList.sort(SortByScoresDesc);

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

