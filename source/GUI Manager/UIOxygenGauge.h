/***********************************************
* Filename:			UIOxygenGauge.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#ifndef _CUIOXYGENGAUGE_H_
#define _CUIOXYGENGAUGE_H_

#include "UIElement.h"

class CPlayer;
class CTexture2D;

class CUIOxygenGauge :
	public CUIElement
{
	float m_fOxygen;

	const float m_fOxygenRotStart = DirectX::XMConvertToRadians(150.f);
	const float m_fOxygenRotEnd = DirectX::XMConvertToRadians(45.f);

	CPlayer* m_pPlayer;

	CTexture2D* m_pBackTexture;

public:
	/*****************************************************************
	* CUIOxygenGauge():
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
	CUIOxygenGauge(CPlayer* pPlayer);

	/*****************************************************************
	* ~CUIOxygenGauge():
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
	virtual ~CUIOxygenGauge();

	/*****************************************************************
	* Update() - Updates the ui element
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      06/04/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void Update(float dt) override;

	/*****************************************************************
	* Render() - Draws the ui element to the screen
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

	/***************
	*  Accessors
	***************/
	

	/***************
	* Mutators
	***************/

};

#endif // _CUIOXYGENGAUGE_H_

