/***********************************************
* Filename:  		CCreditsState.h
* Date:      		08/10/2015
* Mod. Date:
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:   		Shows the credits
************************************************/

#ifndef _CCREDITSSTATE_H_
#define _CCREDITSSTATE_H_


#define CREDITS_TEXT_START_POSITION -1.25f
#define CREDITS_TEXT_END_POSITION 8.5f

#define CREDITS_COLOR_CHANGE 0.4f

//#ifndef TRANSITION_TIME
//#define TRANSITION_TIME 1.0f
//#endif

#include "BaseState.h"

class CUIElement;
class CUIButton;

class CCreditsState:
	public CBaseState
{
public:
	/*****************************************************************
	* Default Constructor
	*****************************************************************/
	CCreditsState();

	/*****************************************************************
	* Disabled Copy Constructor
	*****************************************************************/
	CCreditsState(const CCreditsState& other) = delete;

	/*****************************************************************
	* Disabled Operator =
	*****************************************************************/
	CCreditsState& operator= (const CCreditsState& other) = delete;

	/*****************************************************************
	* Default Destructor
	*****************************************************************/
	~CCreditsState();

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
	* Mod. Date:		     08/10/2015
	* Mod. Initials:	      JM
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
	* Mod. Date:		     08/10/2015
	* Mod. Initials:	      JM
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
	* Mod. Date:		     08/10/2015
	* Mod. Initials:	      JM
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
	* Mod. Date:		     08/10/2015
	* Mod. Initials:	      JM
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
	* Mod. Date:		     08/10/2015
	* Mod. Initials:	      JM
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
	* Mod. Date:		     08/10/2015
	* Mod. Initials:	      JM
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
	* Mod. Date:		     08/10/2015
	* Mod. Initials:	      JM
	*****************************************************************/
	eStateType CheckState(void);

private:
	float			m_fTextPosition;
	std::string		m_sCredits;
	CUIElement* m_pScreen;
	CUIElement* m_pBackground;

	CUIButton* m_pBack;

	float m_fRed;
	float m_fGreen;
	float m_fBlue;

	float m_fColorChange;


};

#endif //_CCREDITSSTATE_H_
