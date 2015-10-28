/***********************************************
* Filename:			HealthBar.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#include "HealthBar.h"
#include "../Entity Manager/LivingEntites.h"
#include "../Renderer/Renderer.h"
#include <DirectXMath.h>
#include "../Renderer/HUDRenderer.h"
#include "../GUI Manager/GUIManager.h"
#include "../Audio Manager/AudioSystemWwise.h"

using namespace DirectX;

/*****************************************************************
* CHealthBar():
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
CHealthBar::CHealthBar(float fMaxHealth) : m_fMaxHealth(fMaxHealth), m_fHealth(fMaxHealth)
{
	//m_pTexture = new CTexture2D(0.f, 0.f, 250.f, 50.f, L"Assets/Images/healthBar.dds");
	m_pTexture = new CTexture2D(100, 300.0f, 250.f, 50.f, L"Assets/Images/RedBar.png");
	m_pTexture->SetColor(XMFLOAT4(1, 0, 0, 1));

	m_pOverlayTexture = new CTexture2D(100, 300.0f, 250.f, 50.f, L"Assets/Images/BarOverlayThin.png");
	
	CRectangle rect = { 0.f, 310.0f, 0.f, 144.0f };
	rect = CGUIManager::ComputeBounds(rect);
	m_pOverlayTexture->SetWidth(rect.ComputeWidth());
	m_pOverlayTexture->SetHeight(rect.ComputeHeight());
	m_pOverlayTexture->SetScreenPosition(0.731f, -0.810f);


	//m_pBackTexture = new CTexture2D(CRenderer::GetReference().GetWidth() * .5f, 300.f, 250.f, 50.f, L"Assets/Images/RedBar.png");
	//m_pBackTexture->SetColor(DirectX::XMFLOAT4 { 0.f, 0.f, 0.f, 1.f });
}

/*****************************************************************
* ~CHealthBar():
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
CHealthBar::~CHealthBar( )
{
	//delete (m_pBackTexture);
	//m_pBackTexture = NULL;
	SafeDelete(m_pOverlayTexture);
}

/*****************************************************************
* Update() - updates the health bar
*
* Ins: dt - the ammount of time that has elapsed since the last update
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CHealthBar::Update(float fDt) /*override*/
{
	CUIElement::Update(fDt);

	if (m_pTgt)
		m_fHealth = m_pTgt->GetCurrentHealth( );

	float width = (m_fHealth / m_fMaxHealth) * 348;
	float height = 44.0f;
	//m_bounds = CRectangle(m_vPosition.x - width / 2, m_vPosition.x + width / 2.f,
	//					  m_vPosition.y - height / 2.f, m_vPosition.y + height / 2.f);

	m_bounds.m_fLeft = 0;
	m_bounds.m_fRight = width;
	m_bounds.m_fTop = 0;
	m_bounds.m_fBottom = height;

	// Flashing health bar

	/*
	if (fCurrentHealth < 100.0f)
	{
		//m_pTexture->Translate(0, 0);
		m_bMoveLeft = true;
		m_fTransTimer = 2.0f;
	}
	*/
	if (m_fColorTimer > 0)
	{
		m_fColorTimer -= fDt;
	}

	if ((m_fHealth / m_fMaxHealth) < 0.45f && m_fColorTimer <= 0)
	{
		m_fColorTimer = 0;
		m_bColorChange = true;
	}

	if (m_bColorChange && m_fColorTimer <= 0.0f)
	{
		XMFLOAT4 color = m_pTexture->GetColor();
		if (color.x > 0.0f)
		{
			color.x -= fDt * 2;
			color.y += fDt * 2;

			if (color.x < 0.0f)
			{
				color.x = 0.0f;
			}
			if (color.y > 1.0f)
			{
				color.y = 1.0f;
			}
			m_pTexture->SetColor(color);
		}
		else
		{
			m_bColorChange = false;
			m_fColorTimer = 0.3f;
			if (m_pTgt->GetAlive())
			{
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_LOW_HEALTH);
			}

		}
	}
	else
	{
		XMFLOAT4 color = m_pTexture->GetColor();
		if (color.y > 0.0f)
		{
			color.x += fDt * 2;
			color.y -= fDt * 2;

			if (color.x > 1.0f)
			{
				color.x = 1.0f;
			}
			if (color.y < 0.0f)
			{
				color.y = 0.0f;
			}

			m_pTexture->SetColor(color);
		}
	}

	
}

/*****************************************************************
* Render() - renders the element to the screen
*
* Ins: dt - the ammount of time that has elapsed since the last update
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CHealthBar::Render(void) /*override*/
{
	CUIElement::Render();
	if (m_bVisible)
	{
		/*//CRectangle rect = { 0.f, 280.f, 0.f, 160.f };
		//rect = CGUIManager::ComputeBounds(rect);

		//m_pBackTexture->SetWidth(rect.ComputeWidth());
		//m_pBackTexture->SetHeight(rect.ComputeHeight());
		//m_pBackTexture->SetScreenPosition(m_vPosition.x, m_vPosition.y);

		//CHUDRenderer::Render2D(m_pBackTexture);*/
		CHUDRenderer::Render2D(m_pOverlayTexture);
	}


}
