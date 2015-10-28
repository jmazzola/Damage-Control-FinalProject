/***********************************************
* Filename:			WaveSpawner.h
* Date:      		06/20/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:			Spawner to handle waves of enemies
************************************************/

#ifndef _WAVE_SPAWNER_H_
#define _WAVE_SPAWNER_H_

#include "..\..\Entity Manager\Entity.h"

class CEntityManager;

struct tWaves
{
	int			CreepNum;
	int			hunterNum;
	int			saboteurNum;
}; 

class CWaveSpawner : public CEntity
{
public:
	/*****************************************************************
	* CMonsterSpawner(): constructor.
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:			06/20/2015
	* Mod. Initials:		JM
	*****************************************************************/
	CWaveSpawner();

	/*****************************************************************
	* CMonsterSpawner(): destructor.
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:			06/20/2015
	* Mod. Initials:		JM
	*****************************************************************/
	~CWaveSpawner();

	/*****************************************************************
	* ReadXml(): initialization.
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:			06/20/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void ReadXml(const char* filename);

	/*****************************************************************
	* Update(): Updates the spawner timer every frame and if this is
	*			greater than the cooldown time it activates this spawner.
	*
	* Ins:		float
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:			06/21/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void Update(float fDt);

	/*****************************************************************
	* Spawn(): activates the monsters for a certain wave.
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:			06/20/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void Spawn();


	/*****************************************************************
	* Reset(): resets the spawners
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:			06/20/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void Reset();

	/***************
	*  Mutators
	***************/
	void	 SetEntityManager(CEntityManager* _pEntityManager) { m_pEntityManager = _pEntityManager; }
	void	SetActive(bool active) { m_bActive = active; }

private:
	std::vector<tWaves>					m_vTotalWaves;
	int									m_nWaveNumber;
	CEntityManager*						m_pEntityManager;

	int									m_nTotalCreep;
	int									m_nTotalHunter;
	int									m_nTotalSaboteurs;

	int									m_nSofarCreeps;
	int									m_nSofarHunters;
	int									m_nSofarSaboteurs;

	int									m_nPackNumber;

	float								m_fPackWait;
	float								m_fCurrentWait;

	bool								m_bActive;

};

#endif