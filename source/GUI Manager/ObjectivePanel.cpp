/***********************************************
* Filename:			ObjectivePanel.h
* Date:      		07/25/2015
* Mod. Date:
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:			Handles the objective panel
************************************************/

#include "ObjectivePanel.h"
#include "GUIManager.h"
#include "../Renderer/Texture2D.h"
#include "../Renderer/HUDRenderer.h"

CObjectivePanel::CObjectivePanel()
{
	m_pTexture = new CTexture2D(0.f, 0.f, 200.f, 90.f, L"Assets/Images/Menus/Main_Menu_Border_v4.png");

	m_bounds.m_fLeft = 0;
	m_bounds.m_fRight = 260;
	m_bounds.m_fTop = 0;
	m_bounds.m_fBottom = 150;

	//if (1 == type)//waifu dialog box
	//{
	//	m_pTexture->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f });
	//}
	//else
	//{
	//	m_pTexture->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f });
	//}
}


CObjectivePanel::~CObjectivePanel()
{
}

/*virtual*/ void CObjectivePanel::Update(float dt) /*override*/
{

}

/*virtual*/ void CObjectivePanel::Render(void) /*override*/
{
	if (true == m_bVisible)
	{
		CUIElement::Render();
	}
}
