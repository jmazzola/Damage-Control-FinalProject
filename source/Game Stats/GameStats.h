/***********************************************
* Filename:			Player.h
* Date:      		05/15/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:			The player represents the user
*					in the game
************************************************/

#include "../Core/stdafx.h"

#ifndef _GAMESTATS_H_
#define _GAMESTATS_H_

class CGameStats
{
public:
	CGameStats();
	~CGameStats();

	enum FavSubSystem { ENGINES, LIFE_SUPPORT, REACTOR, FAV_SUBSYSTEM_MAX };
	enum FavWeapon { PISTOL, MACHINE_GUN, SHOTGUN, FAV_WEAPON_MAX };

	int m_nTotalDamageTaken;
	int m_nTotalDamageGiven;

	int m_nTotalScrapSpent;
	
	int m_nTotalEnemiesKilled;

	float m_fFavoriteWeapTimers[4];

	int m_nTimesSubsystemWentDown[3];

	int m_nShotsFired;

	int m_nTotalScore;

	void Clear();

	int GetScore()const { return m_nTotalScore; }

	void SetScore(int nScore) { m_nTotalScore = nScore; }

	std::string GetFavoriteWeapon();

	std::string GetFavoriteSubsystem();

	std::string DumpedInfo();
};

#endif // _GAMESTATS_H_

