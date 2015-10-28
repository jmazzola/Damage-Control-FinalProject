/***********************************************
* Filename:			StateMachine.cpp
* Date:      		05/12/2015
* Mod. Date:		05/13/2015
* Mod. Initials:	MJG
* Author:    		Michael Gergar
* Purpose:			Contains every system in
*				the application
************************************************/

#include "StateMachine.h"
#include "..\Asset Manager\AssetManager.h"
#include "..\Renderer\Renderer.h"
#include "..\Core\Timer.h"
#include "..\Input Manager\InputManager.h"
#include "..\Audio Manager\AudioSystemWwise.h"
#include "MainMenuState.h"
#include "GameplayState.h"
#include "UpgradeState.h"
#include "ControlsState.h"
#include "CutsceneState.h"
#include "LogoState.h"
#include "LoseState.h"
#include "WinState.h"
#include "OptionsState.h"
#include "PauseState.h"
#include "CreditsState.h"
#include "HighScoreState.h"
#include "YesNoState.h"
#include "Achievements.h"
#include "..\Entity Manager\Pathfinding\PathSearch.h"
#include "../GUI Manager/GUIManager.h"

#include "../Core/BurnsideTimer.h"

#include "../Renderer/DebugRenderer.h"

#include "shlobj.h"

extern HWND window;

std::string pathA;

//Allocates a new state of the type passed in
CBaseState* CreateState(eStateType eType)
{
	CBaseState* pState = nullptr;
	switch (eType)
	{
		case STATE_LOGOS:
		{
			pState = new CLogoState( );
		}
		break;
		case STATE_MAINMENU:
		{
			pState = new CMainMenuState( );
		}
		break;
		case STATE_GAMEPLAY:
		{
			pState = new CGameplayState( );
		}
		break;
		case STATE_CONTROLS:
		{
			pState = new CControlsState( );
		}
		break;
		case STATE_PAUSE:
		{
			pState = new CPauseState( );
		}
		break;
		case STATE_YESNO:
		{
			pState = new CYesNoState( );
		}
		break;
		case STATE_OPTIONS:
		{
			pState = new COptionsState( );
		}
		break;
		case STATE_LOSE:
		{
			pState = new CLoseState( );
		}
		break;
		case STATE_WIN:
		{
			pState = new CWinState( );
		}
		break;
		case STATE_UPGRADE:
		{
			pState = new CUpgradeState( );
		}
		break;
		case STATE_CUTSCENE:
		{
			pState = new CCutsceneState( );
		}
		break;
		case STATE_CREDITS:
		{
			pState = new CCreditsState( );
		}
		break;
		//case STATE_STAGESELECT:
		//{
		//	pState = new CStageSelectState( );
		//}
		//break;
		case STATE_ACHIEVEMENTS:
		{
			pState = new CAchievements( );
		}
		break;
		case STATE_HIGHSCORE:
		{
			pState = new CHighScoreState( );
		}
		break;
		default:
			return nullptr;
	}
	return pState;
}

/*****************************************************************
* CStateMachine():
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
CStateMachine::CStateMachine( )
{
	for (size_t i = 0; i < eStateType::STATE_COUNT; i++)
	{
		m_pStates[i] = nullptr;
	}
}

/*****************************************************************
* ~CStateMachine():
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
CStateMachine::~CStateMachine( )
{
	//Stop();
}

/*****************************************************************
* Start(): Called at the start of the program to allocate memory
*                and start core systems
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
void CStateMachine::Start( )
{
	//DebugPrint("[StateMachine] Starting state machine...\n");

	//Make sure we're not already started
	if (m_bPlaying)
	{
		return;
	}

	HRESULT hr;

	LPWSTR appdataW = NULL;
	hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, NULL, NULL, &appdataW);

	std::wstring pathW = appdataW;
	pathW += L"\\Corrupted Works\\Damage Control\\";

	if (ERROR_SUCCESS != SHCreateDirectoryExW(NULL, pathW.c_str( ), 0))
	{
		if (ERROR_ALREADY_EXISTS != GetLastError( ))
		{
			DebugPrint("fuck. couldn't create appdata path", ConsoleColor::Red);
			pathW.clear( );
		}
	}
	pathA = std::string(pathW.begin( ), pathW.end( ));

	//Logo state has to check if the options file is blank
	m_pStates[eStateType::STATE_LOGOS] = new CLogoState( );
	m_lStateStack.push_front(m_pStates[eStateType::STATE_LOGOS]);

	m_pSoundManager.Initialize( );
	//Load in soundbanks
	m_pSoundManager.LoadSoundBank(L"Init.bnk");
	m_pSoundManager.LoadSoundBank(L"SoundBank.bnk");
	//Unreal sound scale
	m_pSoundManager.SetWorldScale(100.0f);

	//Allocate everything in the universe
	m_pAssetManager = new CAssetManager( );

	if (!m_pStates[eStateType::STATE_OPTIONS])
	{
		m_pStates[eStateType::STATE_OPTIONS] = new COptionsState( );
		COptionsState* options = (COptionsState*)m_pStates[eStateType::STATE_OPTIONS];
		options->Load(m_pAssetManager, m_eCurrentCommands);

		AudioSystemWwise::Get( )->SetRTCPValue(AK::GAME_PARAMETERS::FX_VOLUME, (float)options->GetSFX( ));
		AudioSystemWwise::Get( )->SetRTCPValue(AK::GAME_PARAMETERS::MASTER_VOLUME, (float)options->GetMaster( ));
		AudioSystemWwise::Get( )->SetRTCPValue(AK::GAME_PARAMETERS::MX_VOLUME, (float)options->GetMusic( ));

		UINT width = options->GetWidth( ), height = options->GetHeight( );
		DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

		RECT wndRect = { 0, 0, (LONG)width, (LONG)height };
		//AdjustWindowRect(&wndRect, dwStyle, FALSE);

		int nWindowWidth = wndRect.right - wndRect.left;
		int nWindowHeight = wndRect.bottom - wndRect.top;

		SetWindowPos(window,
					 HWND_TOP,
					 (GetSystemMetrics(SM_CXSCREEN) - nWindowWidth) / 2,
					 (GetSystemMetrics(SM_CYSCREEN) - nWindowHeight) / 2,
					 nWindowWidth,
					 nWindowHeight,
					 SWP_NOZORDER);

		CRenderer::GetReference( ).SetFullscreenState(options->GetFullscreen( ), options->GetGamma( ));

		if (!m_pStates[eStateType::STATE_MAINMENU])
		{
			m_pStates[eStateType::STATE_MAINMENU] = new CMainMenuState( );

			CMainMenuState* main_menu = (CMainMenuState*)m_pStates[eStateType::STATE_MAINMENU];
			main_menu->SetWaifuBander(0 == options->GetWaifu() ? false : true);
		}
	}

	//Start the engines, initialize the other things
	m_pStates[eStateType::STATE_LOGOS]->Load(m_pAssetManager, m_eCurrentCommands);
	CheckCommands( );
	m_pTimer = new CTimer( );
	m_pStates[eStateType::STATE_LOGOS]->Enter( );

	//We're ready to go
	m_bPlaying = true;

	m_fFPS = 0.0f;

	//DebugPrint("\tPress [SPACE] or [ENTER] to enter GameplayState\n");

	if (!m_pStates[eStateType::STATE_HIGHSCORE])
	{
		m_pStates[eStateType::STATE_HIGHSCORE] = new CHighScoreState();
		CHighScoreState* highScores = (CHighScoreState*)m_pStates[eStateType::STATE_HIGHSCORE];
		highScores->Load(m_pAssetManager, m_eCurrentCommands);
		highScores->LoadNormalScores();
		highScores->LoadInfiniteScores();

	}
}

/*****************************************************************
* Update(): Called every frame to capture input, render, and
*                update all contained systems
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
void CStateMachine::Update(void)
{
	CRenderer& renderer = CRenderer::GetReference( ).GetReference( );

	renderer.Clear( );

	const int nFpsUpdateRate = 5;

	// Track frame rate
	static int nNumFrames = 0;
	static BurnsideTimer cFrameTimer;
	++nNumFrames;

	if (nFpsUpdateRate < nNumFrames)
	{
		m_fFPS = nFpsUpdateRate / cFrameTimer.GetElapsedTime( );
		nNumFrames = 0;
		cFrameTimer.Reset( );
	}

	//Update the universal systems
	CInputManager::GetInstance( )->Update( );
	m_pSoundManager.Update( );
	m_pTimer->Signal( );

	//Update our current state
	m_lStateStack.front( )->Input( );
	m_lStateStack.front( )->Update((float)(m_pTimer->Delta( )));

	/*__int64 frequency;
	__int64 startCount, endCount;

	if (QueryPerformanceFrequency((LARGE_INTEGER*)&frequency) != 0)
	QueryPerformanceCounter((LARGE_INTEGER*)&startCount);
	*/

	//Render our current state
	//renderer.SetTimeData((float)m_pTimer->TotalTime( ));
	m_lStateStack.front( )->Render(m_pAssetManager);

	/*if (QueryPerformanceFrequency((LARGE_INTEGER*)&frequency) != 0)
		QueryPerformanceCounter((LARGE_INTEGER*)&endCount);*/

	//Display FPS on the window
	//std::string szStr = "Damage Control ";//| + std::to_string(m_fFPS) + " FPS";
	//SetWindowText(window, szStr.c_str( ));

	//Check the state change
	eStateType eNextState = m_lStateStack.front( )->CheckState( );
	//We're done, shut everything down
	if (eNextState == eStateType::STATE_NULL ||
		(CInputManager::GetInstance( )->IsButtonDown(eControls::LeftAlt) && CInputManager::GetInstance( )->IsKeyPressed(eControls::F4)))
	{
		m_bPlaying = false;
	}
	//Change the state
	else
	{
		//Remove the current state if it wants us to
		while (!m_lStateStack.empty( ) && m_lStateStack.front( )->GetToPop( ))
		{
			ExitState(m_lStateStack.front( ));
			m_lStateStack.pop_front( );
			if (!m_lStateStack.empty( ))
			{
				//Enter the front facing state if not already
				EnterState(m_lStateStack.front( ));
			}
		}
		//We're changing state
		if (m_lStateStack.empty( ) || eNextState != m_lStateStack.front( )->GetType( ))
		{
			bool bInStack = false;
			//Is the next state loaded
			if (m_pStates[eNextState] == nullptr)
			{
				m_pStates[eNextState] = CreateState(eNextState);
			}
			else
			{
				//Is the next state already in the stack
				for (auto iter = m_lStateStack.begin( ); iter != m_lStateStack.end( ); iter++)
				{
					if (iter._Ptr->_Myval->GetType( ) == eNextState)
					{
						bInStack = true;
						break;
					}
				}
			}

			//We found it behind us in the stack
			//Pop everything ahead of the target state
			if (bInStack)
			{
				while (m_lStateStack.front( )->GetType( ) != eNextState)
				{
					if (m_lStateStack.front( )->GetEntered( ))
					{
						ExitState(m_lStateStack.front( ));
					}
					m_lStateStack.pop_front( );
				}
				//Make sure it's still loaded
				if (!m_pStates[eNextState]->GetLoaded( ))
				{
					m_pStates[eNextState]->Load(m_pAssetManager, m_eCurrentCommands);
				}
				//Make sure it's entered
				m_lStateStack.front( )->Enter( );
			}
			//We've gotta put a new guy in the stack
			else
			{
				if (!m_lStateStack.empty( ))
				{
					m_pStates[eNextState]->HandlePrevState(m_lStateStack.front( ));
				}
				if (!m_pStates[eNextState]->GetLoaded( ))
				{
					m_pStates[eNextState]->Load(m_pAssetManager);
				}
				if (!m_lStateStack.empty( ) && !m_pStates[eNextState]->GetTransparent( ))
				{
					ExitState(m_lStateStack.front( ));
				}
				m_pStates[eNextState]->Enter( );
				m_lStateStack.push_front(m_pStates[eNextState]);
			}
			//Load anything else we've been told to
			CheckCommands( );
			//Show or hide the cursor
			if (m_pStates[eNextState]->GetShowCursor( ))
			{
				ShowCursor(TRUE);
			}
			else
			{
				while (ShowCursor(FALSE) >= 0);
			}
			//Make sure the first frame doesn't have a crazy delta time
			m_pTimer->Signal( );
		}
	}

	renderer.Present( );
}

/*****************************************************************
* Stop(): Called at the end of the application, leads to the
*				closing of everything
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
void CStateMachine::Stop(void)
{
	//DebugPrint("[StateMachine] Stopping state machine...\n");
	m_bPlaying = false;
	//Close things down and deallocate memory
	m_lStateStack.clear( );
	for (size_t i = 0; i < eStateType::STATE_COUNT; i++)
	{
		if (m_pStates[i])
		{
			m_pStates[i]->Exit( );
			m_pStates[i]->UnLoad( );
			delete m_pStates[i];
			m_pStates[i] = nullptr;
		}
	}

	if (m_pAssetManager)
	{
		//TODO: Call specific unloading code
		//m_pAssetManager->UnLoadAll();
		delete m_pAssetManager;
		m_pAssetManager = nullptr;
	}

	m_pSoundManager.UnloadSoundBank(L"SoundBank.bnk");
	m_pSoundManager.UnloadSoundBank(L"Init.bnk");
	m_pSoundManager.Shutdown( );

	//if (m_pRenderer)
	//{
	//	//TODO: Call specific unloading code
	//	//m_pRenderer->UnLoad();
	//	delete m_pRenderer;
	//	m_pRenderer = nullptr;
	//}

	if (m_pTimer)
	{
		delete m_pTimer;
		m_pTimer = nullptr;
	}
}

void CStateMachine::CheckCommands(void)
{
	//Whether we should check again
	bool bDoubleCheck = false;
	bool bJustChanged[STATE_COUNT];
	ZeroMemory(bJustChanged, STATE_COUNT);
	do
	{
		bDoubleCheck = false;
		for (size_t i = 0; i < STATE_COUNT; i++)
		{
			//Do nothing if we aren't supposed to or we just changed the status of that state
			if (m_eCurrentCommands[i] == STATE_COMMAND_NULL || bJustChanged[i])
			{

			}
			//Load the state if its not already loaded
			else if (m_eCurrentCommands[i] == STATE_COMMAND_LOAD)
			{
				if (m_pStates[i] == nullptr)
				{
					m_pStates[i] = CreateState((eStateType)i);
				}
				if (!m_pStates[i]->GetLoaded( ))
				{
					m_pStates[i]->Load(m_pAssetManager, m_eCurrentCommands);
					bDoubleCheck = true;
				}
				bJustChanged[i] = true;
			}
			//Unload the state if it's not entered
			else if (m_eCurrentCommands[i] == STATE_COMMAND_UNLOAD && m_pStates[i] && !m_pStates[i]->GetEntered( ) && !m_pStates[i]->GetLoaded( ))
			{
				m_pStates[i]->UnLoad(m_eCurrentCommands);
				bDoubleCheck = true;
				bJustChanged[i] = true;
			}
			m_eCurrentCommands[i] = STATE_COMMAND_NULL;
		}
	} while (bDoubleCheck);

}

void CStateMachine::EnterState(CBaseState* pState)
{
	if (!pState->GetLoaded( ))
	{
		pState->Load(m_pAssetManager, m_eCurrentCommands);
	}
	pState->Enter( );
}

void CStateMachine::ExitState(CBaseState* pState)
{
	//CBaseState* pPrevState = pState->GetPrevState();
	if (pState->Exit( ))
	{
		pState->UnLoad(m_eCurrentCommands);
		//m_pStates[pState->GetType()] = nullptr;
		//delete pState;
	}
	//if (pPrevState && pState->GetTransparent())
	//{
	//	ExitState(pPrevState);
	//}
}