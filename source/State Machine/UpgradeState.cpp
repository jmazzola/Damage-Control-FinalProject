/***********************************************
* Filename:  		UpgradeState.cpp
* Date:      		06/04/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael J. Gergar
* Purpose:   		This is the state used for
*			 upgrading the player's weapons
************************************************/

#include "UpgradeState.h"
#include "GameplayState.h"
#include "../Entity Manager/EntityManager.h"

#include "../GUI Manager/GUIManager.h"
#include "../GUI Manager/UIElement.h"
#include "../GUI Manager/UIButton.h"

#include "../Input Manager/InputManager.h"

#include "../Entity Manager/Entities/Player.h"
#include "../Weapon/IWeapon.h"
#include "../Weapon/ShotGun.h"
#include "../Weapon/DroneBuddy.h"

#include "../Audio Manager/AudioSystemWwise.h"

#include "../Renderer/BitmapFont.h"

#include "../Weapon/ModBank.h"
#include "../Weapon/ModSystem.h"

#define SHOP_CAPACITY 12
#define DRAGGER_COLOR DirectX::XMFLOAT4(0.9f, 0.9f, 0.9f, 0.4f)
#define FONT_COLOR_SCRAP	DirectX::XMFLOAT4(1, 1, 1, 1)
#define FONT_COLOR_CANT_AFFORD DirectX::XMFLOAT4(1, 0, 0, 1)
#define FONT_COLOR_PRICE	DirectX::XMFLOAT4(1, 1, 1, 1)

#define INDICATOR_BLINK_SPEED 1.8f
#define INDICATOR_MIN_ALPHA	0.1f
#define INDICATOR_HARD_MIN_ALPHA 0.5f
#define INDICATOR_MAX_ALPHA 0.95f

#define SCRAP_Y				0.64f
#define ONE_DIGIT_SCRAP_X	0.38f
#define TWO_DIGIT_SCRAP_X	0.35f
#define THREE_DIGIT_SCRAP_X 0.32f
#define FOUR_DIGIT_SCRAP_X	0.29f
#define FIVE_DIGIT_SCRAP_X	0.26f

#define ONE_DIGIT_PRICE_X	0.82f
#define TWO_DIGIT_PRICE_X	0.79f
#define THREE_DIGIT_PRICE_X	0.76f
#define FOUR_DIGIT_PRICE_X	0.73f

#define PISTOL_ID	EMOD_WEAPON
#define SHOTGUN_ID	(EMOD_WEAPON | EMOD_VER1)
#define MACHGUN_ID	(EMOD_WEAPON | EMOD_VER0)

#define DRONE_ID EMOD_GADGET

#define ATTACK_COORDS		0.02f, -0.42f
#define FIRERATE_COORDS		0.24f, -0.42f
#define CLIPSIZE_COORDS		0.02f, -0.64f
#define RELOADCOST_COORDS	0.24f, -0.64f

#define CARD_DECLINE_TIME	0.8f
#define SHOP_HERE_TIME		0.8f
#define SHOP_HERE_MIN		0.2f
#define SHOP_HERE_MAX		1.0f
#define SHOP_HERE_SPEED		4.2f
//How many mods can one weapons upport
#define MAX_BANK_SIZE 5

/*****************************************************************
* Default Constructor
*****************************************************************/
CUpgradeState::CUpgradeState() : CBaseState(eStateType::STATE_UPGRADE)
{
	/*
	m_pPlayer = pPlayer;
	//Get the banks for them
	m_pCargo = pPlayer->GetCargo();
	m_pWeapons[EPISTOL_SLOT] = pPlayer->GetPistol();
	m_pWeapons[ESHOTGUN_SLOT] = pPlayer->GetShotgun();
	m_pWeapons[EMACHGUN_SLOT] = pPlayer->GetMachineGun();
	*/
	for (size_t i = 0; i < EMAX_SLOTS; i++)
	{
		m_pEquipmentLocks[i] = nullptr;
		m_pEquipmentIcons[i] = nullptr;
		m_pWeapons[i] = nullptr;
		m_pGadgets[i] = nullptr;
	}
	for (int i = 0; i < ETAB_COUNT; i++)
	{
		m_pStores[i] = nullptr;
		m_pTabs[i] = nullptr;
		m_pMainPanels[i] = nullptr;
	}
	m_bTransparent = true;
}

/*****************************************************************
* Default Destructor
*****************************************************************/
CUpgradeState::~CUpgradeState(void)
{
}

void CUpgradeState::SetPlayer(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	//Get the banks for them
	if (m_pPlayer)
	{
		//m_pCargo = pPlayer->GetCargo();
		m_pWeapons[EPISTOL_SLOT] = pPlayer->GetPistol();
		m_pWeapons[ESHOTGUN_SLOT] = pPlayer->GetShotgun();
		m_pWeapons[EMACHGUN_SLOT] = pPlayer->GetMachineGun();
		m_pGadgets[EDRONE_SLOT] = m_pPlayer->GetDrone();
		RefreshLocks();
	}
}

void CUpgradeState::Input(void)
{
	//TODO: Menu Controls

	//Leave the controls state
	if (CInputManager::GetInstance( )->IsKeyPressed(eControls::Backspace) ||
		CInputManager::GetInstance( )->IsKeyPressed(eControls::F2) ||
		CInputManager::GetInstance( )->IsKeyPressed(eControls::ESCAPE) ||
		CInputManager::GetInstance()->IsKeyPressed(eControls::Tab) ||
		m_pBack->GetPressed())
	{
		QueueStateChange(eStateChange::STATE_CH_BACK);
	}
	else
	{
		for (size_t i = 0; i < ETAB_COUNT; i++)
		{
			if (m_pTabs[i] && m_pTabs[i]->GetPressed() && i != m_eCurrTab)
			{
				m_eCurrTab = (EShopTab)i;
				m_pSelection = nullptr;
				m_bHardSelect = false;
				m_bStoreItem = false;
				m_uSelectedID = 0;
				m_uSelectedIndex = 0;
				m_pSelectedBank = nullptr;
				RefreshIcons();
				RefreshLocks();
				m_pDescPanel->SetTexture(CModSystem::GetInstance()->GetModDescPanel(0));
				m_pDragger->SetTexture(nullptr);
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_SELECT);
				break;
			}
		}
	}
}

void CUpgradeState::Update(float fDT)
{
	//Without a player this is useless
	if (m_pPlayer == nullptr)
	{
		m_bToPop = true;
		return;
	}
	if (m_pPrevState)
	{
		//Needed to add entities to renderer
		m_pPrevState->Update(0);
	}
	if (m_pGUI)
	{
		m_pGUI->Update(fDT);
	}

	RefreshSelection();

	//Show the indicator
	if (m_pSelection)
	{
		m_pIndicator->SetVisible(true);
		m_pIndicator->SetHeight(m_pSelection->GetBounds().ComputeHeight() + 5);
		m_pIndicator->SetWidth(m_pSelection->GetBounds().ComputeWidth() + 5);
		m_pIndicator->SetPosition(m_pSelection->GetPosition());
		//Blink the indicator
		static bool bWaning;
		static float fAlpha;
		if (bWaning)
		{
			fAlpha -= fDT * INDICATOR_BLINK_SPEED;
			float fMin = INDICATOR_MIN_ALPHA;
			if (m_bHardSelect)
				fMin = INDICATOR_HARD_MIN_ALPHA;
			if (fAlpha <= fMin)
			{
				fAlpha = fMin;
				bWaning = false;
			}
		}
		else
		{
			fAlpha += fDT * INDICATOR_BLINK_SPEED;
			if (fAlpha >= INDICATOR_MAX_ALPHA)
			{
				fAlpha = INDICATOR_MAX_ALPHA;
				bWaning = true;
			}
		}
		m_pIndicator->SetColor(DirectX::XMFLOAT4(1, 1, 1, fAlpha));

	}
	else
	{
		m_pIndicator->SetVisible(false);
	}
	//Point out the user's current scrap
	if (m_fCardDeclinedTimer)
	{
		m_fCardDeclinedTimer -= fDT;
		if (m_fCardDeclinedTimer < 0)
		{
			m_fCardDeclinedTimer = 0;
		}
	}
	//Point out the shop to a confused player
	if (m_fShopHereTimer > 0)
	{
		static float fBlue;
		static bool bWaning;

		m_fShopHereTimer -= fDT;
		if (m_fShopHereTimer <= 0)
		{
			fBlue = SHOP_HERE_MAX;
			bWaning = true;
			m_fShopHereTimer = 0;
			for (size_t i = 0; i < m_vShopIcons.size(); i++)
			{
				m_vShopIcons[i]->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
			}
		}
		else
		{
			if (bWaning)
			{
				fBlue -= fDT * SHOP_HERE_SPEED;
			}
			else
			{
				fBlue += fDT * SHOP_HERE_SPEED;
			}
			DirectX::XMFLOAT4 tempColor(1, 1, fBlue, 1);
			for (size_t i = 0; i < m_vShopIcons.size(); i++)
			{
				m_vShopIcons[i]->SetColor(tempColor);
			}

			if (fBlue <= SHOP_HERE_MIN)
			{
				bWaning = false;
			}
			else if (fBlue >= SHOP_HERE_MAX)
			{
				bWaning = true;
			}
		}
	}

	MerchantUpdate();


}

void CUpgradeState::Render(CAssetManager* pRenderer)
{
	if (m_pPrevState)
	{
		m_pPrevState->Render(pRenderer);
	}
	if (m_pGUI)
	{
		m_pGUI->Render( );
	}
	//Render the player's scrap
	if (m_pPlayer)
	{
		int nScrap = m_pPlayer->GetScrapCount();
		float fScrapX = FIVE_DIGIT_SCRAP_X;
		if (nScrap < 10)
		{
			fScrapX = ONE_DIGIT_SCRAP_X;
		}
		else if (nScrap < 100)
		{
			fScrapX = TWO_DIGIT_SCRAP_X;
		}
		else if (nScrap < 1000)
		{
			fScrapX = THREE_DIGIT_SCRAP_X;
		}
		else if (nScrap < 10000)
		{
			fScrapX = FOUR_DIGIT_SCRAP_X;
		}
		DirectX::XMFLOAT4 xmColor = FONT_COLOR_SCRAP;
		if (m_fCardDeclinedTimer)
		{
			xmColor = FONT_COLOR_CANT_AFFORD;
		}
		CBitmapFont::GetInstance()->DrawString(std::string("$" + std::to_string(m_pPlayer->GetScrapCount())).c_str(), fScrapX, SCRAP_Y, xmColor);
	}
	//Render the sell price
	{
		int nValue = 0;
		float fScrapX = FOUR_DIGIT_PRICE_X;
		if (m_pSelection && !m_bStoreItem)
		{
			nValue = CModSystem::GetInstance()->GetModValue(m_uSelectedID);
		}
		if (nValue < 10)
		{
			fScrapX = ONE_DIGIT_PRICE_X;
		}
		else if (nValue < 100)
		{
			fScrapX = TWO_DIGIT_PRICE_X;
		}
		else if (nValue < 1000)
		{
			fScrapX = THREE_DIGIT_PRICE_X;
		}
		CBitmapFont::GetInstance()->DrawString(std::string("$" + std::to_string(nValue)).c_str(), fScrapX, -0.63f, FONT_COLOR_PRICE);
	}
	//Render the purchase price
	{
		int nValue = 0;
		float fScrapX = FOUR_DIGIT_PRICE_X;
		if (m_pSelection && m_bStoreItem)
		{
			nValue = CModSystem::GetInstance()->GetModValue(m_uSelectedID);
		}
		if (nValue < 10)
		{
			fScrapX = ONE_DIGIT_PRICE_X;
		}
		else if (nValue < 100)
		{
			fScrapX = TWO_DIGIT_PRICE_X;
		}
		else if (nValue < 1000)
		{
			fScrapX = THREE_DIGIT_PRICE_X;
		}
		CBitmapFont::GetInstance()->DrawString(std::string("$" + std::to_string(nValue)).c_str(), fScrapX, -0.45f, FONT_COLOR_PRICE);
	}
	//Render Weapon Stats
	if (m_uSelectedID != 0 && m_pSelectedBank == nullptr)
	{
		if (m_eCurrTab == EGUN_TAB)
		{
			//What weapon is selected
			CIWeapon* pWeapon = nullptr;
			if (m_uSelectedID == PISTOL_ID)
			{
				pWeapon = m_pWeapons[EPISTOL_SLOT];
			}
			else if (m_uSelectedID == SHOTGUN_ID)
			{
				pWeapon = m_pWeapons[ESHOTGUN_SLOT];
			}
			else if (m_uSelectedID == MACHGUN_ID)
			{
				pWeapon = m_pWeapons[EMACHGUN_SLOT];
			}
			//Print their stats
			//Attack
			if (m_uSelectedID == SHOTGUN_ID)
			{
				CBitmapFont::GetInstance()->DrawString(std::string(std::to_string((int)pWeapon->GetDamage()) + "x" + std::to_string(((CShotGun*)pWeapon)->GetPelletCount())).c_str(), ATTACK_COORDS, FONT_COLOR_PRICE);
			}
			else
			{
				CBitmapFont::GetInstance()->DrawString(std::to_string((int)pWeapon->GetDamage()).c_str(), ATTACK_COORDS, FONT_COLOR_PRICE);
			}
			//Fire Rate
			{
				//I don't need no floor()
				std::string temp = std::to_string(pWeapon->GetFireRate());
				if (pWeapon->GetFireRate() >= 10)
				{
					temp[4] = '\0';
				}
				else
				{
					temp[3] = '\0';
				}
				CBitmapFont::GetInstance()->DrawString(temp.c_str(), FIRERATE_COORDS, FONT_COLOR_PRICE);
			}

			//Clip Size
			CBitmapFont::GetInstance()->DrawString(std::to_string(pWeapon->GetMaxAmmo()).c_str(), CLIPSIZE_COORDS, FONT_COLOR_PRICE);

			//Reload Time
			{
				std::string temp = std::to_string(pWeapon->GetReloadTime() / pWeapon->GetReloadSpeed());
				temp[3] = '\0';
				CBitmapFont::GetInstance()->DrawString(temp.c_str(), RELOADCOST_COORDS, FONT_COLOR_PRICE);
			}
		}
		else if (EGADGET_TAB == m_eCurrTab)
		{
			//What weapon is selected
			CDroneBuddy* pDrone = m_pPlayer->GetDroneBuddy();

			//Print their stats
			//Attack
			{
				CBitmapFont::GetInstance()->DrawString(std::to_string((int)pDrone->GetDamage()).c_str(), ATTACK_COORDS, FONT_COLOR_PRICE);
			}
			//Fire Rate
			{
				//I don't need no floor()
				std::string temp = std::to_string(pDrone->GetFireRate());
				if (pDrone->GetFireRate() >= 10)
				{
					temp[4] = '\0';
				}
				else
				{
					temp[3] = '\0';
				}
				CBitmapFont::GetInstance()->DrawString(temp.c_str(), FIRERATE_COORDS, FONT_COLOR_PRICE);
			}
			//Cooldown
			{
				std::string temp = std::to_string(pDrone->GetCoolDownTime());
				temp[4] = '\0';
				CBitmapFont::GetInstance()->DrawString(temp.c_str(), CLIPSIZE_COORDS, FONT_COLOR_PRICE);
			}
			//Life Time
			{
				std::string temp = std::to_string(pDrone->GetLifeTime());
				temp[4] = '\0';
				CBitmapFont::GetInstance()->DrawString(temp.c_str(), RELOADCOST_COORDS, FONT_COLOR_PRICE);

			}
		}
	}

	DrawNavigationInfo();
}

bool CUpgradeState::FirstEnter(void)
{
	CBaseState::FirstEnter();

	m_pSelection = nullptr;
	m_uSelectedID = 0;
	m_uSelectedIndex = 0;
	m_pSelectedBank = nullptr;
	m_bDragging = m_bHardSelect = false;
	m_pIndicator->SetVisible(false);
	m_pDragger->SetVisible(false);
	m_fCardDeclinedTimer = 0;
	m_fShopHereTimer = 0;
	m_eCurrTab = EGUN_TAB;

	RefreshLocks();
	RefreshIcons();
	
	AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_2D_MENU_PURCHASE);
	AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_SHOP_MUSIC_LOOP);

	return true;
}

bool CUpgradeState::ReEnter(void)
{
	return CBaseState::ReEnter();
}

void CUpgradeState::HandlePrevState(CBaseState* pPrevState)
{
	if (pPrevState)
	{
		//Make sure the gameplay doesn't get in the way
		if (pPrevState->GetType( ) == eStateType::STATE_GAMEPLAY)
		{
			SetPlayer(((CGameplayState*)pPrevState)->GetEntityManager()->GetPlayer());
			AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PAUSE_ALL);
		}
		CBaseState::HandlePrevState(pPrevState);
	}
}

bool CUpgradeState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{
	if (GetLoaded( ))
		return true;
	m_bLoaded = !(m_pPlayer == nullptr);
	if (!m_bLoaded)
	{
		return false;
	}
	CBaseState::Load(pAssets, pLoadCommands);
	m_pGUI = new CGUIManager();

	CUIElement* _Vendor = new CUIElement();
	_Vendor->LoadTexture(L"Assets\\Images\\Menus\\UpgradeShop.png");
	_Vendor->SetWidth(400);
	_Vendor->SetHeight(600);
	_Vendor->SetPosition(vec3f(170, 400, 0));
	m_pGUI->AddElement(_Vendor);

	//This bank is exclusive to us
	for (size_t i = 0; i < ETAB_COUNT; i++)
	{
		m_pStores[i] = new CModBank();
		m_pStores[i]->SetBankSize(SHOP_CAPACITY);
	}
	//Populate the store
	m_pStores[EGUN_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Reload Mk.I"));
	m_pStores[EGUN_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Reload Mk.II"));
	m_pStores[EGUN_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Reload Mk.III"));

	m_pStores[EGUN_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Mag Mk.I"));
	m_pStores[EGUN_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Mag Mk.II"));
	m_pStores[EGUN_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Mag Mk.III"));

	m_pStores[EGUN_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Stopper Mk.I"));
	m_pStores[EGUN_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Stopper Mk.II"));
	m_pStores[EGUN_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Stopper Mk.III"));
	
	m_pStores[EGUN_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Pierce Mk.I"));
	m_pStores[EGUN_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Pierce Mk.II"));
	m_pStores[EGUN_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Pierce Mk.III"));

	//Populate the gadget store
	m_pStores[EGADGET_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Cooldown Mk.I"));
	m_pStores[EGADGET_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Cooldown Mk.II"));
	m_pStores[EGADGET_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Cooldown Mk.III"));

	m_pStores[EGADGET_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Repair Mk.I"));
	m_pStores[EGADGET_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Repair Mk.II"));
	m_pStores[EGADGET_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Repair Mk.III"));

	m_pStores[EGADGET_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Explosion Mk.I"));
	m_pStores[EGADGET_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Explosion Mk.II"));
	m_pStores[EGADGET_TAB]->AddMod(CModSystem::GetInstance()->GetModByName("Explosion Mk.III"));

	//Load in the background panels
#pragma region Secondary Panels
	m_pShopPanel = new CUIElement();
	m_pShopPanel->LoadTexture(L"Assets\\Images\\ModMenu\\ShopSlider.png");
	m_pShopPanel->SetHeight(600);
	m_pShopPanel->SetWidth(305);
	m_pShopPanel->SetPosition(vec3f(840, 400, 0));
	m_pGUI->AddElement(m_pShopPanel);

	//m_pCargoPanel = new CUIElement();
	//m_pCargoPanel->LoadTexture(L"Assets\\Images\\ModMenu\\CargoSlider.png");
	//m_pCargoPanel->SetHeight(600);
	//m_pCargoPanel->SetWidth(305);
	//m_pCargoPanel->SetPosition(vec3f(160, 400, 0));
	//m_pGUI->AddElement(m_pCargoPanel);
#pragma endregion

#pragma region Merchant Buttons
	m_pBuy = new CUIButton(std::wstring(L"Assets\\Images\\ModMenu\\BuyButton.png"), 870, 560, 200, 70, std::string());
	m_pBuy->SetEnabled(false);
	m_pGUI->AddElement(m_pBuy);

	m_pSell = new CUIButton(std::wstring(L"Assets\\Images\\ModMenu\\SellButton.png"), 870, 635, 200, 70, std::string());
	
	m_pSell->SetEnabled(false);
	m_pGUI->AddElement(m_pSell);

#pragma endregion

#pragma region Main Panel(s)
	{
		//CUIElement* pTemp = new CUIElement();
		//pTemp->LoadTexture(L"Assets\\Images\\ModMenu\\BackPlate.png");
		//pTemp->SetPosition(vec3f(500, 400, 0));
		//pTemp->SetHeight(650);
		//pTemp->SetWidth(510);
		//m_pGUI->AddElement(pTemp);

		//Scrap counter
		CUIElement* pTemp = new CUIElement();
		pTemp->LoadTexture(L"Assets\\Images\\ModMenu\\ScrapCounter.png");
		pTemp->SetPosition(vec3f(640, 120, 0));
		pTemp->SetHeight(65);
		pTemp->SetWidth(200);
		m_pGUI->AddElement(pTemp);


		//Make the tabs
		m_pTabs[EGUN_TAB] = new CUIButton(std::wstring(L"Assets\\Images\\ModMenu\\WeaponTab.png"), 318, 140, 90, 55, std::string());
		m_pTabs[EGADGET_TAB] = new CUIButton(std::wstring(L"Assets\\Images\\ModMenu\\GadgetTab.png"), 437, 140, 110, 55, std::string());

		for (size_t i = 0; i < ETAB_COUNT; i++)
		{
			m_pGUI->AddElement(m_pTabs[i]);
		}

		for (size_t i = 0; i < ETAB_COUNT; i++)
		{
			m_pMainPanels[i] = new CUIElement();
			m_pMainPanels[i]->SetPosition(vec3f(500, 400, 0));
			m_pMainPanels[i]->SetHeight(650);
			m_pMainPanels[i]->SetWidth(510);
			m_pGUI->AddElement(m_pMainPanels[i]);
		}
		m_pMainPanels[EGUN_TAB]->LoadTexture(L"Assets\\Images\\ModMenu\\WeaponModMenu.png");
		m_pMainPanels[EGADGET_TAB]->LoadTexture(L"Assets\\Images\\ModMenu\\GadgetModMenu.png");

		m_pBack = new CUIButton(std::wstring(L"Assets\\Images\\ModMenu\\EXIT.png"), 860, 70, 200, 65, std::string());
		m_pGUI->AddElement(m_pBack);

		m_pDescPanel = new CUIElement();
		m_pDescPanel->SetTexture(CModSystem::GetInstance()->GetModDescPanel(0));
		m_pDescPanel->SetPosition(vec3f(500, 590, 0));
		m_pDescPanel->SetWidth(460);
		m_pDescPanel->SetHeight(215);
		m_pGUI->AddElement(m_pDescPanel); 
	}
#pragma endregion

#pragma region Weapon Plates
	//Weapon plates
	{
		CUIElement* pPlate = new CUIElement();
		pPlate->LoadTexture(L"Assets\\Images\\ModMenu\\WeaponSlab.png");
		pPlate->SetPosition(vec3f(500, 220, 0));
		pPlate->SetHeight(100);
		pPlate->SetWidth(450);
		m_pGUI->AddElement(pPlate);

		pPlate = new CUIElement();
		pPlate->LoadTexture(L"Assets\\Images\\ModMenu\\WeaponSlab.png");
		pPlate->SetPosition(vec3f(500, 320, 0));
		pPlate->SetHeight(100);
		pPlate->SetWidth(450);
		m_pGUI->AddElement(pPlate);

		pPlate = new CUIElement();
		pPlate->LoadTexture(L"Assets\\Images\\ModMenu\\WeaponSlab.png");
		pPlate->SetPosition(vec3f(500, 420, 0));
		pPlate->SetHeight(100);
		pPlate->SetWidth(450);
		m_pGUI->AddElement(pPlate);
	}
	m_pEquipmentIcons[EPISTOL_SLOT] = new CUIButton(330, 220, 100, 100);
	m_pEquipmentIcons[EPISTOL_SLOT]->SetTexture(CModSystem::GetInstance()->GetModIcon(PISTOL_ID));
	m_pGUI->AddElement(m_pEquipmentIcons[EPISTOL_SLOT]);

	m_pEquipmentIcons[EMACHGUN_SLOT] = new CUIButton(330, 320, 100, 100);
	m_pEquipmentIcons[EMACHGUN_SLOT]->SetTexture(CModSystem::GetInstance()->GetModIcon(MACHGUN_ID));
	m_pGUI->AddElement(m_pEquipmentIcons[EMACHGUN_SLOT]);

	m_pEquipmentIcons[ESHOTGUN_SLOT] = new CUIButton(330, 420, 100, 100);
	m_pEquipmentIcons[ESHOTGUN_SLOT]->SetTexture(CModSystem::GetInstance()->GetModIcon(SHOTGUN_ID));
	m_pGUI->AddElement(m_pEquipmentIcons[ESHOTGUN_SLOT]);
#pragma endregion

#pragma region Weapon Locks
	for (size_t i = 0; i < 3; i++)
	{
		m_pEquipmentLocks[i] = new CUIElement();
		m_pEquipmentLocks[i]->SetHeight(65);
		m_pEquipmentLocks[i]->SetWidth(65);
		m_pEquipmentLocks[i]->LoadTexture(L"Assets\\Images\\ModMenu\\Locked.png");
		m_pEquipmentLocks[i]->SetVisible(false);
		m_pGUI->AddElement(m_pEquipmentLocks[i]);
	}
	m_pEquipmentLocks[GUN_SEMI_AUTO]->SetPosition(vec3f(330, 220, 0));
	m_pEquipmentLocks[GUN_FULL_AUTO]->SetPosition(vec3f(330, 320, 0));
	m_pEquipmentLocks[GUN_SCATTER]->SetPosition(vec3f(330, 420, 0));

#pragma endregion

#pragma region Mod Icons
	//Icons' textures are loaded in the ModSystem, we just need to render them
	CUIButton* pIcon = nullptr;
	//Allocate buttons for everyone
	for (size_t i = 0; i < EMAX_SLOTS; i++)
	{
		for (size_t j = 0; j < MAX_BANK_SIZE; j++)
		{
			pIcon = new CUIButton(420 + (j * 65.0f), 220 + (i * 100.0f), 60, 60);
			m_pGUI->AddElement(pIcon);
			if (m_pWeapons[i]->GetModBank()->GetBankSize() > j)
			{
				pIcon->SetTexture(CModSystem::GetInstance()->GetModIcon(m_pWeapons[i]->GetModBank()->GetMod(j)));
			}
			else
			{
				pIcon->SetTexture(nullptr);
			}
			m_vEquipmentModSlots[i].push_back(pIcon);
		}
	}

	{
		float offset = 0;
		//for (size_t i = 0; i < m_pCargo->GetBankSize(); i++)
		//{
		//	if (i % 3 == 0)
		//	{
		//		pIcon = new CUIButton(65, 270 + offset, 60, 60);
		//	}
		//	else if (i % 3 == 1)
		//	{
		//		pIcon = new CUIButton(130, 270 + offset, 60, 60);
		//	}
		//	else
		//	{
		//		pIcon = new CUIButton(195, 270 + offset, 60, 60);
		//		offset += 65.0f;
		//	}
		//	pIcon->SetTexture(CModSystem::GetInstance()->GetModIcon(m_pCargo->GetMod(i)));
		//	m_pGUI->AddElement(pIcon);
		//	m_vCargoIcons.push_back(pIcon);
		//}
		//offset = 0;
		for (size_t i = 0; i < m_pStores[m_eCurrTab]->GetBankSize(); i++)
		{
			if (i % 3 == 0)
			{
				pIcon = new CUIButton(805, 270 + offset, 60, 60);
			}
			else if (i % 3 == 1)
			{
				pIcon = new CUIButton(870, 270 + offset, 60, 60);
			}
			else
			{
				pIcon = new CUIButton(935, 270 + offset, 60, 60);
				offset += 65.0f;
			}
			pIcon->SetTexture(CModSystem::GetInstance()->GetModIcon(m_pStores[m_eCurrTab]->GetMod(i)));
			m_pGUI->AddElement(pIcon);
			m_vShopIcons.push_back(pIcon);
		}
	}
#pragma endregion

	m_pIndicator = new CUIElement();
	m_pIndicator->LoadTexture(L"Assets\\Images\\ModMenu\\Indicator.png");
	m_pIndicator->SetHeight(65);
	m_pIndicator->SetWidth(65);
	m_pIndicator->SetPosition(vec3f(430, 220, 0));
	m_pGUI->AddElement(m_pIndicator);

	m_pDragger = new CUIElement();
	m_pDragger->SetTexture(CModSystem::GetInstance()->GetModIcon(0));
	m_pDragger->SetWidth(60);
	m_pDragger->SetHeight(60);
	m_pDragger->SetColor(DRAGGER_COLOR);
	m_pGUI->AddElement(m_pDragger);

	return true;
}

void CUpgradeState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded( ))
		return;
	{//These textures are managed by the ModSystem, let that delete them
		//for (size_t i = 0; i < m_vCargoIcons.size(); i++)
		//{
		//	m_vCargoIcons[i]->SetTexture(nullptr);
		//}
		for (size_t i = 0; i < m_vShopIcons.size(); i++)
		{
			m_vShopIcons[i]->SetTexture(nullptr);
		}
		m_pDescPanel->SetTexture(nullptr);
		m_pDragger->SetTexture(nullptr);
		for (size_t i = 0; i < EMAX_SLOTS; i++)
		{
			for (size_t j = 0; j < m_vEquipmentModSlots[i].size(); j++)
			{
				m_vEquipmentModSlots[i][j]->SetTexture(nullptr);
			}
			m_pEquipmentIcons[i]->SetTexture(nullptr);
		}
	}
	CBaseState::UnLoad();
	for (size_t i = 0; i < ETAB_COUNT; i++)
	{
		delete m_pStores[i];
		m_pStores[i] = nullptr;
	}
	for (size_t i = 0; i < EMAX_SLOTS; i++)
	{
		m_pEquipmentIcons[i] = nullptr;
		m_pEquipmentLocks[i] = nullptr;
	}
	m_pShopPanel = m_pPrevSelection =
		m_pIndicator = m_pDragger = m_pCargoPanel = m_pDescPanel = m_pBack =
		m_pBuy = m_pSell = nullptr;

	for (size_t i = 0; i < EMAX_SLOTS; i++)
	{
		m_vEquipmentModSlots[i].clear();
	}
	//m_vCargoIcons.clear();
	m_vShopIcons.clear();
}

bool CUpgradeState::Exit(void)
{
	if (!GetEntered( ))
		return false;
	CBaseState::Exit( );
	AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::STOP_SHOP_MUSIC_LOOP);

	//If the player just baought the pistol, put it in his hands
	if (m_pPlayer->GetCurrentWeapon() == nullptr)
	{
		m_pPlayer->SwitchWeapon(GUN_SEMI_AUTO);
	}
	return false;
}

eStateType CUpgradeState::CheckState(void)
{
	eStateType eNextState = GetType();
	//Go back to gameplay
	if (m_bStateChanges[STATE_CH_BACK] || m_bStateChanges[STATE_CH_QUIT])
	{
		eNextState = STATE_GAMEPLAY;
		AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::RESUME_ALL);
	}
	if (CBaseState::CheckState( ) == STATE_NULL)
	{
		eNextState = STATE_NULL;
	}

	//Tell the state machine what state we're switching to
	return eNextState;
}

void CUpgradeState::RefreshIcons(void)
{
	//Which group are we rendering here
	//Render guns
	if (EGUN_TAB == m_eCurrTab)
	{
		for (size_t i = 0; i < EMAX_SLOTS; i++)
		{
			//Render the mod icons
			for (size_t j = 0; j < MAX_BANK_SIZE; j++)
			{
				if (m_pWeapons[i] && j < m_pWeapons[i]->GetModBank()->GetBankSize())
				{
					m_vEquipmentModSlots[i][j]->SetTexture(CModSystem::GetInstance()->GetModIcon(m_pWeapons[i]->GetModBank()->GetMod(j)));
					m_vEquipmentModSlots[i][j]->SetVisible(true);
					m_vEquipmentModSlots[i][j]->SetEnabled(m_pWeapons[i]->GetUnlocked());
				}
				else
				{
					m_vEquipmentModSlots[i][j]->SetTexture(nullptr);
					m_vEquipmentModSlots[i][j]->SetEnabled(false);
					m_vEquipmentModSlots[i][j]->SetVisible(false);
				}
				m_pEquipmentIcons[i]->SetEnabled(true);
				m_pEquipmentIcons[i]->SetVisible(true);
			}
		}
		m_pEquipmentIcons[0]->SetTexture(CModSystem::GetInstance()->GetModIcon(PISTOL_ID));
		m_pEquipmentIcons[1]->SetTexture(CModSystem::GetInstance()->GetModIcon(MACHGUN_ID));
		m_pEquipmentIcons[2]->SetTexture(CModSystem::GetInstance()->GetModIcon(SHOTGUN_ID));

	}
	else if (EGADGET_TAB == m_eCurrTab)
	{
		for (size_t i = 0; i < EMAX_SLOTS; i++)
		{
			//Render the mod icons
			for (size_t j = 0; j < MAX_BANK_SIZE; j++)
			{
				//Update the icons that are valid for this piece of equipment
				if (m_pGadgets[i] && j < m_pGadgets[i]->GetModBank()->GetBankSize())
				{
					m_vEquipmentModSlots[i][j]->SetTexture(CModSystem::GetInstance()->GetModIcon(m_pGadgets[i]->GetModBank()->GetMod(j)));
					m_vEquipmentModSlots[i][j]->SetVisible(true);
					m_vEquipmentModSlots[i][j]->SetEnabled(m_pGadgets[i]->GetUnlocked());
				}
				//Hide invalid stuff
				else
				{
					m_vEquipmentModSlots[i][j]->SetTexture(nullptr);
					m_vEquipmentModSlots[i][j]->SetEnabled(false);
					m_vEquipmentModSlots[i][j]->SetVisible(false);
				}
			}
		}
		m_pEquipmentIcons[0]->SetTexture(CModSystem::GetInstance()->GetModIcon(DRONE_ID));
		m_pEquipmentIcons[1]->SetTexture(nullptr);
		m_pEquipmentIcons[1]->SetEnabled(false);
		m_pEquipmentIcons[1]->SetVisible(false);
		m_pEquipmentIcons[2]->SetTexture(nullptr);
		m_pEquipmentIcons[2]->SetEnabled(false);
		m_pEquipmentIcons[2]->SetVisible(false);
	}

	unsigned int uID = 0;
	if (m_eCurrTab == EGUN_TAB)
	{
		uID = PISTOL_ID;
	}
	else if (m_eCurrTab == EGADGET_TAB)
	{
		uID = DRONE_ID;
	}
	//for (size_t i = 0; i < m_pCargo->GetBankSize(); i++)
	//{
	//	unsigned int uMod = m_pCargo->GetMod(i);
	//	m_vCargoIcons[i]->SetTexture(CModSystem::GetInstance()->GetModIcon(uMod));
	//	//Gray out incompatible mods
	//	if (uID && uMod && !(uMod & uID))
	//	{
	//		m_vCargoIcons[i]->SetEnabled(false);
	//	}
	//	else
	//	{
	//		m_vCargoIcons[i]->SetEnabled(true);
	//	}
	//}
	for (size_t i = 0; i < m_pStores[m_eCurrTab]->GetBankSize(); i++)
	{
		m_vShopIcons[i]->SetTexture(CModSystem::GetInstance()->GetModIcon(m_pStores[m_eCurrTab]->GetMod(i)));
	}

	//Swich tabs
	for (size_t i = 0; i < ETAB_COUNT; i++)
	{
		m_pMainPanels[i]->SetVisible(i == m_eCurrTab);
		m_pTabs[i]->SetVisible(i != m_eCurrTab);
		m_pTabs[i]->SetEnabled(i != m_eCurrTab);
	}
}

void CUpgradeState::RefreshSelection(void)
{
	CInputManager* pInput = CInputManager::GetInstance( );

	m_pPrevSelection = m_pSelection;
	
	//Drag
	if (m_bDragging && m_pSelection)
	{
		//Draw the dragger
		if (m_uSelectedID && CInputManager::GetInstance()->IsButtonDown(eControls::MouseLeft))
		{
			m_pDragger->SetVisible(true);
			m_pDragger->SetColor(DRAGGER_COLOR);

			vec3f f3Temp = pInput->GetMousePosition( );

			//HACKing the dick off this GUI code: It mostly works in fullscreen
			{
				f3Temp.x = f3Temp.x / CRenderer::GetReference( ).GetWidth( ) * GUI_WIDTH;
				f3Temp.y = f3Temp.y / CRenderer::GetReference( ).GetHeight( ) * GUI_HEIGHT;

				if (!CRenderer::GetReference( ).m_bFullScreen)
				{
					f3Temp.x -= 5.0f;
					f3Temp.y -= 25.0f;
				}

				m_pDragger->SetPosition(f3Temp);
			}
		}
		//Released, toggle the hard select
		else if (m_pSelection->GetMouseHover())
		{
			m_pDragger->SetVisible(false);
			m_bHardSelect = !m_bHardSelect;
			m_bDragging = false;
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_SELECT);
		}
		//If we didn't succeed, make sure we keep looking at the old guy
		else if (!AttemptDrop())
		{
			m_pDragger->SetVisible(false);
			m_bDragging = false;
			m_bHardSelect = true;
		}
		return;
	}

	//Change selection to another icon
	if (!m_bHardSelect || (m_bHardSelect && CInputManager::GetInstance()->IsKeyPressed(eControls::MouseLeft)))
	{
		bool bFound = false;
		for (size_t i = 0; i < EMAX_SLOTS && !bFound; i++)
		{
			//Only check valid options
			if (m_pEquipmentIcons[i]->GetVisible())
			{
				if (!bFound && m_pEquipmentIcons[i]->GetMouseHover())
				{
					m_pSelection = m_pEquipmentIcons[i];
					if (m_eCurrTab == EGUN_TAB)
					{
						if (i == EPISTOL_SLOT)
						{
							m_uSelectedID = PISTOL_ID;
						}
						else if (i == EMACHGUN_SLOT)
						{
							m_uSelectedID = MACHGUN_ID;
						}
						else if (i == ESHOTGUN_SLOT)
						{
							m_uSelectedID = SHOTGUN_ID;
						}
						m_bStoreItem = !m_pWeapons[i]->GetUnlocked();
					}
					else if (m_eCurrTab == EGADGET_TAB)
					{
						if (i == EDRONE_SLOT)
						{
							m_uSelectedID = DRONE_ID;
						}
						m_bStoreItem = !m_pGadgets[i]->GetUnlocked();
					}
					m_pSelectedBank = nullptr;
					m_uSelectedIndex = 0;
					m_pDescPanel->SetTexture(CModSystem::GetInstance()->GetModDescPanel(m_uSelectedID));
					m_pDragger->SetTexture(nullptr);
					bFound = true;
					m_bHardSelect = false;
				}
				for (size_t j = 0; j < MAX_BANK_SIZE && !bFound; j++)
				{
					if (m_vEquipmentModSlots[i][j]->GetVisible() && m_vEquipmentModSlots[i][j]->GetMouseHover())
					{
						m_pSelection = m_vEquipmentModSlots[i][j];
						if (m_eCurrTab == EGUN_TAB)
						{
							m_uSelectedID = m_pWeapons[i]->GetModBank()->GetMod(j);
							m_pSelectedBank = m_pWeapons[i]->GetModBank();
						}
						else if (m_eCurrTab == EGADGET_TAB)
						{
							m_uSelectedID = m_pGadgets[i]->GetModBank()->GetMod(j);
							m_pSelectedBank = m_pGadgets[i]->GetModBank();
						}
						m_uSelectedIndex = j;
						m_pDescPanel->SetTexture(CModSystem::GetInstance()->GetModDescPanel(m_uSelectedID));
						m_pDragger->SetTexture(CModSystem::GetInstance()->GetModIcon(m_uSelectedID));
						bFound = true;
						m_bStoreItem = false;
						m_bHardSelect = false;
					}
				}
			}
		}

		//for (size_t i = 0; i < m_vCargoIcons.size() && !bFound; i++)
		//{
		//	if (m_vCargoIcons[i]->GetMouseHover())
		//	{
		//		m_pSelection = m_vCargoIcons[i];
		//		m_uSelectedID = m_pCargo->GetMod(i);
		//		m_pSelectedBank = m_pCargo;
		//		m_uSelectedIndex = i;
		//		m_pDescPanel->SetTexture(CModSystem::GetInstance()->GetModDescPanel(m_uSelectedID));
		//		m_pDragger->SetTexture(CModSystem::GetInstance()->GetModIcon(m_uSelectedID));
		//		bFound = true;
		//		m_bStoreItem = false;
		//		m_bHardSelect = false;
		//	}
		//}
		for (size_t i = 0; i < m_vShopIcons.size() && !bFound; i++)
		{
			if (m_vShopIcons[i]->GetMouseHover())
			{
				m_pSelection = m_vShopIcons[i];
				m_uSelectedID = m_pStores[m_eCurrTab]->GetMod(i);
				m_pSelectedBank = m_pStores[m_eCurrTab];
				m_uSelectedIndex = i;
				m_pDescPanel->SetTexture(CModSystem::GetInstance()->GetModDescPanel(m_uSelectedID));
				m_pDragger->SetTexture(CModSystem::GetInstance()->GetModIcon(m_uSelectedID));
				bFound = true;
				m_bStoreItem = true;
				m_bHardSelect = false;
			}
		}

		if (m_pBuy->GetMouseHover() && m_pBuy->GetEnabled())
		{
			if (m_bStoreItem)
			{
				AttemptTransaction();
			}
			bFound = true;
		}
		else if (m_pSell->GetMouseHover() && m_pSell->GetEnabled())
		{
			if (!m_bStoreItem)
			{
				AttemptTransaction();
			}
			bFound = true;
		}

		
		//Nothing was selected
		if (!bFound)
		{
			m_pSelection = nullptr;
			m_bHardSelect = false;
			m_bStoreItem = false;
			m_uSelectedID = 0;
			m_uSelectedIndex = 0;
			m_pSelectedBank = nullptr;
			m_pDescPanel->SetTexture(CModSystem::GetInstance()->GetModDescPanel(0));
			m_pDragger->SetTexture(nullptr);
		}
	}
	//Turn on hard select if you click on something
	if (m_pSelection && m_pSelection->GetMouseHover() && CInputManager::GetInstance()->IsKeyPressed(eControls::MouseLeft))
	{
		m_bDragging = true;
		//Flash the shop icons if you click on an empty slot
		if (m_uSelectedID == 0)
		{
			m_fShopHereTimer = SHOP_HERE_TIME;
		}
	}
}

void CUpgradeState::MerchantUpdate(void)
{
	//Nothing
	if (m_pSelection == nullptr || m_uSelectedID == 0 || 
		(m_uSelectedID == PISTOL_ID && m_pWeapons[EPISTOL_SLOT]->GetUnlocked()) || 
		(m_uSelectedID == MACHGUN_ID && m_pWeapons[EMACHGUN_SLOT]->GetUnlocked()) || 
		(m_uSelectedID == SHOTGUN_ID && m_pWeapons[ESHOTGUN_SLOT]->GetUnlocked()) ||
		(m_uSelectedID == DRONE_ID && m_pGadgets[EDRONE_SLOT]->GetUnlocked())
		)
	{
		m_pSell->SetPressed(false);
		m_pSell->SetEnabled(false);
		m_pBuy->SetPressed(false);
		m_pBuy->SetEnabled(false);
	}
	//Buy
	else if (m_bStoreItem)
	{
		m_pSell->SetEnabled(false);
		if (m_bHardSelect)
			m_pBuy->SetEnabled(true);
		else
			m_pBuy->SetEnabled(false);
	}
	//Sell
	else
	{
		if (m_bHardSelect)
			m_pSell->SetEnabled(true);
		else
			m_pSell->SetEnabled(true);
		m_pBuy->SetEnabled(false);
	}
}

//Tries to swap/drop your selected item in the designated spot
bool CUpgradeState::AttemptDrop(void)
{
	//If you're trying to buy something, can you afford it? || You're dragging around a weapon
	if (m_bStoreItem && m_pPlayer->GetScrapCount() < CModSystem::GetInstance()->GetModValue(m_uSelectedID) || m_pSelectedBank == nullptr)
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_DENY);
		m_fCardDeclinedTimer = CARD_DECLINE_TIME;
		return false;
	}
	//Find out where you're trying to drop this
	CUIButton* pTarget = nullptr;
	CModBank* pBank = nullptr;
	unsigned int nTargetID = 0;
	size_t uTargetIndex;
	bool bTargetShop = false;

	for (size_t i = 0; i < EMAX_SLOTS && pTarget == nullptr; i++)
	{
		if (m_pEquipmentIcons[i]->GetVisible())
		{
			for (size_t j = 0; j < MAX_BANK_SIZE && pTarget == nullptr; j++)
			{
				if (m_vEquipmentModSlots[i][j]->GetVisible() && m_vEquipmentModSlots[i][j]->GetMouseHover())
				{
					pTarget = m_vEquipmentModSlots[i][j];
					if (EGUN_TAB == m_eCurrTab)
					{
						nTargetID = m_pWeapons[i]->GetModBank()->GetMod(j);
						pBank = m_pWeapons[i]->GetModBank();
					}
					else if (EGADGET_TAB == m_eCurrTab)
					{
						nTargetID = m_pGadgets[i]->GetModBank()->GetMod(j);
						pBank = m_pGadgets[i]->GetModBank();
					}
					uTargetIndex = j;
				}
			}
		}
	}

	//for (size_t i = 0; i < m_vCargoIcons.size() && pTarget == nullptr; i++)
	//{
	//	if (m_vCargoIcons[i]->GetMouseHover())
	//	{
	//		pTarget = m_vCargoIcons[i];
	//		nTargetID = m_pCargo->GetMod(i);
	//		pBank = m_pCargo;
	//		uTargetIndex = i;
	//	}
	//}
	for (size_t i = 0; i < m_vShopIcons.size() && pTarget == nullptr; i++)
	{
		if (m_vShopIcons[i]->GetMouseHover())
		{
			pTarget = m_vShopIcons[i];
			nTargetID = m_pStores[m_eCurrTab]->GetMod(i);
			pBank = m_pStores[m_eCurrTab];
			bTargetShop = true;
			uTargetIndex = i;
		}
	}
	//We didn't find anyone || we can't rearrange the shop
	if (pTarget == nullptr || (bTargetShop == true && m_bStoreItem == true))
	{
		return false;
	}

	//Buy an item
	if (m_bStoreItem)
	{
		////Move whatever you're overwriting into the cargo hold
		//if (nTargetID)
		//{
		//	//m_pCargo->AddMod(nTargetID);
		//}
		//Keep the shop item there
		if (!pBank->AddMod(m_uSelectedID, uTargetIndex))
		{
			//Tried to purchase suit mod for our gun(or something like that)
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_DENY);
			return false;
		}
		//Charge your account
		m_pPlayer->SubScrap(CModSystem::GetInstance()->GetModValue(m_uSelectedID));
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_PURCHASE);
	}
	//Selling to the shop
	else if (bTargetShop)
	{
		m_pPlayer->AddScrap(CModSystem::GetInstance()->GetModValue(m_uSelectedID));
		m_pSelectedBank->AddMod(EMOD_NULL, m_uSelectedIndex);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_PURCHASE);
	}
	//Swapping between own items
	else
	{
		if (m_pSelectedBank->IsModWelcome(nTargetID) && pBank->IsModWelcome(m_uSelectedID))
		{
			m_pSelectedBank->AddMod(nTargetID, m_uSelectedIndex);
			pBank->AddMod(m_uSelectedID, uTargetIndex);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_SELECT);
		}
		else
		{
			//Trying to put a weapon mod on a sub-system (or something like that)
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_DENY);
			return false;
		}
	}

	//Look at the new guy
	m_pSelection = pTarget;
	m_pSelectedBank = pBank;
	m_uSelectedID = nTargetID;
	m_uSelectedIndex = uTargetIndex;

	//Show the new arrangement
	RefreshIcons();
	RefreshGuns();

	return true;
}
#include "../AchieveTracker.h"
bool CUpgradeState::AttemptTransaction(void)
{
	bool bSuccess = false;
	//Buying a weapon
	if (m_pSelectedBank == nullptr && m_pPlayer->GetScrapCount() >= CModSystem::GetInstance()->GetModValue(m_uSelectedID))
	{
		//Only allowed to buy if not already bought
		if (m_uSelectedID == PISTOL_ID && !m_pWeapons[EPISTOL_SLOT]->GetUnlocked())
		{
			m_pPlayer->SubScrap(CModSystem::GetInstance()->GetModValue(m_uSelectedID));
			m_pWeapons[EPISTOL_SLOT]->SetUnlocked(true);
			bSuccess = true;
		}
		else if (m_uSelectedID == MACHGUN_ID && !m_pWeapons[EMACHGUN_SLOT]->GetUnlocked())
		{
			m_pPlayer->SubScrap(CModSystem::GetInstance()->GetModValue(m_uSelectedID));
			m_pWeapons[EMACHGUN_SLOT]->SetUnlocked(true);
			bSuccess = true;

			CAchieveTracker::GetInstance()->UnlockAchievement(CAchieveTracker::eBuyMachGun);
		}
		else if (m_uSelectedID == SHOTGUN_ID && !m_pWeapons[ESHOTGUN_SLOT]->GetUnlocked())
		{
			m_pPlayer->SubScrap(CModSystem::GetInstance()->GetModValue(m_uSelectedID));
			m_pWeapons[ESHOTGUN_SLOT]->SetUnlocked(true);
			bSuccess = true;

			CAchieveTracker::GetInstance()->UnlockAchievement(CAchieveTracker::eBuyShotGun);
		}
		else if (m_uSelectedID == DRONE_ID && !m_pGadgets[EDRONE_SLOT]->GetUnlocked())
		{
			m_pPlayer->SubScrap(CModSystem::GetInstance()->GetModValue(m_uSelectedID));
			m_pGadgets[EDRONE_SLOT]->SetUnlocked(true);
			bSuccess = true;

			CAchieveTracker::GetInstance()->UnlockAchievement(CAchieveTracker::eBuyDrone);
		}
		else
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_DENY);
			return false;
		}
	}
	//Bought!
	else if (m_bStoreItem && m_uSelectedID && m_pPlayer->GetScrapCount() >= CModSystem::GetInstance()->GetModValue(m_uSelectedID)/* && m_pCargo->AddMod(m_uSelectedID)*/)
	{
		if (m_eCurrTab == EGUN_TAB)
		{
			for (size_t i = 0; i < EMAX_SLOTS; i++)
			{
				if (m_pWeapons[i] && m_pWeapons[i]->GetUnlocked() && m_pWeapons[i]->GetModBank()->AddMod(m_uSelectedID))
				{
					m_pPlayer->SubScrap(CModSystem::GetInstance()->GetModValue(m_uSelectedID));
					bSuccess = true;
					break;
				}
			}
		}
		else if (m_eCurrTab == EGADGET_TAB)
		{
			for (size_t i = 0; i < EMAX_SLOTS; i++)
			{
				if (m_pGadgets[i] && m_pGadgets[i]->GetUnlocked() && m_pGadgets[i]->GetModBank()->AddMod(m_uSelectedID))
				{
					m_pPlayer->SubScrap(CModSystem::GetInstance()->GetModValue(m_uSelectedID));
					bSuccess = true;
					break;
				}
			}
		}
	}
	//Sold!
	else if (!m_bStoreItem && m_uSelectedID)
	{
		m_pPlayer->AddScrap(CModSystem::GetInstance()->GetModValue(m_uSelectedID));
		m_pSelectedBank->AddMod(EMOD_NULL, m_uSelectedIndex);
		m_uSelectedID = EMOD_NULL;
		bSuccess = true;
	}
	//Can't buy or sell
	if (!bSuccess)
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_DENY);
		m_fCardDeclinedTimer = CARD_DECLINE_TIME;
		return false;
	}
	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_PURCHASE);
	RefreshIcons();
	RefreshLocks();
	RefreshGuns();
	return true;
}

void CUpgradeState::RefreshLocks(void)
{
	if (m_eCurrTab == EGUN_TAB)
	{
		if (m_pEquipmentLocks[EPISTOL_SLOT])
		{
			m_pEquipmentLocks[EPISTOL_SLOT]->SetVisible(!(m_pWeapons[EPISTOL_SLOT] && m_pWeapons[EPISTOL_SLOT]->GetUnlocked()));
		}
		if (m_pEquipmentLocks[EMACHGUN_SLOT])
		{
			m_pEquipmentLocks[EMACHGUN_SLOT]->SetVisible(!(m_pWeapons[EMACHGUN_SLOT] && m_pWeapons[EMACHGUN_SLOT]->GetUnlocked()));
		}
		if (m_pEquipmentLocks[ESHOTGUN_SLOT])
		{
			m_pEquipmentLocks[ESHOTGUN_SLOT]->SetVisible(!(m_pWeapons[ESHOTGUN_SLOT] && m_pWeapons[ESHOTGUN_SLOT]->GetUnlocked()));
		}
	}
	else if (m_eCurrTab == EGADGET_TAB)
	{
		if (m_pEquipmentLocks[EDRONE_SLOT])
		{
			m_pEquipmentLocks[EPISTOL_SLOT]->SetVisible(!(m_pGadgets[EDRONE_SLOT] && m_pGadgets[EDRONE_SLOT]->GetUnlocked()));
		}
		if (m_pEquipmentLocks[EGADGET2_SLOT])
		{
			m_pEquipmentLocks[EGADGET2_SLOT]->SetVisible(false);
		}
		if (m_pEquipmentLocks[EGADGET3_SLOT])
		{
			m_pEquipmentLocks[EGADGET3_SLOT]->SetVisible(false);
		}

	}
}

void CUpgradeState::RefreshGuns(void)
{
	//Update the gun stats
	if (m_pWeapons[EPISTOL_SLOT] && m_pWeapons[EPISTOL_SLOT]->GetModBank()->GetIsChanged())
	{
		m_pWeapons[EPISTOL_SLOT]->GetModBank()->ApplyMods(m_pWeapons[EPISTOL_SLOT]);
	}
	if (m_pWeapons[EMACHGUN_SLOT] && m_pWeapons[EMACHGUN_SLOT]->GetModBank()->GetIsChanged())
	{
		m_pWeapons[EMACHGUN_SLOT]->GetModBank()->ApplyMods(m_pWeapons[EMACHGUN_SLOT]);
	}
	if (m_pWeapons[ESHOTGUN_SLOT] && m_pWeapons[ESHOTGUN_SLOT]->GetModBank()->GetIsChanged())
	{
		m_pWeapons[ESHOTGUN_SLOT]->GetModBank()->ApplyMods(m_pWeapons[ESHOTGUN_SLOT]);
	}
	if (m_pGadgets[EDRONE_SLOT] && m_pGadgets[EDRONE_SLOT]->GetModBank()->GetIsChanged())
	{
		m_pGadgets[EDRONE_SLOT]->GetModBank()->ApplyMods(m_pGadgets[EDRONE_SLOT]);
	}

}
