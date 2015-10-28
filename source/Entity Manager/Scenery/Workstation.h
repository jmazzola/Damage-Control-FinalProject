/***********************************************
* Filename:			Workstation.h
* Date:      		06/14/2015
* Mod. Date:		06/14/2015
* Mod. Initials:	MJG
* Author:    		Michael Gergar
* Purpose:			Entity that when interacted
*				with will open a menu for the player
************************************************/
#ifndef _WORKSTATION_H_
#define _WORKSTATION_H_

#include "Scenery.h"
#include "../../State Machine/BaseState.h"
class CUIElement;

class CWorkStation : public CScenery
{
public:
	//ctor
	CWorkStation(CBaseState* pOwner, eStateChange eMessage, const char* szHUDMessage);
	virtual ~CWorkStation(void);

	virtual void Update(float fDt) override;

	//-------Accessors--------
	CBaseState* GetOwner(void) const { return m_pOwnerState; }
	eStateChange GetStateMessage(void) const { return m_eMessage; }

private:
	//Who we send our message to
	CBaseState* m_pOwnerState;
	//What message we send
	eStateChange m_eMessage;
	//How often the greeter can speak
	float m_fGreeterDelay = 0.0f;

	//Press E to interact
	// Added to GUIManager, handled there
	CUIElement* m_pHUDElement;
};


#endif