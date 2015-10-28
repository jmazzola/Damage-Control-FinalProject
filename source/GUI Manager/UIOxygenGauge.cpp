/***********************************************
* Filename:			UIOxygenGauge.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Brandon Lucier
* Purpose:
************************************************/

#include "UIOxygenGauge.h"
#include "../Entity Manager/Entities/Player.h"
#include "../Renderer/HUDRenderer.h"
#include "GUIManager.h"


/*****************************************************************
* CUIOxygenGauge():
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
CUIOxygenGauge::CUIOxygenGauge(CPlayer* pPlayer)
{
	m_pTexture = new CTexture2D(100, 300.0f, 250.f, 50.f, L"Assets/Images/OxygenBar.png");
	m_pBackTexture = new CTexture2D(100, 300.0f, 250.f, 50.f, L"Assets/Images/OxygenBar.png");
	m_pBackTexture->SetColor(DirectX::XMFLOAT4 { 0.f, 0.f, 0.f, 1.f });

	CRectangle rect = { 0.f, 398.0, 0.f, 34.25f };
	rect = CGUIManager::ComputeBounds(rect);
	m_pBackTexture->SetWidth(rect.ComputeWidth());
	m_pBackTexture->SetHeight(rect.ComputeHeight());
	m_pBackTexture->SetScreenPosition(0.835f, -0.933f);


	//m_bounds = { 0.f, 146.f, 0.f, 168.f };

	m_pPlayer = pPlayer;

}

/*****************************************************************
* ~CUIOxygenGauge():
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
CUIOxygenGauge::~CUIOxygenGauge( )
{
	SafeDelete(m_pBackTexture);
}

/*****************************************************************
* Update() - Updates the ui element
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      06/04/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CUIOxygenGauge::Update(float dt) /*override*/
{
	CUIElement::Update(dt);

	if (m_pPlayer)
		m_fOxygen = m_pPlayer->GetOxygen();

	float width = (m_fOxygen / 100) * 398.0f;
	float height = 34.25f;
	//m_bounds = CRectangle(m_vPosition.x - width / 2, m_vPosition.x + width / 2.f,
	//					  m_vPosition.y - height / 2.f, m_vPosition.y + height / 2.f);

	m_bounds.m_fLeft = 0;
	m_bounds.m_fRight = width;
	m_bounds.m_fTop = 0;
	m_bounds.m_fBottom = height;
	
}

/*****************************************************************
* Render() - Draws the ui element to the screen
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
/*virtual*/ void CUIOxygenGauge::Render(void) /*override*/
{
	if (m_bVisible)
	{
		CHUDRenderer::Render2D(m_pBackTexture);
		CUIElement::Render();
	}

	

	/*if (m_bVisible)
	{
		float fOxygen = m_pPlayer->GetOxygen();
		float ammount = fOxygen / 100.f;

		float oxygenRange = m_fOxygenRotEnd - m_fOxygenRotStart;
		float oxyRotation = m_fOxygenRotStart + oxygenRange * ammount;

		m_pNeedleTexture->Rotate(oxyRotation);
		m_pNeedleTexture->SetScreenPosition(m_vPosition.x, m_vPosition.y);

		CRectangle needleRect = CGUIManager::ComputeBounds(CRectangle(0.f, 116.f, 0.f, 18.f));
		m_pNeedleTexture->SetWidth(needleRect.ComputeWidth());
		m_pNeedleTexture->SetHeight(needleRect.ComputeHeight());
		CHUDRenderer::Render2D(m_pNeedleTexture);

		if (fOxygen != m_fOxygenPrev)
		{
			CRectangle lightRect = CGUIManager::ComputeBounds(CRectangle(0.f, 12.f, 0.f, 12.f));
			m_pLightTexture->SetWidth(lightRect.ComputeWidth());
			m_pLightTexture->SetHeight(lightRect.ComputeHeight());
			m_pLightTexture->SetScreenPosition(m_vPosition.x, m_vPosition.y - 0.045f);

			CHUDRenderer::Render2D(m_pLightTexture);
		}

		m_fOxygenPrev = fOxygen;
	}*/
}