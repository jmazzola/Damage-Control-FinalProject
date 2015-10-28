/***********************************************
* Filename:			UISubsystemIndicator.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#include "UISubsystemIndicator.h"
#include "../Entity Manager/Entities/Subsystems/Subsystem.h"
#include "../Entity Manager/Entities/Subsystems/LifeSupport.h"
#include "../Entity Manager/Entities/Subsystems/Engines.h"
#include "../Entity Manager/Entities/Subsystems/Reactor.h"
#include "../Renderer/HUDRenderer.h"
#include "../Input Manager/InputManager.h"
#include "../GUI Manager/GUIManager.h"
#include "../Renderer/BitmapFont.h"
#include "../State Machine/GameplayState.h"
#include "../Entity Manager/Entities/Player.h"
#include "../Entity Manager/EntityManager.h"
#include <DirectXMath.h>
#include <string>

#define SCALE 1.f
#define RIGHTLIMIT 1025.0f
#define LEFTLIMIT 930.0f

using namespace std;
using namespace DirectX;

/*****************************************************************
* CUISubsystemIndicator():
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
CUISubsystemIndicator::CUISubsystemIndicator(PSUBSYSTEM target)
{
	if (dynamic_cast<CLifeSupport*>(target))
	{
		m_pTexture = new CTexture2D(0.f, 0.f, 230.f, 70.f, L"Assets/Images/lifeSupport.png");
		//m_pBackTex = new CTexture2D(0.f, 0.f, 230.f, 70.f, L"Assets/Images/lifeSupportBack.png");
	}
	if (dynamic_cast<CReactor*>(target))
	{
		m_pTexture = new CTexture2D(0.f, 0.f, 230.f, 70.f, L"Assets/Images/reactor.png");
		//m_pBackTex = new CTexture2D(0.f, 0.f, 230.f, 70.f, L"Assets/Images/reactorBack.png");
	}
	if (dynamic_cast<CEngines*>(target))
	{
		m_pTexture = new CTexture2D(0.f, 0.f, 230.f, 70.f, L"Assets/Images/engines.png");
		//m_pBackTex = new CTexture2D(0.f, 0.f, 230.f, 70.f, L"Assets/Images/enginesBack.png");
	}

	m_bounds = CRectangle(m_vPosition.x - 140.f, m_vPosition.x + 140.f, m_vPosition.y - 36.f, m_vPosition.y + 36.f);
	
	m_pLightTex = new CTexture2D(0.f, 0.f, 230.f, 70.f, L"Assets/Images/ColorLight.png");
	m_pLightTex->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	CRectangle rect = { m_vPosition.x - 70.f, m_vPosition.x + 70.f, m_vPosition.y - 18.f, m_vPosition.y + 18.f };
	rect = CGUIManager::ComputeBounds(rect);
	m_pLightTex->SetWidth(rect.ComputeWidth());
	m_pLightTex->SetHeight(rect.ComputeHeight());

	m_pSubsystem = target;

	m_bSpaceToggled = false;
	m_fPositionMod = 0;
}

/*****************************************************************
* ~CUISubsystemIndicator():
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
CUISubsystemIndicator::~CUISubsystemIndicator( )
{
	SafeDelete(m_pLightTex);
}

/*****************************************************************
* Update() - updates the subsystem
*
* Ins: dt - the ammount of time, in seconds, since the last update
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CUISubsystemIndicator::Update(float dt) /*override*/
{
	float fCurrentHealth = m_pSubsystem->GetCurrentHealth( );
	float fMaxHealth = m_pSubsystem->GetMaxHealth( );

	/*if (m_pSubsystem->GetIsDestroyed())
		m_pTexture->SetColor({ 1.0f, 1.0f, 1.0f, .1f });
	else
		m_pTexture->SetColor({ 1.0f, 1.0f, 1.0f, 1.f });*/

	if (CInputManager::GetInstance()->IsButtonDown(eControls::Enter) && CInputManager::GetInstance()->IsButtonDown(eControls::LeftControl))
		m_pSubsystem->SetHealth(fCurrentHealth - 50.f * dt);
	if (CInputManager::GetInstance()->IsButtonDown(eControls::RightShift) && CInputManager::GetInstance()->IsButtonDown(eControls::LeftControl))
		m_pSubsystem->SetHealth(fCurrentHealth + 50.f * dt);

	if (m_pSubsystem->GetCurrentHealth() < 0)
		m_pSubsystem->SetHealth(0);

	if (m_pSubsystem->GetCurrentHealth() > m_pSubsystem->GetMaxHealth())
		m_pSubsystem->SetHealth(m_pSubsystem->GetMaxHealth());
	
	
	DirectX::XMFLOAT2 scale = m_pTexture->GetScale();
	/*
	if (fCurrentHealth < m_fHealthPrev)
		scale.x = scale.y = 2.f;
	*/
	scale.x -= dt;
	scale.y -= dt;
	
	if (scale.x < SCALE)
		scale.x = SCALE;
	if (scale.y < SCALE)
		scale.y = SCALE;

	m_pTexture->Scale(scale.x, scale.y);
	

	// Translation stuff goes here...
	if (fCurrentHealth < 100.0f)
	{
		//m_pTexture->Translate(0, 0);
		m_bMoveLeft = true;
		m_fTransTimer = 2.0f;
	}

	if (CInputManager::GetInstance()->IsKeyPressed(eControls::Spacebar))
	{
		m_bSpaceToggled = !m_bSpaceToggled;
	}

	if (m_bSpaceToggled)
	{
		m_bMoveLeft = true;
		m_fTransTimer = 0.0f;
	}

	float number = (120.0f * dt);

	// moving to the left
	if (m_bMoveLeft && dt > 0)
	{
		if (m_vPosition.x > LEFTLIMIT)
		{
			//m_fPositionMod += number;
			m_vPosition.x -= number;
			if (m_vPosition.x < LEFTLIMIT)
			{
				m_vPosition.x = LEFTLIMIT;
			}
		}
		else
		{
			m_fTransTimer -= dt;
			if (m_fTransTimer <= 0.0f)
			{
				m_bMoveLeft = false;
			}
		}
	}
	else // moving to the right... hopefully
	{
		if (m_vPosition.x < RIGHTLIMIT)
		{
			//m_fPositionMod -= number;
			m_vPosition.x += number;
			if (m_vPosition.x > RIGHTLIMIT)
			{
				m_vPosition.x = RIGHTLIMIT;
			}
		}
		m_fTransTimer = 0.0f;
	}

	//m_pBackTex->SetScreenPosition(m_vPosition.x, m_vPosition.y);
	m_pTexture->SetScreenPosition(m_vPosition.x, m_vPosition.y + 100.f);
	
	float x = (((m_vPosition.x) / 1000) - .5f) * 2.f;
	float y = -(((m_vPosition.y) / 800) - .5f) * 2.f;

	m_pLightTex->SetScreenPosition(x, y);
	//m_pTexture->SetWidth((fCurrentHealth / fMaxHealth) * 200.f);


	m_fHealthPrev = fCurrentHealth;
	CUIElement::Update(dt);
}

/*****************************************************************
* Render() - renders the indicator to the screen
*
* Ins: dt - the ammount of time, in seconds, since the last update
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CUISubsystemIndicator::Render(void) /*override*/
{
	//if (m_bVisible)
	//{
	//	CRectangle rect = CGUIManager::ComputeBounds(CRectangle(0, 230, 0, 70));

	//	m_pBackTex->SetWidth(rect.ComputeWidth());
	//	m_pBackTex->SetHeight(rect.ComputeHeight());

	//	m_pBackTex->SetScreenPosition(m_vPosition.x, m_vPosition.y);

	//	CHUDRenderer::Render2D(m_pBackTex);
	//}
	
	if (m_bVisible)
	{
		if (m_pSubsystem->GetCurrentHealth() < 100.f)
		{
			XMVECTOR subsystemPos = XMLoadFloat3(&m_pSubsystem->GetPosition());
			XMVECTOR playerPos = XMLoadFloat3(&CSubSystem::g_pGamePlayState->GetEntityManager()->GetPlayer()->GetPosition());

			XMVECTOR length = XMVectorSubtract(subsystemPos, playerPos);
			length = XMVector3Length(length);
			XMFLOAT3 distance;
			XMStoreFloat3(&distance, length);
			if (distance.x < m_pSubsystem->GetRepairRange())
				CBitmapFont::GetInstance()->DrawString("Press \'E\' to repair", -0.3f, 0.f);
		}

		CUIElement::Render( );

		CBitmapFont::GetInstance()->DrawString(to_string(int(m_pSubsystem->GetCurrentHealth())).c_str(), m_vPosition.x - .04f, m_vPosition.y - .04f, (DirectX::XMFLOAT4)DirectX::Colors::White, m_pTexture->GetScale().x);
		
		if (m_pSubsystem->GetCurrentHealth() > 75.0f)
		{
			m_pLightTex->SetColor((DirectX::XMFLOAT4)(DirectX::Colors::Green));
		}
		else if (m_pSubsystem->GetCurrentHealth() > 25.0f)
		{
			m_pLightTex->SetColor((DirectX::XMFLOAT4)(DirectX::Colors::Yellow));
		}
		else
		{
			m_pLightTex->SetColor((DirectX::XMFLOAT4)(DirectX::Colors::Red));
		}
		
		CHUDRenderer::Render2D(m_pLightTex);
	}
}
