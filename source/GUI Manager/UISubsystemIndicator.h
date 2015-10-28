/***********************************************
* Filename:			UISubsystemIndicator.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#ifndef _CUISUBSYSTEMINDICATOR_H_
#define _CUISUBSYSTEMINDICATOR_H_

class CSubSystem;
class CTexture2D;
// base type for subsystem
typedef CSubSystem* PSUBSYSTEM;

#include "UIElement.h"

class CUISubsystemIndicator : public CUIElement
{
public:
	/*****************************************************************
	* CUISubsystemIndicator():
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:
	*****************************************************************/
	CUISubsystemIndicator(PSUBSYSTEM target);

	/*****************************************************************
	* ~CUISubsystemIndicator():
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:
	*****************************************************************/
	virtual ~CUISubsystemIndicator();

	/*****************************************************************
	* Update() - updates the subsystem
	*
	* Ins: dt - the ammount of time, in seconds, since the last update
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void Update(float dt) override;

	/*****************************************************************
	* Render() - renders the indicator to the screen
	*
	* Ins: dt - the ammount of time, in seconds, since the last update
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void Render(void) override;

	/***************
	*  Accessors
	***************/

	PSUBSYSTEM GetSubsystem(void) const { return m_pSubsystem; }

	/***************
	* Mutators
	***************/
	void SetMoveLeft(bool move) { m_bMoveLeft = move; m_bSpaceToggled = move; }

private:
	PSUBSYSTEM m_pSubsystem;

	CTexture2D* m_pLightTex;

	// the previous health of the target subsystem
	float m_fHealthPrev;

	float m_fTransTimer;
	float m_fPositionMod;
	bool m_bMoveLeft;
	bool m_bSpaceToggled;
};

#endif // _CUISUBSYSTEMINDICATOR_H_
