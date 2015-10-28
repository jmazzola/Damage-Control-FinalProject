#include "stdafx.h"
#include "AchieveTracker.h"
#include "Renderer\Texture2D.h"
#include "Renderer\BitmapFont.h"
#include "Renderer\HUDRenderer.h"
#include "GUI Manager\Rectangle.h"
#include "GUI Manager\GUIManager.h"
#include "State Machine\StateMachine.h"

CAchieveTracker* CAchieveTracker::InstancePtr = nullptr;
extern std::string pathA;

CAchieveTracker::CAchieveTracker()
{
	Load();

	m_pTexture = new CTexture2D(0, 0, 300, 300, L"Assets/Images/Achievements/AchievePopUp.png");
	m_pTexture->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.75f });

	// Base Achievement Texture
	CRectangle rect = { 0.f, 250, 0.f, 75 };
	rect = CGUIManager::ComputeBounds(rect);
	m_pTexture->SetWidth(rect.ComputeWidth());
	m_pTexture->SetHeight(rect.ComputeHeight());
	m_pTexture->SetScreenPosition(0.0f, 0.85f);

	m_fTextureTimer = 10.0f;
}

CAchieveTracker::~CAchieveTracker()
{
	SafeDelete(m_pTexture);
}

CAchieveTracker* CAchieveTracker::GetInstance()
{
	if (!InstancePtr)
	{
		InstancePtr = new CAchieveTracker();
	}
	return InstancePtr;
}

void CAchieveTracker::UnlockAchievement(eAchievements Achieve, float dt)
{
	//eKiller, eSurvivor, eCampaigner, eLifeLover, eRectorLover,
	//eEnginesLover, eBuyMachGun, eBuyShotGun, eBuyDrone, eAchieveCount
	if (true == m_bExit)
	{
		return;
	}

	switch (Achieve)
	{
		case eKiller:
		{
			m_nEnemiesKilled++;
			if (m_nEnemiesKilled >= 1337 && m_bKiller == false)
			{
				m_bKiller = true;
				m_bNotifyPlayer = true;
				m_sText = "BloodBath";
			}
		}
		break;

		case eSurvivor:
		{
			m_fSurvivorTimer += dt;
			if (m_bSurvive10mins == false && m_fSurvivorTimer >= 900.0f)
			{
				m_bSurvive10mins = true;
				m_bNotifyPlayer = true;
				m_sText = "Survivor";
			}
		}
		break;

		case eCampaigner:
		{
			if (m_bCampaignComplete == false)
			{
				m_bCampaignComplete = true;
				m_bNotifyPlayer = true;
				m_sText = "Campaigner";
			}
		}
		break;

		case eLifeLover:
		{
			if (m_bdidLifeSuppLive == false)
			{
				m_bdidLifeSuppLive = true;
				m_bNotifyPlayer = true;
				m_sText = "Life Lover";
			}
		}
		break;

		case eRectorLover:
		{
			if (m_bdidReactorLive == false)
			{
				m_bdidReactorLive = true;
				m_bNotifyPlayer = true;
				m_sText = "Power Lover";
			}
		}
		break;

		case eEnginesLover:
		{
			if (m_bdidEnginesLive == false)
			{
				m_bdidEnginesLive = true;
				m_bNotifyPlayer = true;
				m_sText = "Great Movers";
			}
		}
		break;
		
		case eBuyMachGun:
		{
			if (m_bBoughtMG == false)
			{
				m_bBoughtMG = true;
				m_bNotifyPlayer = true;
				m_sText = "Hot Lead";
			}
		}
		break;
		
		case eBuyShotGun:
		{
			if (m_bBoughtSG == false)
			{
				m_bBoughtSG = true;
				m_bNotifyPlayer = true;
				m_sText = "My Boomstick";
			}
		}
		break;
		
		case eBuyDrone:
		{
			if (m_bBoughtDB == false)
			{
				m_bBoughtDB = true;
				m_bNotifyPlayer = true;
				m_sText = "BFFs";
			}
		}
		break;
	}
}

void CAchieveTracker::Update(float dt)
{
	if (m_fTextureTimer > 0.0f && m_bNotifyPlayer == true)
	{
		m_fTextureTimer -= dt;
	}
	else
	{
		m_fTextureTimer = 10.0f;
		m_bNotifyPlayer = false;
	}
}

void CAchieveTracker::RenderAchievement()
{
	if (m_fTextureTimer > 0.0f)
	{
		CHUDRenderer::Render2D(m_pTexture);
		CBitmapFont::GetInstance()->DrawString(m_sText.c_str(), -0.125f, 0.775f, (DirectX::XMFLOAT4)(DirectX::Colors::White), 0.75f);
	}
}

void CAchieveTracker::Load()
{
	std::ifstream fin(pathA + "Achievements.txt", std::ios_base::in);

	if (!fin.is_open())
	{
		LoadDefault();
		Save();
		return;
	}

	fin >> m_nEnemiesKilled;
	fin >> m_bSurvive10mins;
	fin >> m_bCampaignComplete;

	fin >> m_bdidLifeSuppLive;
	fin >> m_bdidReactorLive;
	fin >> m_bdidEnginesLive;

	fin >> m_bBoughtMG;
	fin >> m_bBoughtSG;
	fin >> m_bBoughtDB;

	//if (m_nEnemiesKilled <= -1 ||
	//	(m_bSurvive10mins != false && m_bSurvive10mins != true) ||
	//	(m_bCampaignComplete != false && m_bCampaignComplete != true) ||
	//	(m_bdidLifeSuppLive != false && m_bdidLifeSuppLive != true) ||
	//	(m_bdidReactorLive != false && m_bdidReactorLive != true) ||
	//	(m_bdidEnginesLive != false && m_bdidEnginesLive != true) ||
	//	(m_bBoughtMG != false && m_bBoughtMG != true) ||
	//	(m_bBoughtSG != false && m_bBoughtSG != true) ||
	//	(m_bBoughtDB != false && m_bBoughtDB != true))
	//{
	//	LoadDefault();
	//	Save();
	//	return;
	//}
}

void CAchieveTracker::LoadDefault()
{
	m_nEnemiesKilled = 0;
	//Over1337 = false;
	m_bSurvive10mins = false;
	m_bCampaignComplete = false;
	m_bdidLifeSuppLive = false;
	m_bdidReactorLive = false;
	m_bdidEnginesLive = false;
	m_bBoughtMG = false;
	m_bBoughtSG = false;
	m_bBoughtDB = false;
}

void CAchieveTracker::Save()
{
	std::ofstream fout(pathA + "Achievements.txt", std::ios_base::out | std::ios_base::trunc);

	fout << m_nEnemiesKilled << std::endl;
	fout << m_bSurvive10mins << std::endl;
	fout << m_bCampaignComplete << std::endl;
	
	fout << m_bdidLifeSuppLive << std::endl;
	fout << m_bdidReactorLive << std::endl;
	fout << m_bdidEnginesLive << std::endl;

	fout << m_bBoughtMG << std::endl;
	fout << m_bBoughtSG << std::endl;
	fout << m_bBoughtDB << std::endl;
}