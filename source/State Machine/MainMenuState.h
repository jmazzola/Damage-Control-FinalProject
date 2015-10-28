/***********************************************
* Filename:  		MainMenuState.h
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This is the state used for
*			 the main menu
************************************************/

#ifndef _CMAINMENUSTATE_H_
#define _CMAINMENUSTATE_H_

#include "BaseState.h"

class CScene;

class CEntity;
class CUIElement;
class CCutscene;
class CUIButton;

class CMainMenuState : public CBaseState
{
public:
	/*****************************************************************
	* Default Constructor
	*****************************************************************/
	CMainMenuState(void);

	/*****************************************************************
	* Disabled Copy Constructor
	*****************************************************************/
	CMainMenuState(const CMainMenuState& other) = delete;

	/*****************************************************************
	* Disabled Operator =
	*****************************************************************/
	CMainMenuState& operator= (const CMainMenuState& other) = delete;

	/*****************************************************************
	* Default Destructor
	*****************************************************************/
	~CMainMenuState(void);

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
	void Render(CAssetManager* pAssets) override;

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
	bool FirstEnter(void);

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

	bool GetEndlessMode(void) const { return m_bEndlessMode; }
	bool GetWaifuBander()const { return m_bWaifuBander; }

	void SetWaifuBander(bool _waifu) { m_bWaifuBander = _waifu; }

private:

	bool FadeToBlack(float _fDT, float _fAlpha);
	bool FadeToNotBlack(float _fDT, float _fAlpha);

	CUIElement* m_pBackground = NULL;
	CUIElement* m_pLogo;

	CCutscene* m_pCutsceneTest;

	bool m_bSelectMode;
	bool m_bEndlessMode;

	CUIElement* m_pBackPlate;
	CUIButton* m_pBack;
	CUIButton* m_pCampaign;
	CUIButton* m_pEndless;
	bool m_bWaifuBander;
};

#endif // _CMainMenuState_H_
