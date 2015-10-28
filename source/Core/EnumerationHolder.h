/***********************************************
* Filename:			EnumerationHolder.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:			Header to store all enumerations and access them in an easy/clean way.
************************************************/

#ifndef _ENUMERATION_HOLDER_H_
#define _ENUMERATION_HOLDER_H_

/*****************************************************************
* eEntityTypes: Enumeration that defines the entity types.
*
* Mod. Date:		05/13/2015
* Mod. Initials:	BL
*****************************************************************/
enum eEntityTypes { ENT_ENEMY = 0, ENT_ENVIRO, ENT_SUBSYSTEM, ENT_BULLET, ENT_NUMBER };

/*****************************************************************
* eEntityTypes: Enumeration that defines the enemy types.
*
* Mod. Date:		06/07/2015
* Mod. Initials:	JM
*****************************************************************/
enum eEnemyType { ENEM_CREE = 0, ENEM_HUNT, ENEM_SABO };

/*****************************************************************
* eEnemyVariant: Enumeration that defines the enemy's different variants.
*
* Mod. Date:		06/10/2015
* Mod. Initials:	JM
*****************************************************************/
enum eEnemyVariant
{
	eBronze = 0, eSilver, eGold, eVariantCount, eNoVariant = eVariantCount
};

enum MODULE_TRAITS : unsigned int
{
	EMOD_NULL = 0,
	//What the mod effects
	EMOD_WEAPON = 0x80000000, EMOD_SUIT = 0x40000000, EMOD_SUBSYSTEM = 0x20000000, EMOD_GADGET = 0x10000000,
	//How the mod affects it
	//Weapons
	//Add special properties
	EWEAPON_BOUNCE = 0x1,//Unused
	EWEAPON_CHAIN = 0x2,//Unused
	EWEAPON_PIERCE = 0x4,//Bullets go through their mark and hit the people behind them
	EWEAPON_EXPLODE = 0x8,//Unused
	EWEAPON_FREEZE = 0x10,//Unused
	EWEAPON_POISON = 0x20,//Unused
	EWEAPON_SLOW = 0x40,//Bullets apply a status effect that slows the movement of enemies
	EWEAPON_FUCKYOU = 0x80,//Shh... It's a secret
	//Increase stats
	EWEAPON_DAMAGE = 0x100,//Increases weapon damage
	EWEAPON_FIRERATE = 0x200,//Increases weapon fire rate
	EWEAPON_RELOADSPEED = 0x400,//Increases weapon reload speed
	EWEAPON_MAGSIZE = 0x800,//Increases weapon magazine size
	//(Where these go if we put them in)
	//Suit
	//Sub-System
	//Gadget
	EGADGET_DURATION = 0x400,
	EGADGET_COOLDOWN = 0x800,
	EGADGET_EXP_RADIUS = 0x1000,
	EGADGET_EXP_DAMAGE = 0x2000,
	EGADGET_REPAIR = 0x4000,
	//Arbitrary Version numbers (in case we have a Mk II with the same abillities, but better versions)
	EMOD_VER0 = 0x10000, EMOD_VER1 = 0x20000, EMOD_VER2 = 0x40000, EMOD_VER3 = 0x80000,
	EMOD_VER4 = 0x100000, EMOD_VER5 = 0x200000, EMOD_VER6 = 0x400000, EMOD_VER7 = 0x800000,
};

enum EStatBoostType {
	//Weapon stats
	EBOOST_DAMAGE = 0, 
	EBOOST_FIRERATE = 1,
	EBOOST_RELOADSPEED = 2,
	EBOOST_MAGSIZE = 3,

	//Gadget stats
	EBOOST_EXP_DAMAGE = 0,
	EBOOST_EXP_RADIUS = 1,
	EBOOST_COOLDOWN = 2,
	EBOOST_DURATION = 3,
	EBOOST_REPAIR = 4,

	//Be careful not to mess this one up
	EBOOST_COUNT = 5
};
	
enum EMapInfo {
	EMAP_NONE = 0,
	EMAP_RENDER = 0x1,
	EMAP_SCENERY = 0x2,
	EMAP_COLLISION = 0x4,
	EMAP_LIGHTS = 0x8,
	EMAP_INANIMATE = 0x10,
	EMAP_ALL = EMAP_RENDER | EMAP_SCENERY | EMAP_COLLISION | EMAP_LIGHTS | EMAP_INANIMATE
};

#endif //_ENUMERATION_HOLDER_H_