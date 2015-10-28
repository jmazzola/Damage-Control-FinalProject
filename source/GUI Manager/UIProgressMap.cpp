/***********************************************
* Filename:			UIProgressMap.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#include "UIProgressMap.h"
#include "GUIManager.h"
#include "../Renderer/Texture2D.h"
#include "../Renderer/HUDRenderer.h"
#include "../Entity Manager/Entities/Subsystems/Engines.h"
//#include "../Input Manager/InputManager.h"

/*****************************************************************
* CUIProgressMap():
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
CUIProgressMap::CUIProgressMap(CEngines* target) : m_fShipStart(300.f), m_fShipEnd(700.0f)
{
	m_pTexture = new CTexture2D(0.f, 0.f, 90.f, 470.f, L"Assets/Images/ProgressBar.png");
	m_pIconTexture = new CTexture2D(0.f, 0.f, 37.f, 69.f, L"Assets/Images/ShipGlowNoFire.png");
	m_pShipFire = new CTexture2D(0.f, 0.f, 18.f, 25.f, L"Assets/Images/ShipGlowFire.png");

	m_pEngines = target;
	m_fShipPositionX = 40.0f;

	m_bounds.m_fLeft = 0;
	m_bounds.m_fRight = 45;
	m_bounds.m_fTop = 0;
	m_bounds.m_fBottom = 450;

}

/*****************************************************************
* ~CUIProgressMap():
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
CUIProgressMap::~CUIProgressMap( )
{
	delete (m_pIconTexture);
	//delete (m_pShipDeadTexture);

	m_pIconTexture = NULL;
	//m_pShipDeadTexture = NULL;

	delete (m_pShipFire);

	m_pShipFire = NULL;
}

/*****************************************************************
* Update(): - updates the current progress of the ship
*
* Ins: dt - the ammount of time that has elapsed since the last update
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/19/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CUIProgressMap::Update(float dt) /*override*/
{
	CUIElement::Update(dt);

	m_pCurrentShipIcon = m_pIconTexture;
	if (!m_pEngines->GetAlive())
	{
		m_pCurrentShipIcon->SetColor({ 0.5f, 0.5f, 0.5f, 0.8f });

	}
	else
		m_pCurrentShipIcon->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	m_fProgress = m_pEngines->GetProgress( ) / m_pEngines->GetGoalDistance( );

	// small ship icon movement
	vec3f pos = CGUIManager::ComputeClipPosition(vec3f{ m_fShipPositionX, m_fShipPositionY, 0.f });
	CRectangle shipBounds = CRectangle(pos.x - .025f, pos.x + .025f, pos.y - .075f, pos.y + .075f);

	m_pCurrentShipIcon->SetScreenPosition(pos.x, pos.y);

	m_pCurrentShipIcon->SetWidth(shipBounds.ComputeWidth());
	m_pCurrentShipIcon->SetHeight(shipBounds.ComputeHeight());

}

/*****************************************************************
* Render(): - renders the progress map to the screen
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/19/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CUIProgressMap::Render(void) /*override*/
{
	CUIElement::Render( );

	float range = m_fShipEnd - m_fShipStart;
	m_fShipPositionY = m_fShipStart + range * m_fProgress;

	vec3f pos = CGUIManager::ComputeClipPosition(vec3f{ m_fShipPositionX, m_fShipPositionY, 0.f });
	CRectangle shipBounds = CRectangle(pos.x - .025f, pos.x + .025f, pos.y - .075f, pos.y + .075f);
	//shipBounds = CGUIManager::ComputeBounds(shipBounds);

	m_pCurrentShipIcon->SetScreenPosition(pos.x, pos.y);

	

	m_pCurrentShipIcon->SetWidth(shipBounds.ComputeWidth( ));
	m_pCurrentShipIcon->SetHeight(shipBounds.ComputeHeight( ));

	if (m_bVisible)
	{
		//ship's Thruster effect
		if (m_pEngines != nullptr && false == m_pEngines->GetStopped() && true == m_pEngines->GetAlive() )
		{
			float ratio = m_pEngines->GetHealthRatio();
			shipBounds = CRectangle(pos.x - .0125f * ratio, pos.x + .0125f * ratio, pos.y - .05f, pos.y + .03f  * ratio);
			m_pShipFire->SetWidth(shipBounds.ComputeWidth());
			m_pShipFire->SetHeight(shipBounds.ComputeHeight());
			m_pShipFire->SetScreenPosition(pos.x, pos.y - 0.075f);
			CHUDRenderer::Render2D(m_pShipFire);
		}

		CHUDRenderer::Render2D(m_pCurrentShipIcon);
	}

}
