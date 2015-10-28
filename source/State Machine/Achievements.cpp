#include "stdafx.h"

#include "Achievements.h"
#include "../GUI Manager/UIElement.h"

#include "..\Input Manager\InputManager.h"
#include "..\GUI Manager\UIButton.h"
#include "..\Audio Manager\AudioSystemWwise.h"
#include "../GUI Manager/GUIManager.h"

#include "../Renderer/Renderer.h"

#include "../AchieveTracker.h"


CAchievements::CAchievements() : CBaseState(STATE_ACHIEVEMENTS)
{
	m_bTransparent = true;

	{
		m_pScreen = new CUIElement();

		m_pScreen->SetWidth(10000);
		m_pScreen->SetHeight(8000);
		m_pScreen->SetPosition(vec3f(500, 400, 1));
		m_pScreen->SetVisible(true);

		m_pScreen->LoadTexture(L"Assets/Images/Black.png");

		//m_pGUI->AddElement(m_pScreen);
	}
	m_vColor = { 0.0f, 1.0f, 0.0f, 1 };

	// Background stuffs
	m_pBackground = new CTexture2D(0, 0, 10, 10, L"Assets//Images//Achievements//background.jpg");
	CRectangle rect = { 0.f, 1280, 0.f, 1024 };
	rect = CGUIManager::ComputeBounds(rect);
	m_pBackground->SetWidth(rect.ComputeWidth());
	m_pBackground->SetHeight(rect.ComputeHeight());
	m_pBackground->SetColor({ 1, 1, 1, 1 });
	//Load();

	m_pTracker = CAchieveTracker::GetInstance();
}


CAchievements::~CAchievements()
{
	SafeDelete(m_pScreen);
	SafeDelete(m_pBackground);
}

/*CAchievements &CAchievements::GetReference()
{
if (!instance)
{
instance = new CAchievements();
}
return *instance;
}*/

void CAchievements::Input(void)
{
	if (m_pBack->GetPressed() ||
		CInputManager::GetInstance()->IsKeyPressed(eControls::ESCAPE) ||
		CInputManager::GetInstance()->IsKeyPressed(eControls::F1))
	{
		QueueStateChange(STATE_CH_BACK);
	}
	else if (m_pLeft->GetPressed() /*|| CInputManager::GetInstance()->IsKeyPressed(eControls::Backspace)*/)
	{
		//Flip to the left
		PageFlip(eAchievePage(m_nPageIndex - 1));
	}
	else if (m_pRight->GetPressed() /*|| CInputManager::GetInstance()->IsKeyPressed(eControls::Enter)*/)
	{
		//Flip to the right
		PageFlip(eAchievePage(m_nPageIndex + 1));
	}
}

void CAchievements::Update(float fDT)
{
	if (m_pPrevState)
	{
		m_pPrevState->Update(0);
	}

	m_pGUI->Update(fDT);

	if (m_pDelete->GetPressed())
	{
		m_pTracker->LoadDefault();
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
		m_bFadeIn = false;
	}
	else if (m_pScreen->GetColor().w >= 1.0f)
	{
		m_fFadeTimer = 0.0f;
		m_bFadeOut = false;
	}
}
#include "../Renderer/BitmapFont.h"
#include "../Renderer/HUDRenderer.h"
void CAchievements::Render(CAssetManager* pRenderer)
{
	if (m_pPrevState)
	{
		m_pPrevState->Render(pRenderer);
	}

	CHUDRenderer::Render2D(m_pBackground);

	m_pGUI->Render();

	DrawNavigationInfo();
	// 0.45f on X
	// 0.2f, -0.075, -0.34 on Y

	CBitmapFont* Bitmap = CBitmapFont::GetInstance();

	auto screen_to_clip = [&](DirectX::XMFLOAT2 _fPosition, DirectX::XMFLOAT2 _fDimensions) -> DirectX::XMFLOAT2
	{
		float x = ((_fPosition.x / _fDimensions.x) - .5f) * 2.f;
		float y = ((_fPosition.y / _fDimensions.y) - .5f) * 2.f;
		return DirectX::XMFLOAT2(x, -y);
	};

	if (m_pPages[PAGE_0]->GetVisible())
	{
		DirectX::XMFLOAT2 point = screen_to_clip({ 620, 235 }, { 1000, 800 });
		Bitmap->DrawString(std::to_string(CAchieveTracker::GetInstance()->GetEnemiesKilled()).c_str(),
						   point.x, point.y, m_vColor);
		if (m_pTracker->GetEnemiesKilled() > 1337)
		{
			DirectX::XMFLOAT2 point2 = screen_to_clip({ 420, 295 }, { 1000, 800 });
			Bitmap->DrawString("Complete", point2.x, point2.y, m_vColor);
		}
		
		if (m_pTracker->GetSurvivor())
		{
			DirectX::XMFLOAT2 point2 = screen_to_clip({ 420, 405 }, { 1000, 800 });
			Bitmap->DrawString("Complete", point2.x, point2.y, m_vColor);
		}
		
		if (m_pTracker->GetCampaignComplete())
		{
			DirectX::XMFLOAT2 point2 = screen_to_clip({ 420, 520 }, { 1000, 800 });
			Bitmap->DrawString("Complete", point2.x, point2.y, m_vColor);
		}
	}
	else if (m_pPages[PAGE_1]->GetVisible())
	{
		if (m_pTracker->GetLifeSuppLover())
		{
			DirectX::XMFLOAT2 point2 = screen_to_clip({ 420, 295 }, { 1000, 800 });
			Bitmap->DrawString("Complete", point2.x, point2.y, m_vColor);
		}
		if (m_pTracker->GetReactorLover())
		{
			DirectX::XMFLOAT2 point2 = screen_to_clip({ 420, 405 }, { 1000, 800 });
			Bitmap->DrawString("Complete", point2.x, point2.y, m_vColor);
		}
		if (m_pTracker->GetEnginesLover())
		{
			DirectX::XMFLOAT2 point2 = screen_to_clip({ 420, 520 }, { 1000, 800 });
			Bitmap->DrawString("Complete", point2.x, point2.y, m_vColor);
		}
	}
	else if (m_pPages[PAGE_2]->GetVisible())
	{
		if (m_pTracker->GetBoughtMachineGun())
		{
			DirectX::XMFLOAT2 point2 = screen_to_clip({ 420, 295 }, { 1000, 800 });
			Bitmap->DrawString("Complete", point2.x, point2.y, m_vColor);
		}
		if (m_pTracker->GetBoughtShotgun())
		{
			DirectX::XMFLOAT2 point2 = screen_to_clip({ 420, 405 }, { 1000, 800 });
			Bitmap->DrawString("Complete", point2.x, point2.y, m_vColor);
		}
		if (m_pTracker->GetBoughtDrone())
		{
			DirectX::XMFLOAT2 point2 = screen_to_clip({ 420, 520 }, { 1000, 800 });
			Bitmap->DrawString("Complete", point2.x, point2.y, m_vColor);
		}
	}

	if (m_pScreen)
	{
		m_pScreen->Render();
	}
}

bool CAchievements::FirstEnter(void)
{
	CBaseState::FirstEnter();

	for (size_t i = 0; i < PAGE_COUNT; i++)
	{
		m_pPages[i]->SetVisible(false);
	}

	//Always open to the first page
	PageFlip(eAchievePage(0));

	m_bFadeIn = true, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	return true;
}

bool CAchievements::ReEnter(void)
{
	m_bFadeIn = true, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	return CBaseState::ReEnter();
}

bool CAchievements::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded())
		return true;
	CBaseState::Load(pAssets);

	m_pGUI = new CGUIManager();

	//Add the pages
	m_pPages[PAGE_0] = new CUIElement();
	m_pPages[PAGE_0]->LoadTexture(L"Assets//Images//Achievements//NewAchievePage1.png");
	m_pPages[PAGE_1] = new CUIElement();
	m_pPages[PAGE_1]->LoadTexture(L"Assets//Images//Achievements//NewAchievePage2.png");
	m_pPages[PAGE_1]->SetVisible(false);
	m_pPages[PAGE_2] = new CUIElement();
	m_pPages[PAGE_2]->LoadTexture(L"Assets//Images//Achievements//NewAchievePage3.png");
	m_pPages[PAGE_2]->SetVisible(false);
	/*m_pPages[PAGE_3] = new CUIElement();
	m_pPages[PAGE_3]->LoadTexture(L"Assets//Images//Help//WeaponsPage.png");
	m_pPages[PAGE_3]->SetVisible(false);
	m_pPages[PAGE_4] = new CUIElement();
	m_pPages[PAGE_4]->LoadTexture(L"Assets//Images//Help//HUDInfo.png");
	m_pPages[PAGE_4]->SetVisible(false);
	*/
	for (signed char i = 0; i < PAGE_COUNT; i++)
	{
		m_pPages[i]->SetPosition(vec3f(512, 384, 0));
		m_pPages[i]->SetWidth(800);
		m_pPages[i]->SetHeight(680);
		m_pGUI->AddElement(m_pPages[i]);
	}

	//m_vElements.push_back(new CTexture2D(0.f, 0.f, 2.f, 2.f, L"Assets/Images/Controls.png"));
	m_pLeft = new CUIButton(std::wstring(L"Assets//Images//Menus//Left_Arrow_v1.png"), 310, 720, 180, 50, std::string("Left"));;
	m_pGUI->AddElement(m_pLeft);
	m_pRight = new CUIButton(std::wstring(L"Assets//Images//Menus//Right_Arrow_v1.png"), 690, 720, 180, 50, std::string("Right"));;
	m_pGUI->AddElement(m_pRight);
	m_pBack = new CUIButton(std::wstring(L"Assets//Images//Menus//Back.png"), 500, 720, 180, 50, std::string("Back"));
	m_pGUI->AddElement(m_pBack);
	m_pDelete = new CUIButton(std::wstring(L"Assets//Images//Menus//Reset.png"), 500, 650, 180, 50, std::string("Delete"));
	m_pGUI->AddElement(m_pDelete);
	//m_pGUI->AddElement(m_pScreen);

	return true;
}

void CAchievements::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded())
		return;

	CBaseState::UnLoad();
}

bool CAchievements::Exit(void)
{
	if (!GetEntered())
		return false;
	CBaseState::Exit();
	return false;
}

eStateType CAchievements::CheckState(void)
{
	eStateType eNextState = GetType();
	//Go back to gameplay
	if (m_bStateChanges[STATE_CH_BACK] || m_bStateChanges[STATE_CH_QUIT])
	{
		if (m_pScreen->GetColor().w < 1.0f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = true;
			return eNextState;
		}

		eNextState = m_pPrevState->GetType();
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::RESUME_ALL);
	}
	//else if (m_bStateChanges[STATE_CH_PLAY])
	//{
	//	eNextState = STATE_GAMEPLAY;
	//	m_bToPop = true;
	//}

	if (CBaseState::CheckState() == STATE_NULL)
	{
		eNextState = STATE_NULL;
	}

	//Tell the state machine what state we're switching to
	return eNextState;
}

bool CAchievements::PageFlip(eAchievePage nTargetPage)
{
	if (nTargetPage < 0 || nTargetPage >= PAGE_COUNT)
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
	}
	else
	{
		m_pLeft->SetEnabled(true);
	}
	//Right button
	if (m_nPageIndex == PAGE_COUNT - 1)
	{
		m_pRight->SetEnabled(false);
		m_pRight->SetPressed(false);
	}
	else
	{
		m_pRight->SetEnabled(true);
	}

	return true;
}