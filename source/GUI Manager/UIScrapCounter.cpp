/***********************************************
* Filename:			UIScrapCounter.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#include "UIScrapCounter.h"
#include "../Renderer/BitmapFont.h"
#include "../Entity Manager/Entities/Player.h"

#define COLOR_CHANGE_RATE 5.0f
#define SCALE_CHANGE_RATE 0.001f

/*****************************************************************
* CUIScrapCounter():
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
CUIScrapCounter::CUIScrapCounter(CPlayer* pTgt)
{
	//m_pTexture = new CTexture2D(0, 0, 108.f, 104.f, L"Assets/Images/uiScrapCounter.png");
	//m_bounds = { 0.f, 108.f, 0.f, 104.f };
	m_pPlayer = pTgt;
	m_textColor = (DirectX::XMFLOAT4)DirectX::Colors::White;
}

/*****************************************************************
* ~CUIScrapCounter():
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
CUIScrapCounter::~CUIScrapCounter()
{
}

/*****************************************************************
* Update: Updates the ui element
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      06/11/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CUIScrapCounter::Update(float dt) /*override*/
{
	CUIElement::Update(dt);

	unsigned int unScrap = m_pPlayer->GetScrapCount();

	/*DirectX::XMFLOAT2 scale = m_pTexture->GetScale();

	if (unScrap > m_unScrapPrev)
	{
		scale.y = 3.f;
		scale.x = 3.f;
		m_textColor = (DirectX::XMFLOAT4)DirectX::Colors::Lime;
	}
	else if (unScrap < m_unScrapPrev)
	{
		scale.y = 3.f;
		scale.x = 3.f;
		m_textColor = (DirectX::XMFLOAT4)DirectX::Colors::Red;
	}

	scale.x -= SCALE_CHANGE_RATE * dt;
	scale.y -= SCALE_CHANGE_RATE * dt;

	if (scale.x < 2.f) 
		scale.x = 2.f;
	if (scale.y < 2.f) 
		scale.y = 2.f;

	m_pTexture->Scale(scale.x, scale.y);
	*/

	if (m_textColor.x < 1.f)
		m_textColor.x += COLOR_CHANGE_RATE * dt;

	if (m_textColor.y < 1.f)
		m_textColor.y += COLOR_CHANGE_RATE * dt;

	if (m_textColor.z < 1.f)
		m_textColor.z += COLOR_CHANGE_RATE * dt;

	if (m_textColor.w < 1.f)
		m_textColor.w += COLOR_CHANGE_RATE * dt;

	m_unScrapPrev = unScrap;
}

/*****************************************************************
* Render: renders the ui element
*
* Ins: 
*
* Outs:
*
* Returns:
*
* Mod. Date:		      06/11/2015
* Mod. Initials:
*****************************************************************/
#include "../Weapon/DroneBuddy.h"
/*virtual*/ void CUIScrapCounter::Render(void) /*override*/
{
	//CUIElement::Render();
	if (m_bVisible)
	{
		CBitmapFont::GetInstance()->DrawString(std::to_string(m_pPlayer->GetScrapCount()).c_str(), m_vPosition.x + 0.03f, m_vPosition.y + 0.015f, m_textColor, 0.5f);
	}
}