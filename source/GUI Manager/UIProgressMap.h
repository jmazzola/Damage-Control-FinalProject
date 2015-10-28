/***********************************************
* Filename:			UIProgressMap.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#ifndef _CUIPROGRESSMAP_H_
#define _CUIPROGRESSMAP_H_

#include "UIElement.h"

class CTexture2D;
class CEngines;

class CUIProgressMap : public CUIElement
{
	bool m_bMoveRight;
	bool m_bSpaceToggled;
	const float m_fShipStart;
	const float m_fShipEnd;

	float m_fShipPositionX;
	float m_fShipPositionY;
	float m_fProgress;
	float m_fTransTimer;
	float m_fPositionMod;

	CTexture2D* m_pIconTexture;
	CTexture2D* m_pShipDeadTexture;
	CTexture2D* m_pCurrentShipIcon;
	CTexture2D* m_pShipFire;

	CEngines* m_pEngines;

public:
	/*****************************************************************
	* CUIProgressMap():
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:
	*****************************************************************/
	CUIProgressMap(CEngines* target);

	/*****************************************************************
	* CUIProgressMap():
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:
	*****************************************************************/
	~CUIProgressMap();

	/*****************************************************************
	* Update(): - updates the current progress of the ship
	*
	* Ins: dt - the ammount of time that has elapsed since the last update
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/19/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void Update(float dt) override;

	/*****************************************************************
	* Render(): - renders the progress map to the screen
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/19/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void Render(void) override;

	/***************
	*  Accessors
	***************/


	/***************
	* Mutators
	***************/

	void SetProgress(float value) { m_fProgress = value; }


};

#endif // _CUIPROGRESSMAP_H_
