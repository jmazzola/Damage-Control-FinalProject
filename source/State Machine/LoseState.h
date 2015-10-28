/***********************************************
* Filename:  		LooseState.h
* Date:      		05/23/2015
* Mod. Date: 		
* Mod. Initials:	
* Author:    		Michael Mozdzierz
* Purpose:   		This state is used to indicate the
*					player has lost the game
************************************************/
#ifndef _LOOSESTATE_H_
#define _LOOSESTATE_H_

#include "BaseState.h"

#include "../Renderer/HUDRenderer.h"
#include "../Core/StructureHolder.h"

enum eLooseReason
{
	LOOSE_REASON_KILLED,
	LOOSE_REASON_REACTOR_MELTDOWN,
};

class CUIButton;

class CLoseState : public CBaseState
{
public:
	/*****************************************************************
	* Default Constructor
	*****************************************************************/
	CLoseState();

	/*****************************************************************
	* Disabled Copy Constructor
	*****************************************************************/
	CLoseState(const CLoseState& pOther) = delete;

	/*****************************************************************
	* Disabled Operator =
	*****************************************************************/
	CLoseState& operator= (const CLoseState& pOther) = delete;

	/*****************************************************************
	* Default Destructor
	*****************************************************************/
	//~CLoseState(void);

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
	virtual void Input(void);

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
	virtual void Update(float fDT);

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
	virtual void Render(CAssetManager* cRenderer);

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
	virtual bool FirstEnter(void);

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
	* Returns:		  void
	*
	* Mod. Date:		      05/13/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	virtual bool Exit(void);

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

	/*****************************************************************
	* CheckScores(): Checks the highscores to show/hide name enter buttons
	*
	* Ins:			  void
	*
	* Outs:		      void
	*
	* Returns:		  bool
	*
	* Mod. Date:		      08/20/2015
	* Mod. Initials:	      JM
	*****************************************************************/
	bool CheckScores();

	/*****************************************************************
	* SaveScores(): Updates the highscores with the new entry
	*
	* Ins:			  void
	*
	* Outs:		      void
	*
	* Returns:		  bool
	*
	* Mod. Date:		      08/20/2015
	* Mod. Initials:	      JM
	*****************************************************************/
	bool SaveScores();

private:
	//GUi elements
	std::vector<CTexture2D*> m_vElements;

	std::string m_cOne;
	std::string m_cTwo;
	std::string m_cThree;

	float m_fTimer;
	bool m_bSoundPlayed;

	CUIButton* m_pCharOneUp;
	CUIButton* m_pCharOneDown;

	CUIButton* m_pCharTwoUp;
	CUIButton* m_pCharTwoDown;

	CUIButton* m_pCharThreeUp;
	CUIButton* m_pCharThreeDown;

	CUIButton* m_pBack;

	std::vector<TMsScores> m_vAllScores;

	bool m_bShowButtons;
	bool m_bisInfinite;
	int m_nScore;
	bool m_bPlaySound;
};

#endif