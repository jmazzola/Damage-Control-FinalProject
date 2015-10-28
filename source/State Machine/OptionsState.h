/***********************************************
* Filename:  		OptionsState.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This is the state used for
*			 changing user options
************************************************/

#ifndef _COPTIONSSTATE_H_
#define _COPTIONSSTATE_H_

#include "BaseState.h"

class CUIButton;
class CUIElement;

//TODO: Add values for every page
enum eOptionsPage
{
	OPTIONS_PAGE_SOUNDS = 0,
	OPTIONS_PAGE_GRAPHICS,
	OPTIONS_PAGE_COUNT
};

enum eGraphicsOptions
{
	OPTIONS_GRAPHICS_COUNT,
};

class COptionsState : public CBaseState
{
public:
	/*****************************************************************
	* Default Constructor
	*****************************************************************/
	COptionsState(void);

	/*****************************************************************
	* Disabled Copy Constructor
	*****************************************************************/
	COptionsState(const COptionsState& pOther) = delete;

	/*****************************************************************
	* Disabled Operator =
	*****************************************************************/
	COptionsState& operator= (const COptionsState& pOther) = delete;

	/*****************************************************************
	* Default Destructor
	*****************************************************************/
	~COptionsState(void);

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
	* Ins:			  void
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

	int GetMaster( ) const
	{
		return m_nMasterBank;
	}
	int GetSFX( ) const
	{
		return m_nSFXBank;
	}
	int GetMusic( ) const
	{
		return m_nMusicBank;
	}

	BOOL GetFullscreen( ) const
	{
		return m_nFullscreen;
	}

	BOOL GetWaifu( ) const
	{
		return m_nWaifu;
	}

	float GetGamma( ) const
	{
		return m_nGamma / 100.0f;
	}

	int GetWidth( ) const;

	int GetHeight( ) const;

private:

	void LoadDefaults( );
	void WriteDefaults( );

	//master
	//sfx
	//bgm

	//resolution
	//gamma
	//fullscreen

	//waifu

	void WriteAllOptions( );

	void ApplyFullscreen( );

	bool PageFlip(eOptionsPage nTargetPage);

	CUIElement* m_pPages[OPTIONS_PAGE_COUNT];
	signed char m_nPageIndex = 0;

	//Holds the decided upon volume
	int m_nMasterBank = -1;
	int m_nSFXBank = -1;
	int m_nMusicBank = -1;

	int m_nResolution = -1;
	int m_nGamma = -1;
	BOOL m_nFullscreen = -1;

	BOOL m_nWaifu = -1;

	bool m_bOptionsChanged;

	//Volume buttons
	CUIButton* m_pMinusButtons[4];
	CUIButton* m_pPlusButtons[4];

	//waifu
	//CUIButton* m_pWaifuToggle;
	//CUIElement* m_pSelector;

	//Menu buttons
	CUIButton* m_pAccept = nullptr;
	CUIButton* m_pApply = nullptr;
	CUIButton* m_pDefault = nullptr;

	CUIButton* m_pBack = nullptr;
	CUIButton* m_pLeft = nullptr;
	CUIButton* m_pRight = nullptr;
};

#endif // _COptionsState_H_

