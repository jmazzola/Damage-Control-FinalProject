/***********************************************
* Filename:			UIAmmoCounter.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:
************************************************/

#include "MeltdownCounter.h"
#include "../Renderer/BitmapFont.h"
#include "../Renderer/Texture2D.h"
#include "../Renderer/Renderer.h"
#include "../Entity Manager/Entities/Subsystems/Reactor.h"
#include "../Renderer/HUDRenderer.h"
#include "../GUI Manager/GUIManager.h"

#include <string>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

/*****************************************************************
* CMeltdownCounter()
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
CMeltdownCounter::CMeltdownCounter(CReactor* pReactor)
{
	m_pReactor = pReactor;
	//m_pTexture = new CTexture2D(CRenderer::GetReference().GetWidth( ) * 0.5f, 150.f, 250.f, 10.f, L"Assets/Images/uiSystemHealthBack.jpg");

	m_bounds = CRectangle(m_vPosition.x - 45, m_vPosition.x + 45, m_vPosition.y - 45, m_vPosition.y + 45);

}

/*****************************************************************
* ~CMeltdownCounter()
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
/*virtual*/ CMeltdownCounter::~CMeltdownCounter(void)
{
}

/*****************************************************************
* Update() - Updates the meltdown counter
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
/*virtual*/ void CMeltdownCounter::Update(float fDt) /*override*/
{
	/*int miliseconds = (int(m_pReactor->GetMeltdownTimer() * 1000) % 1000);
	if (miliseconds < 50)
		m_fTextSize = 1.5;
	else if (m_fTextSize < 1.f) m_fTextSize = 1.f;
	m_fTextSize -= fDt;*/
	CUIElement::Update(fDt);
}

/*****************************************************************
* Render() - renders the ui element
*s
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CMeltdownCounter::Render(void) /*override*/
{
	if (!m_pReactor->GetIsDestroyed( ))
		return;

	if (m_bVisible)
	{
		float meltdownTimer = m_pReactor->GetMeltdownTimer();
		int seconds = int(meltdownTimer);
		int miliseconds = (int(m_pReactor->GetMeltdownTimer() * 100) % 100);
		
		

		CBitmapFont::GetInstance()->DrawString(("Meltdown in " + to_string(seconds) + "." + to_string(miliseconds)).c_str(), m_vPosition.x, m_vPosition.y, (XMFLOAT4)DirectX::Colors::Red, m_fTextSize);
	}

	CUIElement::Render( );
}
