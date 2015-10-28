/***********************************************
* Filename:			UIElement.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:
************************************************/

#ifndef _CUIELEMENT_H_
#define _CUIELEMENT_H_

#include "..\Core\stdafx.h"
#include "..\Renderer\Texture2D.h"
#include "..\Math\vec3.h"
#include "Rectangle.h"

class CScene;

class CUIElement
{
protected:
	mutable vec3f m_vPosition;

	DirectX::XMFLOAT4 m_hoverColor = (DirectX::XMFLOAT4)DirectX::Colors::DarkGray;
	DirectX::XMFLOAT4 m_ClickColor = (DirectX::XMFLOAT4)DirectX::Colors::Red;
	DirectX::XMFLOAT4 m_baseColor = (DirectX::XMFLOAT4)DirectX::Colors::White;

	mutable CRectangle m_bounds;

	CTexture2D* m_pTexture = nullptr;

	// indicates that the mouse is currently hovering over the element
	bool m_bMouseHover;
	// indicates that the element is visible
	bool m_bVisible = true;
public:
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
	CUIElement( );

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
	virtual ~CUIElement( );

	/*****************************************************************
	* Update(): Updates the ui element
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
	virtual void Update(float dt);

	/*****************************************************************
	* Draw(): Renders the element to the screen
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
	virtual void Render(void);

	/*****************************************************************
	* LoadTexture(): Loads a texture from file to be rendered on the
	*				element
	* Ins:		svFileName
	*
	* Outs:		void
	*
	* Returns:	bool
	*
	* Mod. Date:		      06/04/2015
	* Mod. Initials:		MJG
	*****************************************************************/
	virtual bool LoadTexture(const wchar_t* svFileName);

	/***************
	*  Accessors
	***************/
	bool GetMouseHover(void) const
	{
		return m_bMouseHover;
	}

	CRectangle& GetBounds(void) const
	{
		return m_bounds;
	}

	vec3f& GetPosition(void) const
	{
		return m_vPosition;
	}

	bool GetVisible(void) const
	{
		return m_bVisible;
	}
	CTexture2D* GetTexture( )
	{
		return m_pTexture;
	}

	/***************
	* Mutators
	***************/

	virtual void SetPosition(vec3f& value);
	virtual void SetWidth(float fWidth);
	virtual void SetHeight(float fHeight);

	//Only use if textures being used are held/managed elsewhere
	void SetTexture(TTexture2D* pTexture)
	{
		if (m_pTexture == nullptr)
			m_pTexture = new CTexture2D( );
		m_pTexture->SetTexture(pTexture);
	}
	void SetVisible(bool value)
	{
		m_bVisible = value;
	}

	void SetColor(DirectX::XMFLOAT4 _XMColor)
	{
		if (m_pTexture != NULL)
		{
			m_pTexture->SetColor(_XMColor);

			if (_XMColor.w < 0.0f)
			{
				SetVisible(false);
			}
		}
	}
	DirectX::XMFLOAT4 GetColor( ) const
	{
		if (m_pTexture)
		{
			return m_pTexture->GetColor();
		}
		return{ 1, 1, 1, 0 };
	}
};

#endif // _CUIELEMENT_H_

