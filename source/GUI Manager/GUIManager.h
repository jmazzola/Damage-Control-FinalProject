/***********************************************
* Filename:			GUIManager.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#ifndef _GUIMANAGER_H_
#define _GUIMANAGER_H_

#include "..\Core\stdafx.h"

#include "..\Math\vec3.h"

#define GUI_WIDTH 1000.f
#define GUI_HEIGHT 800.f

class CUIElement; 
class CGameplayState;
class CRectangle;
class CRadar;
class CReactor;
class CObjectivePanel;
class CUIProgressMap;

class CGUIManager
{

public:
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
	CGUIManager();
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
	~CGUIManager();

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
	void Update(float dt);

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
	void Render(void);

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
	void AddElement(CUIElement* element) { m_elements.push_back(element); }
	void PushFront(CUIElement* element) { m_elements.insert(m_elements.begin(), element); }

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
	CUIElement* GetElementAt(vec3f& position);

	/*****************************************************************
	* GetElementAt: Returns the ui element at the provided point
	*
	* Ins:	position - the position to test for a ui element
	*		nArraySize - how many UIElements we can store
	*
	* Outs:	pElements - All UI elements that overlap with vPosition
	*
	* Returns:	int - number of elements returned
	*
	* Mod. Date:		      06/06/2015
	* Mod. Initials:			MJG
	*****************************************************************/
	unsigned int GetElementsAt(vec3f& vPosition, std::vector<CUIElement*>& pElements);

	/*****************************************************************
	* ComputeBounds: Rebuilds 'bounds' such that they are scaled 
	*				 for clip space
	*
	* Ins:	Bounds - the rectangle to rebuild
	*
	* Outs:
	*
	* Returns:	The rebuilt rectangle
	*
	* Mod. Date:		      06/06/2015
	* Mod. Initials:			MM
	*****************************************************************/
	static CRectangle ComputeBounds(CRectangle& bounds);

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
	static vec3f ComputeClipPosition(vec3f& position);

	/*****************************************************************
	* Load: Loads ui elements out of the xml doc at 'filename' and
	*		  places them in the ui
	*
	* Ins: filename - the name of the xml document to load
	*	   pGameplayState - the gameplaystate, use only if calling inside gameplaystate
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/14/2015
	* Mod. Initials:
	*****************************************************************/
	static CGUIManager* Load(std::string& filename, CGameplayState* pGameplayState = nullptr);

	/*****************************************************************
	* Load: Loads ui elements out of the xml doc at 'filename' and
	*		  places them in the ui
	*
	* Ins: filename - the name of the xml document to load
	*	   pGameplayState - the gameplaystate, use only if calling inside gameplaystate
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/14/2015
	* Mod. Initials:
	*****************************************************************/
	bool LoadMe(std::string& filename, CGameplayState* pGameplayState = nullptr);

	/***************
	*  Accessors
	***************/
	CUIProgressMap* GetProgressMap()const { return m_pProgressMap; }

	/***************
	* Mutators
	***************/
	void SetVisible(bool value);
	void SetRadarRender(bool value);
	void SetSubtuitleBoxVisible(bool value);

private:
	std::vector<CUIElement*> m_elements;

	CRadar* m_pRadar = nullptr;
	CReactor* m_pReactor = nullptr;

	const float m_fMaxWidth = 1920.f;
	const float m_fMaxHeight = 1080.f;
	float m_fFlickerTimer = 0.25f;

	bool SeeRadar = false;
	bool m_bVisible = true;
	CObjectivePanel* m_pSubtitle;
	CUIProgressMap* m_pProgressMap;

};

#endif // _GUIMANAGER_H_
