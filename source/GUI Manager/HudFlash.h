/***********************************************
* Filename:			HealthBar.h
* Date:      		06/06/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:
************************************************/

#ifndef _HUDFLASH_H
#define _HUDFLASH_H

#include "UIElement.h"

class CPlayer;

class CHudFlash : public CUIElement
{
	// the value for the current timer
	float m_fCurrentTime;

	// the total ammount of time the flash is going to spend on screan
	const float m_fMaxTime = .03f;

	// the ammount of health the player previously had
	float m_fHealthPrev;
	// the ammount of oxygen the player previously had
	float m_fOxygenPrev;
	
	float m_fHealthTimer;

	float m_fOxyTimer;

	CPlayer* m_pPlayer = nullptr;


public:
	/*****************************************************************
	* HudFlash()
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		     06/06/2015
	* Mod. Initials:
	*****************************************************************/
	CHudFlash(CPlayer* pPlayer);
	/*****************************************************************
	* ~HudFlash()
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		     06/06/2015
	* Mod. Initials:
	*****************************************************************/
	virtual ~CHudFlash(void);

	/*****************************************************************
	* Update - updates the ui elemment
	*
	* Ins: dt - The expance of temproal space traversed since the
	*			previous computer cycle has transpired
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		     06/06/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void Update(float dt) override;

	/*****************************************************************
	* Render: renders the hud element to the screen
	*
	* Ins: 
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		     06/06/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void Render(void) override;
};

#endif