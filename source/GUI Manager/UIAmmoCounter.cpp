/***********************************************
* Filename:			UIAmmoCounter.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#include "UIAmmoCounter.h"
#include "GUIManager.h"
#include "../Entity Manager/Entities/Player.h"
#include "../Renderer/Texture2D.h"
#include "../Renderer/HUDRenderer.h"
#include "../Weapon/IWeapon.h"
#include "../Renderer/BitmapFont.h"
#include "../Entity Manager/Entities/Subsystems/Subsystem.h"
#include "../Weapon/DroneBuddy.h"
#include <DirectXMath.h>
#include <list>

using namespace DirectX;
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
CUIAmmoCounter::CUIAmmoCounter(CPlayer* pPlayer)
{
	ZeroMemory(&weapons, sizeof(weapons));
	weapons.m_pSemiAuto = new CTexture2D(0.f, 0.f, 75.0f, 60.0f, L"Assets/Images/WeaponIcons/handgun1.png");
	weapons.m_pAssultRifle = new CTexture2D(0.f, 0.f, 120.f, 23.f, L"Assets/Images/WeaponIcons/rifle1.png");
	weapons.m_pShotgun = new CTexture2D(0.f, 0.f, 175.0f, 70.0f, L"Assets/Images/WeaponIcons/shotgun1.png");
	m_pDroneBuddyTex = new CTexture2D(0.f, 0.f, 189.f, 164.f, L"Assets/Images/WeaponIcons/droneBuddyIcon.png");

	weapons.m_semiAutoRect = { 0, 75, 0, 60 };
	weapons.m_assultRifleRect = { 0, 85, 0, 23 };
	weapons.m_shotGunRect = { 0, 85, 0, 58 };

	m_droneBuddyRect = { 0.f, 60, 0.f, 60 };

	m_pCurrWeaponTexture = weapons.m_pAssultRifle;

	m_bounds = CRectangle(0, 280, 0, 125);

	m_pTexture = new CTexture2D(0.f, 0.f, 275, 150, L"Assets/Images/HUDbg.png");
	m_pPlayer = pPlayer;

	m_vDroneTextColor = { 0, 1, 0, 1 };
}

/*****************************************************************
* ~CUIAmmoCounter():
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
CUIAmmoCounter::~CUIAmmoCounter()
{
	SafeDelete(weapons.m_pAssultRifle);
	SafeDelete(weapons.m_pSemiAuto);
	SafeDelete(weapons.m_pShotgun);
	SafeDelete(m_pDroneBuddyTex);
}

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
/*virtual*/ void CUIAmmoCounter::Update(float fDt) /*override*/
{
	if (m_pPlayer && m_pPlayer->GetCurrentWeapon())
	{
		switch (m_pPlayer->GetCurrentWeapon()->GetGunType())
		{
			case GUN_SEMI_AUTO:
			m_pCurrWeaponTexture = weapons.m_pSemiAuto;
			break;
			case GUN_FULL_AUTO:
			m_pCurrWeaponTexture = weapons.m_pAssultRifle;
			break;
			case GUN_SCATTER:
			m_pCurrWeaponTexture = weapons.m_pShotgun;
			break;
		}

		if (m_pPlayer->GetCurrentWeapon()->GetReloading())
		{
			static float alphaTimer = 0.f;
			alphaTimer += fDt;

			XMFLOAT4 blendColor = m_pTexture->GetColor();
			blendColor.w = (float)abs(sin(alphaTimer));
			m_pTexture->SetColor(blendColor);
		}
		else
		{
			XMFLOAT4 blendColor = m_pTexture->GetColor();
			blendColor.w = 1.f;
			m_pTexture->SetColor(blendColor);
		}
	}
	else
	{
		m_pCurrWeaponTexture = nullptr;
	}
	m_pTexture->SetScreenPosition(m_vPosition.x, m_vPosition.y);
	m_pTexture->UpdateBuffers();

	m_ftimer += fDt;
	CUIElement::Update(fDt);
}

/*****************************************************************
* Render(): renders the element to the screen
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		     06/03/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CUIAmmoCounter::Render(void) /*override*/
{
	CUIElement::Render();
	
	if (m_pCurrWeaponTexture && m_bVisible)
	{
		// find what the bounds of the current weapon texture is
		CRectangle rect;
		if (m_pCurrWeaponTexture == weapons.m_pShotgun)
			rect = weapons.m_shotGunRect;
		if (m_pCurrWeaponTexture == weapons.m_pAssultRifle)
			rect = weapons.m_assultRifleRect;
		if (m_pCurrWeaponTexture == weapons.m_pSemiAuto)
			rect = weapons.m_semiAutoRect;


		CIWeapon* pWep = m_pPlayer->GetCurrentWeapon();
		if (!pWep->GetReloading())
		{

			// compute the width and height of the weapon texture
			rect = CGUIManager::ComputeBounds(rect);

			m_pCurrWeaponTexture->SetWidth(rect.ComputeWidth());
			m_pCurrWeaponTexture->SetHeight(rect.ComputeHeight());

			m_pCurrWeaponTexture->SetScreenPosition(m_vPosition.x + 0.175f, m_vPosition.y);

			CHUDRenderer::Render2D(m_pCurrWeaponTexture);

			std::string ammoString = std::to_string(pWep->GetCurrentMagazine());// +"/" + to_string(pWep->GetCurrentMagazine() % pWep->GetMaxAmmo());
			CBitmapFont::GetInstance()->DrawString(ammoString.c_str(), m_vPosition.x + .12f, m_vPosition.y - 0.1f, (XMFLOAT4)Colors::White, 0.6f);
		}
		else
		{
			CBitmapFont::GetInstance()->DrawString("Reloading", m_vPosition.x + 0.088f, m_vPosition.y - 0.02f, (XMFLOAT4)Colors::White, 0.55f);
		}
	}

	if (m_bVisible)
	{
		if (!m_pPlayer->GetDroneBuddy()->GetUnlocked())
		{
			CBitmapFont::GetInstance()->DrawString("X", 0.5f, -0.76f, ((DirectX::XMFLOAT4)(DirectX::Colors::Red)), 0.5f);
			return;
		}
		
		if (m_pPlayer->GetDroneBuddy()->GetCooldown() > 0 ||
				 !m_pPlayer->GetDroneBuddy()->IsReady())
		{
			CBitmapFont::GetInstance()->DrawString(
				std::to_string((int)m_pPlayer->GetDroneBuddy()->GetCooldown()).c_str(),
				0.5f, -0.76f, ((DirectX::XMFLOAT4)(DirectX::Colors::Red)), 0.5f);

			m_ftimer = 0;
		}
		else
		{
			m_vDroneTextColor.x = m_vDroneTextColor.z = AbsPulse(m_ftimer);

			CBitmapFont::GetInstance()->DrawString(
				std::to_string((int)m_pPlayer->GetDroneBuddy()->GetCooldown()).c_str(),
				0.5f, -0.76f, (m_vDroneTextColor), 0.5f);
		}
	}
}

float CUIAmmoCounter::AbsPulse(float time)
{
	const int frequency = 2; // Frequency in Hz
	const int resolution = 25; // How many steps are there between 0 and 1
	return abs(resolution - ((int)(time * frequency * 2 * resolution) % (resolution * 2))) / (float)resolution;
}