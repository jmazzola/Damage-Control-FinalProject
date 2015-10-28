/***********************************************
* Filename:			UIButton.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#include "UIButton.h"
#include "../Renderer/Renderer.h"
#include "../Input Manager/InputManager.h"
#include "../Renderer/BitmapFont.h"
#include <string>
#include "..\Audio Manager\AudioSystemWwise.h"

using namespace MathHelper;

/*****************************************************************
* CUIButton():
*
* Ins: textureFilename - the name of the texture to load for the back texture
*	   pos_x - the x position of the button
*	   pos_y - the y position of the button
*	   width - the width of the button
*	   height - the height of the button
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/28/2015
* Mod. Initials:
*****************************************************************/
CUIButton::CUIButton(std::wstring& szTextureFilename, float fPos_x, float fPos_y, float fWidth, float fHeight, std::string& szText)
{
	//Intended to get junk
	if (szTextureFilename == L"")
	{
		m_pTexture = new CTexture2D(fPos_x, fPos_y, fWidth, fHeight, L"Assets/Images/nothing.jpg");
	}
	else
	{
		m_pTexture = new CTexture2D(fPos_x, fPos_y, fWidth, fHeight, szTextureFilename);
	}

	m_bounds = { fPos_x - fWidth / 2.f, fPos_x + fWidth / 2.f, fPos_y - fHeight / 2.f, fPos_y + fHeight / 2.f };

	m_vPosition.x = fPos_x;
	m_vPosition.y = fPos_y;

	m_szText = szText;

	m_fWidth = fWidth;
	m_fHeight = fHeight;

	m_xmDisabledColor = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	m_xmToggledColor = DirectX::XMFLOAT4(1, 1, 0, 1);
	SetRenderText(false);

	m_xmTextColor = (DirectX::XMFLOAT4)DirectX::Colors::Black;

	m_bSoundPlayed = false;
}

CUIButton::CUIButton(float fPos_x, float fPos_y, float fWidth, float fHeight)
{
	m_pTexture = nullptr;
	m_bounds = { fPos_x - fWidth / 2.f, fPos_x + fWidth / 2.f, fPos_y - fHeight / 2.f, fPos_y + fHeight / 2.f };

	m_vPosition.x = fPos_x;
	m_vPosition.y = fPos_y;

	m_fWidth = fWidth;
	m_fHeight = fHeight;

	m_xmDisabledColor = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	m_xmToggledColor = DirectX::XMFLOAT4(1, 1, 0, 1);
	SetRenderText(false);

	m_xmTextColor = (DirectX::XMFLOAT4)DirectX::Colors::Black;

	m_bSoundPlayed = false;
}

/*****************************************************************
* ~CUIButton():
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
CUIButton::~CUIButton()
{
}

/*****************************************************************
* Update: Updates the button
*
* Ins: dt - the ammount of time that has elapsed since the last
*			frame
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/14/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CUIButton::Update(float dt) /*override*/
{
	bool bWasPressed = m_bPressed;
	//If we're disabled
	if (!m_bEnabled)
	{
		m_pTexture->SetColor(m_xmDisabledColor);
		return;
	}
	//Only store the old value if we're a toggle
	if (!m_bToggle)
	{
		m_bPressed = false;
	}
	//Hovering over the button
	if (m_bMouseHover)
	{
		if (false == m_bSoundPlayed && true == this->GetVisible())
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_BUTTON_HOVER);
			m_bSoundPlayed = true;
		}

		//The button is being clicked
		if (CInputManager::GetInstance()->IsButtonDown(eControls::MouseLeft))
		{
			//Keep the color changed as long as you click
			m_pTexture->SetColor(m_ClickColor);
		}
		//Only flip the bool on release
		else if (CInputManager::GetInstance()->WasKeyDown(eControls::MouseLeft))
		{
			m_bPressed = !m_bPressed;
		}
		//Mousing over a toggled button
		else if (m_bPressed && m_bToggle)
		{
			m_pTexture->SetColor((m_hoverColor + m_xmToggledColor) * 0.5f);
		}
		//A normal button is being hovered over
		else
		{
			m_pTexture->SetColor(m_hoverColor);
		}
	}
	//No one is near the the button
	else
	{
		//Toggled and pressed
		if (m_bPressed)
		{
			m_pTexture->SetColor(m_xmToggledColor);
		}
		//Normal button
		else
		{
			m_pTexture->SetColor(m_baseColor);
			if (true == m_bSoundPlayed && true == this->GetVisible())
			{
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_BUTTON_EXIT);
				m_bSoundPlayed = false;
			}
		}
	}
	//Did the press change this frame
	m_bChanged = bWasPressed != m_bPressed;

	CUIElement::Update(dt);
}

/*****************************************************************
* Update: Renders the button to the screen
*
* Ins: dt - the ammount of time that has elapsed since the last
*			frame
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/14/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CUIButton::Render(void) /*override*/
{
	if (m_bVisible)
	{
		CUIElement::Render();
		if (m_bRenderText)
		{
			CBitmapFont::GetInstance()->DrawString(m_szText.c_str(), m_vPosition.x - .23f, m_vPosition.y - .05f, m_xmTextColor);
		}
	}
}