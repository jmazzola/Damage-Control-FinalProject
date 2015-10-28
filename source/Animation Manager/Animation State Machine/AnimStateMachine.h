/***********************************************
* Filename:			AnimStateMachine.h
* Date:      		06/16/2015
* Mod. Date:		06/16/2015
* Mod. Initials:	JPM
* Author:    		Justin Mazzola
* Purpose:			This class will be in charge of
*					handling the states and transitions
*					between animations for a single entity
************************************************/

#ifndef _CANIMSTATEMACHINE_H_
#define _CANIMSTATEMACHINE_H_

class IAnimState;
class CLivingEntities;

class CAnimStateMachine
{

public:

	CAnimStateMachine();
	~CAnimStateMachine();

	void Update(float fDt);
	void ChangeState(IAnimState* pNextState);


	// Accessors
	IAnimState* GetCurrentState() { return m_pCurrentState; }
	CLivingEntities* GetEntity() { return m_pEntity; }


	void SetEntity(CLivingEntities* pEnt) { m_pEntity = pEnt; }

private:

	IAnimState* m_pCurrentState;
	CLivingEntities* m_pEntity;

};

#endif // _CANIMSTATEMACHINE_H_
