/***********************************************
* Filename:			StateMachine.h
* Date:      		05/12/2015
* Mod. Date:		05/13/2015
* Mod. Initials:	MJG
* Author:    		Michael Gergar
* Purpose:			Contains every system in
*				the application
************************************************/

#ifndef _CSTATEMACHINE_H_
#define _CSTATEMACHINE_H_

#include "..\Core\stdafx.h"
#include "..\Audio Manager\AudioSystemWwise.h"
#include <forward_list>
#include "BaseState.h"
class CRenderer;
class CAssetManager;
class CTimer;
class CPathSearch;
class CGameplayState;
class CAchievements;

class CStateMachine
{
public:
	/*****************************************************************
	* Default Constructor
	*****************************************************************/
	CStateMachine( );

	/*****************************************************************
	* Disabled Copy Constructor
	*****************************************************************/
	CStateMachine(const CStateMachine& pOther) = delete;

	/*****************************************************************
	* Disabled Operator =
	*****************************************************************/
	CStateMachine& operator= (const CStateMachine& pOther) = delete;

	/*****************************************************************
	* Default Destructor
	*****************************************************************/
	~CStateMachine( );

	/*****************************************************************
	* Start(): Called at the start of the program to allocate memory
	*                and start core systems
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
	void Start( );

	/*****************************************************************
	* Update(): Called every frame to capture input, render, and
	*                update all contained systems
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
	void Update(void);

	/*****************************************************************
	* Stop(): Called at the end of the application, leads to the
	*				closing of everything
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
	void Stop(void);

	/***************
	*  Accessors
	***************/
	bool GetPlaying( ) const
	{
		return m_bPlaying;
	}

private:
	//Checks our list of commads to see if we should (un)load any states ahead of time
	void CheckCommands(void);

	void EnterState(CBaseState* pState);
	void ExitState(CBaseState* pState);

	//Indicates if the program is running
	//Call Start to set to true
	bool m_bPlaying = false;
	//Stores all 2D and 3D assets in the game
	CAssetManager* m_pAssetManager = nullptr;
	//Gives delta time to everything
	CTimer* m_pTimer = nullptr;
	//The stack
	std::forward_list<CBaseState*> m_lStateStack;
	CBaseState* m_pStates[eStateType::STATE_COUNT];
	//Tell the state machine if it should keep a state loaded/unloaded
	eStateLoadCommands m_eCurrentCommands[eStateType::STATE_COUNT];
	//Everyone's sound system
	AudioSystemWwise m_pSoundManager;

	float m_fFPS;

	float m_fTotalTime;
};

#endif // _CSTATEMACHINE_H_
