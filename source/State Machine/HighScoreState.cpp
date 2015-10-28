/***********************************************
* Filename:  		HighScoreState.cpp
* Date:      		08/17/2015
* Mod. Date:
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:   		Class to show the high scores
************************************************/

#include "stdafx.h"
#include "HighScoreState.h"
#include "..\Renderer\Scene.h"
#include "..\Renderer\HUDRenderer.h"
#include "..\Renderer\Renderer.h"

#include "..\GUI Manager\GUIManager.h"
#include "..\GUI Manager\UIButton.h"

#include "..\Input Manager\InputManager.h"
#include "..\Audio Manager\AudioSystemWwise.h"

#include "../Renderer/BitmapFont.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

extern std::string pathA;

#define PAGECHANGETIME 10.0f

bool SortByScoreDesc(TMsScores const& left, TMsScores const& right)
{
	return left.score > right.score;
}

CHighScoreState::CHighScoreState() : CBaseState(STATE_HIGHSCORE)
{

	m_pScene = new CScene();
	m_bTransparent = true;
}

CHighScoreState::~CHighScoreState()
{
	delete m_pScene;
	SafeDelete(m_pScreen);
}

void CHighScoreState::Input(void)
{
	if (CInputManager::GetInstance()->IsKeyPressed(eControls::Spacebar) ||
		CInputManager::GetInstance()->IsKeyPressed(eControls::Enter) ||
		CInputManager::GetInstance()->IsKeyPressed(eControls::ESCAPE))
	{
		QueueStateChange(STATE_CH_BACK);
	}

	if (true == m_pBack->GetPressed())
	{
		QueueStateChange(STATE_CH_BACK);
	}

	if (true == m_pLeft->GetVisible() && true == m_pLeft->GetPressed())
	{
		m_bNormal = true;
		m_fPageTimer = 0.0f;
		m_bChanged = true;
	}
	if (true == m_pRight->GetVisible() && true == m_pRight->GetPressed())
	{
		m_bNormal = false;
		m_fPageTimer = 0.0f;
		m_bChanged = true;
	}
}

void CHighScoreState::Update(float fDT)
{
	m_pPrevState->Update(0);
	m_pGUI->Update(fDT);

	m_fPageTimer += fDT;

	if (m_fPageTimer > PAGECHANGETIME)
	{
		m_fPageTimer = 0.0f;
		m_bNormal = !m_bNormal;
		m_bChanged = true;
	}

	if (true == m_bChanged)
	{
		if (true == m_bNormal)
		{
			m_pLeft->SetVisible(false);
			m_pRight->SetVisible(true);
			m_pBackplateEndless->SetVisible(false);
			m_pBackplateNormal->SetVisible(true);
		}
		else
		{
			m_pRight->SetVisible(false);
			m_pLeft->SetVisible(true);
			m_pBackplateEndless->SetVisible(true);
			m_pBackplateNormal->SetVisible(false);
		}
		m_bChanged = false;
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

void CHighScoreState::Render(CAssetManager* pAssetManager)
{
	m_pPrevState->Render(pAssetManager);
	CRenderer::GetReference().Render(m_pScene, pAssetManager);
	m_pGUI->Render();

	if (m_pScreen)
	{
		m_pScreen->Render();
	}

	if (true == m_bNormal)
	{
		//CBitmapFont::GetInstance()->DrawString("CAMPAIGN MODE", -0.5f, 0.7f, DirectX::XMFLOAT4{ 0, 1, 0, 1 }, 2.5f);
		CBitmapFont::GetInstance()->DrawString(m_sHighScoresNormal.c_str(), -0.5f, 0.6f, DirectX::XMFLOAT4{ 0.9f, 0.9f, 0, 1 }, 2.0f);
	}
	else
	{
		//CBitmapFont::GetInstance()->DrawString("ENDLESS MODE", -0.5f, 0.7f, DirectX::XMFLOAT4{ 0, 0, 1, 1 }, 2.5f);
		CBitmapFont::GetInstance()->DrawString(m_sHighScoresInfinite.c_str(), -0.5f, 0.6f, DirectX::XMFLOAT4{ 0, 1, 0.2f, 1 }, 2.0f);
	}

	CBaseState::DrawNavigationInfo();

	if (nullptr != m_pScreen)
	{
		m_pScreen->Render();
	}
}

bool CHighScoreState::FirstEnter(void)
{
	if (!CBaseState::FirstEnter())
	{
		return false;
	}
	//CRenderer::GetReference().SetColorData(DirectX::XMFLOAT4(1, 1, 1, 1));
	m_bFadeIn = true, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;
	m_bNormal = true;
	m_bChanged = false;
	m_fPageTimer = 0.0f;

	//load from file
	LoadNormalScores();
	LoadInfiniteScores();

	return true;
}

bool CHighScoreState::ReEnter(void)
{
	m_bFadeIn = true;
	m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	return CBaseState::ReEnter();
}

bool CHighScoreState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded())
	{
		return true;
	}

	CBaseState::Load(pAssets);
	m_pGUI = new CGUIManager;

	m_pScreen = new CUIElement();
	//Add the screen element for fading in and out
	m_pScreen->SetWidth(10000);
	m_pScreen->SetHeight(8000);
	m_pScreen->SetPosition(vec3f(500, 400, 1));
	m_pScreen->SetVisible(true);

	m_pScreen->LoadTexture(L"Assets/Images/Black.png");
	//m_pGUI->AddElement(m_pScreen);

	m_pSBackground = new CUIElement();
	//Add the screen element for fading in and out
	m_pSBackground->SetWidth(1000);
	m_pSBackground->SetHeight(800);
	m_pSBackground->SetPosition(vec3f(500, 400, 1));
	m_pSBackground->SetVisible(true);

	m_pSBackground->LoadTexture(L"Assets/Images/HighScoresBack.jpg");
	m_pSBackground->SetColor(DirectX::XMFLOAT4(1,1,1,1));

	m_pGUI->AddElement(m_pSBackground);

	m_pBack = new CUIButton(std::wstring(L"Assets//Images//Menus//Back.png"), 75, 750, 150, 60, std::string("Void"));
	m_pBack->SetRenderText(false);
	m_pGUI->AddElement(m_pBack);

	m_pLeft = new CUIButton(std::wstring(L"Assets//Images//Menus//Left_Arrow_v1.png"), 75, 670, 150, 60, std::string("Void"));
	m_pLeft->SetRenderText(false);
	m_pLeft->SetVisible(false);
	m_pGUI->AddElement(m_pLeft);

	m_pRight = new CUIButton(std::wstring(L"Assets//Images//Menus//Right_Arrow_v1.png"), 75, 670, 150, 60, std::string("Void"));
	m_pRight->SetRenderText(false);
	m_pRight->SetVisible(true);
	m_pGUI->AddElement(m_pRight);

	m_pBackplateNormal = new CUIElement();
	//Add the screen element for fading in and out
	m_pBackplateNormal->SetWidth(700);
	m_pBackplateNormal->SetHeight(800);
	m_pBackplateNormal->SetPosition(vec3f(500, 400, 1));
	m_pBackplateNormal->SetVisible(true);
	m_pBackplateNormal->SetColor(DirectX::XMFLOAT4{ 1, 1, 1, 0.5f });

	m_pBackplateNormal->LoadTexture(L"Assets/Images/Menus/CampaignBackPlate.png");
	m_pGUI->AddElement(m_pBackplateNormal);

	m_pBackplateEndless = new CUIElement();
	//Add the screen element for fading in and out
	m_pBackplateEndless->SetWidth(700);
	m_pBackplateEndless->SetHeight(800);
	m_pBackplateEndless->SetPosition(vec3f(500, 400, 1));
	//m_pBackplateEndless->SetVisible(true);
	m_pBackplateEndless->SetColor(DirectX::XMFLOAT4{ 1, 1, 1, 0.5f });
	m_pBackplateEndless->SetVisible(false);

	m_pBackplateEndless->LoadTexture(L"Assets/Images/Menus/EndlessBackPlate.png");
	m_pGUI->AddElement(m_pBackplateEndless);

	return true;
}

void CHighScoreState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded())
	{
		return;
	}

	CBaseState::UnLoad();
}

bool CHighScoreState::Exit(void)
{

	CBaseState::Exit();
	return true;
}

eStateType CHighScoreState::CheckState(void)
{
	//If it stays this, we continue running
	eStateType eNextState = GetType();

	if (m_bStateChanges[STATE_CH_BACK])
	{
		if (m_pScreen->GetColor().w < 1.0f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}
		m_bToPop = true;
		eNextState = m_pPrevState->GetType();
	}

	if (CBaseState::CheckState() == STATE_NULL)
	{
		eNextState = STATE_NULL;
	}

	return eNextState;
}

bool CHighScoreState::LoadNormalScores()
{

	std::string filename = pathA + std::string("HighScoresNormal.bin");

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
			vecScores.push_back(temp);
		}
		loadfile.close();
	}
	else
	{
		SaveAndLoadDefault(0);
		return true;
	}

	list<TMsScores> scoreList(vecScores.begin(), vecScores.end());

	scoreList.sort(SortByScoreDesc);

	size_t size = scoreList.size();
	m_sHighScoresNormal.clear();
	m_sHighScoresNormal = "";
	int i = 0;
	for each (TMsScores score in scoreList)
	{
		if (10 == i)
		{
			break;
		}

		std::stringstream help2;
		
		m_sHighScoresNormal += std::string(score.name) + std::string("       ");
		help2 << score.score;
		m_sHighScoresNormal = m_sHighScoresNormal + help2.str() + '\n' + '\n' + '\n' + '\n';

		i++;
	}

	return true;
}

bool CHighScoreState::LoadInfiniteScores()
{
	std::string filename = pathA + std::string("HighScoresEndless.bin");

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

			loadfile.read((char*)&temp, sizeof(TMsScores));
			vecScores.push_back(temp);
		}
		loadfile.close();
	}
	else
	{
		SaveAndLoadDefault(1);
		return true;
	}

	list<TMsScores> scoreList(vecScores.begin(), vecScores.end());

	scoreList.sort(SortByScoreDesc);

	size_t size = scoreList.size();
	m_sHighScoresInfinite.clear();
	m_sHighScoresInfinite = "";
	int i = 0;


	for each (TMsScores score in scoreList)
	{
		if (10 == i)
		{
			break;
		}
		std::stringstream help2;

		m_sHighScoresInfinite += std::string(score.name) + std::string("       ");
		help2 << score.score;
		m_sHighScoresInfinite = m_sHighScoresInfinite + help2.str() + '\n' + '\n' + '\n' + '\n';
		i++;
	}

	return true;
}

void CHighScoreState::SaveAndLoadDefault(int type)
{
	//std::vector<TMsScores> m_pMScores;
	m_pMScores.resize(10);
	std::string filename;

	if (0 == type)
	{
		m_pMScores[0].name[0] = 'M';
		m_pMScores[0].name[1] = 'a';
		m_pMScores[0].name[2] = 'z';
		m_pMScores[0].name[3] = '\0';
		m_pMScores[0].score = 10000;

		m_pMScores[1].name[0] = 'D';
		m_pMScores[1].name[1] = 'v';
		m_pMScores[1].name[2] = 'D';
		m_pMScores[1].name[3] = '\0';
		m_pMScores[1].score = 9000;

		m_pMScores[2].name[0] = 'L';
		m_pMScores[2].name[1] = 'u';
		m_pMScores[2].name[2] = 'c';
		m_pMScores[2].name[3] = '\0';
		m_pMScores[2].score = 8000;

		m_pMScores[3].name[0] = 'G';
		m_pMScores[3].name[1] = 'e';
		m_pMScores[3].name[2] = 'g';
		m_pMScores[3].name[3] = '\0';
		m_pMScores[3].score = 7000;

		m_pMScores[4].name[0] = 'J';
		m_pMScores[4].name[1] = 'k';
		m_pMScores[4].name[2] = 'r';
		m_pMScores[4].name[3] = '\0';
		m_pMScores[4].score = 6000;

		m_pMScores[5].name[0] = 'B';
		m_pMScores[5].name[1] = 'o';
		m_pMScores[5].name[2] = 't';
		m_pMScores[5].name[3] = '\0';
		m_pMScores[5].score = 5000;

		m_pMScores[6].name[0] = 'P';
		m_pMScores[6].name[1] = 'J';
		m_pMScores[6].name[2] = '2';
		m_pMScores[6].name[3] = '\0';
		m_pMScores[6].score = 4000;

		m_pMScores[7].name[0] = 'N';
		m_pMScores[7].name[1] = 'o';
		m_pMScores[7].name[2] = ' ';
		m_pMScores[7].name[3] = '\0';
		m_pMScores[7].score = 2000;

		m_pMScores[8].name[0] = 'C';
		m_pMScores[8].name[1] = 'a';
		m_pMScores[8].name[2] = 'b';
		m_pMScores[8].name[3] = '\0';
		m_pMScores[8].score = 1000;

		m_pMScores[9].name[0] = 'P';
		m_pMScores[9].name[1] = 'J';
		m_pMScores[9].name[2] = '1';
		m_pMScores[9].name[3] = '\0';
		m_pMScores[9].score = 500;

		filename = pathA + std::string("HighScoresNormal.bin");

		std::fstream savefile;

		savefile.open(filename, ios_base::out | ios_base::binary | ios_base::trunc);
		if (savefile.is_open())
		{
			savefile.seekg(0, savefile.end);
			for (int i = 0; i < 10; i++)
			{
				savefile.write((char*)&m_pMScores[i], sizeof(TMsScores));

				std::stringstream help2;

				m_sHighScoresNormal += std::string(m_pMScores[i].name) + std::string("       ");
				help2 << m_pMScores[i].score;
				m_sHighScoresNormal = m_sHighScoresNormal + help2.str() + '\n' + '\n' + '\n' + '\n';
			}
		}
		savefile.close();

	}
	else if (1 == type)
	{
		m_pMScores[0].name[0] = 'J';
		m_pMScores[0].name[1] = 'k';
		m_pMScores[0].name[2] = 'r';
		m_pMScores[0].name[3] = '\0';
		m_pMScores[0].score = 100000;

		m_pMScores[1].name[0] = 'L';
		m_pMScores[1].name[1] = 'u';
		m_pMScores[1].name[2] = 'c';
		m_pMScores[1].name[3] = '\0';
		m_pMScores[1].score = 90000;

		m_pMScores[2].name[0] = 'G';
		m_pMScores[2].name[1] = 'e';
		m_pMScores[2].name[2] = 'g';
		m_pMScores[2].name[3] = '\0';
		m_pMScores[2].score = 80000;

		m_pMScores[3].name[0] = 'D';
		m_pMScores[3].name[1] = 'v';
		m_pMScores[3].name[2] = 'D';
		m_pMScores[3].name[3] = '\0';
		m_pMScores[3].score = 70000;

		m_pMScores[4].name[0] = 'M';
		m_pMScores[4].name[1] = 'a';
		m_pMScores[4].name[2] = 'z';
		m_pMScores[4].name[3] = '\0';
		m_pMScores[4].score = 60000;

		m_pMScores[5].name[0] = 'W';
		m_pMScores[5].name[1] = 'a';
		m_pMScores[5].name[2] = 'i';
		m_pMScores[5].name[3] = '\0';
		m_pMScores[5].score = 50000;

		m_pMScores[6].name[0] = 'C';
		m_pMScores[6].name[1] = 'A';
		m_pMScores[6].name[2] = 'B';
		m_pMScores[6].name[3] = '\0';
		m_pMScores[6].score = 40000;

		m_pMScores[7].name[0] = 'P';
		m_pMScores[7].name[1] = 'J';
		m_pMScores[7].name[2] = '1';
		m_pMScores[7].name[3] = '\0';
		m_pMScores[7].score = 20000;

		m_pMScores[8].name[0] = 'A';
		m_pMScores[8].name[1] = 'L';
		m_pMScores[8].name[2] = 'F';
		m_pMScores[8].name[3] = '\0';
		m_pMScores[8].score = 10000;

		m_pMScores[9].name[0] = 'N';
		m_pMScores[9].name[1] = 'o';
		m_pMScores[9].name[2] = 'b';
		m_pMScores[9].name[3] = '\0';
		m_pMScores[9].score = 5000;

		filename = pathA + std::string("HighScoresEndless.bin");

		std::fstream savefile;

		savefile.open(filename, ios_base::out | ios_base::binary | ios_base::trunc);
		if (savefile.is_open())
		{
			savefile.seekg(0, savefile.end);
			for (int i = 0; i < 10; i++)
			{
				savefile.write((char*)&m_pMScores[i], sizeof(TMsScores));

				std::stringstream help2;

				m_sHighScoresInfinite += std::string(m_pMScores[i].name) + std::string("       ");
				help2 << m_pMScores[i].score;
				m_sHighScoresInfinite = m_sHighScoresInfinite + help2.str() + '\n' + '\n' + '\n' + '\n';
			}
		}
		savefile.close();
	}
}

