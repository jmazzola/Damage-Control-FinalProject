/***********************************************
* Filename:  		ControlsState.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This is the state used for
*			 displaying the controls to the user
************************************************/
#include "ControlsState.h"
#include "..\GUI Manager\GUIManager.h"
#include "..\Input Manager\InputManager.h"
#include "..\GUI Manager\UIButton.h"
#include "..\Audio Manager\AudioSystemWwise.h"

#include "../Renderer/Renderer.h"

/*****************************************************************
* Default Constructor
*****************************************************************/
CControlsState::CControlsState(void) : CBaseState(STATE_CONTROLS)
{
	m_bTransparent = true;

	{
		m_pBackground = new CTexture2D(0, 0, 10, 10, L"Assets//Images//Help//background.jpg");
		CRectangle rect = { 0.f, 1280, 0.f, 1024 };
		rect = CGUIManager::ComputeBounds(rect);
		m_pBackground->SetWidth(rect.ComputeWidth());
		m_pBackground->SetHeight(rect.ComputeHeight());
		m_pBackground->SetColor({ 1, 1, 1, 1 });

		//m_pGUI->AddElement(m_pScreen);
	}
}

/*****************************************************************
* Default Destructor
*****************************************************************/
CControlsState::~CControlsState(void)
{ 
	SafeDelete(m_pBackground);
	SafeDelete(m_pScreen);
}

/*****************************************************************
* Input(): Called every frame, processes any state specific input
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  void
*
* Mod. Date:		      05/12/2015
* Mod. Initials:	      MJG
*****************************************************************/
void CControlsState::Input(void)
{
	//Unpause the game
	if (m_pBack->GetPressed( ) ||
		CInputManager::GetInstance( )->IsKeyPressed(eControls::ESCAPE) ||
		CInputManager::GetInstance( )->IsKeyPressed(eControls::F1))
	{
		QueueStateChange(STATE_CH_BACK);
	}
	else if (m_pLeft->GetPressed( ) /*|| CInputManager::GetInstance()->IsKeyPressed(eControls::Backspace)*/)
	{
		//Flip to the left
		PageFlip(eHelpPage(m_nPageIndex - 1));
	}
	else if (m_pRight->GetPressed( ) /*|| CInputManager::GetInstance()->IsKeyPressed(eControls::Enter)*/)
	{
		//Flip to the right
		PageFlip(eHelpPage(m_nPageIndex + 1));
	}
}

/*****************************************************************
* Update(): Called every frame, updates anything inside that
*                needs it
*
* Ins:			  float fDT
*
* Outs:		      void
*
* Returns:		  void
*
* Mod. Date:		      05/12/2015
* Mod. Initials:	      MJG
*****************************************************************/
void CControlsState::Update(float fDT)
{
	if (m_pPrevState)
	{
		m_pPrevState->Update(0);
	}

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
}

/*****************************************************************
* Render(): Called every frame, renders everything in the state
*                that needs it to the back buffer
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  void
*
* Mod. Date:		      05/12/2015
* Mod. Initials:	      MJG
*****************************************************************/
void CControlsState::Render(CAssetManager* cRenderer)
{
	if (m_pPrevState)
	{
		m_pPrevState->Render(cRenderer);
	}

	CHUDRenderer::Render2D(m_pBackground);

	m_pGUI->Render( );

	DrawNavigationInfo( );

	if (m_pScreen)
	{
		m_pScreen->Render();
	}
}
/*****************************************************************
* Enter(): Called when switching to a state, initializes members
*                and allocates memory
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  bool, true if successfully entered
*
* Mod. Date:		      05/12/2015
* Mod. Initials:	      MJG
*****************************************************************/
bool CControlsState::FirstEnter(void)
{
	//DebugPrint("[ControlsState] Entering Help State...\n");
	CBaseState::FirstEnter( );

	for (size_t i = 0; i < HELP_PAGE_COUNT; i++)
	{
		m_pPages[i]->SetVisible(false);
	}

	//Always open to the first page
	PageFlip(eHelpPage(0));

	m_bFadeIn = true, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	return true;
}

bool CControlsState::ReEnter(void)
{
	m_bFadeIn = true, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	return CBaseState::ReEnter( );
}

/*****************************************************************
* Load(): Called before switching to a state to ensure all
*				assets are loaded
*
* Ins:			  CAssetManager* pAssets
*
* Outs:		      void
*
* Returns:		  bool, true if all assets are loaded
*
* Mod. Date:		      05/13/2015
* Mod. Initials:	      MJG
*****************************************************************/
bool CControlsState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded( ))
		return true;
	CBaseState::Load(pAssets);

	m_pGUI = new CGUIManager( );
	m_pScreen = new CUIElement();

	m_pScreen->SetWidth(10000);
	m_pScreen->SetHeight(8000);
	m_pScreen->SetPosition(vec3f(500, 400, 1));
	m_pScreen->SetVisible(true);

	m_pScreen->LoadTexture(L"Assets/Images/Black.png");
	//m_pGUI->AddElement(m_pScreen);

	//Add the pages
	m_pPages[HELP_PAGE_CONTROLS] = new CUIElement( );
	m_pPages[HELP_PAGE_CONTROLS]->LoadTexture(L"Assets//Images//Help//NewControlsPage.png");
	m_pPages[HELP_PAGE_OBJECTIVES] = new CUIElement( );
	m_pPages[HELP_PAGE_OBJECTIVES]->LoadTexture(L"Assets//Images//Help//NewObjectivesPage.png");
	m_pPages[HELP_PAGE_OBJECTIVES]->SetVisible(false);
	m_pPages[HELP_PAGE_ENEMIES] = new CUIElement( );
	m_pPages[HELP_PAGE_ENEMIES]->LoadTexture(L"Assets//Images//Help//NewEnemyPage.png");
	m_pPages[HELP_PAGE_ENEMIES]->SetVisible(false);
	m_pPages[HELP_PAGE_WEAPONS] = new CUIElement( );
	m_pPages[HELP_PAGE_WEAPONS]->LoadTexture(L"Assets//Images//Help//NewWeaponsPage.png");
	m_pPages[HELP_PAGE_WEAPONS]->SetVisible(false);
	m_pPages[HELP_PAGE_HUD] = new CUIElement( );
	m_pPages[HELP_PAGE_HUD]->LoadTexture(L"Assets//Images//Help//NewHUDInfo.png");
	m_pPages[HELP_PAGE_HUD]->SetVisible(false);

	for (signed char i = 0; i < HELP_PAGE_COUNT; i++)
	{
		m_pPages[i]->SetPosition(vec3f(512, 384, 0));
		m_pPages[i]->SetWidth(800);
		m_pPages[i]->SetHeight(680);
		m_pGUI->AddElement(m_pPages[i]);
	}

	//m_vElements.push_back(new CTexture2D(0.f, 0.f, 2.f, 2.f, L"Assets/Images/Controls.png"));
	m_pLeft = new CUIButton(std::wstring(L"Assets//Images//Menus//Left_Arrow_v1.png"), 310, 720, 180, 100, std::string("Left"));;
	m_pGUI->AddElement(m_pLeft);
	m_pRight = new CUIButton(std::wstring(L"Assets//Images//Menus//Right_Arrow_v1.png"), 690, 720, 180, 100, std::string("Right"));;
	m_pGUI->AddElement(m_pRight);
	m_pBack = new CUIButton(std::wstring(L"Assets//Images//Menus//Back.png"), 500, 720, 180, 100, std::string("Back"));
	m_pGUI->AddElement(m_pBack);

	//m_pGUI->AddElement(m_pScreen);

	return true;
}

/*****************************************************************
* UnLoad(): Called when you no longer need this state's assets
*
* Ins:			  CAssetManager* pAssets
*
* Outs:		      void
*
* Returns:		  bool, true if all assets are unloaded
*
* Mod. Date:		      05/13/2015
* Mod. Initials:	      MJG
*****************************************************************/
void CControlsState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded( ))
		return;

	CBaseState::UnLoad( );

	//for (auto& element : m_vElements)
	//{
	//	delete element;
	//	element = nullptr;
	//}
	//m_vElements.clear();
}

/*****************************************************************
* Exit(): Called when you switch states and don't need to preserve
*				this one
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
bool CControlsState::Exit(void)
{
	if (!GetEntered( ))
		return false;
	CBaseState::Exit( );
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
* Mod. Date:		      05/13/2015
* Mod. Initials:	      MJG
*****************************************************************/
eStateType CControlsState::CheckState(void)
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
		AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::RESUME_ALL);
	}
	//else if (m_bStateChanges[STATE_CH_PLAY])
	//{
	//	eNextState = STATE_GAMEPLAY;
	//	m_bToPop = true;
	//}

	if (CBaseState::CheckState( ) == STATE_NULL)
	{
		eNextState = STATE_NULL;
	}

	//Tell the state machine what state we're switching to
	return eNextState;
}

bool CControlsState::PageFlip(eHelpPage nTargetPage)
{

	//Make sure that page exists
	if (nTargetPage < 0 || nTargetPage >= HELP_PAGE_COUNT)
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
	if (m_nPageIndex == HELP_PAGE_COUNT - 1)
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
