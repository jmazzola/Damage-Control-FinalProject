/***********************************************
* Filename:  		ModBank.h
* Date:      		07/08/2015
* Mod. Date:
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		The modbank is the local repository
*				for storing and applying modules
************************************************/

#ifndef _MODBANK_H_
#define _MODBANK_H_

using namespace std;
#include "../Core/StructureHolder.h"
#include "../Core/EnumerationHolder.h"

class CModSystem;
class CBullet;
class CIWeapon;
struct TTexture2D;

//Stores the local modules
class CModBank
{
public:
	CModBank(unsigned int uRequiredTraits = 0);
	~CModBank() = default;

	void ApplyMods(CBullet* pBullet);
	void ApplyMods(CIWeapon* pOwner);

	//Inserts a mod at the first empty slot
	bool AddMod(unsigned int uModID);
	//Inserts a mod in the designated slot
	bool AddMod(unsigned int uModID, unsigned int uSlot);
	//Switches the mods of two slots
	void SwapSlots(unsigned int uSlotA, unsigned int uSlotB);

	//Resets all modules in the bank
	void Reset(void);

	//Indicates whether incoming module is accepted by this bank
	bool IsModWelcome(unsigned int uModID) const;

	float GetModifiedStat(float fStat, EStatBoostType eType);

	//-----Accessors--------
	unsigned int GetBankSize(void) const { return (unsigned int)(m_vModules.size()); }
	unsigned int GetModCount(void) const;
	unsigned int GetMod(unsigned int uSlot) const { return m_vModules[uSlot]; }
	unsigned int GetRequiredTraits(void) const { return m_uRequiredTraits; }
	bool GetIsChanged(void) const { return m_bChanged; }
	float GetStressValue(void) const;
	//-----Mutators---------
	void SetBankSize(unsigned int uBankSize);
	void SetRequiredTraits(unsigned int uRequiredTraits) { m_uRequiredTraits = uRequiredTraits; }

private:
	//What kind of modules do we take
	unsigned int m_uRequiredTraits = 0;
	//True if we've changed but haven't updated our owner
	bool m_bChanged = false;

	//Stores our modules
	std::vector<unsigned int> m_vModules;
};

#endif