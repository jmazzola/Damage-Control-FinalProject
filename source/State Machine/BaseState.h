/***********************************************
* Filename:  		BaseState.h
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This is the base class of
*			 all gameplay states
************************************************/

#ifndef _CBASESTATE_H_
#define _CBASESTATE_H_

#include "..\Core\stdafx.h"

//#ifndef TRANSITION_TIME
//#endif

/*****************************************************************
* eStateChange - Defines the transitions from one state to another
*				STATE_CH_EXIT closes the application
*
* Mod. Date:		      05/12/2015
* Mod. Initials:	      MJG
*****************************************************************/
enum eStateChange : unsigned char
{
	STATE_CH_PLAY = 0, STATE_CH_PAUSE, STATE_CH_OPTIONS, STATE_CH_QUIT,
	STATE_CH_BACK, STATE_CH_LOSE, STATE_CH_WIN, STATE_CH_YES, STATE_CH_NO,
	STATE_CH_EXIT, STATE_CH_SHOP, STATE_CH_CONTROLS, STATE_CH_CUTSCENE,
	STATE_CH_LOGOS, STATE_CH_MAINMENU, STATE_CH_CREDITS, STATE_CH_ACHIEVEMENTS,
	STATE_CH_HIGHSCORE,
	STATE_CH_COUNT
};

//Allows children to esily distinguish from their different types
enum eStateType : unsigned char
{
	STATE_NULL = 0,
	STATE_BASE = 0, STATE_MAINMENU, STATE_GAMEPLAY, STATE_CONTROLS,
	STATE_PAUSE, STATE_YESNO, STATE_OPTIONS, STATE_LOSE, STATE_WIN,
	STATE_UPGRADE, STATE_CUTSCENE, STATE_LOGOS, STATE_CREDITS, 
	/*STATE_STAGESELECT,*/ STATE_HIGHSCORE,
	STATE_ACHIEVEMENTS, STATE_COUNT
};

enum eStateLoadCommands : unsigned char
{
	STATE_COMMAND_NULL = 0,
	STATE_COMMAND_LOAD,
	STATE_COMMAND_UNLOAD,
};

class CScene;
class CTexture2D;
class CRenderer;
class CGUIManager;
class CAssetManager;
class CUIElement;

class CBaseState
{
public:
	/*****************************************************************
	* Default Constructor
	*****************************************************************/
	CBaseState( ) = default;

	/*****************************************************************
	* Primary Constructor
	*****************************************************************/
	CBaseState(eStateType eType);

	/*****************************************************************
	* Disabled Copy Constructor
	*****************************************************************/
	CBaseState(const CBaseState& pOther) = delete;

	/*****************************************************************
	* Disabled Operator =
	*****************************************************************/
	CBaseState& operator= (const CBaseState& pOther) = delete;

	/*****************************************************************
	* Default Destructor
	*****************************************************************/
	virtual ~CBaseState(void) = default;

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
	virtual void Input(void) = 0;

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
	virtual void Update(float fDT) = 0;

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
	virtual void Render(CAssetManager* pRenderer) = 0;

	/*****************************************************************
	* Enter(): Called when switching to a state, initializes members
	*                and allocates memory
	*
	* Ins:			  CBaseState* cPrevState
	*
	* Outs:		      void
	*
	* Returns:		  bool, true if successfully entered
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	bool Enter(void);

	/*****************************************************************
	* Enter(): Shared function to show the text of how to navigate in the menus
	*
	* Ins:			  CBaseState* cPrevState
	*
	* Outs:		      void
	*
	* Returns:		  bool, true if successfully entered
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	void DrawNavigationInfo(void);

	/*****************************************************************
	* HandlePrevState(): Called before entering this state, stores/deletes
	*					state based on the types
	*
	* Ins:			  CBaseState* cPrevState, CAssetManager* pAssets
	*
	* Outs:		      void
	*
	* Returns:		  void
	*
	* Mod. Date:		      05/23/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	virtual void HandlePrevState(CBaseState* pPrevState);

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
	virtual bool Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands = nullptr);

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
	virtual void UnLoad(eStateLoadCommands* pLoadCommands = nullptr);

	/*****************************************************************
	* Exit(): Called when you switch states and don't need to preserve
	*				this one
	*
	* Ins:			  void
	*
	* Outs:		      void
	*
	* Returns:		  bool - whether or not to unload with as well
	*
	* Mod. Date:		      05/13/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	virtual bool Exit(void);

	/*****************************************************************
	* QueueStateChange: Informs the state that it should change to
	*				another state
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
	void QueueStateChange(eStateChange eChange)
	{
		m_bStateChanges[eChange] = true;
	};

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
	virtual eStateType CheckState(void);

	//-----ACCESSORS---------
	eStateType GetType(void) const
	{
		return m_eType;
	}
	bool GetEntered(void) const
	{
		return m_bEntered;
	}
	bool GetLoaded(void) const
	{
		return m_bLoaded;
	}
	CGUIManager* GetGUI(void) const
	{
		return m_pGUI;
	}
	CBaseState* GetPrevState(void) const
	{
		return m_pPrevState;
	}
	CScene* GetScene(void) const
	{
		return m_pScene;
	}
	bool GetTransparent(void) const
	{
		return m_bTransparent;
	}
	bool GetToPop(void) const
	{
		return m_bToPop;
	}
	bool GetShowCursor(void) const
	{
		return m_bShowCursor;
	}
	//-----MUTATORS---------
	void SetPrevState(CBaseState* pPrevState)
	{
		m_pPrevState = pPrevState;
	}
	void SetTransparency(bool bTransparent)
	{
		m_bTransparent = bTransparent;
	}

protected:
	//Called the first time you enter a function
	virtual bool FirstEnter(void);
	//Called if you've already entered (handles transitions and such)
	virtual bool ReEnter(void);

	/*****************************************************************
	* ClearStateChanges: Sets all statechange flags to false
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
	void ClearStateChanges(void)
	{
		memset(m_bStateChanges, 0, STATE_CH_COUNT);
	}

	float blend(float a, float b, float alpha)
	{
		float bL = (1.f - alpha) * a + alpha * b;

		if (a < b)
			return bL >= b ? b : bL;

		return bL <= b ? b : bL;
	}

	//Displays the GUI for this state
	CGUIManager* m_pGUI = nullptr;

	//Tells what switch messages were called, so we can 
	//	react to the most important one
	//Cleared every frame
	bool m_bStateChanges[STATE_CH_COUNT];
	//Can you see the state behind this
	bool m_bTransparent = false;
	//Should we keep this in the state stack
	bool m_bToPop = false;
	//Whether or not to show the cursor while this state is on top
	bool m_bShowCursor = true;
	//The previous state
	CBaseState* m_pPrevState = nullptr;

	CScene* m_pScene = NULL;

	//Tells whether entered
	bool m_bEntered = false;
	//Tells whether loaded
	bool m_bLoaded = false;

	float m_fFadeTimer = 0.0f;
	bool m_bFadeOut = false, m_bFadeIn = false;
	CUIElement* m_pScreen = nullptr;

	float TRANSITION_TIME = .5f;

private:
	//What state type is it
	eStateType m_eType = STATE_BASE;

	//Tell the player that LMB selects elements
	CUIElement* m_pLMBSelect = nullptr;

};

#endif // _CBASESTATE_H_
