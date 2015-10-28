/***********************************************
* Filename:  		ModSystem.h
* Date:      		07/08/2015
* Mod. Date:
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This class stores the global info for
*				each mod which is then passed on to the ModBank
************************************************/

#ifndef _MODSYSTEM_H_
#define _MODSYSTEM_H_

using namespace std;
#include "../Core/StructureHolder.h"
#include "../Core/EnumerationHolder.h"

class CBullet;
class CIWeapon;
class CModBank;
struct TTexture2D;


//SINGLETON: Holds the resources for each module
class CModSystem
{
public:

	//Holds the data for each module
	struct TModData
	{
		//The display name of that module
		std::string m_szName;

		//What image file should be loaded for this module
		std::string m_szIconFileName;
		std::string m_szDescFileName;
		//The textures to render for the modules
		TTexture2D* m_pIcon;
		TTexture2D* m_pDescPanel;

		//Numerical values
		int m_nScrapValue;
		//How much harder will the game get thanks to this mod
		float m_fStressValue;
		//Trait levels
		TWeaponSpecials m_tTraitLevels;
		//Stat boosts
		float m_fStatBoosts[EBOOST_COUNT];
	};

	~CModSystem();
	//Disable these guys
	CModSystem(CModSystem& cOther) = delete;
	CModSystem operator=(CModSystem& other) = delete;

	static CModSystem* GetInstance(void);
	//static void DeleteInstance(void);

	//Loads in all modules from a file
	bool LoadModules(const char* szFileName);
	//Adds module to the map
	void AddModule(TModData& tModule, MODULE_TRAITS eModType = EMOD_WEAPON);

	//Checks if entered mod ID is registered
	bool IsIDValid(unsigned int uModID) const;

	//Load the textures for m_umModData
	bool LoadModIcon(unsigned int uModID);
	bool LoadModDescPanel(unsigned int uModID);

	//Adds the traits of applicable mods to the bullet
	void ApplyMods(CModBank* pMods, CBullet* pBullet);

	//void ApplyMods(CModBank* pMods, CBullet* pBullets, int nBulletCount);

	//Adds the traits of applicable mods to the weapon
	void ApplyMods(CModBank* pMods, CIWeapon* pWeapon);

	//Clears everything in m_umModInfo
	void ClearModData(void);
	//Unloads all textures from m_umModInfo, but keeps their filenames, so we can reload them
	void UnloadModData(void);

	//Get the stat boosts out of the modules
	float GetStatBoost(unsigned int uModID, EStatBoostType eType) { return m_umModInfo[uModID].m_fStatBoosts[eType]; }

	//------------Accessors-----------
	const char* GetModName(unsigned int uModID);
	//Get the textures from m_umModData
	TTexture2D* GetModIcon(unsigned int uModID);
	TTexture2D* GetModDescPanel(unsigned int uModID);
	TWeaponSpecials GetModSpecials(unsigned int uModID) { return m_umModInfo[uModID].m_tTraitLevels; }
	unsigned int GetModByName(const char* szModName);
	//Returns the scrap value
	int GetModValue(unsigned int uModID) { return m_umModInfo[uModID].m_nScrapValue; }
	//Returns the stress value
	float GetModStress(unsigned int uModID) { return m_umModInfo[uModID].m_fStressValue; }

private:
	CModSystem() = default;

	//Is the system usable?
	bool m_bLoaded = false;
	//All the texture info for modules
	std::unordered_map<unsigned int, TModData> m_umModInfo;

};


#endif