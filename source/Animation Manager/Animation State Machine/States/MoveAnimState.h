/***********************************************
* Filename:			MoveAnimState.h
* Date:      		06/16/2015
* Mod. Date:		06/16/2015
* Mod. Initials:	JPM
* Author:    		Justin Mazzola
* Purpose:			This class will be in charge of
*					handling the states and animations
*					between moving animations.
************************************************/

#ifndef _CMOVEANIMSTATE_H_
#define _CMOVEANIMSTATE_H_

#include "../IAnimState.h"

class IAnimState;
class CLivingEntities;


class CMoveAnimState : public IAnimState
{

public:
	~CMoveAnimState();

	static CMoveAnimState* GetInstance();

	virtual void Enter(CLivingEntities* pEntity);
	virtual void Run(CLivingEntities* pEntity, float fDt);
	virtual void Exit(CLivingEntities* pEntity);

private:

	CMoveAnimState();
	CMoveAnimState(const CMoveAnimState&);
	const CMoveAnimState& operator=(const CMoveAnimState&);


};
#endif // _CMOVEANIMSTATE_H_
