/***********************************************
* Filename:			Radar.h
* Date:      		07/13/2015
* Mod. Date:		07/13/2015
* Mod. Initials:	BL
* Author:    		Brandon Lucier
* Purpose:			Show the player where enemies are
*					relative to him/herself
************************************************/

#ifndef _CRADAR_H_
#define _CRADAR_H_

#include <vector>

class CBaseEnemy;
class CPlayer;
class CTexture2D;
class CSubSystem;

class CRadar
{
public:
	CRadar();
	~CRadar();

	void Update(float dt);

	void Render();

	/*****************************************************************
	* ClearRadar():				This function will empty out m_lpEnemyList 
	*							for when we restart the game
	* Ins:						void
	* Outs:						void
	* Returns:					void
	* Mod. Date:				07/16/2015
	* Mod. Initials:			BL
	*****************************************************************/
	void ClearRadar();

	/*****************************************************************
	* FillEnemyList():			This function will fill out m_lpEnemyList
	* Ins:						void
	* Outs:						void
	* Returns:					void
	* Mod. Date:				07/16/2015
	* Mod. Initials:			BL
	*****************************************************************/
	void FillEnemyList();

	/*****************************************************************
	* FillSubsystems():			This function will fill out the subsystem pointers
	* Ins:						void
	* Outs:						void
	* Returns:					void
	* Mod. Date:				07/16/2015
	* Mod. Initials:			BL
	*****************************************************************/
	void FillSubsystems();

	void SubSystemRadarRender(DirectX::XMFLOAT3 SubSysPos, int WhichOne);

	/*********************
	* Mutators
	*********************/
	void SetVisible(bool Visible) { m_bVisible = Visible; }
	void SetTransparency(float fAlpha);

private:
	std::vector<CBaseEnemy*> m_lpEnemyList;
	CPlayer* m_pPlayer;

	CTexture2D* m_pTexture;
	CTexture2D* m_pCreepTexture;
	CTexture2D* m_pHuntTexture;
	CTexture2D* m_pSaboTexture;

	CTexture2D* m_pLifeSupTexture;
	CTexture2D* m_pReactorTexture;
	CTexture2D* m_pEnginesTexture;

	DirectX::XMFLOAT3 m_vLifeSup;
	DirectX::XMFLOAT3 m_vReactor;
	DirectX::XMFLOAT3 m_vEngines;

	bool m_bVisible;
	bool m_bHaveSystems;

	float m_fRange;
};

#endif