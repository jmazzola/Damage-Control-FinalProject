#ifndef _CDEPLOYANIMSTATE_H_
#define _CDEPLOYANIMSTATE_H_
#include "../IAnimState.h"

class IAnimState;
class CLivingEntities;

class CDeployAnimState : public IAnimState
{
public:

	~CDeployAnimState();

	static CDeployAnimState* GetInstance();

	virtual void Enter(CLivingEntities* pEntity);
	virtual void Run(CLivingEntities* pEntity, float fDt);
	virtual void Exit(CLivingEntities* pEntity);

private:

	CDeployAnimState();
	CDeployAnimState(const CDeployAnimState&);
	const CDeployAnimState& operator=(const CDeployAnimState&);
};

#endif //_CDEPLOYANIMSTATE_H_

