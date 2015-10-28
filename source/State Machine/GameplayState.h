/***********************************************
* Filename:  		GameplayState.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This is the state used for
*			 confirming any actions
************************************************/

#ifndef _CGAMEPLAYSTATE_H_
#define _CGAMEPLAYSTATE_H_

#include "BaseState.h"
#include "../Animation Manager/Interpolator.h"
#include "../Animation Manager/Deformer.h"

class CEntityManager;
class CScene;
class CRObject;
class CUIElement;
class CLevel;

class CGameplayState : public CBaseState
{
public:
	/*****************************************************************
	* Default Constructor
	*****************************************************************/
	CGameplayState(void);

	/*****************************************************************
	* Disabled Copy Constructor
	*****************************************************************/
	CGameplayState(const CGameplayState& pOther) = delete;

	/*****************************************************************
	* Disabled Operator =
	*****************************************************************/
	CGameplayState& operator= (const CGameplayState& pOther) = delete;

	/*****************************************************************
	* Default Destructor
	*****************************************************************/
	~CGameplayState(void);

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
	void Render(CAssetManager* pAssetManager) override;

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
	* Mod. Initials:	      MJGbool Load(CAssetManager* pAssets);
	*****************************************************************/
	bool FirstEnter(void) override;

	bool ReEnter(void);

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
	void HandlePrevState(CBaseState* pPrevState);

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

	/*****************************************************************
	* NotifyGuiForSubtitle(): notifies to the GUI when to show / hide the rectangle
	*
	* Ins:			  bool
	*
	* Outs:		      void
	*
	* Returns:		  void
	*
	* Mod. Date:		      08/08/2015
	* Mod. Initials:	      JM
	*****************************************************************/
	void NotifyGuiForSubtitle(bool value);

	//-----------Accessors----------------
	static CEntityManager* GetEntityManager( )
	{
		return m_pEntityManager;
	}
	static CAssetManager* GetAssetManager(void)
	{
		return m_pAssetManager;
	}
	unsigned int GetDefconLevel(void) const
	{
		return m_unDefConLevel;
	}
	DirectX::XMFLOAT3 GetPlayerStartPos(void) const
	{
		return m_xmStartPos;
	}
	CLevel* GetLevelManager(void) const { return m_pLevelManager; }

	bool GetIsInfinite()const
	{
		return m_bInfinite;
	}

	//-----------Mutators----------------
	void SetDefconLevel(unsigned int unDefconLevel)
	{
		m_unDefConLevel = unDefconLevel;
	}
	void SetPlayerStartPos(DirectX::XMFLOAT3 xmStartPos)
	{
		m_xmStartPos = xmStartPos;
	}

	void SetIsInfinite(bool isInfinite)
	{
		m_bInfinite = isInfinite;
	}

private:
	static CEntityManager* m_pEntityManager;
	static CAssetManager* m_pAssetManager;

	// current defcon level
	unsigned int m_unDefConLevel;
	bool		m_bInfinite;
	int   m_nControl;

	//Most importantly holds level-wide lights for us
	CLevel* m_pLevelManager = nullptr;

	DirectX::XMFLOAT3 m_xmStartPos;

	float amount_to_fade_in, amount_to_fade_out;
};

#endif // _CGameplayState_H_
