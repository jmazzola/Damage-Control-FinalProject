/***********************************************
* Filename:			IAnimState.h
* Date:      		06/16/2015
* Mod. Date:		06/16/2015
* Mod. Initials:	JPM
* Author:    		Justin Mazzola
* Purpose:			This class will be in charge of
*					being the interface for the states
*					of an entity's state machine
************************************************/

#ifndef _IANIMSTATE_H_
#define _IANIMSTATE_H_

#include "../../Asset Manager/AssetManager.h"

class CLivingEntities;

class __declspec(novtable) IAnimState
{
public:
	
	IAnimState();

	virtual ~IAnimState();

	virtual void Enter(CLivingEntities* pEntity) = 0;
	virtual void Run(CLivingEntities* pEntity, float fDt) = 0;
	virtual void Exit(CLivingEntities* pEntity) = 0;

protected:

	CAssetManager::TAnimation* GetAnimation() { return m_pAnimation; }

	CAssetManager::TAnimation* m_pAnimation;

};

#endif // _IANIMSTATE_H_
