/***********************************************
* Filename:			UIScrapCounter.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#ifndef _CUISCRAPCOUNTER_H_
#define _CUISCRAPCOUNTER_H_

#include "UIElement.h"

class CPlayer;

class CUIScrapCounter :
	public CUIElement
{
	CPlayer* m_pPlayer;

	unsigned int m_unScrapPrev;

	// the current color of the text
	DirectX::XMFLOAT4 m_textColor;
public:
	CUIScrapCounter(CPlayer* pTgt);
	~CUIScrapCounter();

	/*****************************************************************
	* Update: Updates the ui element
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      06/11/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void Update(float dt) override;

	/*****************************************************************
	* Render: renders the ui element
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      06/11/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void Render(void) override;

	/***************
	*  Accessors
	***************/

	/***************
	* Mutators
	***************/

};

#endif // _CUISCRAPCOUNTER_H_

