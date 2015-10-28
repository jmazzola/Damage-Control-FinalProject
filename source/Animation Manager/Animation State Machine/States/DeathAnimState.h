/***********************************************
* Filename:			DeathAnimState.h
* Date:      		06/17/2015
* Mod. Date:		06/17/2015
* Mod. Initials:	JPM
* Author:    		Justin Mazzola
* Purpose:			This class will be in charge of
*					handling the states and animations
*					between dying animations.
************************************************/

#include "../IAnimState.h"

#ifndef _CDEATHANIMSTATE_H_
#define _CDEATHANIMSTATE_H_

class IAnimState;
class CLivingEntities;

class CDeathAnimState :	public IAnimState
{

public:
	~CDeathAnimState();

	static CDeathAnimState* GetInstance();

	virtual void Enter(CLivingEntities* pEntity);
	virtual void Run(CLivingEntities* pEntity, float fDt);
	virtual void Exit(CLivingEntities* pEntity);

private:

	CDeathAnimState();
	CDeathAnimState(const CDeathAnimState&);
	const CDeathAnimState& operator=(const CDeathAnimState&);

};

#endif // _CDEATHANIMSTATE_H_

