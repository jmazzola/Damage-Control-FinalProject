/***********************************************
* Filename:			GUIManager.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:			GUIManager updates and renders
*					all components on the gui at
*					any one time
************************************************/

#include "GUIManager.h"
#include "UIElement.h"
#include "UIButton.h"
#include "UIProgressMap.h"
#include "UISubsystemIndicator.h"
#include "HealthBar.h"
#include "UIAmmoCounter.h"
#include "UIOxygenGauge.h"
#include "HudFlash.h"
#include "DefconManager.h"
#include "UIScrapCounter.h"
#include "MeltdownCounter.h"

#include "ObjectivePanel.h"

#include "../State Machine/GameplayState.h"
#include "../Entity Manager/EntityManager.h"
#include "../Entity Manager/Entities/Subsystems/LifeSupport.h"
#include "../Entity Manager/Entities/Subsystems/Reactor.h"
#include "../Entity Manager/Entities/Subsystems/Engines.h"
#include "..\TinyXML2\tinyxml2.h"
#include <string>

#include "../Radar.h"

#include "../Renderer/Renderer.h"

using namespace std;
using namespace tinyxml2;

#define FLICKER_DELAY_MIN 0.1f
#define FLICKER_DELAY_MAX 1.0f

#define FLICKER_ALPHA_MIN 0.2f
#define FLICKER_ALPHA_MAX 0.8f

/*****************************************************************
* GUIManager():
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
CGUIManager::CGUIManager( )
{ }

/*****************************************************************
* ~GUIManager():
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
CGUIManager::~CGUIManager( )
{
	for (size_t i = 0; i < m_elements.size( ); i++)
	{
		SafeDelete(m_elements[i]);
	}
	SafeDelete(m_pRadar);
}

/*****************************************************************
* Load: Loads ui elements out of the xml doc at 'filename' and
*		  places them in the ui
*
* Ins: filename - the name of the xml document to load
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/14/2015
* Mod. Initials:
*****************************************************************/
bool CGUIManager::LoadMe(std::string& filename, CGameplayState* pGameplayState)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename.c_str( ));

	XMLElement* root = doc.FirstChildElement( );
	XMLElement* itr = root->FirstChildElement( );

	while (itr)
	{
		std::string szTypeName = itr->Name( );
		if (szTypeName == "Button")
		{
			// find all button relevent nodes
			XMLElement* pTextNode = itr->FirstChildElement("Text");
			XMLElement* pPositionNode = itr->FirstChildElement("Position");

			// extract position data from position node
			std::string x_attr = pPositionNode->Attribute("x");
			std::string y_attr = pPositionNode->Attribute("y");

			std::string text = pTextNode->GetText( );

			// parse button position
			vec3f position;
			memset(&position, 0, sizeof(vec3f));
			position.x = (float)atof(x_attr.c_str( ));
			position.y = (float)atof(y_attr.c_str( ));

			CUIButton* pButton = new CUIButton(std::wstring(L"Assets/Images/mainMenuButton.png"), position.x, position.y, 285.f, 128.f, text);
			pButton->SetPosition(position);
			pButton->SetRenderText(true);
			AddElement(pButton);
		}
		//Maybe some other time
		else if (szTypeName == "Element")
		{/*
			// find all button relevent nodes
			XMLElement* pTextNode = itr->FirstChildElement("Text");
			XMLElement* pFileNameNode = itr->FirstChildElement("FileName");
			XMLElement* pPositionNode = itr->FirstChildElement("Position");
			XMLElement* pBoundsNode = itr->FirstChildElement("Bounds");

			// extract position data from position node
			std::string x_attr = pPositionNode->Attribute("x");
			std::string y_attr = pPositionNode->Attribute("y");

			std::string text = pTextNode->GetText();
			std::string fileName = pFileNameNode->GetText();

			// parse button position
			DirectX::XMFLOAT4 position;
			memset(&position, 0, sizeof(DirectX::XMFLOAT4));
			position.x = (float)atof(x_attr.c_str());
			position.y = (float)atof(y_attr.c_str());

			x_attr = pBoundsNode->Attribute("Width");
			y_attr = pBoundsNode->Attribute("Height");

			position.z = (float)atof(x_attr.c_str());
			position.w = (float)atof(y_attr.c_str());

			CUIElement* pElement = new CUIElement();
			pElement->LoadTexture(fileName.c_str());
			pElement->SetPosition(vec3f(position.x, position.y, 0));
			pElement->SetWidth(position.z);
			pElement->SetHeight(position.w);
			outval->AddElement(pElement);*/
		}
		else if (szTypeName == "ProgressMap")
		{
			XMLElement* pPositionNode = itr->FirstChildElement("Position");

			float fPositionX = (float)atof(pPositionNode->Attribute("x"));
			float fPositionY = (float)atof(pPositionNode->Attribute("y"));

			CEngines* pEngines = nullptr;
			list<CSubSystem*> subSystems = pGameplayState->GetEntityManager( )->GetSubSystems( );
			for (auto i = subSystems.begin( ); i != subSystems.end( ); i++)
			{
				if (dynamic_cast<CEngines*>((*i)))
					pEngines = (CEngines*)*i;
			}

			m_pProgressMap = new CUIProgressMap(pEngines);
			m_pProgressMap->SetPosition(vec3f{ fPositionX, fPositionY, 0.f });
			AddElement(m_pProgressMap);


		}
		else if (szTypeName == "SubsystemIndicator")
		{
			XMLElement* pTarget = itr->FirstChildElement("TargetSystem");
			XMLElement* pPositionNode = itr->FirstChildElement("Position");

			float fPositionX = (float)atof(pPositionNode->Attribute("x"));
			float fPositionY = (float)atof(pPositionNode->Attribute("y"));

			std::string target = pTarget->GetText( );

			CSubSystem* pSubSystem = nullptr;
			list<CSubSystem*> subSystems = pGameplayState->GetEntityManager( )->GetSubSystems( );
			for (auto i = subSystems.begin( ); i != subSystems.end( ); i++)
			{
				if (target == "Engines" && dynamic_cast<CEngines*>((*i)))
				{
					pSubSystem = *i;
				}
				if (target == "LifeSupport" && dynamic_cast<CLifeSupport*>((*i)))
				{
					pSubSystem = *i;
				}
				if (target == "Reactor" && dynamic_cast<CReactor*>((*i)))
				{
					pSubSystem = *i;
					m_pReactor = static_cast<CReactor*>((*i));
				}
			}

			CUISubsystemIndicator* indicator = new CUISubsystemIndicator(pSubSystem);
			indicator->SetPosition(vec3f { fPositionX, fPositionY, 0.f });
			AddElement(indicator);
		}
		else if (szTypeName == "AmmoCounter")
		{
			XMLElement* pPositionNode = itr->FirstChildElement("Position");

			float fPosX = (float)atof(pPositionNode->Attribute("x"));
			float fPosY = (float)atof(pPositionNode->Attribute("y"));

			CUIAmmoCounter* uiCounter = new CUIAmmoCounter(pGameplayState->GetEntityManager( )->GetPlayer( ));
			uiCounter->SetPosition(vec3f { fPosX, fPosY, 0.f });
			AddElement(uiCounter);
		}
		else if (szTypeName == "HealthBar")
		{
			XMLElement* pPosition = itr->FirstChildElement("Position");
			XMLElement* pBounds = itr->FirstChildElement("Bounds");

			float fPosX = (float)atof(pPosition->Attribute("x"));
			float fPosY = (float)atof(pPosition->Attribute("y"));

			float fWidth = (float)atof(pBounds->Attribute("Width"));
			float fHeight = (float)atof(pBounds->Attribute("Height"));

			CHealthBar* pHealthBar = new CHealthBar(100.f);
			pHealthBar->SetTarget(pGameplayState->GetEntityManager( )->GetPlayer( ));
			pHealthBar->SetPosition(vec3f { fPosX, fPosY, 0.f });
			pHealthBar->GetBounds( ) = CRectangle(fPosX - fWidth / 2.f, fPosX + fWidth / 2.f, fPosY - fHeight / 2.f, fPosX + fHeight / 2.f);
			AddElement(pHealthBar);
		}
		else if (szTypeName == "OxygenGauge")
		{
			XMLElement* pPositionNode = itr->FirstChildElement("Position");

			float fPosX = (float)atof(pPositionNode->Attribute("x"));
			float fPosY = (float)atof(pPositionNode->Attribute("y"));

			CUIOxygenGauge* pOxyGuage = new CUIOxygenGauge(pGameplayState->GetEntityManager( )->GetPlayer( ));
			pOxyGuage->SetPosition(vec3f { fPosX, fPosY, 0.f });
			AddElement(pOxyGuage);
		}
		else if (szTypeName == "ObjectivePanel")
		{
			XMLElement* pPositionNode = itr->FirstChildElement("Position");

			float fPosX = (float)atof(pPositionNode->Attribute("x"));
			float fPosY = (float)atof(pPositionNode->Attribute("y"));

			CObjectivePanel* pObjective = new CObjectivePanel( );
			pObjective->SetPosition(vec3f { fPosX, fPosY, 0.f });
			AddElement(pObjective);
		}
		else if (szTypeName == "WaifuDialog")
		{
			XMLElement* pPositionNode = itr->FirstChildElement("Position");

			float fPosX = (float)atof(pPositionNode->Attribute("x"));
			float fPosY = (float)atof(pPositionNode->Attribute("y"));

			m_pSubtitle = new CObjectivePanel( );
			m_pSubtitle->SetPosition(vec3f { fPosX, fPosY, 0.f });
			m_pSubtitle->SetColor(DirectX::XMFLOAT4 { 1, 1, 1, 0.5f });
			m_pSubtitle->SetWidth(500);
			m_pSubtitle->SetHeight(140);
			AddElement(m_pSubtitle);
		}
		else if (szTypeName == "ScrapCounter")
		{
			XMLElement* pPositionNode = itr->FirstChildElement("Position");

			float fPosX = (float)atof(pPositionNode->Attribute("x"));
			float fPosY = (float)atof(pPositionNode->Attribute("y"));

			CUIScrapCounter* pScrapCounter = new CUIScrapCounter(pGameplayState->GetEntityManager( )->GetPlayer( ));

			pScrapCounter->SetPosition(vec3f { fPosX, fPosY, 0.f });
			AddElement(pScrapCounter);
		}
		else if (szTypeName == "MeltdownCounter")
		{
			XMLElement* pPositionNode = itr->FirstChildElement("Position");

			float fPosX = (float)atof(pPositionNode->Attribute("x"));
			float fPosY = (float)atof(pPositionNode->Attribute("y"));

			list<CSubSystem*> subsystems = pGameplayState->GetEntityManager( )->GetSubSystems( );
			CReactor* pReactor = nullptr;
			for (auto i = subsystems.begin( ); i != subsystems.end( ); i++)
				if (dynamic_cast<CReactor*>(*i))
				{
					pReactor = (CReactor*)*i;
					break;
				}

			CMeltdownCounter* pMeltdownCounter = new CMeltdownCounter(pReactor);

			pMeltdownCounter->SetPosition(vec3f { fPosX, fPosY, 0.f });
			AddElement(pMeltdownCounter);
		}

		itr = itr->NextSiblingElement( );
	}

	//this was in the above loop, which caused ~18 items to be in the HUD.
	//	Here, there are only 10 objects [oh ho ho! can't have that]
	//it seems there was one roughly every other hud object.
	// --David
	/*if (pGameplayState)
	{
	CHudFlash* flash = new CHudFlash(pGameplayState->GetEntityManager( )->GetPlayer( ));
	AddElement(flash);
	}*/

	if (pGameplayState)
	{
		m_pRadar = new CRadar( );
	}
	return true;
}

CGUIManager* CGUIManager::Load(std::string& filename, CGameplayState* pGameplayState)
{
	CGUIManager* outVal = new CGUIManager( );
	outVal->LoadMe(filename, pGameplayState);
	return outVal;
}


/*****************************************************************
* GetElementAt: Returns the ui element at the provided point
*
* Ins: position - the position to test for a ui element
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/14/2015
* Mod. Initials:
*****************************************************************/
CUIElement* CGUIManager::GetElementAt(vec3f& position)
{
	for (unsigned int i = 0; i < m_elements.size( ); i++)
	{
		CRectangle bounds = m_elements[i]->GetBounds( );

		// find clip space bounds
		float left = (bounds.GetLeft( ) / GUI_WIDTH) * CRenderer::GetReference( ).GetWidth( );
		float right = (bounds.GetRight( ) / GUI_WIDTH) * CRenderer::GetReference( ).GetWidth( );
		float top = (bounds.GetTop( ) / GUI_HEIGHT) * CRenderer::GetReference( ).GetHeight( );
		float bottom = (bounds.GetBottom( ) / GUI_HEIGHT) * CRenderer::GetReference( ).GetHeight( );

		m_elements[i]->GetBounds( ) = CRectangle(left, right, top, bottom);
		if (m_elements[i]->GetBounds( ).Contains(position))
		{
			m_elements[i]->GetBounds( ) = bounds;
			return m_elements[i];
		}
		m_elements[i]->GetBounds( ) = bounds;
	}

	return nullptr;
}

unsigned int CGUIManager::GetElementsAt(vec3f& vPosition, std::vector<CUIElement*>& pElements)
{
	unsigned int nElementsFound = 0;
	//Check each element in the manager
	for (unsigned int i = 0; i < m_elements.size( ); i++)
	{
		CRectangle bounds = m_elements[i]->GetBounds( );

		// find clip space bounds
		float left = (bounds.GetLeft( ) / GUI_WIDTH) * CRenderer::GetReference( ).GetWidth( );
		float right = (bounds.GetRight( ) / GUI_WIDTH) * CRenderer::GetReference( ).GetWidth( );
		float top = (bounds.GetTop( ) / GUI_HEIGHT) * CRenderer::GetReference( ).GetHeight( );
		float bottom = (bounds.GetBottom( ) / GUI_HEIGHT) * CRenderer::GetReference( ).GetHeight( );

		m_elements[i]->GetBounds( ) = CRectangle(left, right, top, bottom);
		if (m_elements[i]->GetBounds( ).Contains(vPosition))
		{
			//Add the element to our return list, increment the counter
			pElements.push_back(m_elements[i]);
		}
		m_elements[i]->GetBounds( ) = bounds;
	}
	return nElementsFound;
}

/*****************************************************************
* Update: Updates all elements
*
* Ins: dt - the ammount of time, in seconds, that has passed since the last update
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/14/2015
* Mod. Initials:
*****************************************************************/
#include "../AchieveTracker.h"
void CGUIManager::Update(float dt)
{
	if (!m_bVisible)
	{
		return;
	}
	for (unsigned int i = 0; i < m_elements.size( ); i++)
	{
		if (!m_elements[i]->GetVisible())
		{
			continue;
		}
		if (dynamic_cast<CUIButton*>(m_elements[i]))
		{
			CRectangle bounds = m_elements[i]->GetBounds( );

			// find clip space bounds
			float left = (bounds.GetLeft( ) / GUI_WIDTH) * CRenderer::GetReference( ).GetWidth( );
			float right = (bounds.GetRight( ) / GUI_WIDTH) * CRenderer::GetReference( ).GetWidth( );
			float top = (bounds.GetTop( ) / GUI_HEIGHT) * CRenderer::GetReference( ).GetHeight( );
			float bottom = (bounds.GetBottom( ) / GUI_HEIGHT) * CRenderer::GetReference( ).GetHeight( );

			m_elements[i]->GetBounds( ) = CRectangle(left, right, top, bottom);

			m_elements[i]->Update(dt);

			m_elements[i]->GetBounds( ) = bounds;
		}
		else m_elements[i]->Update(dt);
	}
	if (m_pRadar)
	{
		m_pRadar->Update(dt);
	}

	CAchieveTracker::GetInstance()->Update(dt);

	m_fFlickerTimer -= dt;
}

/*****************************************************************
* Render: Renders all elements to the screen
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/14/2015
* Mod. Initials:
*****************************************************************/
void CGUIManager::Render(void)
{
	//Don't render a thing if we're set to invisible
	if (!m_bVisible)
	{
		return;
	}
	CRenderer::GetReference( ).BeginEvent(L"HUD Render");

	float fAlphaModifier = 1;
	if (m_pReactor && !m_pReactor->GetAlive() && m_fFlickerTimer <= 0.0f)
	{
		auto randF = [&](float lower_bound, float uppder_bound) -> float
		{
			return lower_bound + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (uppder_bound - lower_bound)));
		};
		fAlphaModifier = randF(FLICKER_ALPHA_MIN, FLICKER_ALPHA_MAX);
		m_fFlickerTimer = randF(FLICKER_DELAY_MIN, FLICKER_DELAY_MAX);
	}

	if (m_pRadar && SeeRadar)
	{
		m_pRadar->SetTransparency(fAlphaModifier);
		m_pRadar->Render( );
	}

	float width = GUI_WIDTH;
	float height = GUI_HEIGHT;

	for (unsigned int i = 0; i < m_elements.size( ); i++)
	{
		CUIElement* pElement = m_elements[i];
		//If this element is set to invisible, just skip it
		if (!pElement->GetVisible())
		{
			continue;
		}

		vec3f posPrev = pElement->GetPosition( );
		CRectangle boundsPrev = pElement->GetBounds( );

		// find clip space position
		float x = ((pElement->GetPosition( ).x / width) - .5f) * 2.f;
		float y = ((pElement->GetPosition( ).y / height) - .5f) * 2.f;

		CRectangle newBounds = ComputeBounds(boundsPrev);

		pElement->GetBounds( ) = newBounds;
		pElement->SetPosition(vec3f { x, -y, 0.f });

		if (m_pReactor)
		{
			DirectX::XMFLOAT4 xmElementColor = m_elements[i]->GetColor();
			xmElementColor.w = xmElementColor.w * fAlphaModifier;
			m_elements[i]->SetColor(xmElementColor);
			pElement->Render();
			xmElementColor.w = xmElementColor.w / fAlphaModifier;
			m_elements[i]->SetColor(xmElementColor);
		}
		else
		{
			pElement->Render();
		}

		pElement->SetPosition(posPrev);
		pElement->GetBounds( ) = boundsPrev;
	}

	// Rendering whether or not we got an achievement
	if (CAchieveTracker::GetInstance()->GetNotification())
	{
		CAchieveTracker::GetInstance()->RenderAchievement();
	}

	CRenderer::GetReference( ).EndEvent( );
}

/*****************************************************************
* Render: Renders all elements to the screen
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/14/2015
* Mod. Initials:
*****************************************************************/
void CGUIManager::SetVisible(bool value)
{
	m_bVisible = value;
	//for (unsigned int i = 0; i < m_elements.size( ); i++)
	//	m_elements[i]->SetVisible(value);
}

void CGUIManager::SetRadarRender(bool value)
{
	if (m_pRadar)
	{
		SeeRadar = value;
		m_pRadar->SetVisible(value);
	}
}

/*****************************************************************
* ComputeBounds: Rebuilds 'bounds' such that they are scaled
*				 for clip space
*
* Ins:	Bounds - the rectangle to rebuild
*
* Outs:
*
* Returns:	The rebuild rectangle
*
* Mod. Date:		      06/06/2015
* Mod. Initials:			MJG
*****************************************************************/
/*static*/ CRectangle CGUIManager::ComputeBounds(CRectangle& bounds)
{
	const float width = GUI_WIDTH;
	const float height = GUI_HEIGHT;

	// find clip space bounds
	float left = ((bounds.GetLeft( ) / width) - .5f) * 2.f;
	float right = ((bounds.GetRight( ) / width) - .5f) * 2.f;
	float top = ((bounds.GetTop( ) / height) - .5f) * 2.f;
	float bottom = ((bounds.GetBottom( ) / height) - .5f) * 2.f;

	return CRectangle(left, right, top, bottom);
}

/*****************************************************************
* ComputeClipPosition: converts the screen-space position into
*					   clip space
*
* Ins:	position - the position to convert
*
* Outs:
*
* Returns:	The converted point
*
* Mod. Date:		      06/06/2015
* Mod. Initials:			MM
*****************************************************************/
/*static*/ vec3f CGUIManager::ComputeClipPosition(vec3f& position)
{
	const float width = GUI_WIDTH;
	const float height = GUI_HEIGHT;

	vec3f pos;
	memset(&pos, 0, sizeof(vec3f));

	// find clip space position
	pos.x = ((position.x / width) - .5f) * 2.f;
	pos.y = ((position.y / height) - .5f) * 2.f;

	return pos;
}

void CGUIManager::SetSubtuitleBoxVisible(bool value)
{
	if (nullptr != m_pSubtitle)
	{
		m_pSubtitle->SetVisible(value);
	}
}