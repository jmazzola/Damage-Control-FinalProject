/***********************************************
* Filename:  		ControlsState.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This is the state used for
*			 displaying the controls to the user
************************************************/

#ifndef _CCONTROLSSTATE_H_
#define _CCONTROLSSTATE_H_

#include "BaseState.h"

#include "../Renderer/HUDRenderer.h"

class CUIButton;
class CUIElement;
class CTexture2D;

//TODO: Add values for every page
enum eHelpPage : signed char
{
	HELP_PAGE_CONTROLS = 0,
	HELP_PAGE_OBJECTIVES, HELP_PAGE_ENEMIES, HELP_PAGE_WEAPONS, HELP_PAGE_HUD,
	HELP_PAGE_COUNT
};

class CControlsState : public CBaseState
{
public:
	/*****************************************************************
	* Default Constructor
	*****************************************************************/
	CControlsState(void);

	/*****************************************************************
	* Disabled Copy Constructor
	*****************************************************************/
	CControlsState(const CControlsState& pOther) = delete;

	/*****************************************************************
	* Disabled Operator =
	*****************************************************************/
	CControlsState& operator= (const CControlsState& pOther) = delete;

	/*****************************************************************
	* Default Destructor
	*****************************************************************/
	~CControlsState(void);

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
	void Input(void);

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
	void Update(float fDT);

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
	void Render(CAssetManager* pRenderer) override;

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
	bool FirstEnter(void) override;

	bool ReEnter(void);

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
	bool Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands = nullptr);

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
	void UnLoad(eStateLoadCommands* pLoadCommands = nullptr);

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
	bool Exit(void);

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
	eStateType CheckState(void);

private:

	bool PageFlip(eHelpPage nTargetPage);

	//Flips to the page to the left
	CUIButton* m_pLeft = nullptr;
	//Flips to the page to the right
	CUIButton* m_pRight = nullptr;
	//Returns to the previous state
	CUIButton* m_pBack = nullptr;

	//The different help pages
	CUIElement* m_pPages[HELP_PAGE_COUNT];

	//The current page
	signed char m_nPageIndex = 0;

	CTexture2D* m_pBackground = nullptr;

};

#endif // _CControlsState_H_
