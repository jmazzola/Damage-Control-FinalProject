/***********************************************
* Filename:			AnimStateMachine.cpp
* Date:      		06/16/2015
* Mod. Date:		06/16/2015
* Mod. Initials:	JPM
* Author:    		Justin Mazzola
* Purpose:			This class will be in charge of
*					handling the states and transitions
*					between animations for a single entity
************************************************/

#include "stdafx.h"
#include "AnimStateMachine.h"
#include "IAnimState.h"


CAnimStateMachine::CAnimStateMachine()
{
}


CAnimStateMachine::~CAnimStateMachine()
{
}

void CAnimStateMachine::Update(float fDt)
{
	m_pCurrentState->Run(m_pEntity, fDt);
}
void CAnimStateMachine::ChangeState(IAnimState* pNextState)
{
		if (m_pCurrentState)
	{
		m_pCurrentState->Exit(m_pEntity);
	}

	pNextState->Enter(m_pEntity);
	m_pCurrentState = pNextState;
}
