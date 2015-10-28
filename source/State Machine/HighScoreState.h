/***********************************************
* Filename:  		HighScoreState.h
* Date:      		08/17/2015
* Mod. Date:
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:   		Class to show the high scores
************************************************/

#ifndef _CHIGHSCORE_H
#define _CHIGHSCORE_H

#include "BaseState.h"
#include "../Core/StructureHolder.h"

class CUIElement;
class CUIButton;

class CHighScoreState :
	public CBaseState
{
public:
	/*****************************************************************
	* Default Constructor
	*****************************************************************/
	CHighScoreState();

	/*****************************************************************
	* Disabled Copy Constructor
	*****************************************************************/
	CHighScoreState(const CHighScoreState& other) = delete;

	/*****************************************************************
	* Disabled Operator =
	*****************************************************************/
	CHighScoreState& operator= (const CHighScoreState& other) = delete;

	/*****************************************************************
	* Default Destructor
	*****************************************************************/
	~CHighScoreState();

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
	* Mod. Date:		     08/17/2015
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
	* Mod. Date:		     08/17/2015
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
	* Mod. Date:		     08/17/2015
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
	* Mod. Date:		     08/17/2015
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
	* Mod. Date:		     08/17/2015
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
	* Mod. Date:		     08/17/2015
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
	* Mod. Date:		     08/17/2015
	* Mod. Initials:	      JM
	*****************************************************************/
	eStateType CheckState(void);

	/*****************************************************************
	* LoadNormalScores(): loads normal mode scores.
	*
	* Ins:			  void
	*
	* Outs:		      void
	*
	* Returns:		  bool
	*
	* Mod. Date:		     08/18/2015
	* Mod. Initials:	      JM
	*****************************************************************/
	bool LoadNormalScores();

	/*****************************************************************
	* LoadInfiniteScores(): loads endless mode scores.
	*
	* Ins:			  void
	*
	* Outs:		      void
	*
	* Returns:		  bool
	*
	* Mod. Date:		     08/18/2015
	* Mod. Initials:	      JM
	*****************************************************************/
	bool LoadInfiniteScores();

	/*****************************************************************
	* SaveAndLoadDefault(): if there isn't any available data, we need to create a new file and load it to print it to screen
	*
	* Ins:			  int type - normal = 0 
							   - infinite = 1
	*
	* Outs:		      void
	*
	* Returns:		  void
	*
	* Mod. Date:		     08/25/2015
	* Mod. Initials:	      JM
	*****************************************************************/
	void SaveAndLoadDefault(int type);

private:
	bool FadeToBlack(float _fDT, float _fAlpha);
	bool FadeToNotBlack(float _fDT, float _fAlpha);

	CUIElement* m_pScreen;
	CUIElement* m_pSBackground;
	CUIElement* m_pBackplateNormal;
	CUIElement* m_pBackplateEndless;


	CUIButton* m_pBack;
	CUIButton* m_pLeft;
	CUIButton* m_pRight;

	bool m_bChanged;
	bool m_bNormal;
	float m_fPageTimer;

	std::string m_sHighScoresNormal;
	std::string m_sHighScoresInfinite;
	std::vector<TMsScores> m_pMScores; 

};

#endif // _CHIGHSCORE_H
