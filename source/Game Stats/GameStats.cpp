#include "stdafx.h"
#include "GameStats.h"

#include <algorithm>

CGameStats::CGameStats()
{
	Clear();
}

CGameStats::~CGameStats()
{
}

void CGameStats::Clear()
{
	m_nTimesSubsystemWentDown[FavSubSystem::ENGINES] = 0;
	m_nTimesSubsystemWentDown[FavSubSystem::LIFE_SUPPORT] = 0;
	m_nTimesSubsystemWentDown[FavSubSystem::REACTOR] = 0;

	m_fFavoriteWeapTimers[FavWeapon::PISTOL] = 0.0f;
	m_fFavoriteWeapTimers[FavWeapon::MACHINE_GUN] = 0.0f;
	m_fFavoriteWeapTimers[FavWeapon::SHOTGUN] = 0.0f;

	m_nShotsFired = m_nTotalDamageGiven = m_nTotalScrapSpent = m_nTotalEnemiesKilled = m_nTotalDamageTaken = 0;
}

std::string CGameStats::GetFavoriteWeapon()
{
	auto gun = std::distance(m_fFavoriteWeapTimers, std::max_element(m_fFavoriteWeapTimers, m_fFavoriteWeapTimers + ARRAYSIZE(m_fFavoriteWeapTimers)));

	if (!m_fFavoriteWeapTimers[0] && !m_fFavoriteWeapTimers[1] && !m_fFavoriteWeapTimers[2])
		return "N/A";

	switch (gun)
	{
		case 0:
		{
			return "Laser";
		}

		case 1:
		{
			return "Machine Gun";
		}

		case 2:
		{
			return "Shotgun";
		}
	}

	return "N/A";
}

std::string CGameStats::GetFavoriteSubsystem()
{
	if (!m_nTimesSubsystemWentDown[0] && !m_nTimesSubsystemWentDown[1] && !m_nTimesSubsystemWentDown[2])
		return "N/A";

	auto ss = std::distance(m_nTimesSubsystemWentDown, std::min_element(m_nTimesSubsystemWentDown, m_nTimesSubsystemWentDown + ARRAYSIZE(m_nTimesSubsystemWentDown)));
	switch (ss)
	{
		case 0:
		{
			return "Engine";
		}

		case 1:
		{
			return "Life Support";
		}

		case 2:
		{
			return "Reactor";
		}
	}

	return "N/A";
}

std::string CGameStats::DumpedInfo()
{
	std::string stats = "Enemies Killed: " + std::to_string(m_nTotalEnemiesKilled) + '\n' + '\n';
	stats += "Total Damage Given: " + std::to_string(m_nTotalDamageGiven) + '\n' + '\n';
	stats += "Total Damage Taken: " + std::to_string(m_nTotalDamageTaken) + '\n' + '\n';
	stats += "Total Scrap Spent: " + std::to_string(m_nTotalScrapSpent) + '\n' + '\n';
	stats += "Total Shots Fired: " + std::to_string(m_nShotsFired) + '\n' + '\n';
	stats += "Favorite Weapon: " + GetFavoriteWeapon() + '\n' + '\n';
	stats += "Total Score: " + std::to_string(m_nTotalScore) + '\n' + '\n';
	

	return stats;
}