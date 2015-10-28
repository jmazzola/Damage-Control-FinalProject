/***********************************************
* Filename:  		LogoState.h
* Date:      		08/06/2015
* Mod. Date:
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:   		This is the state that shows the studio logo and
team name
************************************************/

#ifndef _CLOGOSTATE_H_
#define _CLOGOSTATE_H_

#include "BaseState.h"

class CUIElement;
#ifndef IMAGECHANGETIMER
#define IMAGECHANGETIMER 3.0f
#endif

//#ifndef TRANSITION_TIME
//#define TRANSITION_TIME 1.0f
//#endif

class CLogoState :
	public CBaseState
{
public:
	/*****************************************************************
	* Default Constructor
	*****************************************************************/
	CLogoState( );

	/*****************************************************************
	* Disabled Copy Constructor
	*****************************************************************/
	CLogoState(const CLogoState& other) = delete;

	/*****************************************************************
	* Disabled Operator =
	*****************************************************************/
	CLogoState& operator= (const CLogoState& other) = delete;

	/*****************************************************************
	* Default Destructor
	*****************************************************************/
	~CLogoState( );

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
	* Mod. Date:		     08/06/2015
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
	* Mod. Date:		     08/06/2015
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
	* Mod. Date:		     08/06/2015
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
	* Mod. Date:		     08/06/2015
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
	* Mod. Date:		     08/06/2015
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
	* Mod. Date:		     08/06/2015
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
	* Mod. Date:		     08/06/2015
	* Mod. Initials:	      JM
	*****************************************************************/
	eStateType CheckState(void);

private:
	bool FadeToBlack(float _fDT, float _fAlpha);
	bool FadeToWhite(float _fDT, float _fAlpha);
	bool FadeToNotBlack(float _fDT, float _fAlpha);

	CUIElement* m_pStudioLogo;
	CUIElement* m_pTeamLogo;
	CUIElement* m_pGPLogo;
	CUIElement* m_pScreen;

	int m_nPhaseCheck;

	float m_fStateTimer;

	float m_fFadeIn;
	float m_fFadeOut;
	float m_fDT;

	bool  m_bCanSkip;
};


#endif // _CLOGOSTATE_H_