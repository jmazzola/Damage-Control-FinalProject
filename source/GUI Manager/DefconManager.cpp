/***********************************************
* Filename:  		GameplayState.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael Mozdzierz
* Purpose:   		Manage the number of enemies in the game
*
************************************************/

#include "DefconManager.h"
#include "../State Machine/GameplayState.h"
#include "../Entity Manager/EntityManager.h"
#include "../Entity Manager/AI/CreepEnemy.h"
#include "../Entity Manager/AI/HunterEnemy.h"
#include "../Entity Manager/AI/SaboteurEnemy.h"

#define DEFCON_1 2
#define DEFCON_2 10
#define DEFCON_3 20
#define DEFCON_4 40
#define DEFCON_5 60

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
CDefconManager::CDefconManager(CGameplayState* pGameplayState)
{
	m_pGameplayState = pGameplayState;
}

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
/*virtual*/ CDefconManager::~CDefconManager(void)
{

}

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
/*virtual*/ void CDefconManager::Update(float dt) /*override*/
{
	CUIElement::Update(dt);

	unsigned int unNumEnemies = 0;
	CEntityManager* pEntities = m_pGameplayState->GetEntityManager();
	unNumEnemies += pEntities->GetAllSaboteur().size();
	unNumEnemies += pEntities->GetAllHunters().size();
	unNumEnemies += pEntities->GetAllCreeps().size();
	
	unsigned int unDefconLevel = 0;
	if (unNumEnemies > 0 && unNumEnemies < DEFCON_1)
		unDefconLevel = 1;
	if (unNumEnemies >= DEFCON_1 && unNumEnemies < DEFCON_2)
		unDefconLevel = 2;
	if (unNumEnemies >= DEFCON_2 && unNumEnemies < DEFCON_3)
		unDefconLevel = 3;
	if (unNumEnemies >= DEFCON_3 && unNumEnemies < DEFCON_4)
		unDefconLevel = 4;
	if (unNumEnemies >= DEFCON_4 && unNumEnemies < DEFCON_5)
		unDefconLevel = 5;

	m_pGameplayState->SetDefconLevel(unDefconLevel);
}

/*****************************************************************
* Render() - Renders the element
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
/*virtual*/ void CDefconManager::Render(void) /*override*/
{
	CUIElement::Render();
}