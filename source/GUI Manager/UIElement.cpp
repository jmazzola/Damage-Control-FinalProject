/***********************************************
* Filename:			UIElement.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#include "UIElement.h"
#include "..\Input Manager\InputManager.h"
#include <memory>
#include "../Renderer/HUDRenderer.h"

/*****************************************************************
* CUIElement():
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
CUIElement::CUIElement()
{
	m_vPosition.x = m_vPosition.y = m_vPosition.z = 0;
	memset(&m_bounds, 0, sizeof(CRectangle));
	m_bMouseHover = false;
}

/*****************************************************************
* ~CUIElement():
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
CUIElement::~CUIElement()
{
	SafeDelete(m_pTexture);
}

/*****************************************************************
* Update():
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/13/2015
* Mod. Initials:
*****************************************************************/
void CUIElement::Update(float dt)
{
	CInputManager* pInput = CInputManager::GetInstance();
	vec3f& mousePos = pInput->GetMousePosition();

	HWND dis = GetActiveWindow();
	HWND other = GetForegroundWindow();

	bool isActiveWindow = (dis == other);

	if (isActiveWindow)
	{
		if (m_bounds.Contains(mousePos))
			m_bMouseHover = true;
		else
			m_bMouseHover = false;

		if (m_pTexture)
			m_pTexture->SetScreenPosition(m_vPosition.x, m_vPosition.y);
	}
}

/*****************************************************************
* Draw():
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/13/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CUIElement::Render(void)
{
	if (m_bVisible && m_pTexture)
	{
		m_pTexture->SetScreenPosition(m_vPosition.x, m_vPosition.y);
		m_pTexture->SetWidth(m_bounds.ComputeWidth());
		m_pTexture->SetHeight(m_bounds.ComputeHeight());

		CHUDRenderer::Render2D(m_pTexture);
	}
}

bool CUIElement::LoadTexture(const wchar_t* svFileName)
{
	if (svFileName == nullptr)
	{
		return false;
	}
	//Create a new texture
	CTexture2D* pNextTexture =
		new CTexture2D(m_vPosition.x, m_vPosition.y, abs(m_bounds.GetLeft() - m_bounds.GetRight()), abs(m_bounds.GetTop() - m_bounds.GetBottom()), svFileName);

	//Make sure we have a texture
	if (pNextTexture->RetrieveThyTexture() == nullptr)
	{
		SafeDelete(pNextTexture);
		return false;
	}
	//Clear out old texture
	SafeDelete(m_pTexture);

	//SUCCESS
	m_pTexture = pNextTexture;
	return true;
}

void CUIElement::SetPosition(vec3f& value)
{
	m_vPosition = value;
	if (m_pTexture)
	{
		m_pTexture->SetScreenPosition(value.x, value.y);
	}
}

void CUIElement::SetWidth(float fWidth)
{
	if (m_pTexture)
	{
		m_pTexture->SetWidth(fWidth);
	}
	m_bounds.SetWidth(fWidth);
}

void CUIElement::SetHeight(float fHeight)
{
	if (m_pTexture)
	{
		m_pTexture->SetHeight(fHeight);
	}
	m_bounds.SetHeight(fHeight);
}
