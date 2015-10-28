#ifndef _MELTDOWNCOUNTER_H
#define _MELTDOWNCOUNTER_H

#include "UIElement.h"

class CReactor;

class CMeltdownCounter : public CUIElement
{
	CReactor* m_pReactor;

	float m_fTextSize = 1.f;

public:

	/*****************************************************************
	* CMeltdownCounter()
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
	CMeltdownCounter(CReactor* pReactor);

	/*****************************************************************
	* ~CMeltdownCounter()
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
	virtual ~CMeltdownCounter(void);

	/*****************************************************************
	* Update() - Updates the meltdown counter
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
	virtual void Update(float fDt) override;

	/*****************************************************************
	* Render() - renders the ui element
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
	virtual void Render(void) override;
};

#endif