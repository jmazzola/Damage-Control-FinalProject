#ifndef _LIFESUPPORT_H
#define _LIFESUPPORT_H
/***********************************************
* Filename:			LifeSupport.h
* Date:      		05/22/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:
************************************************/
#include "Subsystem.h"

class CLifeSupport : public CSubSystem
{
public:
	/*****************************************************************
	* CLifeSupport()
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
	CLifeSupport(void);

	/*****************************************************************
	* ~CLifeSupport() - Sets the position of the engines
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
	virtual ~CLifeSupport(void);

	/*****************************************************************
	* SetPosition() - Sets the position of the engines
	*
	* Ins: position - the position to set the engines
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void SetPosition(DirectX::XMFLOAT3& position) override;

	/*****************************************************************
	* Updates() - Updates the subsystem
	*
	* Ins: dt - the ammount of time that has elapsed
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void Update(float dt) override;

	void TakeDamage(float fDamage) override;

	/***************
	*  Accessors
	***************/

	// returns the ammount of health regen for the player
	float GetHealthRegen(void) const { return m_fHealthRegen; }

	/***************
	* Mutators
	***************/

protected:
	virtual bool LoadStats(void) override;

private:
	// the ammount of health regen 
	// the player recieves from the lifesupport
	float m_fHealthRegen = 5.0f;
	float m_fOxygenRegen = 2.5f;
};

#endif