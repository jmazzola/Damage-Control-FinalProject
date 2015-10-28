/***********************************************
* Filename:			AttackAnimState.h
* Date:      		07/24/2015
* Mod. Date:		07/24/2015
* Mod. Initials:	JPM
* Author:    		Justin Mazzola
* Purpose:			This class will be in charge of
*					handling the states and animations
*					between dying animations.
************************************************/

#include "../IAnimState.h"

#ifndef _CATTACKANIMSTATE_H_
#define _CATTACKANIMSTATE_H_

class IAnimState;
class CLivingEntities;

class CAttackAnimState : public IAnimState
{

public:
	~CAttackAnimState();

	static CAttackAnimState* GetInstance();

	virtual void Enter(CLivingEntities* pEntity);
	virtual void Run(CLivingEntities* pEntity, float fDt);
	virtual void Exit(CLivingEntities* pEntity);

private:

	CAttackAnimState();
	CAttackAnimState(const CAttackAnimState&);
	const CAttackAnimState& operator=(const CAttackAnimState&);

};

#endif // _CATTACKANIMSTATE_H_

