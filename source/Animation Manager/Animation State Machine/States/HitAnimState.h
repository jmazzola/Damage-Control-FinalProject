#ifndef _CHITANIMSTATE_H_
#define _CHITANIMSTATE_H_

#include "../IAnimState.h"

class IAnimState;
class CLivingEntities;

class CHitAnimState : public IAnimState
{

public:
	~CHitAnimState();

	static CHitAnimState* GetInstance();

	virtual void Enter(CLivingEntities* pEntity);
	virtual void Run(CLivingEntities* pEntity, float fDt);
	virtual void Exit(CLivingEntities* pEntity);

private:

	CHitAnimState();
	CHitAnimState(const CHitAnimState&);
	const CHitAnimState& operator=(const CHitAnimState&);
};

#endif // _CHITANIMSTATE_H_