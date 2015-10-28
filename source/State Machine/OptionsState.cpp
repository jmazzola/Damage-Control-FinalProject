/***********************************************
* Filename:  		OptionsState.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This is the state used for
*			 changing user options
************************************************/
#include "OptionsState.h"
#include "../GUI Manager/GUIManager.h"
#include "../Input Manager/InputManager.h"
#include "../GUI Manager/UIButton.h"
#include "../Audio Manager/AudioSystemWwise.h"

#include "../Renderer/BitmapFont.h"

#include "MainMenuState.h"
#include "GameplayState.h"
#include "../Entity Manager/EntityManager.h"
#include "../Entity Manager/AI/AIDirector.h"

extern std::string pathA;
extern HWND window;

#define MAX_GAMMA 125
#define MIN_GAMMA 75

#define MAX_VOLUME 100
#define VOLUME_STEP 5

std::string resolutions[ ] =
{
	"1024 x 768",
	"1440 x 810",
	"1920 x 1080"
};

//master
//sfx
//bgm

//resolution
//gamma
//fullscreen

//waifu

#include <algorithm>

#undef min
#undef max

int w = 0;

/*****************************************************************
* Default Constructor
*****************************************************************/
COptionsState::COptionsState(void) : CBaseState(STATE_OPTIONS)
{
	m_bTransparent = false;

	std::ifstream fin(pathA + "Options.txt", std::ios_base::in);

	if (!fin.is_open( ))
	{
		WriteDefaults( );
	}
	else
	{
		fin >> m_nMasterBank;
		fin >> m_nSFXBank;
		fin >> m_nMusicBank;

		fin >> m_nResolution;
		fin >> m_nGamma;
		fin >> m_nFullscreen;

		fin >> m_nWaifu;

		if (m_nMasterBank == -1 || m_nSFXBank == -1 || m_nMusicBank == -1 ||
			m_nResolution == -1 || m_nGamma <= -1 ||
			m_nFullscreen == -1 || m_nWaifu == -1)
		{
			WriteDefaults( );
		}
		else if (m_nMasterBank > MAX_VOLUME || m_nSFXBank > MAX_VOLUME || m_nMusicBank > MAX_VOLUME ||
				 m_nResolution > _countof(resolutions) - 1 || m_nGamma > MAX_VOLUME ||
				 m_nFullscreen > 1 || m_nWaifu > 1)
		{
			WriteDefaults( );
		}
	}
	fin.close( );

	m_pScene = new CScene( );
}

void COptionsState::LoadDefaults( )
{
	m_nMasterBank = MAX_VOLUME;
	m_nSFXBank = MAX_VOLUME;
	m_nMusicBank = MAX_VOLUME;

	m_nResolution = 2;
	m_nGamma = 100;
	m_nFullscreen = TRUE;

	m_nWaifu = TRUE;
	m_pScene = new CScene();
}

void COptionsState::WriteDefaults( )
{
	LoadDefaults( );

	// write options to the .txt file

	std::ofstream fout(pathA + "Options.txt", std::ios_base::out | std::ios_base::trunc);

	fout << m_nMasterBank << std::endl;
	fout << m_nSFXBank << std::endl;
	fout << m_nMusicBank << std::endl;
	fout << m_nResolution << std::endl;
	fout << m_nGamma << std::endl;
	fout << m_nFullscreen << std::endl;
	fout << m_nWaifu << std::endl;
}

/*****************************************************************
* Default Destructor
*****************************************************************/
COptionsState::~COptionsState(void)
{
	SafeDelete(m_pScreen);
	SafeDelete(m_pScene);
}

void COptionsState::Input(void)
{
	m_nFullscreen = CRenderer::GetReference( ).m_bFullScreen;
	//BOOL _nWaifu = m_nWaifu;

	bool play_sound = false;

	//Unpause the game
	if (m_pBack->GetPressed( ) ||
		CInputManager::GetInstance( )->IsKeyPressed(eControls::ESCAPE) ||
		CInputManager::GetInstance( )->IsKeyPressed(eControls::Backspace))
	{
		QueueStateChange(STATE_CH_BACK);
	}
	else if (m_pLeft->GetPressed( ))
	{
		//Flip to the left
		PageFlip(eOptionsPage(m_nPageIndex - 1));
	}
	else if (m_pRight->GetPressed( ))
	{
		//Flip to the right
		PageFlip(eOptionsPage(m_nPageIndex + 1));
	}

	//if (m_pWaifuToggle->GetPressed( ))
	//{
	//	m_nWaifu = !m_nWaifu;
	//	//m_pWaifuToggle->SetPressed(false);
	//}

	/*if (m_pAccept->GetPressed( ))
	{
	m_pAccept->SetPressed(false);
	}*/
	else if (m_pApply->GetPressed( ))
	{
		//m_pApply->SetPressed(false);

		QueueStateChange(STATE_CH_BACK);
		return;
	}
	else if (m_pDefault->GetPressed( ))
	{
		//m_pDefault->SetPressed(false);

		WriteDefaults( );
		play_sound = true;
	}

	for (size_t i = 0; i < _countof(m_pPlusButtons); i++)
	{

#pragma region plus button
		if (m_pPlusButtons[i]->GetPressed( ) && true == m_pPlusButtons[i]->GetEnabled( ))
		{
			play_sound = true;

			switch (i)
			{
				case 0:
				{
					switch (m_nPageIndex)
					{
						case eOptionsPage::OPTIONS_PAGE_SOUNDS:
						{
							m_nMasterBank += VOLUME_STEP;
						}
						break;

						case eOptionsPage::OPTIONS_PAGE_GRAPHICS:
						{
							m_nResolution++;
						}
						break;
					}
				}
				break;

				case 1:
				{
					switch (m_nPageIndex)
					{
						case eOptionsPage::OPTIONS_PAGE_SOUNDS:
						{
							m_nSFXBank += VOLUME_STEP;
						}
						break;

						case eOptionsPage::OPTIONS_PAGE_GRAPHICS:
						{
							m_nGamma++;
						}
						break;
					}
				}
				break;

				case 2:
				{
					switch (m_nPageIndex)
					{
						case eOptionsPage::OPTIONS_PAGE_SOUNDS:
						{
							m_nMusicBank += VOLUME_STEP;
						}
						break;

						case eOptionsPage::OPTIONS_PAGE_GRAPHICS:
						{
							m_nFullscreen++;
						}
						break;
					}
				}
				break;

				case 3:
				{
					switch (m_nPageIndex)
					{
						case eOptionsPage::OPTIONS_PAGE_SOUNDS:
						{
							m_nWaifu++;
						}
						break;

						case eOptionsPage::OPTIONS_PAGE_GRAPHICS:
						{
						}
						break;
					}
				}
				break;
			}
		}
#pragma endregion

#pragma region minus button
		else if (m_pMinusButtons[i]->GetPressed( ) && true == m_pMinusButtons[i]->GetEnabled( ))
		{
			play_sound = true;

			switch (i)
			{
				case 0:
				{
					switch (m_nPageIndex)
					{
						case eOptionsPage::OPTIONS_PAGE_SOUNDS:
						{
							m_nMasterBank -= VOLUME_STEP;
						}
						break;

						case eOptionsPage::OPTIONS_PAGE_GRAPHICS:
						{
							m_nResolution--;
						}
						break;
					}
				}
				break;

				case 1:
				{
					switch (m_nPageIndex)
					{
						case eOptionsPage::OPTIONS_PAGE_SOUNDS:
						{
							m_nSFXBank -= VOLUME_STEP;
						}
						break;

						case eOptionsPage::OPTIONS_PAGE_GRAPHICS:
						{
							m_nGamma--;
						}
						break;
					}
				}
				break;

				case 2:
				{
					switch (m_nPageIndex)
					{
						case eOptionsPage::OPTIONS_PAGE_SOUNDS:
						{
							m_nMusicBank -= VOLUME_STEP;
						}
						break;

						case eOptionsPage::OPTIONS_PAGE_GRAPHICS:
						{
							m_nFullscreen--;
						}
						break;
					}
				}
				break;

				case 3:
				{
					switch (m_nPageIndex)
					{
						case eOptionsPage::OPTIONS_PAGE_SOUNDS:
						{
							m_nWaifu--;
						}
						break;

						case eOptionsPage::OPTIONS_PAGE_GRAPHICS:
						{
						}
						break;
					}
				}
				break;
			}
		}
#pragma endregion

	}

	auto bind = [&] (const float _fMin, const float _fMax, int& _nVal) -> void
	{
		_nVal = (int)std::min(_fMax, std::max(_fMin, (float)_nVal));
	};

	auto wrap = [&] (const int _nMin, const int _nMax, int& _nVal)
	{
		if (_nVal < _nMin)
		{
			_nVal = _nMax;
		}

		if (_nVal > _nMax)
		{
			_nVal = _nMin;
		}
	};

	bind(0, MAX_VOLUME, m_nMasterBank);
	bind(0, MAX_VOLUME, m_nSFXBank);
	bind(0, MAX_VOLUME, m_nMusicBank);

	wrap(0, _countof(resolutions) - 1, m_nResolution);
	bind(MIN_GAMMA, MAX_GAMMA, m_nGamma);
	wrap(0, 1, m_nFullscreen);

	wrap(0, 1, m_nWaifu);

	if (play_sound)
	{
		m_bOptionsChanged = true;

		AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_2D_MENU_SELECT);

		switch (m_nPageIndex)
		{
			case eOptionsPage::OPTIONS_PAGE_SOUNDS:
			{
				AudioSystemWwise::Get( )->SetRTCPValue(AK::GAME_PARAMETERS::FX_VOLUME, (float)GetSFX( ));
				AudioSystemWwise::Get( )->SetRTCPValue(AK::GAME_PARAMETERS::MASTER_VOLUME, (float)GetMaster( ));
				AudioSystemWwise::Get( )->SetRTCPValue(AK::GAME_PARAMETERS::MX_VOLUME, (float)GetMusic( ));
			}
			break;
		}
	}

}

void COptionsState::Update(float fDT)
{
	//m_pScene->Update({ 0, 0, 0 }, fDT);

    //if (m_pPrevState)
	//{
	//	m_pPrevState->Update(0);
	//}

	//if (m_bOptionsChanged)
	//{
	//}
	//else
	//{
	//}

	m_pGUI->Update(fDT);

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

	auto bind = [&] (const float _fMin, const float _fMax, int& _nVal) -> void
	{
		_nVal = (int)std::min(_fMax, std::max(_fMin, (float)_nVal));
	};

	auto wrap = [&] (const int _nMin, const int _nMax, int& _nVal)
	{
		if (_nVal < _nMin)
		{
			_nVal = _nMax;
		}

		if (_nVal > _nMax)
		{
			_nVal = _nMin;
		}
	};

	bind(0, MAX_VOLUME, m_nMasterBank);
	bind(0, MAX_VOLUME, m_nSFXBank);
	bind(0, MAX_VOLUME, m_nMusicBank);

	wrap(0, _countof(resolutions) - 1, m_nResolution);
	bind(MIN_GAMMA, MAX_GAMMA, m_nGamma);
	wrap(0, 1, m_nFullscreen);

	wrap(0, 1, m_nWaifu);

	ApplyFullscreen( );
}

void COptionsState::Render(CAssetManager* pAssets)
{
	CRenderer::GetReference( ).Render(m_pScene, pAssets);

	//RECT r;
	//GetWindowRect(window, &r);

	float screen_width = (float)CRenderer::GetReference( ).GetWidth( );
	float screen_height = (float)CRenderer::GetReference( ).GetHeight( );

	float width = GUI_WIDTH;
	float height = GUI_HEIGHT;

	auto screen_to_clip = [&] (DirectX::XMFLOAT2 _fPosition, DirectX::XMFLOAT2 _fDimensions) -> DirectX::XMFLOAT2
	{
		float x = ((_fPosition.x / _fDimensions.x) - .5f) * 2.f;
		float y = ((_fPosition.y / _fDimensions.y) - .5f) * 2.f;
		return DirectX::XMFLOAT2(x, -y);
	};

	CBitmapFont* pF = CBitmapFont::GetInstance( );

	//if (m_pPrevState)
	//{
	//	m_pPrevState->Render(pAssets);
	//}

	bool res = false;
	if (GetWidth( ) != w)
	{
		res = true;
	}
	if (m_nFullscreen != CRenderer::GetReference( ).m_bFullScreen)
	{
		m_nFullscreen = CRenderer::GetReference( ).m_bFullScreen;
	}

	m_pGUI->Render( );

	DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
	float scale = 1.0f;

	for (size_t i = 0; i < _countof(m_pPlusButtons); i++)
	{
		float y = float(287.5 + i * 81.125f);

		switch (i)
		{
			case 0:
			{
				switch (m_nPageIndex)
				{
					case eOptionsPage::OPTIONS_PAGE_SOUNDS:
					{
						DirectX::XMFLOAT2 clip_gui = screen_to_clip({ 653, y }, { width, height });
						pF->DrawString(std::to_string(m_nMasterBank).c_str( ), clip_gui.x, clip_gui.y, color, scale);
					}
					break;

					case eOptionsPage::OPTIONS_PAGE_GRAPHICS:
					{
						DirectX::XMFLOAT2 clip_gui = screen_to_clip({ 615, y }, { width, height });
						pF->DrawString(resolutions[m_nResolution].c_str( ), clip_gui.x, clip_gui.y, color, .75f);
					}
					break;
				}
			}
			break;

			case 1:
			{
				switch (m_nPageIndex)
				{
					case eOptionsPage::OPTIONS_PAGE_SOUNDS:
					{
						DirectX::XMFLOAT2 clip_gui = screen_to_clip({ 653, y }, { width, height });
						pF->DrawString(std::to_string(m_nSFXBank).c_str( ), clip_gui.x, clip_gui.y, color, scale);
					}
					break;

					case eOptionsPage::OPTIONS_PAGE_GRAPHICS:
					{
						DirectX::XMFLOAT2 clip_gui = screen_to_clip({ 653, y }, { width, height });
						pF->DrawString(std::to_string(m_nGamma).c_str( ), clip_gui.x, clip_gui.y, color, scale);
					}
					break;
				}
			}
			break;

			case 2:
			{
				switch (m_nPageIndex)
				{
					case eOptionsPage::OPTIONS_PAGE_SOUNDS:
					{
						DirectX::XMFLOAT2 clip_gui = screen_to_clip({ 653, y }, { width, height });
						pF->DrawString(std::to_string(m_nMusicBank).c_str( ), clip_gui.x, clip_gui.y, color, scale);
					}
					break;

					case eOptionsPage::OPTIONS_PAGE_GRAPHICS:
					{
						DirectX::XMFLOAT2 clip_gui = screen_to_clip({ 653, y }, { width, height });
						pF->DrawString(std::string(m_nFullscreen == TRUE ? "Yes" : "No").c_str( ), clip_gui.x, clip_gui.y, color, scale);

						if (res)
						{
							pF->DrawString(std::string("*Resolution applied on Game Restart*").c_str( ), -0.575f, -0.4f, color, scale);
						}
					}
					break;
				}
			}
			break;

			case 3:
			{
				switch (m_nPageIndex)
				{
					case eOptionsPage::OPTIONS_PAGE_SOUNDS:
					{
						DirectX::XMFLOAT2 clip_gui = screen_to_clip({ 653, y }, { width, height });
						pF->DrawString(m_nWaifu == TRUE ? "On" : "Off", clip_gui.x, clip_gui.y, color, scale);
					}
					break;

					case eOptionsPage::OPTIONS_PAGE_GRAPHICS:
					{
					}
					break;
				}
			}
			break;
		}
	}

	//DirectX::XMFLOAT2 clip_gui = screen_to_clip({ 475, 255 }, { width, height });
	//pF->DrawString("Waifu Banter", clip_gui.x, clip_gui.y, color, scale);

	//m_pSelector->SetVisible(m_nWaifu == TRUE ? true : false);



	DrawNavigationInfo( );

	if (m_pScreen)
	{
		m_pScreen->Render();
	}
}

bool COptionsState::FirstEnter(void)
{
	//DebugPrint("[OptionsState] Entering Options State...\n");
	CBaseState::FirstEnter( );

	for (size_t i = 0; i < OPTIONS_PAGE_COUNT; i++)
	{
		m_pPages[i]->SetVisible(false);
	}

	//Always open to the first page
	PageFlip(eOptionsPage(0));

	m_bFadeIn = true, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_MUSIC_LOOP_MENU);

	return true;
}

bool COptionsState::ReEnter(void)
{
	m_bFadeIn = true, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	return CBaseState::ReEnter( );
}

bool COptionsState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded( ))
		return true;
	CBaseState::Load(pAssets);

	m_pGUI = new CGUIManager( );

	m_pScreen = new CUIElement( );

	m_pScreen->SetWidth(10000);
	m_pScreen->SetHeight(8000);
	m_pScreen->SetPosition(vec3f(500, 400, 1));
	m_pScreen->SetVisible(true);

	m_pScreen->LoadTexture(L"Assets/Images/Black.png");
	//m_pGUI->AddElement(m_pScreen);

	//Add the pages
	m_pPages[OPTIONS_PAGE_SOUNDS] = new CUIElement( );
	m_pPages[OPTIONS_PAGE_SOUNDS]->LoadTexture(L"Assets\\Images\\Options\\OptionsPage1.png");
	m_pPages[OPTIONS_PAGE_GRAPHICS] = new CUIElement( );
	m_pPages[OPTIONS_PAGE_GRAPHICS]->LoadTexture(L"Assets\\Images\\Options\\OptionsPage2.png");

	for (signed char i = 0; i < OPTIONS_PAGE_COUNT; i++)
	{
		m_pPages[i]->SetPosition(vec3f(500, 400, 0));
		m_pPages[i]->SetWidth(1000);
		m_pPages[i]->SetHeight(800);
		m_pGUI->AddElement(m_pPages[i]);
	}

	m_pLeft = new CUIButton(std::wstring(L"Assets//Images//Help//Left_Arrow_v2.png"), 310, 720, 182, 46, std::string("Left"));
	m_pGUI->AddElement(m_pLeft);
	m_pRight = new CUIButton(std::wstring(L"Assets//Images//Help//Right_Arrow_v2.png"), 690, 720, 182, 46, std::string("Right"));
	m_pGUI->AddElement(m_pRight);
	m_pBack = new CUIButton(std::wstring(L"Assets//Images//Options//Back.png"), 500, 720, 182, 46, std::string("Back"));
	m_pGUI->AddElement(m_pBack);

	m_pAccept = new CUIButton(std::wstring(L"Assets//Images//Options//Accept.png"), 310, 630, 182 / 1.5f, 46 * .75f, std::string("Accept"));
	m_pGUI->AddElement(m_pAccept);
	m_pDefault = new CUIButton(std::wstring(L"Assets//Images//Options//Default.png"), 500, 630, 182 / 1.5f, 46 * .75f, std::string("Default"));
	m_pGUI->AddElement(m_pDefault);
	m_pApply = new CUIButton(std::wstring(L"Assets//Images//Options//Apply.png"), 690, 630, 182 / 1.5f, 46 * .75f, std::string("Apply"));
	m_pGUI->AddElement(m_pApply);

	for (size_t i = 0; i < _countof(m_pPlusButtons); i++)
	{
		float y = float(277.5f + i * 81.125);
		m_pMinusButtons[i] = new CUIButton(std::wstring(L"Assets//Images//Options//left_Arrow_v1.png"), 615 - 25, y, 182 / 4.5f, 46 / 2.25f, std::string("Minus"));
		m_pPlusButtons[i] = new CUIButton(std::wstring(L"Assets//Images//Options//Right_Arrow_v1.png"), 771, y, 182 / 4.5f, 46 / 2.25f, std::string("Plus"));
		m_pGUI->AddElement(m_pPlusButtons[i]);
		m_pGUI->AddElement(m_pMinusButtons[i]);
	}

	//m_pWaifuToggle = new CUIButton(std::wstring(L"Assets//Images//Options//Toggle_Blank.png"), 715, 250, 91 / 2.0f, 46, std::string("Waifu"));
	//m_pGUI->AddElement(m_pWaifuToggle);
	//
	//m_pSelector = new CUIElement( );
	//m_pSelector->LoadTexture(std::wstring(L"Assets//Images//Options//Toggle_Selector.png").c_str( ));
	//m_pSelector->SetPosition(vec3f(715, 250, 0));
	//m_pSelector->SetWidth(91 / 2.0f);
	//m_pSelector->SetHeight(46);
	//m_pGUI->AddElement(m_pSelector);

	//m_pGUI->AddElement(m_pScreen);

	w = GetWidth( );

	return true;
}

void COptionsState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded( ))
		return;

	CBaseState::UnLoad( );
}

bool COptionsState::Exit(void)
{
	if (!GetEntered( ))
		return false;

	//AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::STOP_MUSIC_LOOP_MENU);

	WriteAllOptions( );

	CGameplayState* pG = dynamic_cast<CGameplayState*>(m_pPrevState->GetPrevState( ));
	if (nullptr != pG)
	{
		pG->GetEntityManager( )->GetDirector( )->SetWaifuOn(m_nWaifu ? true : false);
	}
	else
	{
		CMainMenuState* pM = dynamic_cast<CMainMenuState*>(m_pPrevState);
		if (pM)
		{
			pM->SetWaifuBander(0 == m_nWaifu ? false : true);
		}
	}

	AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::STOP_MUSIC_LOOP_MENU);

	CBaseState::Exit( );
	return false;
}

eStateType COptionsState::CheckState(void)
{
	eStateType eNextState = GetType( );
	//Go back to gameplay
	if (m_bStateChanges[STATE_CH_BACK] || m_bStateChanges[STATE_CH_QUIT])
	{
		if (m_pScreen->GetColor( ).w < 1.0f || m_pScreen->GetColor( ).w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}

		eNextState = m_pPrevState->GetType( );
	}
	if (CBaseState::CheckState( ) == STATE_NULL)
	{
		eNextState = STATE_NULL;
	}

	//Tell the state machine what state we're switching to
	return eNextState;
}

bool COptionsState::PageFlip(eOptionsPage nTargetPage)
{
	//Make sure that page exists
	if (nTargetPage < 0 || nTargetPage >= OPTIONS_PAGE_COUNT)
	{
		return false;
	}

	//Switch pages
	m_pPages[m_nPageIndex]->SetVisible(false);
	m_nPageIndex = nTargetPage;
	m_pPages[m_nPageIndex]->SetVisible(true);

	//Set button status
	//Left button
	if (m_nPageIndex == 0)
	{
		m_pLeft->SetEnabled(false);
		m_pLeft->SetPressed(false);

		m_pPlusButtons[3]->SetVisible(true);
		m_pMinusButtons[3]->SetVisible(true);

		m_pPlusButtons[3]->SetEnabled(true);
		m_pMinusButtons[3]->SetEnabled(true);

		m_pPlusButtons[3]->SetPressed(false);
		m_pMinusButtons[3]->SetPressed(false);
	}
	else
	{
		m_pLeft->SetEnabled(true);
	}
	//Right button
	if (m_nPageIndex == OPTIONS_PAGE_COUNT - 1)
	{
		m_pRight->SetEnabled(false);
		m_pRight->SetPressed(false);

		m_pPlusButtons[3]->SetVisible(false);
		m_pMinusButtons[3]->SetVisible(false);

		m_pPlusButtons[3]->SetEnabled(false);
		m_pMinusButtons[3]->SetEnabled(false);

		m_pPlusButtons[3]->SetPressed(false);
		m_pMinusButtons[3]->SetPressed(false);
	}
	else
	{
		m_pRight->SetEnabled(true);
	}

	return true;
}

int COptionsState::GetWidth( ) const
{
	std::string res = resolutions[m_nResolution];

	auto width = res.find_first_of(" ");
	res.erase(width);

	return std::stoi(res);
}

int COptionsState::GetHeight( ) const
{
	std::string res = resolutions[m_nResolution];

	auto height = res.find_last_of(" ");
	res.erase(0, height + 1);

	return std::stoi(res);
}

void COptionsState::ApplyFullscreen( )
{
	CRenderer::GetReference( ).SetFullscreenState(m_nFullscreen, m_nGamma / 100.f);
}

void COptionsState::WriteAllOptions( )
{
	// write options to the .txt file

	std::ofstream fout(pathA + "Options.txt", std::ios_base::out | std::ios_base::trunc);

	fout << m_nMasterBank << std::endl;
	fout << m_nSFXBank << std::endl;
	fout << m_nMusicBank << std::endl;
	fout << m_nResolution << std::endl;
	fout << m_nGamma << std::endl;
	fout << m_nFullscreen << std::endl;
	fout << m_nWaifu << std::endl;
}

