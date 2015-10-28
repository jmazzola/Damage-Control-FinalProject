/***********************************************
* Filename:			UIAmmoCounter.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#ifndef _CUIAMMOCOUNTER_H_
#define _CUIAMMOCOUNTER_H_

#include "UIElement.h"

class CTexture2D;
class CPlayer;

class CUIAmmoCounter :
	public CUIElement
{
	// the ammount of ammo left in the current clip
	unsigned int m_unClip;

	// the ammount of total ammo the player has
	unsigned int m_unAmmo;

	// current weapon texture
	CTexture2D* m_pCurrWeaponTexture;

	CPlayer* m_pPlayer;

	CTexture2D* m_pDroneBuddyTex;
	CRectangle m_droneBuddyRect;

	DirectX::XMFLOAT4 m_vDroneTextColor;
	float m_ftimer;

	struct
	{
		CTexture2D* m_pAssultRifle;
		CTexture2D* m_pShotgun;
		CTexture2D* m_pSemiAuto;

		CRectangle m_assultRifleRect;
		CRectangle m_shotGunRect;
		CRectangle m_semiAutoRect;
	} weapons;

public:
	/*****************************************************************
	* CUIAmmoCounter():
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
	CUIAmmoCounter(CPlayer* pPlayer);

	/*****************************************************************
	* CUIAmmoCounter():
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      06/03/2015
	* Mod. Initials:
	*****************************************************************/
	~CUIAmmoCounter();

	/*****************************************************************
	* Update(): updates the Ammo counter
	*
	* Ins: dt - ammount of time since the last update
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      06/03/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void Update(float fDt) override;


	/*****************************************************************
	* Render(): renders the element to the screen
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

	float AbsPulse(float time);

	/***************
	* Mutators
	***************/

	void SetTotalAmmo(unsigned int value) { m_unAmmo = value; }

	void SetCurrentClip(unsigned int value) { m_unClip = value; }

};

#endif // _CUIAMMOCOUNTER_H_

