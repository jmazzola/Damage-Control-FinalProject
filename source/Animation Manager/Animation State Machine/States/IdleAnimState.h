/***********************************************
* Filename:			IdleAnimState.h
* Date:      		06/16/2015
* Mod. Date:		06/16/2015
* Mod. Initials:	JPM
* Author:    		Justin Mazzola
* Purpose:			This class will be in charge of
*					handling the states and transitions
*					between idle animations
************************************************/

#ifndef _CIDLEANIMSTATE_H_
#define _CIDLEANIMSTATE_H_

#include "../IAnimState.h"

class IAnimState;
class CLivingEntities;

class CIdleAnimState : public IAnimState
{

public:
	~CIdleAnimState();

	static CIdleAnimState* GetInstance();
	
	virtual void Enter(CLivingEntities* pEntity);
	virtual void Run(CLivingEntities* pEntity, float fDt);
	virtual void Exit(CLivingEntities* pEntity);

private:

	CIdleAnimState();
	CIdleAnimState(const CIdleAnimState&);
	const CIdleAnimState& operator=(const CIdleAnimState&);

	
};
#endif // _CIDLEANIMSTATE_H_