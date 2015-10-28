/***********************************************
* Filename:			ObjectivePanel.h
* Date:      		07/25/2015
* Mod. Date:
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:			Handles the objective panel
************************************************/

#ifndef _CUOBJECTIVEPANEL_H_
#define _CUOBJECTIVEPANEL_H_

#include "UIElement.h"

class CTexture2D;


class CObjectivePanel : public CUIElement
{
public:


	/*****************************************************************
	* ~CObjectivePanel(): - constructor
	*
	* Ins: int type
	*
	* Outs:void
	*
	* Returns: N/A
	*
	* Mod. Date:		      07/25/2015
	* Mod. Initials:		JM
	*****************************************************************/
	CObjectivePanel();

	/*****************************************************************
	* ~CObjectivePanel(): - Destructor
	*
	* Ins: void
	*
	* Outs:void
	*
	* Returns: N/A
	*
	* Mod. Date:		      07/25/2015
	* Mod. Initials:		JM
	*****************************************************************/
	~CObjectivePanel();

	/*****************************************************************
	* Update(): - overrided update
	*
	* Ins: dt - the ammount of time that has elapsed since the last update
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      07/25/2015
	* Mod. Initials:		JM
	*****************************************************************/
	virtual void Update(float dt) override;

	/*****************************************************************
	* Render(): - overrided render draws a rectangle in the top right zone
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      07/25/2015
	* Mod. Initials:		JM
	*****************************************************************/
	virtual void Render(void) override;

};

#endif //_CUOBJECTIVEPANEL_H_