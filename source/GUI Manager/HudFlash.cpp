#include "HudFlash.h"
#include "../Entity Manager/Entities/Player.h"

/*****************************************************************
* HudFlash()
*
* Outs:
*
* Returns:
*
* Mod. Date:		     06/06/2015
* Mod. Initials:
*****************************************************************/
CHudFlash::CHudFlash(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	m_fHealthPrev = pPlayer->GetCurrentHealth();
	m_pTexture = new CTexture2D(960.f, 540.f, 1920.f, 1080.f, L"Assets/Images/pixel.jpg");
	m_fCurrentTime = m_fMaxTime;
	m_vPosition = { 960.f, 540.f, 0.f };
	m_fOxyTimer = 0.f;
	m_fHealthTimer = 0.f;

	m_vPosition = { 500.f, 500.f, 0.f };
	m_bounds = { 0, 1000, 0, 1000 };
}

/*****************************************************************
* ~HudFlash()
*
* Outs:
*
* Returns:
*
* Mod. Date:		     06/06/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ CHudFlash::~CHudFlash(void)
{

}

/*****************************************************************
* Update - updates the ui elemment
*
* Ins: dt - The expance of temproal space traversed since the
*			previous computer cycle has transpired
* Outs:
*
* Returns:
*
* Mod. Date:		     06/06/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CHudFlash::Update(float dt)
{
	float fHealth = m_pPlayer->GetCurrentHealth();
	float fOxygen = m_pPlayer->GetOxygen();

	if (fOxygen < m_fOxygenPrev && fOxygen > 0.f)
	{
		m_fHealthTimer = 0.f;
		m_fOxyTimer += dt;
	}
	if (fHealth < m_fHealthPrev)
	{
		m_fHealthTimer += dt;
		m_fOxyTimer = 0.f;
	}

	if (m_fOxyTimer > 1.f)
	{
		m_baseColor = (DirectX::XMFLOAT4)DirectX::Colors::Blue;
		m_fCurrentTime = m_fMaxTime;
		m_fOxyTimer = 0.f;
	}
	if (m_fHealthTimer > .01f)
	{
		m_baseColor = (DirectX::XMFLOAT4)DirectX::Colors::Red;
		m_fCurrentTime = m_fMaxTime;
		m_fHealthTimer = 0.f;
	}

	float alpha = m_fCurrentTime / m_fMaxTime;
	m_fCurrentTime -= dt;

	m_baseColor.w = alpha * .3f;
	m_pTexture->SetColor(m_baseColor);

	m_fHealthPrev = fHealth;
	m_fOxygenPrev = fOxygen;
	CUIElement::Update(dt);
}

/*****************************************************************
* Render: renders the hud element to the screen
*
* Ins:
* Outs:
*
* Returns:
*
* Mod. Date:		     06/06/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CHudFlash::Render(void)
{
	CUIElement::Render();
}