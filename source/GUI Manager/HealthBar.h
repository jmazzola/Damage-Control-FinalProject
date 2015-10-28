/***********************************************
* Filename:			HealthBar.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#ifndef _CHEALTHBAR_H_
#define _CHEALTHBAR_H_

#include "UIElement.h"
#include "../Renderer/Texture2D.h"

class CLivingEntities;

class CHealthBar : public CUIElement
{
public:
	/*****************************************************************
	* CHealthBar():
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
	CHealthBar(float maxHealth);

	/*****************************************************************
	* ~CHealthBar():
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
	virtual ~CHealthBar();


	/*****************************************************************
	* Update() - updates the health bar
	*
	* Ins: dt - the ammount of time that has elapsed since the last update
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
	* Update() - updates the health bar
	*
	* Ins: dt - the ammount of time that has elapsed since the last update
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

	float GetHealth(void) const { return m_fHealth; }

	/***************
	* Mutators
	***************/

	void SetTarget(CLivingEntities* value) { m_pTgt = value; }

	void SetHealth(float value) { m_fHealth = value; }

private:
	const float m_fMaxHealth;
	float m_fHealth;
	float m_fColorTimer;

	bool m_bColorChange;

	CLivingEntities* m_pTgt;

	CTexture2D* m_pOverlayTexture;


};

#endif // _CHEALTHBAR_H_

