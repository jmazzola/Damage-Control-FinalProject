/***********************************************
* Filename:  		UpgradeState.h
* Date:      		06/04/2015
* Mod. Date: 		
* Mod. Initials:	
* Author:    		Michael J. Gergar
* Purpose:   		This is the state used for
*			 upgrading the player's weapons
************************************************/

#ifndef _UPGRADESTATE_H_
#define _UPGRADESTATE_H_

#include "BaseState.h"
using namespace std;

class CUIElement;
class CUIButton;
class CPlayer;
class CModBank;
class CIWeapon;
class CUIScrapCounter;

class CUpgradeState : public CBaseState
{
public:
	/*****************************************************************
	* Default Constructor
	*****************************************************************/
	CUpgradeState();

	/*****************************************************************
	* Disabled Copy Constructor
	*****************************************************************/
	CUpgradeState(const CUpgradeState& pOther) = delete;

	/*****************************************************************
	* Disabled Operator =
	*****************************************************************/
	CUpgradeState& operator= (const CUpgradeState& pOther) = delete;

	/*****************************************************************
	* Default Destructor
	*****************************************************************/
	~CUpgradeState(void);

	/*****************************************************************
	* Input(): Called every frame, processes any state specific input
	*
	* Ins:			  void
	*
	* Outs:		      void
	*
	* Returns:		  void
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	void Input(void);

	/*****************************************************************
	* Update(): Called every frame, updates anything inside that
	*                needs it
	*
	* Ins:			  float fDT
	*
	* Outs:		      void
	*
	* Returns:		  void
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	void Update(float fDT);

	/*****************************************************************
	* Render(): Called every frame, renders everything in the state
	*                that needs it to the back buffer
	*
	* Ins:			  void
	*
	* Outs:		      void
	*
	* Returns:		  void
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	void Render(CAssetManager* pRenderer) override;

	/*****************************************************************
	* Enter(): Called when switching to a state, initializes members
	*                and allocates memory
	*
	* Ins:			  void
	*
	* Outs:		      void
	*
	* Returns:		  bool, true if successfully entered
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	bool FirstEnter(void);

	bool ReEnter(void);

	/*****************************************************************
	* HandlePrevState(): Called before entering this state, stores/deletes
	*					state based on the types
	*
	* Ins:			  CBaseState* cPrevState, CAssetManager* pAssets
	*
	* Outs:		      void
	*
	* Returns:		  void
	*
	* Mod. Date:		      05/23/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	void HandlePrevState(CBaseState* pPrevState);

	/*****************************************************************
	* Load(): Called before switching to a state to ensure all
	*				assets are loaded
	*
	* Ins:			  CAssetManager* pAssets
	*
	* Outs:		      void
	*
	* Returns:		  bool, true if all assets are loaded
	*
	* Mod. Date:		      05/13/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	bool Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands = nullptr);

	/*****************************************************************
	* UnLoad(): Called when you no longer need this state's assets
	*
	* Ins:			  CAssetManager* pAssets
	*
	* Outs:		      void
	*
	* Returns:		  bool, true if all assets are unloaded
	*
	* Mod. Date:		      05/13/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	void UnLoad(eStateLoadCommands* pLoadCommands = nullptr);

	/*****************************************************************
	* Exit(): Called when you switch states and don't need to preserve
	*				this one
	*
	* Ins:			  void
	*
	* Outs:		      void
	*
	* Returns:		  void
	*
	* Mod. Date:		      05/13/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	bool Exit(void);

	/*****************************************************************
	* CheckState(): Called at the end of each StateMachine's Update
	*				to see if the state should switch to another state
	*				returns the state that it switches to, returns
	*				nullptr if application is closing, this if
	*				no change
	*
	* Ins:			  void
	*
	* Outs:		      void
	*
	* Returns:		  CBaseState*
	*
	* Mod. Date:		      05/13/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	eStateType CheckState(void);

	//---------Accessors--------------
	bool GetSelectionChange(void) const { return m_pPrevSelection != m_pSelection; }

	//---------Mutators---------------
	void SetPlayer(CPlayer* pPlayer);
private:
	//What tab is the shop on
	enum EShopTab{
		EGUN_TAB = 0, EGADGET_TAB,
		ETAB_COUNT
	};
	//Which of the three slots are we sing
	enum EEquipmentSlot{
		//Guns
		EPISTOL_SLOT = 0,
		EMACHGUN_SLOT = 1,
		ESHOTGUN_SLOT = 2,
		//Gadgets
		EDRONE_SLOT = 0,
		EGADGET2_SLOT = 1,
		EGADGET3_SLOT = 2,
		//How many total slots
		EMAX_SLOTS = 3
	};


	//Updates all icons to represent the mod in that spot
	void RefreshIcons(void);
	//Finds the next selection if anything
	//	Updates the Desc Panel if anything changes
	void RefreshSelection(void);
	//Configure the buttons for selling, buying, or neither
	void MerchantUpdate(void);
	//Tries to swap/drop your selected item in the designated spot
	bool AttemptDrop(void);
	//Buy or sell the selected item
	bool AttemptTransaction(void);
	//Shows/hides locks based on what weapons the player has unlocked
	void RefreshLocks(void);
	//Refreshes guns' stats
	void RefreshGuns(void);

	//Mod repositories
	//CModBank* m_pCargo = nullptr;
	CModBank* m_pStores[ETAB_COUNT];
	EShopTab m_eCurrTab = EGUN_TAB;

	//Store the player for convenience
	CPlayer* m_pPlayer = nullptr;
	//Weapons for convenience
	CIWeapon* m_pWeapons[EMAX_SLOTS];
	CIWeapon* m_pGadgets[EMAX_SLOTS];

	//GUI Backgrounds
	CUIElement* m_pMainPanels[ETAB_COUNT];
	CUIButton*	m_pTabs[ETAB_COUNT];
	CUIElement* m_pCargoPanel = nullptr;
	CUIElement* m_pShopPanel = nullptr;

	//GUI Buttons
	CUIButton* m_pBuy = nullptr;
	CUIButton* m_pSell = nullptr;
	CUIButton* m_pBack = nullptr;

	//Weapons' plates
	CUIButton* m_pEquipmentIcons[EMAX_SLOTS];
	//CUIButton* m_pPistolIcon = nullptr;
	//CUIButton* m_pShotgunIcon = nullptr;
	//CUIButton* m_pMachGunIcon = nullptr;
	CUIElement* m_pEquipmentLocks[EMAX_SLOTS];

	//The description of the selected thing
	CUIElement* m_pDescPanel = nullptr;
	//Currently selected element
	CUIElement* m_pSelection = nullptr;
	CUIElement* m_pPrevSelection = nullptr;
	unsigned int m_uSelectedID = 0;
	size_t m_uSelectedIndex = 0;
	CModBank* m_pSelectedBank = nullptr;

	//Are we selecting an item from the store
	bool m_bStoreItem = false;
	//Whether the player clicked to select something or just moused over
	bool m_bHardSelect = false;
	//Whether or not you're dragging
	bool m_bDragging = false;

	//How long to alert the user their credit card was declined
	float m_fCardDeclinedTimer = 0;
	//How long to alert the player to the shop's existence
	float m_fShopHereTimer = 0;

	//Used to display the module we're dragging
	CUIElement* m_pDragger = nullptr;
	//Element that renders over the selected module
	CUIElement* m_pIndicator = nullptr;

	//Module's visual representation
	std::vector<CUIButton*> m_vEquipmentModSlots[EMAX_SLOTS];
	//std::vector<CUIButton*> m_vPistolIcons;
	//std::vector<CUIButton*> m_vMachGunIcons;
	//std::vector<CUIButton*> m_vShotgunIcons;
	//std::vector<CUIButton*> m_vCargoIcons;
	std::vector<CUIButton*> m_vShopIcons;
};


#endif