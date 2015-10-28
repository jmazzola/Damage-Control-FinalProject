/***********************************************
* Filename:  		Achievements.cpp
* Date:      		08/14/2015
* Mod. Date: 		08/14/2015
* Mod. Initials:	BL
* Author:    		Brandon Lucier
* Purpose:   		This is the state used for
*					keeping track of the players achievements
************************************************/

#ifndef _CACHIEVEMENTSTATE_H_
#define _CACHIEVEMENTSTATE_H_
#include "BaseState.h"

class CUIButton;
class CUIElement;
class CAchieveTracker;

enum eAchievePage : signed char
{
	PAGE_0 = 0, PAGE_1, PAGE_2, /*PAGE_3, PAGE_4, */PAGE_COUNT
};

class CAchievements : public CBaseState
{
public:
	CAchievements();
	~CAchievements();
/*
	CAchievements(const CAchievements &);

	CAchievements &operator=(const CAchievements &)
	{
		return *this;
	}
*/

	void Input(void);

	void Update(float fDT);

	void Render(CAssetManager* pRenderer) override;

	bool FirstEnter(void) override;

	bool ReEnter(void);

	bool Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands = nullptr);

	void UnLoad(eStateLoadCommands* pLoadCommands = nullptr);

	bool Exit(void);

	eStateType CheckState(void);	

private:
	//static CAchievements *instance;
	bool PageFlip(eAchievePage nTargetPage);

	//Flips to the page to the left
	CUIButton* m_pLeft = nullptr;
	//Flips to the page to the right
	CUIButton* m_pRight = nullptr;
	//Returns to the previous state
	CUIButton* m_pBack = nullptr;

	CUIButton* m_pDelete = nullptr;

	//The different help pages
	CUIElement* m_pPages[PAGE_COUNT];

	//The current page
	signed char m_nPageIndex = 0;

	DirectX::XMFLOAT4 m_vColor;

	CTexture2D* m_pBackground = nullptr;

	CAchieveTracker* m_pTracker = nullptr;


};

#endif