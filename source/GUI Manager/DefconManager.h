/***********************************************
* Filename:  		GameplayState.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael Mozdzierz
* Purpose:   		Manage the number of enemies in the game
*					
************************************************/

#ifndef _DEFCONMANAGER_H
#define _DEFCONMANAGER_H

#include "UIElement.h"

class CGameplayState;

class CDefconManager : public CUIElement
{
	CGameplayState* m_pGameplayState;
public:

	/*****************************************************************
	* DefconManager()
	*
	* Ins:			  void
	*
	* Outs:		      void
	*
	* Returns:		  void
	*
	* Mod. Date:		      06/09/2015
	* Mod. Initials:	      
	*****************************************************************/
	CDefconManager(CGameplayState* pGameplayState);

	/*****************************************************************
	* ~DefconManager
	*
	* Ins:			  void
	*
	* Outs:		      void
	*
	* Returns:		  void
	*
	* Mod. Date:		      06/09/2015
	* Mod. Initials:	       
	*****************************************************************/
	virtual ~CDefconManager(void);

	/*****************************************************************
	* Update()
	*
	* Ins:			  dt - the ammount of time elapsed since the last
	*					   frame
	*
	* Outs:		      void
	*
	* Returns:		  void
	*
	* Mod. Date:		      06/09/2015
	* Mod. Initials:	       
	*****************************************************************/
	virtual void Update(float dt) override;

	/*****************************************************************
	* Render() - Renders the function
	*
	* Ins:			  void
	*
	* Outs:		      void
	*
	* Returns:		  void
	*
	* Mod. Date:		      06/09/2015
	* Mod. Initials:	      
	*****************************************************************/
	virtual void Render(void) override;
};

#endif