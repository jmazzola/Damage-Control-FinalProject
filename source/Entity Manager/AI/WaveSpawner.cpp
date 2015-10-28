/***********************************************
* Filename:			WaveSpawner.cpp
* Date:      		06/20/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:			Spawner to handle waves of enemies
************************************************/

#include "stdafx.h"
#include "WaveSpawner.h"
#include "../../TinyXML2/tinyxml2.h"
#include "../../Core/EnumerationHolder.h"
#include "../EntityManager.h"
#include "../../Core/DebugLogger.h"

CWaveSpawner::CWaveSpawner( )
{
	m_nWaveNumber = 0;

	m_bActive = false;
	m_fPackWait = 4.0f;
}

CWaveSpawner::~CWaveSpawner( )
{
}

void CWaveSpawner::ReadXml(const char* filename)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename);
	tinyxml2::XMLElement* pRoot = doc.RootElement( );

	if (pRoot == nullptr)
		DebugPrint("you broke the build\n");

	tinyxml2::XMLElement* weiv = pRoot->FirstChildElement("numberofwaves");

	int loop = weiv->IntAttribute("value");

	tinyxml2::XMLElement* itr = pRoot->FirstChildElement("Wave");
	tinyxml2::XMLElement* creeps = itr->FirstChildElement("Creeper");

	tWaves myCurrent;

	if (creeps != nullptr)
	{
		tinyxml2::XMLElement* variant = creeps->FirstChildElement("numero");
		int number = variant->IntAttribute("value");

		myCurrent.CreepNum = number;
	}

	tinyxml2::XMLElement* hunters = itr->FirstChildElement("Hunter");
	if (hunters != nullptr)
	{
		tinyxml2::XMLElement* variant = hunters->FirstChildElement("numero");
		int number = variant->IntAttribute("value");

		myCurrent.hunterNum = number;
	}

	tinyxml2::XMLElement* sabos = itr->FirstChildElement("Saboteur");
	if (sabos != nullptr)
	{
		tinyxml2::XMLElement* variant = sabos->FirstChildElement("numero");
		int number = variant->IntAttribute("value");

		myCurrent.saboteurNum = number;
	}

	m_vTotalWaves.push_back(myCurrent);
	loop--;
	for (int i = 0; i < loop; i++)
	{
		itr = itr->NextSiblingElement("Wave");
		creeps = itr->FirstChildElement("Creeper");

		tWaves newone;

		if (creeps != nullptr)
		{
			tinyxml2::XMLElement* variant = creeps->FirstChildElement("numero");
			int number = variant->IntAttribute("value");

			newone.CreepNum = number;
		}

		tinyxml2::XMLElement* hunters = itr->FirstChildElement("Hunter");
		if (hunters != nullptr)
		{
			tinyxml2::XMLElement* variant = hunters->FirstChildElement("numero");
			int number = variant->IntAttribute("value");

			newone.hunterNum = number;
		}

		tinyxml2::XMLElement* sabos = itr->FirstChildElement("Saboteur");
		if (sabos != nullptr)
		{
			tinyxml2::XMLElement* variant = sabos->FirstChildElement("numero");
			int number = variant->IntAttribute("value");

			newone.saboteurNum = number;
		}

		m_vTotalWaves.push_back(newone);
	}
}

void CWaveSpawner::Update(float fDt)
{
	if (m_bActive)
	{
		if (m_fCurrentWait > m_fPackWait)
		{
			Spawn( );
			m_fCurrentWait = 0.0f;
		}
		else
		{
			m_fCurrentWait += fDt;
		}
	}
}

void CWaveSpawner::Spawn( )
{
	int current = 0;
	bool creep, hunt, sabo;
	creep = hunt = sabo = false;

	if (m_nWaveNumber < (int)m_vTotalWaves.size( ))
	{
		eEnemyVariant myVariant;

		m_nTotalCreep = m_vTotalWaves[m_nWaveNumber].CreepNum;
		if (m_nTotalCreep > 0)
		{
			for (int i = 0; i < m_nTotalCreep; i++)
			{
				if (current == m_nPackNumber)
					break;

				if (m_nWaveNumber >= 2)
				{
					int random = rand( ) % 100;
					if (random < 50)
						myVariant = eBronze;
					else
						myVariant = eGold;
				}
				else
					myVariant = eBronze;

				m_pEntityManager->CycleIntoLiveList(CEntity::eCREEP, GetPosition( ), nullptr, myVariant);

				m_nSofarCreeps++;
				current++;

				if (m_nSofarCreeps == m_nTotalCreep - 1)
				{
					//	m_nSofarCreeps = 0;
					creep = true;
				}
			}
		}
		else
			creep = true;

		m_nTotalHunter = m_vTotalWaves[m_nWaveNumber].hunterNum;

		if (m_nTotalHunter > 0)
		{
			for (int i = 0; i < m_nTotalHunter; i++)
			{
				if (current == m_nPackNumber)
					break;
				if (m_nWaveNumber >= 2)
				{
					int random = rand( ) % 99;
					if (random < 33)
						myVariant = eBronze;
					else if (random < 66)
						myVariant = eSilver;
					else
						myVariant = eGold;
				}
				else
					myVariant = eBronze;

				m_pEntityManager->CycleIntoLiveList(CEntity::eHUNTER, GetPosition( ), nullptr, myVariant);

				m_nSofarHunters++;
				current++;

				if (m_nSofarHunters == m_nTotalHunter - 1)
				{
					//m_nSofarHunters = 0;
					hunt = true;
				}
			}
		}
		else
			hunt = true;

		m_nTotalSaboteurs = m_vTotalWaves[m_nWaveNumber].saboteurNum;

		if (m_nTotalSaboteurs > 0)
		{
			for (int i = 0; i < m_nTotalSaboteurs; i++)
			{
				if (current == m_nPackNumber)
					break;

				if (m_nWaveNumber >= 2)
				{
					int random = rand( ) % 100;
					if (random < 50)
						myVariant = eBronze;
					else
						myVariant = eGold;
				}
				else
					myVariant = eBronze;

				m_pEntityManager->CycleIntoLiveList(CEntity::eSABOTEUR, GetPosition( ), nullptr, myVariant);

				m_nSofarSaboteurs++;
				current++;

				if (m_nSofarSaboteurs == m_nTotalSaboteurs - 1)
				{
					//m_nSofarSaboteurs = 0;
					sabo = true;
				}
			}
		}
		else
			sabo = true;

		if (sabo == true && hunt == true && creep == true)
		{
			m_nWaveNumber++;
			m_bActive = false;
		}
	}
}

void CWaveSpawner::Reset( )
{
	m_nWaveNumber = 0;
	m_nSofarCreeps = 0;
	m_nSofarHunters = 0;
	m_nSofarSaboteurs = 0;
}
