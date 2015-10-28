/***********************************************
* Filename:			UIButton.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#ifndef _CUIBUTTON_H_
#define _CUIBUTTON_H_

#include "UIElement.h"
#include <string>

class CUIButton :
	public CUIElement
{
	std::string m_szText;

	float m_fWidth;
	float m_fHeight;

	//Is the user pressing the button?
	bool m_bPressed = false;
	//Did the user change its state this frame
	bool m_bChanged = false;
	//Is the user is allowed to press the button?
	bool m_bEnabled = true;
	//Does it revert to unpressed next frame?
	bool m_bToggle = false;
	//Does the button render its text?
	bool m_bRenderText = true;

	bool  m_bSoundPlayed;

	//The color to set the button if it's disabled
	DirectX::XMFLOAT4 m_xmDisabledColor;
	//The color to set the button when it's toggled but not clicked
	DirectX::XMFLOAT4 m_xmToggledColor;
	//The color of the text
	DirectX::XMFLOAT4 m_xmTextColor;

public:
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
	CUIButton(std::wstring& textureFilename, float pos_x, float pos_y, float width, float height, std::string& text);

	CUIButton(float pos_x, float pos_y, float width, float height);
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
	virtual ~CUIButton();

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
	virtual void Update(float dt) override;

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
	virtual void Render(void) override;

	/***************
	*  Accessors
	***************/
	const std::string& GetText(void) const { return m_szText; }
	bool GetPressed(void) const { return m_bPressed; }
	bool GetChanged(void) const { return m_bChanged; }
	bool GetEnabled(void) const { return m_bEnabled; }
	bool GetToggle(void) const { return m_bToggle; }
	bool GetRenderText(void) const { return m_bRenderText; }

	DirectX::XMFLOAT4 GetToggledColor(void) const { return m_xmToggledColor; }
	DirectX::XMFLOAT4 GetTextColor(void) const { return m_xmTextColor; }

	/***************
	* Mutators
	***************/
	void SetText(std::string& value) { m_szText = value; }
	void SetPressed(bool bPressed) {m_bPressed = bPressed;}
	void SetEnabled(bool bEnabled) {m_bEnabled = bEnabled;}
	void SetToggle(bool bToggle) {m_bToggle = bToggle;}
	void SetRenderText(bool bRenderText) { m_bRenderText = bRenderText; }

	void SetToggledColor(DirectX::XMFLOAT4 xmToggledColor) { m_xmToggledColor = xmToggledColor; }
	void SetTextColor(DirectX::XMFLOAT4 xmTextColor) { m_xmTextColor = xmTextColor; }
};

#endif // _CUIBUTTON_H_
