/***********************************************
* Filename:			AIDirector.cpp
* Date:      		07/05/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:			Director class to handle the enemy spawns and
control the pacing of the game
************************************************/

#include "stdafx.h"
#include "AIDirector.h"

#include "../Entities/Subsystems/Engines.h"
#include "../Entities/Subsystems/LifeSupport.h"
#include "../Entities/Subsystems/Reactor.h"

#include "../Pathfinding/PathSearch.h"
#include "../Entities/Player.h"
#include "../EntityManager.h"
#include "../../TinyXML2/tinyxml2.h"
#include <stdlib.h>
#include "../../Weapon/ModSystem.h"
#include "../../Weapon/ModBank.h"
#include "../../Weapon/MachineGun.h"
#include "../../Weapon/Pistol.h"
#include "../../Weapon/ShotGun.h"

#include "../../State Machine/GameplayState.h"

#include "../../Audio Manager/AudioSystemWwise.h"
#include "../../Renderer/BitmapFont.h"

#include <iostream>
#include <string>
#include <sstream>


#define FRUSTUMRADIUS  1200.0f
#define INTROWAITTIME 12.0f
#define OBJECTIVEFLASHTIMER 0.35f
#define OBJECTIVETOTALFLASH  7.0f
#define TUTORIALDELAY 5.0f
#define SUBTITLETIMER 8.0f

#define EVENSPAWNAMOUNT 4

#define INCREMENTBASETIMER 25.0f
#define INFINITEMODEEVENTTIMER 45.0f

CAIDirector::CAIDirector()
{
	std::string helper = std::string(XMLPATH + std::string("Director.xml"));
	if (ReadXml(helper.c_str()) == false)
	{
		m_ePhase = eDisabled; // wrong initializated
	}
	//Reset();
	//m_bWaifuEnabled = true;
}

CAIDirector::~CAIDirector()
{
}

bool CAIDirector::ReadXml(const char* filename)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename);
	tinyxml2::XMLElement* pRoot = doc.RootElement();

	if (pRoot == nullptr)
	{
		DebugPrint("you broke the build\n");
		return false;
	}
	tinyxml2::XMLElement* weiv = pRoot->FirstChildElement("MinPeak");
	if (weiv == nullptr)
	{
		DebugPrint("No MinPeak\n");
		return false;
	}
	m_fMinimumPeak = (float)atoi(weiv->GetText());

	weiv = pRoot->FirstChildElement("MaxPeak");
	if (weiv == nullptr)
	{
		DebugPrint("No MaxPeak\n");
		return false;
	}
	m_fMaximumPeak = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("SystemDown");
	if (weiv == nullptr)
	{
		DebugPrint("No SystemDown\n");
		return false;
	}
	m_fSystemDownWeight = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("EnginesWeight");
	if (weiv == nullptr)
	{
		DebugPrint("No EnginesWeight\n");
		return false;
	}
	m_fEnginesWeight = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("LifeSupportWeight");
	if (weiv == nullptr)
	{
		DebugPrint("No LifeSupportWeight\n");
		return false;
	}
	m_fLifeSupportWeight = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("ReactorWeight");
	if (weiv == nullptr)
	{
		DebugPrint("No ReactorWeight\n");
		return false;
	}
	m_fReactorWeight = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("ScrapWeight");
	if (weiv == nullptr)
	{
		DebugPrint("No ScrapWeight\n");
		return false;
	}
	m_fScrapWeight = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("PlayerDamage");
	if (weiv == nullptr)
	{
		DebugPrint("No PlayerDamage\n");
		return false;
	}
	m_fPlayerDamageWeight = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("Distance");
	if (weiv == nullptr)
	{
		DebugPrint("No Distance\n");
		return false;
	}
	m_fDistanceWeight = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("Creep");
	if (weiv == nullptr)
	{
		DebugPrint("No Creep\n");
		return false;
	}
	m_fCreepCost = (float)atof(weiv->GetText());

	//weiv = pRoot->FirstChildElement("Spammer");
	//if (weiv == nullptr)
	//{
	//	DebugPrint("No Spammer\n");
	//	return false;
	//}
	//m_fSpammerCost = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("Hunter");
	if (weiv == nullptr)
	{
		DebugPrint("No Hunter\n");
		return false;
	}
	m_fHunterCost = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("Saboteur");
	if (weiv == nullptr)
	{
		DebugPrint("No Saboteur\n");
		return false;
	}
	m_fSaboteurCost = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("Decay");
	if (weiv == nullptr)
	{
		DebugPrint("No Decay\n");
		return false;
	}
	m_fDecayOverTime = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("HunterChanceModifier");
	if (weiv == nullptr)
	{
		DebugPrint("No HunterChanceModifier\n");
		return false;
	}
	m_fHunterChanceModifier = (float)atof(weiv->GetText());

	/*weiv = pRoot->FirstChildElement("SpammerChanceModifier");
	if (weiv == nullptr)
	{
	DebugPrint("No SpammerChanceModifier\n");
	return false;
	}
	m_fSpammerChanceModifier = (float)atof(weiv->GetText());*/

	weiv = pRoot->FirstChildElement("SaboteurChanceModifier");
	if (weiv == nullptr)
	{
		DebugPrint("No SaboteurChanceModifier\n");
		return false;
	}
	m_fSaboteurChanceModifier = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("BuildUpSplit");
	if (weiv == nullptr)
	{
		DebugPrint("No BuildUpSplit\n");
		return false;
	}
	m_fBuildUpSPlit = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("MonitorDelay");
	if (weiv == nullptr)
	{
		DebugPrint("No MonitorDelay\n");
		return false;
	}
	m_fMonitorDelayTimer = (float)atof(weiv->GetText());

	//weiv = pRoot->FirstChildElement("MaxSpammerChance");
	//if (weiv == nullptr)
	//{
	//	DebugPrint("No MaxSpammerChance\n");
	//	return false;
	//}
	//m_fMaxSpammerChance = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("MaxSaboteurChance");
	if (weiv == nullptr)
	{
		DebugPrint("No MaxSaboteurChance\n");
		return false;
	}
	m_fMaxSaboteurChance = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("MaxHunterChance");
	if (weiv == nullptr)
	{
		DebugPrint("No MaxHunterChance\n");
		return false;
	}
	m_fMaxHunterChance = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("UpgradeWeight");
	if (weiv == nullptr)
	{
		DebugPrint("No UpgradeWeight\n");
		return false;
	}
	m_fUpgradeWeight = (float)atof(weiv->GetText());


	weiv = pRoot->FirstChildElement("MaxAmountCreeps");
	if (weiv == nullptr)
	{
		DebugPrint("No MaxAmountCreeps\n");
		return false;
	}
	m_nMaxAmountCreep = (int)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("MaxAmountHunters");
	if (weiv == nullptr)
	{
		DebugPrint("No MaxAmountHunters\n");
		return false;
	}
	m_nMaxAmountHunter = (int)atof(weiv->GetText());

	//weiv = pRoot->FirstChildElement("MaxAmountSpammers");
	//if (weiv == nullptr)
	//{
	//	DebugPrint("No MaxAmountSpammers\n");
	//	return false;
	//}
	//m_nMaxAmountSpammer = (int)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("MaxAmountSaboteurs");
	if (weiv == nullptr)
	{
		DebugPrint("No MaxAmountSaboteurs\n");
		return false;
	}
	m_nMaxAmountSaboteur = (int)atof(weiv->GetText());

	//weiv = pRoot->FirstChildElement("SpammersAmountIncrement");
	//if (weiv == nullptr)
	//{
	//	DebugPrint("No SpammersAmountIncrement\n");
	//	return false;
	//}
	//m_nSpammerAmountIncrement = (int)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("HuntersAmountIncrement");
	if (weiv == nullptr)
	{
		DebugPrint("No SaboteursAmountIncrement\n");
		return false;
	}
	m_nSaboteurAmountIncrement = (int)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("HuntersAmountIncrement");
	if (weiv == nullptr)
	{
		DebugPrint("No HuntersAmountIncrement\n");
		return false;
	}
	m_nHunterAmountIncrement = (int)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("CreepAmountIncrement");
	if (weiv == nullptr)
	{
		DebugPrint("No CreepAmountIncrement\n");
		return false;
	}
	m_nCreepAmountIncrement = (int)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("ProgressPercentCheck");
	if (weiv == nullptr)
	{
		DebugPrint("No ProgressPercentCheck\n");
		return false;
	}
	m_fProgressPercentCheck = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("CreepEventDistance");
	if (weiv == nullptr)
	{
		DebugPrint("No CreepEventDistance\n");
		return false;
	}
	m_fCreepEventDistance = (float)atof(weiv->GetText());

	//weiv = pRoot->FirstChildElement("SpammerEventDistance");
	//if (weiv == nullptr)
	//{
	//	DebugPrint("No SpammerEventDistance\n");
	//	return false;
	//}
	//m_fSpammerEventDistance = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("HunterEventDistance");
	if (weiv == nullptr)
	{
		DebugPrint("No HunterEventDistance\n");
		return false;
	}
	m_fHunterEventDistance = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("SaboteurEventDistance");
	if (weiv == nullptr)
	{
		DebugPrint("No SaboteurEventDistance\n");
		return false;
	}
	m_fSaboteurEventDistance = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("EventPlayerHealth");
	if (weiv == nullptr)
	{
		DebugPrint("No EventPlayerHealth\n");
		return false;
	}
	m_fPlayerHealthEvent = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("DialogTimer");
	if (weiv == nullptr)
	{
		DebugPrint("No DialogTimer\n");
		return false;
	}
	m_fdialogTimer = (float)atof(weiv->GetText());

	weiv = pRoot->FirstChildElement("VariantIncrement");
	if (weiv == nullptr)
	{
		DebugPrint("No VariantIncrement\n");
		return false;
	}
	m_fVariantIncrement = (float)atof(weiv->GetText());

	return true;
}

void CAIDirector::MonitorStress()
{

	/*
	- Monitor player's stress (enemies on screen/ health/ damage/ upgrades/ travelled distance)

	- When injured, proportional to damage taken
	- When the player loses the regeneration
	- Amount of enemies on screen
	- When nearby enemies die, inversely proportional to distance
	- Decay Intesity towards zero over time
	- Do NOT decay Intensity if there are enemies actively engaging the player
	- Upgrades
	*/

	//=================== Subsystems damaged
	float maxHealth, currentHealth, totalWeight;
	totalWeight = 0;
	//Engines
	maxHealth = m_pTheEngines->GetMaxHealth();
	currentHealth = m_pTheEngines->GetCurrentHealth();
	totalWeight = (1 - (currentHealth / maxHealth)) * m_fEnginesWeight;
	//Life Support
	maxHealth = m_pTheLifeSupport->GetMaxHealth();
	currentHealth = m_pTheLifeSupport->GetCurrentHealth();
	totalWeight += (1 - (currentHealth / maxHealth)) * m_fLifeSupportWeight;
	//Reactor
	maxHealth = m_pTheReactor->GetMaxHealth();
	currentHealth = m_pTheReactor->GetCurrentHealth();
	totalWeight += (1 - (currentHealth / maxHealth)) * m_fReactorWeight;

	//subsystems down
	if (m_pTheEngines->GetAlive() == false)
		totalWeight += m_fSystemDownWeight;
	if (m_pTheLifeSupport->GetAlive() == false)
		totalWeight += m_fSystemDownWeight * 1.5f;
	if (m_pTheReactor->GetAlive() == false)
		totalWeight += m_fSystemDownWeight * 2.0f;

	//player damage
	maxHealth = m_pPlayer->GetMaxHealth();
	currentHealth = m_pPlayer->GetCurrentHealth();
	totalWeight += (1 - (currentHealth / maxHealth)) * m_fCurrentMaximumPeak;

	//traveled distance
	if (eBuildUp == m_ePhase)
	{
		maxHealth = m_pTheEngines->GetGoalDistance();
		currentHealth = m_pTheEngines->GetProgress();
		totalWeight += ((maxHealth - currentHealth) / maxHealth) * m_fDistanceWeight;
	}

	//total anmount of scrap
	int scrap = m_pPlayer->GetTotalScrapCount();
	totalWeight -= (scrap / 4.0f) * m_fScrapWeight;

	//enemies on screen? I don't know if this would work as expected

	//Creeps spawned
	int crip = (int)m_pEntityManager->GetAllCreeps().size();
	totalWeight += crip * m_fCreepCost;
	//Spammers spawned
	//	int spam = (int)m_pEntityManager->GetAllSpammers().size();
	//totalWeight += spam * m_fSpammerCost;
	//Hunters spawnedJakeman01
	int hent = (int)m_pEntityManager->GetAllHunters().size();
	totalWeight += hent * m_fHunterCost;
	//Saboteurs spawned
	int subo = (int)m_pEntityManager->GetAllSaboteur().size();
	totalWeight += subo * m_fSaboteurChance;


	totalWeight -= m_pPlayer->GetPistol()->GetModBank()->GetStressValue();
	totalWeight -= m_pPlayer->GetShotgun()->GetModBank()->GetStressValue();
	totalWeight -= m_pPlayer->GetMachineGun()->GetModBank()->GetStressValue();

	m_fStressLevel = totalWeight;
	////printf("stress level is : %d \n", m_fStressLevel);

}

void CAIDirector::DecaySTress()
{
	//decay intensity over time
	//if (m_ePhase == ePeakFade)
	m_fStressLevel -= m_fDecayOverTime;
}

void CAIDirector::BuildUpPhase(float fDt)
{
	if (m_bSpawnLimit == true)
	{
		m_ePhase = eRelax;
		//printf("I am on relax \n");
		return;
	}

	if ((true == m_bSplitted) && (m_fCurrentSplit > m_fBuildUpSPlit) || (false == m_bSplitted))
	{
		bool bSabote = true;
		bool bHunter = true;
		bool bCreep = true;
		bool bSpam = true;

		float myChance = (float)(rand() % 100) + 1;


		//Calculate the position now (When finished every loop is guarunteed to spawn something)
		DirectX::XMFLOAT3 spawnPos = FindValidLocation();

		int nAmountSabs = m_pEntityManager->GetAllSaboteur().size();
		int nAmountHunts = m_pEntityManager->GetAllHunters().size();
		int nAmountCreeps = m_pEntityManager->GetAllCreeps().size();
		//		int nAmountSpammers = m_pEntityManager->GetAllSpammers().size();


		if (m_fSaboteurChance > 0 && myChance <= m_fSaboteurChance && nAmountSabs < m_nCurrentMaxAmountSaboteur)
		{
			bSabote = SpawnSpecial(eSpawnSaboteur);
		}
		else if (m_fHunterChance > 0 && myChance <= m_fHunterChance && nAmountHunts < m_nCurrentMaxAmountHunter)
		{
			bHunter = SpawnSpecial(eSpawnHunter);
		}
		//else if (m_fSpammerChance > 0 && myChance <= m_fSpammerChance && nAmountSpammers < m_nCurrentMaxAmountSpammer)
		//{
		//	bSpam = SpawnSpecial(eSpawnSpammer);
		//}
		else if (nAmountCreeps< m_nCurrentMaxAmountCreep)
		{
			bCreep = SpawnSpecial(eSpawnCreep);
		}

		if (m_fStressLevel > m_fCurrentMaximumPeak)
		{
			m_bBuildDone = true;
			m_fTimerToSpawn = 3.0f + (rand() % 5);
		}

		if (0 <= m_fTimerToSpawn && true == m_bBuildDone)
		{
			m_ePhase = eDirectorPhase::ePeakFade;
			//printf("I am on fading \n");
			m_fCurrentToSpawn = 0;
			m_fCurrentSplit = 0.0f;
			m_bBuildDone = false;
		}

		if ((false == bHunter && false == bSabote && false == bCreep && false == bSpam))
		{
			m_bSpawnLimit = true;
		}

		if (m_fStressLevel >= m_fCurrentMaximumPeak / 2.0f)
		{
			m_bSplitted = true;
		}
	}
	else
	{
		m_fCurrentSplit += fDt;
	}

	MonitorStress();
}

void CAIDirector::PeakFadePhase(float fDt)
{
	m_fCurrentDelay += fDt;
	if (m_fCurrentDelay > m_fMonitorDelayTimer)
	{
		MonitorStress();
		m_fCurrentDelay = 0.0f;
	}
	else
	{
		DecaySTress();
	}

	if (m_fStressLevel < m_fCurrentMinimumPeak)
	{
		//tweaking
		m_fTimerRelax = 10.0f + rand() % 10 + 3;
		m_ePhase = eDirectorPhase::eRelax;
		m_bSplitted = false;
		m_fCurrentSplit = 0.0f;
		//printf("I am on relax \n");

		m_fRelaxSpawn = 0.5f + rand() % 4;
		if (m_fHunterChance < m_fMaxHunterChance)
		{
			m_fHunterChance += m_fHunterChanceModifier;
			if (m_fHunterChance > m_fMaxHunterChance)
			{
				m_fHunterChance = m_fMaxHunterChance;
			}
		}

		/*if (m_fSpammerChance > m_fMaxSpammerChance)
		{
		m_fSpammerChance += m_fSpammerChanceModifier;
		if (m_fSpammerChance > m_fMaxSpammerChance)
		{
		m_fSaboteurChance = m_fMaxSpammerChance;
		}
		}*/

		if (m_fSaboteurChance < m_fMaxSaboteurChance)
		{
			m_fSaboteurChance += m_fSaboteurChanceModifier;
			if (m_fSaboteurChance > m_fMaxSaboteurChance)
			{
				m_fSaboteurChance = m_fMaxSaboteurChance;
			}
		}

		m_fCurrentMaximumPeak += m_fMinimumPeak;
		m_fCurrentMinimumPeak += m_fMinimumPeak / 2.0f;

		m_fVariantChance += m_fVariantIncrement;
		if (m_fVariantChance > 100)
		{
			m_fVariantChance = 100;
		}
	}
}

void CAIDirector::RelaxPhase(float fDt)
{
	m_fTimerRelax -= fDt;
	m_fRelaxSpawn -= fDt;

	if (m_fStressLevel < m_fCurrentMinimumPeak)
	{

		if (m_fRelaxSpawn <= 0)
		{
			SpawnSpecial(eSpawnType::eSpawnCreep);
			m_fRelaxSpawn = 1.0f + rand() % 5 + 1;
		}
	}
	if (m_fTimerRelax <= 0)
	{
		m_fTimerRelax = 5.0f + rand() % 5 + 2;
		IncreaseMaxEnemyAmount();
		m_ePhase = eDirectorPhase::eBuildUp;
		//printf("I am on Building\n");
	}
}

void CAIDirector::Update(float fDt)
{
	//normal check
	if (nullptr == m_pEntityManager || nullptr == m_pPlayer || nullptr == m_pTheEngines ||
		nullptr == m_pTheLifeSupport || nullptr == m_pTheReactor || false == m_pPlayer->GetAlive())
	{
		m_ePhase = eDisabled;
		return;
	}

	if (m_pPlayer->GetCurrentHealth() < 0)
	{
		FinalSpawn();
		m_bPlayerDead = true;
		return;
	}

	if (m_fSubtitleCurrent > SUBTITLETIMER)
	{
		m_sSubtitle = " ";
		m_bShowSubtitle = false;
		m_pGamestate->NotifyGuiForSubtitle(false);
	}
	else
	{
		m_fSubtitleCurrent += fDt;
	}


	switch (m_ePhase)
	{

	case (eDirectorPhase::eBuildUp) :
	{
		if (m_fCurrentToSpawn > m_fTimerRelax)
		{
			BuildUpPhase(fDt);
		}
		else
		{
			m_fCurrentToSpawn += fDt;
		}
		break;
	}
	case (eDirectorPhase::eRelax) :
	{
		RelaxPhase(fDt);
		break;
	}
	case (eDirectorPhase::ePeakFade) :
	{
		PeakFadePhase(fDt);
		break;
	}
	case (eDirectorPhase::eBeforeEvent) :
	{
		BeforeEvent(fDt);
		break;
	}
	case (eDirectorPhase::eSpawnEvent) :
	{
		SpawnEvent();
		break;
	}
	case (eDirectorPhase::eEventCooldown) :
	{
		EventCoolDown(fDt);
		break;
	}
	case (eDirectorPhase::eWaitUntilKillAll) :
	{
		WaitUntilKillAll();
		return;
	}
	case (eDirectorPhase::eBeforeTutorial) :
	{
		BeforeTutorialPhase(fDt);
		break;
	}
	case (eDirectorPhase::eTutorialPhase) :
	{
		TutorialPhase(fDt);
		return;
	}

	case (eDirectorPhase::eFinalCountDown) :
	{
		FinalCountDownPhase();
		return;
	}
	case (eDirectorPhase::eDisabled) :
	{
		return;
	}
	}

	HandleSoundAndSubtitles(fDt);

	m_fSpawnIncrementTimer -= fDt;

	if (m_ePhase == eDirectorPhase::eOff && m_pTheEngines->GetProgress() != 0)
	{
		m_ePhase = eDirectorPhase::eBuildUp;
		//printf("I am on Building \n");

		m_sObjective.clear();
		m_sObjective = "Survive";
		ResetObjectiveFlash();
	}

	//difference between the current distance
	m_nScore += (int)(floor(m_pTheEngines->GetProgress()) - floor(m_fEnginesCurrentProgress));
	m_fEnginesCurrentProgress = floor(m_pTheEngines->GetProgress());
	//difference in scrap
	m_nScore += m_pPlayer->GetTotalScrapCount() - m_nPlayerCurrentScrap;
	m_nPlayerCurrentScrap = m_pPlayer->GetTotalScrapCount();

	m_pPlayer->GetGameStats()->SetScore(m_nScore);

	ChangeObjective(fDt);
	//FlashScore(fDt);

	if (true == m_bInfiniteMode)
	{
		EvaluateIfEventInfinite(fDt);
		IncreaseMaxEnemyAmount();
	}
	else
	{
		EvaluateIfEvent(fDt);
	}
}

void CAIDirector::Reset()
{
	m_bIsPaused = false;
	m_fStressLevel = 0;
	m_fEventTimer = 0.0f;
	//m_fSpammerChance = 0;
	m_fSaboteurChance = 0;
	m_fHunterChance = 0;
	m_fTimerToSpawn = 3.0f + (rand() % 5);
	m_fCurrentMaximumPeak = m_fMaximumPeak;
	m_fCurrentMinimumPeak = m_fMinimumPeak;
	m_fCurrentSplit = 0.0f;
	m_fCurrentDelay = 0.0f;
	m_fCurrentToSpawn = 0.0f;
	m_fRelaxSpawn = 0.0f;

	m_nCurrentMaxAmountCreep = m_nMaxAmountCreep;
	//	m_nCurrentMaxAmountSpammer = m_nMaxAmountSpammer;
	m_nCurrentMaxAmountHunter = m_nMaxAmountHunter;
	m_nCurrentMaxAmountSaboteur = m_nMaxAmountSaboteur;
	m_fCurrentProgress = 1.0f;

	m_bSpawnLimit = false;
	m_bBuildDone = false;
	m_bSplitted = false;
	m_bEventSelected = false;
	m_bChangeTo = false;

	m_bCreepEventTriggered = false;
	//	m_bSpammerEventTriggered = false;
	m_bHunterEventTriggered = false;
	m_bSaboteurEventTriggered = false;

	m_fdialogCurrent = m_fdialogTimer / 2.0f;

	m_bTutorialCreep = false;
	m_bTutorialHunter = false;
	//	m_bTutorialSpammer = false;
	m_bTutorialSpawn = false;

	m_bTutorialTipSaid = false;
	m_bTutorialSpawned = false;
	m_bIntro = false;

	m_fDelay = 0;
	m_bIsObjectiveChanged = false;
	m_fIntroWait = false;
	m_fObjectiveTotalFlashTime = 0.0f;
	m_fObjectiveFlashing = 0.0f;

	m_sObjective.clear();
	m_sObjective.resize(60);
	m_sObjective = " ";

	m_ObjectiveBaseColor = m_ObjectiveCurrentColor = DirectX::XMFLOAT4{ 0.23f, 0.8f, 1.0f, 1.0f };

	m_ObjectiveFlashColor = DirectX::XMFLOAT4{ 1.0f, 0.9f, 0.0f, 1.0f };

	m_bIsBaseColor = true;
	m_fSubtitleCurrent = 0.0f;

	m_fVariantChance = 0.0f;
	if (nullptr != m_pTheEngines)
	{
		m_pTheEngines->SetStopped(false);
	}

	m_xmScoreColor = DirectX::XMFLOAT4{ 0, 0.5f, 1, 1 };
	//m_xmScoreColor = m_xmScoreBase;
	//m_xmScoreDiff = DirectX::XMFLOAT4{ 1, 0, 1, 1 };
	m_bFlashScore = false;
	m_nScore = 0;
	m_fEnginesCurrentProgress = 0;

	m_nSpawnAmount = EVENSPAWNAMOUNT;
	m_fSpawnIncrementTimer = INCREMENTBASETIMER;
	m_sSubtitle = " ";
	m_bShowSubtitle = false;

	if (true == m_bInfiniteMode)
	{
		m_ePhase = eDirectorPhase::eOff;

		m_sObjective.clear();
		m_sObjective = "Repair the ENGINES\n  <Ship's back>";
		ResetObjectiveFlash();
		m_pPlayer->GetGameStats()->SetScore(0);
		m_fdialogTimer = 8.0f;
	}
	else
	{
		m_ePhase = eDirectorPhase::eBeforeTutorial;
		m_pPlayer->GetGameStats()->SetScore(0);
		m_fdialogTimer = 30.0f;
	}

	m_nPlayerCurrentScrap = m_pPlayer->GetScrapCount();

	m_fInfiniteTimer = 0.0f;
	m_bInfiniteTrigger = false;
	m_bPlayerDead = false;
	m_bShowRemainding = false;
	m_bShowRemainding = 0;
}

bool CAIDirector::SpawnSpecial(eSpawnType Type, eEnemyVariant variant)
{
	DirectX::XMFLOAT3 spawnPos = FindValidLocation();
	eEnemyVariant newVariant;
	newVariant = variant;
	float multiplier, variantchance;

	if (eNoVariant == variant)
	{
		if (0 != m_fVariantChance)
		{
			variantchance = (float)(rand() % (int)(m_fVariantChance)) + 1;
		}
		else
		{
			variantchance = 0;
		}

		if (variantchance < 40)
		{
			newVariant = eEnemyVariant::eBronze;
			multiplier = 1.0f;
		}
		else if (variantchance >= 75)
		{
			newVariant = eEnemyVariant::eGold;
			multiplier = 2.0f;
		}
		else
		{
			newVariant = eEnemyVariant::eSilver;
			multiplier = 1.5;
		}
	}
	else
	{
		if (eEnemyVariant::eBronze == newVariant)
		{
			multiplier = 1.0f;
		}
		else if (eEnemyVariant::eSilver == newVariant)
		{
			multiplier = 1.5f;
		}
		else if (eEnemyVariant::eGold == newVariant)
		{
			multiplier = 2.0f;
		}
	}

	switch (Type)
	{
	case(eSpawnType::eSpawnCreep) :
	{
		if (m_pEntityManager->CycleIntoLiveList(CEntity::eCREEP, spawnPos, nullptr, newVariant) == true)
		{
			m_fStressLevel += m_fCreepCost * multiplier;
			return true;
		}
		break;
	}
								  /*case(eSpawnType::eSpawnSpammer) :
								  {
								  if (m_pEntityManager->CycleIntoLiveList(CEntity::eSPAMMER, spawnPos, nullptr, variant) == true)
								  {
								  m_fStressLevel += m_fSpammerCost * multiplier;
								  return true;
								  }
								  break;
								  }*/
	case(eSpawnType::eSpawnHunter) :
	{
		if (m_pEntityManager->CycleIntoLiveList(CEntity::eHUNTER, spawnPos, nullptr, newVariant) == true)
		{
			m_fStressLevel += m_fHunterCost * multiplier;
			return true;
		}
		break;
	}
	case(eSpawnType::eSpawnSaboteur) :
	{
		if (m_pEntityManager->CycleIntoLiveList(CEntity::eSABOTEUR, spawnPos, nullptr, newVariant) == true)
		{
			m_fStressLevel += m_fSaboteurCost * multiplier;
			return true;
		}
		break;
	}
	}
	return false;
}

DirectX::XMFLOAT3 CAIDirector::FindValidLocation()
{
	//check the frustum and select a point that is outside it but inside the navmesh.

	DirectX::XMFLOAT3 pos, player;

	pos.y = 0.2f;

	player = m_pPlayer->GetPosition();
	bool outTheFrustum;

	do
	{
		outTheFrustum = false;
		pos = CPathSearch::GetInstance()->RandomPoint();

		float dist = MathHelper::Dot((player - pos), (player - pos));

		if (abs(dist) > FRUSTUMRADIUS * FRUSTUMRADIUS)
		{
			outTheFrustum = true;
		}

	} while (outTheFrustum == false);

	return pos;
}

void CAIDirector::IncreaseMaxEnemyAmount()
{

	if (true == m_bInfiniteMode)
	{
		if (m_fSpawnIncrementTimer < 0)
		{
			m_fVariantChance += m_fVariantIncrement;
			if (m_fVariantChance > 100)
			{
				m_fVariantChance = 100;
			}

			m_nSpawnAmount += EVENSPAWNAMOUNT;

			m_nCurrentMaxAmountCreep += m_nCreepAmountIncrement;
			//			m_nCurrentMaxAmountSpammer += m_nSpammerAmountIncrement;
			m_nCurrentMaxAmountHunter += m_nHunterAmountIncrement;
			m_nCurrentMaxAmountSaboteur += m_nSaboteurAmountIncrement;
			m_fSpawnIncrementTimer = INCREMENTBASETIMER * m_fCurrentProgress;
			m_fCurrentProgress++;

			if (m_fHunterChance < m_fMaxHunterChance)
			{
				m_fHunterChance += m_fHunterChanceModifier;
				if (m_fHunterChance > m_fMaxHunterChance)
				{
					m_fHunterChance = m_fMaxHunterChance;
				}
			}

			/*if (m_fSpammerChance > m_fMaxSpammerChance)
			{
			m_fSpammerChance += m_fSpammerChanceModifier;
			if (m_fSpammerChance > m_fMaxSpammerChance)
			{
			m_fSaboteurChance = m_fMaxSpammerChance;
			}
			}*/

			if (m_fSaboteurChance < m_fMaxSaboteurChance)
			{
				m_fSaboteurChance += m_fSaboteurChanceModifier;
				if (m_fSaboteurChance > m_fMaxSaboteurChance)
				{
					m_fSaboteurChance = m_fMaxSaboteurChance;
				}
			}

		}
	}
	else
	{
		float currentPercent = m_pTheEngines->GetProgress() / m_pTheEngines->GetGoalDistance();

		if (currentPercent >= (m_fProgressPercentCheck * m_fCurrentProgress))
		{
			m_nCurrentMaxAmountCreep += m_nCreepAmountIncrement;
			//			m_nCurrentMaxAmountSpammer += m_nSpammerAmountIncrement;
			m_nCurrentMaxAmountHunter += m_nHunterAmountIncrement;
			m_nCurrentMaxAmountSaboteur += m_nSaboteurAmountIncrement;

			m_fCurrentProgress++;

			//AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_CHECKPOINT);

			m_sSubtitle = "Reminder, checkpoint reached.";
			m_fSubtitleCurrent = 0.0f;
			m_bShowSubtitle = true;
			m_fdialogCurrent = 0.0f;
		}
	}
}

bool CAIDirector::WaitUntilKillAll()
{
	size_t crep, hun, sabo; // , spam;
	crep = m_pEntityManager->GetAllCreeps().size();
	hun = m_pEntityManager->GetAllHunters().size();
	sabo = m_pEntityManager->GetAllSaboteur().size();
	//	spam = m_pEntityManager->GetAllSpammers().size();

	m_nRemainding= (int)(crep + hun + sabo);

	if (0 == crep && 0 == hun && 0 == sabo /*&& 0 == spam*/)
	{
		/*if (m_pTheEngines->GetProgress() >= m_pTheEngines->GetGoalCheck())
		{
		m_pTheEngines->SetStopped(false);
		m_ePhase = eDirectorPhase::eDisabled;

		m_sObjective.clear();
		m_sObjective = "COMPLETED";
		ResetObjectiveFlash();
		}*/
		m_bShowRemainding = false;
		return true;
	}

	return false;
}

void CAIDirector::Render()
{
	if (false == m_bIsPaused)
	{
		CBitmapFont::GetInstance()->DrawString("OBJECTIVE:", 0.53f, 0.93f, m_ObjectiveCurrentColor, 0.8f);

		if (eDirectorPhase::eTutorialPhase == m_ePhase || eDirectorPhase::eBeforeTutorial == m_ePhase)
		{
			CBitmapFont::GetInstance()->DrawString("TUTORIAL", 0.53f, 0.88f, m_ObjectiveCurrentColor, 0.7f);

			CBitmapFont::GetInstance()->DrawString(m_sObjective.c_str(), 0.53f, 0.83f, m_ObjectiveCurrentColor, 0.7f);
		}
		else
		{
			CBitmapFont::GetInstance()->DrawString(m_sObjective.c_str(), 0.53f, 0.88f, m_ObjectiveCurrentColor, 0.7f);
		}

		if (true == m_bShowSubtitle)
		{
			CBitmapFont::GetInstance()->DrawString(m_sSubtitle.c_str(), -0.525f, -0.75f, DirectX::XMFLOAT4{ 1, 1, 1, 1 }, 0.75f);
		}

		if (true == m_bInfiniteMode)
		{
			std::string  help = "Score:   ";
			std::stringstream help2;
			help2 << m_nScore;
			help = help + help2.str();

			CBitmapFont::GetInstance()->DrawString(help.c_str(), -0.93f, 0.87f, m_xmScoreColor, 1.2f);
		}

		if (true == m_bShowRemainding)
		{
			std::string  help = "Remaining: ";
			std::stringstream help2;
			help2 << m_nRemainding;
			help = help + help2.str();

			if (true == m_bInfiniteMode)
			{
				CBitmapFont::GetInstance()->DrawString(help.c_str(), -0.93f, 0.75f, m_xmScoreColor, 1.2f);
			}
			else
			{
				CBitmapFont::GetInstance()->DrawString(help.c_str(), -0.95f, 0.89f, m_xmScoreColor, 1.2f);
			}
		}
	}
}

void CAIDirector::SpawnEvent()
{
	//0 = max creeps
	//1 = max hunters
	//2 = max saboteurs
	//3 = max spammers
	//add sound to each event

	int amount;
	bool isDone = false;

	if (eSpawnType::eSpawnCreep == m_nSpawnEvent)
	{
		amount = (int)m_pEntityManager->GetAllCreeps().size();

		if (amount < m_nCurrentMaxAmountCreep)
		{
			SpawnSpecial(m_nSpawnEvent);
		}
		else
		{
			isDone = true;
		}
	}
	else if (eSpawnType::eSpawnHunter == m_nSpawnEvent)
	{
		amount = (int)m_pEntityManager->GetAllHunters().size();

		if (amount < m_nSpawnAmount)
		{
			SpawnSpecial(m_nSpawnEvent);
		}
		else
		{
			isDone = true;
		}
	}
	else if (eSpawnType::eSpawnSaboteur == m_nSpawnEvent)
	{
		amount = (int)m_pEntityManager->GetAllSaboteur().size();

		if (amount < m_nSpawnAmount)
		{
			SpawnSpecial(m_nSpawnEvent);
		}
		else
		{
			isDone = true;
		}
	}
	/*else if (eSpawnType::eSpawnSpammer == m_nSpawnEvent)
	{
	amount = (int)m_pEntityManager->GetAllSpammers().size();

	if (amount < m_nSpawnAmount)
	{
	SpawnSpecial(eSpawnType::eSpawnSpammer);
	}
	else
	{
	isDone = true;
	}
	}*/
	m_bShowRemainding = true;
	if (true == isDone)
	{
		m_ePhase = eDirectorPhase::eEventCooldown;
	}
}

void CAIDirector::EvaluateIfEvent(float fDt)
{
	if (false == m_bEventSelected && eDirectorPhase::eWaitUntilKillAll != m_ePhase)
	{
		//stop the ship to give time the player 

		float progress = m_pTheEngines->GetProgress();
		float distance = m_pTheEngines->GetGoalDistance();

		float check = (progress / distance);

		float currentHealth = m_pPlayer->GetCurrentHealth();
		float maxHealth = m_pPlayer->GetMaxHealth();

		float healthCheck = currentHealth / maxHealth;
		if (eDirectorPhase::eFinalCountDown != m_ePhase)
		{
			if (check >= m_fSaboteurEventDistance && healthCheck <= m_fPlayerHealthEvent
				&& false == m_bSaboteurEventTriggered)
			{
				m_nSpawnEvent = eSpawnType::eSpawnSaboteur;
				m_ePhase = eBeforeEvent;
				m_bChangeTo = true;
				m_bEventSelected = true;
				m_bSaboteurEventTriggered = true;

				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_BEFOREEVENT);
				m_sSubtitle = "Attention! Hostiles nearing \ncritical mass. Rerouting power \nfrom engines to defensive \nmeasures.";
				m_pGamestate->NotifyGuiForSubtitle(true);
				m_fSubtitleCurrent = 0.0f;
				m_pTheEngines->SetStopped(true);
				m_fdialogCurrent = 0.0f;
				m_bShowSubtitle = true;
				m_sObjective.clear();
				m_sObjective = "Destroy all threats";
				ResetObjectiveFlash();
			}
			else if (check >= m_fHunterEventDistance && healthCheck <= m_fPlayerHealthEvent
				&& false == m_bHunterEventTriggered)
			{
				m_nSpawnEvent = eSpawnType::eSpawnHunter;
				m_ePhase = eBeforeEvent;
				m_bChangeTo = true;
				m_bEventSelected = true;
				m_bHunterEventTriggered = true;

				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_BEFOREEVENT);
				m_sSubtitle = "Attention! Hostiles nearing \ncritical mass. Rerouting power \nfrom engines to defensive \nmeasures.";
				m_pGamestate->NotifyGuiForSubtitle(true);
				m_fSubtitleCurrent = 0.0f;
				m_pTheEngines->SetStopped(true);
				m_fdialogCurrent = 0.0f;
				m_bShowSubtitle = true;
				m_sObjective.clear();
				m_sObjective = "Destroy all threats";
				ResetObjectiveFlash();
			}
			/*if (check >= m_fSpammerEventDistance && healthCheck <= m_fPlayerHealthEvent
			&& false == m_bSpammerEventTriggered)
			{
			m_nSpawnEvent = eSpawnType::eSpawnSpammer;
			m_ePhase = eBeforeEvent;
			m_bChangeTo = true;
			m_bEventSelected = true;
			m_bSpammerEventTriggered = true;

			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_BEFOREEVENT);
			m_sSubtitle = "Attention! Hostiles nearing \ncritical mass. Rerouting power \nfrom engines to defensive measures.";
			m_pGamestate->NotifyGuiForSubtitle(true);
			m_fSubtitleCurrent = 0.0f;
			m_pTheEngines->SetStopped(true);
			m_fdialogCurrent = 0.0f;
			m_bShowSubtitle = true;
			m_sObjective.clear();
			m_sObjective = "Destroy all threats";
			ResetObjectiveFlash();
			}*/
			else if (check >= m_fCreepEventDistance && healthCheck <= m_fPlayerHealthEvent
				&& false == m_bCreepEventTriggered)
			{
				m_nSpawnEvent = eSpawnType::eSpawnCreep;
				m_ePhase = eBeforeEvent;
				m_bChangeTo = true;
				m_bCreepEventTriggered = true;
				m_bEventSelected = true;

				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_BEFOREEVENT);
				m_sSubtitle = "Attention! Hostiles nearing \ncritical mass. Rerouting power \nfrom engines to defensive \nmeasures.";
				m_pGamestate->NotifyGuiForSubtitle(true);
				m_fSubtitleCurrent = 0.0f;
				m_pTheEngines->SetStopped(true);
				m_fdialogCurrent = 0.0f;
				m_bShowSubtitle = true;
				m_sObjective.clear();
				m_sObjective = "Destroy all threats";
				ResetObjectiveFlash();
			}
		}
	}

	//blast enemies before hitting the goal -> final wow
	if (m_pTheEngines->GetProgress() >= m_pTheEngines->GetGoalCheck())
	{
		m_ePhase = eDirectorPhase::eFinalCountDown;

		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_ALLGOINGDOWN_LOOP);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_FINALMESSAGE);

		m_sSubtitle = "Alert! Alert! Massive amount of \nenemies onboard. Please survive!";
		m_fSubtitleCurrent = 0.0f;
		m_bShowSubtitle = true;
		m_pGamestate->NotifyGuiForSubtitle(true);

		m_fdialogCurrent = 0.0f;
	}
}

void CAIDirector::FinalCountDownPhase()
{
	if (false == m_bTutorialTipSaid)
	{
		m_bTutorialTipSaid = true;

		m_nCurrentMaxAmountHunter = 12;
		m_nCurrentMaxAmountCreep = 50;
		m_nCurrentMaxAmountSaboteur = 20;
		//		m_nCurrentMaxAmountSpammer = 13;
		m_sObjective.clear();
		m_sObjective = "RUN FOR YOUR LIFE";
		ResetObjectiveFlash();
	}

	//m_pTheEngines->SetStopped(true);
	m_fdialogCurrent = 0.0f;
	m_fStressLevel = 0.0f;
	m_bSplitted = false;
	if (m_pTheEngines->GetCurrentHealth() < 50)
	{
		m_pTheEngines->SetHealth(70);
	}
	if (m_pTheReactor->GetCurrentHealth() < 30)
	{
		m_pTheReactor->SetHealth(60);
	}
	if (m_pTheLifeSupport->GetCurrentHealth() < 65)
	{
		m_pTheLifeSupport->SetHealth(80);
	}

	FinalSpawn();
}

void CAIDirector::BeforeEvent(float fDt)
{
	if (true == WaitUntilKillAll())
	{
		if (eSpawnType::eSpawnCreep == m_nSpawnEvent)
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_CREEPEVENT);

			m_sSubtitle = "My sensors have detected multiple \ncreeps on the ship. Consider \nusing the pistol.";
			m_pGamestate->NotifyGuiForSubtitle(true);
			m_fSubtitleCurrent = 0.0f;
			m_bShowSubtitle = true;
			m_fdialogCurrent = 0.0f;

			m_sObjective.clear();
			m_sObjective = "Destroy all creeps";
			ResetObjectiveFlash();
			m_ePhase = eSpawnEvent;
		}
		else if (eSpawnType::eSpawnHunter == m_nSpawnEvent)
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_HUNTEREVENT);

			m_sSubtitle = "Large volume of hunters detected. \nThis could be a problem. Get \nthe shotgun. Now.";
			m_pGamestate->NotifyGuiForSubtitle(true);
			m_fSubtitleCurrent = 0.0f;
			m_bShowSubtitle = true;
			m_fdialogCurrent = 0.0f;

			m_sObjective.clear();
			m_sObjective = "Destroy all hunters";
			ResetObjectiveFlash();
			m_ePhase = eSpawnEvent;
		}
		else if (eSpawnType::eSpawnSaboteur == m_nSpawnEvent)
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_SABOTEUREVENT);

			m_sSubtitle = "Gaggle of saboteurs inbound. \nWhat? That's what a group of \nsaboteurs is called.";
			m_pGamestate->NotifyGuiForSubtitle(true);
			m_fSubtitleCurrent = 0.0f;
			m_bShowSubtitle = true;
			m_fdialogCurrent = 0.0f;

			m_sObjective.clear();
			m_sObjective = "Destroy all \nsaboteurs";
			ResetObjectiveFlash();
			m_ePhase = eSpawnEvent;
		}
		//else if (eSpawnType::eSpawnSpammer == m_nSpawnEvent)
		//{
		//	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
		//	//AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_SPAMMEREVENT);

		//	m_sSubtitle = "Gaggle of saboteurs inbound. What? That's \n    what a group of saboteurs is called.";
		//	m_pGamestate->NotifyGuiForSubtitle(true);
		//	m_fSubtitleCurrent = 0.0f;
		//	m_bShowSubtitle = true;
		//	m_fdialogCurrent = 0.0f;

		//	m_sObjective.clear();
		//	m_sObjective = "Destroy all spammers";
		//	ResetObjectiveFlash();
		//}
	}
}

void CAIDirector::EventCoolDown(float fDt)
{

	if (true == WaitUntilKillAll())
	{
		m_ePhase = eBuildUp;
		//restart the engines
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_EVENTDONE);

		m_sSubtitle = "Enemy mass dwindling. Returning power to \n             the engines....";
		m_pGamestate->NotifyGuiForSubtitle(true);
		m_fSubtitleCurrent = 0.0f;
		m_bShowSubtitle = true;
		m_pTheEngines->SetStopped(false);
		m_fdialogCurrent = 0.0f;

		m_bEventSelected = false;

		m_sObjective.clear();
		m_sObjective = "Survive";
		ResetObjectiveFlash();
	}
}

void CAIDirector::TutorialPhase(float fDt)
{
	if (GetKeyState(VK_CONTROL) & 0x800 && GetKeyState('O') & 0x800)
	{
		m_ePhase = eDirectorPhase::eOff;
		m_sObjective.clear();
		m_sObjective = "Repair the ENGINES\n  <Ship's back>";
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
		ResetObjectiveFlash();
		m_pGamestate->NotifyGuiForSubtitle(false);
		m_bShowSubtitle = false;
		return;
	}

	if (TUTORIALDELAY < m_fDelay)
	{
		m_fdialogCurrent += fDt;

		if (false == m_bTutorialCreep)
		{
			if (false == m_bTutorialSpawned)
			{
				SpawnSpecial(eSpawnType::eSpawnCreep, eEnemyVariant::eBronze);
				m_bTutorialSpawned = true;

				m_sObjective.clear();
				m_sObjective = "Kill the Creep";
				ResetObjectiveFlash();
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_CREEPTIP);

				m_sSubtitle = "Creeps have been detected inside the \nship! They should be easy to kill, \nbut watch out for the blinking ones: \nthey explode.";
				m_pGamestate->NotifyGuiForSubtitle(true);
				m_fSubtitleCurrent = 0.0f;
				m_bShowSubtitle = true;

				m_fdialogCurrent = 0.0f;
			}

			if (true == WaitUntilKillAll())
			{
				m_bTutorialCreep = true;
				m_bTutorialSpawned = false;
				m_bTutorialTipSaid = false;
				m_bIsObjectiveChanged = false;
			}
		}
		//else if (true == m_bTutorialCreep && false == m_bTutorialSpammer)
		//{
		//if (false == m_bTutorialSpawned)
		//{
		//SpawnSpecial(eSpawnType::eSpawnSpammer, eEnemyVariant::eBronze);
		//m_bTutorialSpawned = true;

		//m_sObjective.clear();
		//m_sObjective = "Kill the Spammer";
		//ResetObjectiveFlash();
		//AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
		////AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_SPAMMERTIP);

		//m_sSubtitle = "Watch out for Spammers. Their projectiles \n      are gross. And also deadly.";
		//m_pGamestate->NotifyGuiForSubtitle(true);
		//m_fSubtitleCurrent = 0.0f;
		//m_bShowSubtitle = true;
		//m_bTutorialTipSaid = true;
		//m_fdialogCurrent = 0;
		//}

		//if (true == WaitUntilKillAll())
		//{
		//	m_bTutorialTipSaid = false;
		//	m_bTutorialSpammer = true;
		//	m_bTutorialSpawned = false;
		//}
		//}
		else if (true == m_bTutorialCreep && false == m_bTutorialHunter)
		{
			if (false == m_bTutorialSpawned)
			{
				SpawnSpecial(eSpawnType::eSpawnHunter, eEnemyVariant::eBronze);
				m_bTutorialSpawned = true;

				m_sObjective.clear();
				m_sObjective = "Kill the Hunter";
				ResetObjectiveFlash();
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_HUNTERTIP);

				m_sSubtitle = "The hunters are fun. They hunt meat \nbags like you. I'm thankful that \nI'm a machine.";
				m_pGamestate->NotifyGuiForSubtitle(true);
				m_fSubtitleCurrent = 0.0f;
				m_bShowSubtitle = true;
				m_bTutorialTipSaid = true;
				m_fdialogCurrent = 0;
			}

			if (true == WaitUntilKillAll())
			{
				m_bTutorialTipSaid = false;
				m_bTutorialHunter = true;
				m_bTutorialSpawned = false;
			}
		}
		else if (true == m_bTutorialHunter)
		{
			if (false == m_bTutorialSpawned)
			{
				SpawnSpecial(eSpawnType::eSpawnSaboteur, eEnemyVariant::eBronze);
				m_bTutorialSpawned = true;

				m_sObjective.clear();
				m_sObjective = "Kill the Saboteur";
				ResetObjectiveFlash();

				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_SABOTEURTIP);

				m_sSubtitle.clear();
				m_sSubtitle = "Saboteurs detected. They attack me \nmore than you. It really... is \nquite flattering.";
				m_pGamestate->NotifyGuiForSubtitle(true);
				m_fSubtitleCurrent = 0.0f;
				m_bShowSubtitle = true;
				m_fdialogCurrent = 0;
				m_bTutorialTipSaid = true;
			}

			if (true == WaitUntilKillAll())
			{
				m_ePhase = eOff;
				m_bTutorialTipSaid = false;
				//check this
				if (m_fdialogCurrent > m_fdialogTimer / 2)
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_YOUANDME);

					m_sSubtitle = "It's you and me babe. I'll give \nyou support.";
					m_pGamestate->NotifyGuiForSubtitle(true);
					m_fSubtitleCurrent = 0.0f;
					m_bShowSubtitle = true;
					m_fdialogCurrent = 0;

					CBitmapFont::GetInstance()->ClearBuffer();
				}

				m_sObjective.clear();
				m_sObjective = "Repair the ENGINES\n  <Ship's back>";
				ResetObjectiveFlash();
			}
		}
	}
	else
	{
		m_fDelay += fDt;
	}

	m_pTheEngines->Kill();
	m_pTheLifeSupport->Heal(100);
	m_pTheReactor->Heal(100);
}

void CAIDirector::BeforeTutorialPhase(float fDt)
{
	if (GetKeyState(VK_CONTROL) & 0x800 && GetKeyState('O') & 0x800)
	{
		m_ePhase = eDirectorPhase::eOff;
		m_sObjective.clear();
		m_sObjective = "Repair the ENGINES\n  <Ship's back>";
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
		m_pGamestate->NotifyGuiForSubtitle(false);
		m_bShowSubtitle = false;
		ResetObjectiveFlash();
		return;
	}

	if (false == m_bIntro)
	{
		CBitmapFont::GetInstance()->ClearBuffer();
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_INTRO);


		m_sSubtitle = "Hello, I am the ship's artificial \nintelligence. the W.A.I.F.U. but you \ncan call me Waifu.";
		m_pGamestate->NotifyGuiForSubtitle(true);
		m_fSubtitleCurrent = 0.0f;
		m_bShowSubtitle = true;
		m_fdialogCurrent = 0;
		m_bIntro = true;
		m_fIntroWait = 0.0f;
	}
	else
	{
		m_fIntroWait += fDt;
	}

	if (m_fIntroWait > INTROWAITTIME)
	{
		if (false == m_bTutorialTipSaid && nullptr == m_pPlayer->GetCurrentWeapon())
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_SHOPTIP);
			m_bTutorialTipSaid = true;

			m_sSubtitle = "Alert! Enemies on the ship. Press \nTab to open the shop and buy the \npistol.";
			m_pGamestate->NotifyGuiForSubtitle(true);
			m_fSubtitleCurrent = 0.0f;
			m_bShowSubtitle = true;
			//cange objective
			m_sObjective.clear();
			m_sObjective = "Buy the pistol <TAB>";
			ResetObjectiveFlash();
		}

		else if (nullptr != m_pPlayer->GetCurrentWeapon())
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
			m_sObjective.clear();
			m_sObjective = " ";
			m_bIsObjectiveChanged = false;
			m_bTutorialTipSaid = false;
			m_ePhase = eTutorialPhase;
		}
	}

	m_pTheEngines->Kill();
	m_pTheLifeSupport->Heal(100);
	m_pTheReactor->Heal(100);
}

void CAIDirector::ChangeObjective(float fDt)
{
	if (true == m_bIsObjectiveChanged)
	{
		if (m_fObjectiveTotalFlashTime < OBJECTIVETOTALFLASH)
		{
			if (m_fObjectiveFlashing > OBJECTIVEFLASHTIMER)
			{
				if (true == m_bIsBaseColor)
				{
					m_bIsBaseColor = false;
					m_ObjectiveCurrentColor = m_ObjectiveFlashColor;
				}
				else
				{
					m_bIsBaseColor = true;
					m_ObjectiveCurrentColor = m_ObjectiveBaseColor;
				}
				m_fObjectiveFlashing = 0.0f;
			}
			else
			{


				m_fObjectiveFlashing += fDt;
			}
		}
		else
		{
			if (!m_bIsBaseColor)
			{
				m_bIsBaseColor = true;
				m_ObjectiveCurrentColor = m_ObjectiveBaseColor;
			}
		}

		m_fObjectiveTotalFlashTime += fDt;
	}
	else
	{
		m_ObjectiveCurrentColor = m_ObjectiveBaseColor;
		m_bIsObjectiveChanged = false;
		m_bIsBaseColor = true;
	}
}

void CAIDirector::ResetObjectiveFlash()
{
	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_OBJECTIVECHANGED);
	m_bIsObjectiveChanged = true;
	m_bIsBaseColor = true;
	m_fObjectiveFlashing = 0.0f;
	m_fObjectiveTotalFlashTime = 0.0f;
	m_ObjectiveCurrentColor = m_ObjectiveBaseColor;

}

void CAIDirector::FinalSpawn()
{
	int currCrip = m_pEntityManager->GetAllCreeps().size();
	int currHunt = m_pEntityManager->GetAllHunters().size();
	int currSabo = m_pEntityManager->GetAllSaboteur().size();
	//	int currSpam = m_pEntityManager->GetAllSpammers().size();

	if (currCrip < m_nCurrentMaxAmountCreep)
	{
		SpawnSpecial(eSpawnType::eSpawnCreep);
	}
	if (currHunt < m_nCurrentMaxAmountHunter)
	{
		SpawnSpecial(eSpawnType::eSpawnHunter);
	}
	if (currSabo < m_nCurrentMaxAmountSaboteur)
	{
		SpawnSpecial(eSpawnType::eSpawnSaboteur);
	}
	//if (currSpam < m_nCurrentMaxAmountSpammer)
	//{
	//	SpawnSpecial(eSpawnType::eSpawnSpammer);
	//}

}

void CAIDirector::HandleSoundAndSubtitles(float fDt)
{
	//sound check
	if (m_ePhase != eDirectorPhase::eTutorialPhase && m_ePhase != eDirectorPhase::eBeforeTutorial)
	{
		if (m_fdialogCurrent > m_fdialogTimer * 0.5f && fDt > 0)
		{
			//int number = rand() % 10 + 1;

			//Make sure the sub-systems are up before making a joke
			if (m_pTheLifeSupport && !m_pTheLifeSupport->GetAlive())
			{
				//AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_LSUPPORT_DOWN);
				m_sSubtitle = "Life support system, offline. I don't \nmind, all that air was making it \nstuffy in here.";
				m_fSubtitleCurrent = 0.0f;
				m_fdialogCurrent = 0.0f;
				m_pGamestate->NotifyGuiForSubtitle(true);
				m_bShowSubtitle = true;
			}
			else if (m_pTheEngines && !m_pTheEngines->GetAlive())
			{
				//AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_ENGINES_DOWN);
				m_sSubtitle = "Engine system offline.";
				m_fSubtitleCurrent = 0.0f;
				m_fdialogCurrent = 0.0f;
				m_pGamestate->NotifyGuiForSubtitle(true);
				m_bShowSubtitle = true;
			}
			else  if (true == m_bWaifuEnabled && m_fdialogCurrent > m_fdialogTimer)
			{
				int number = rand() % 21;

				switch (number)
				{
				case(0) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_ALIENJOKE);
					m_sSubtitle = "What is the best kind of alien? \nA dead one. For better jokes upgrade \nto Comedisoft 10 now!";
					break;
				}
				case(1) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_CHECKTWITTER);
					m_sSubtitle = "Hang on, checking Space-Twitter... \nBig news; @Badass69 says \"you suck\".";
					break;
				}
				case(2) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_DEADCREW);
					m_sSubtitle = "When did you became the commander \nof the ship? Oh! I remember, when \nthe whole crew died.";
					break;
				}
				case(3) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_FREEAD);
					m_sSubtitle = "I love Gadgetron. I can buy all \nthe nice stuff a beautiful AI \nneeds: atomic grenades, self-\ndestruct devices and nano guardians.";
					break;
				}
				case(4) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_FRIENDLY);
					m_sSubtitle = "You shouldn't have killed that \nlast one: it looked friendly...";
					break;
				}
				case(5) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_JOKE);
					m_sSubtitle = "Shift to the left! Shift to the \nright! Pop up, push down! \nByte! Byte! Byte!";
					break;
				}
				case(6) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_KILLKILL);
					m_sSubtitle = "Kill. Kill! KI... Sorry, I've \nbeen trying not to exterminate \nall life. It gets away from me \nsometimes.";
					break;
				}
				case(7) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_NOPE);
					m_sSubtitle = "That's a big NOPE.";
					break;
				}
				case(8) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_SMART);
					m_sSubtitle = "Are you a \"one\" or a \"zero\"? \nA \"yes\" or a \"no\"?";
					break;
				}
				case(9) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_SOWHAT);
					m_sSubtitle = "So... what are you waiting for?";
					break;
				}
				case(10) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_USUCK);
					m_sSubtitle = "After that last performance, I \nget the feeling I'm going to \nneed to look for new employer \nsoon...";
					break;
				}
				case(11) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_VAMOSALAPLAYA);
					m_sSubtitle = "Sorry for that... the old captain \nhad weird tastes...";
					break;
				}
				case(12) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_CREEPTIP);
					m_sSubtitle = "Creeps have been detected inside the \nship! They should be easy to kill, \nbut watch out for the blinking ones: \nthey explode.";
					break;
				}
				case(13) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_DONTDIE);
					m_sSubtitle = "Let's get our priorities straight: \nFirst: Survive. \nSecond: Don't die. \nThird: Have a nice day.";
					break;
				}
				case(14) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_DRONETIP);
					m_sSubtitle = "That cooldown on the HUD is for \nthe drone. Deploy it and I can \nkill things too.";
					break;
				}
				case(15) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_HEALTHTIP);
					m_sSubtitle = "Don't die. Those aliens don't \nknow how to treat an AI like me.\n";
					break;
				}
				case(16) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_HUNTERTIP);
					m_sSubtitle = "The hunters are fun. They hunt \nmeat-bags like you. I am thankful \nthat I'm a machine.";
					break;
				}
						 /*case(17) :
						 {
						 AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_HUNTERTIP2);
						 m_sSubtitle = "Silver hunters are pretty smart. They've \n    learned how to shoot guns.";
						 break;
						 }*/
				case(17) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_LARGETHREATS);
					m_sSubtitle = "Warning! Bad guys are coming. \nLots of them.";
					break;
				}
				case(18) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_NEWTHREATS);
					m_sSubtitle = "Detecting more enemies onboard. \nThis calls for a shopping spree!";
					break;
				}
				case(19) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_RADARTIP);
					m_sSubtitle = "The radar is the blue disc in \nthe bottom left corner of your \nscreen. It should help you detect \nenemies that are farther away.";
					break;
				}
						 /*case(21) :
						 {
						 AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_SABOTEURGOLDTIP);
						 m_sSubtitle = "Gold saboteurs are pretty dangerous. They \nhave poison gas, claws, and sub-machine guns.";
						 break;
						 }*/
				case(20) :
				{
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_SABOTEURTIP);
					m_sSubtitle = "Saboteurs detected. They attack \nme more than you. It's really... \nis quite flattering.";
					break;
				}
						 //case(21) :
						 //{
						 //	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_WEAPONTIP);
						 //	m_sSubtitle = "Is it just me or are the aliens \ngetting tougher? Either way, \nwhy don't you try a beefier gun?";
						 //	break;
						 //}
						 /*case(22) :
						 {
						 AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_SPAMMERTIP);
						 m_sSubtitle = "Watch out for Spammers. Their projectiles \n      are gross. And also deadly.";
						 break;
						 }*/
				}

				//make the subtitle stuff
				m_fdialogCurrent = 0.0f;
				m_fSubtitleCurrent = 0.0f;
				m_pGamestate->NotifyGuiForSubtitle(true);
				m_bShowSubtitle = true;
			}
			else
			{
				m_fdialogCurrent += fDt;
			}

		}
		else
		{
			m_fdialogCurrent += fDt;
		}
	}
}

void CAIDirector::SetSubtitle(std::string message)
{
	if (nullptr != m_pPlayer && true == m_pPlayer->GetAlive())
	{
		m_sSubtitle = message;
		m_fSubtitleCurrent = 0.0f;
		m_pGamestate->NotifyGuiForSubtitle(true);
		m_bShowSubtitle = true;
		m_fdialogCurrent = 0.0f;
	}
}

void CAIDirector::EvaluateIfEventInfinite(float fDt)
{
	if (eDirectorPhase::eOff == m_ePhase)
	{
		return;
	}

	if (false == m_bEventSelected  && true == m_bInfiniteTrigger)
	{
		//stop the ship to give time the player 

		float currentHealth = m_pPlayer->GetCurrentHealth();
		float maxHealth = m_pPlayer->GetMaxHealth();

		float healthCheck = currentHealth / maxHealth;

		if ((healthCheck <= m_fPlayerHealthEvent || true == m_bInfiniteTrigger) && false == m_bCreepEventTriggered)
		{
			m_nSpawnEvent = eSpawnType::eSpawnCreep;
			m_ePhase = eBeforeEvent;
			m_bChangeTo = true;
			m_bCreepEventTriggered = true;
			m_bEventSelected = true;

			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_BEFOREEVENT);
			m_sSubtitle = "Attention! Hostiles nearing \ncritical mass. Rerouting power \nfrom engines to defensive \nmeasures.";
			m_pGamestate->NotifyGuiForSubtitle(true);
			m_fSubtitleCurrent = 0.0f;
			m_pTheEngines->SetStopped(true);
			m_fdialogCurrent = 0.0f;
			m_bShowSubtitle = true;
			m_sObjective.clear();
			m_sObjective = "Destroy all threats";
			ResetObjectiveFlash();
			m_bInfiniteTrigger = false;
			m_fInfiniteTimer = 0;
		}
		/*else if (healthCheck <= m_fPlayerHealthEvent && false == m_bSpammerEventTriggered)
		{
		m_nSpawnEvent = eSpawnType::eSpawnSpammer;
		m_ePhase = eBeforeEvent;
		m_bChangeTo = true;
		m_bEventSelected = true;
		m_bSpammerEventTriggered = true;

		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_BEFOREEVENT);
		m_sSubtitle = "Attention! Hostiles nearing critical mass. Rerouting power\n       from engines to defensive measures.";
		m_pGamestate->NotifyGuiForSubtitle(true);
		m_fSubtitleCurrent = 0.0f;
		m_pTheEngines->SetStopped(true);
		m_fdialogCurrent = 0.0f;
		m_bShowSubtitle = true;
		m_sObjective.clear();
		m_sObjective = "Destroy all threats";
		ResetObjectiveFlash();
		}*/
		else if ((healthCheck <= m_fPlayerHealthEvent || true == m_bInfiniteTrigger) && false == m_bHunterEventTriggered)
		{
			m_nSpawnEvent = eSpawnType::eSpawnHunter;
			m_ePhase = eBeforeEvent;
			m_bChangeTo = true;
			m_bEventSelected = true;
			m_bHunterEventTriggered = true;
			m_bShowSubtitle = true;
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_BEFOREEVENT);
			m_sSubtitle = "Attention!Hostiles nearing \ncritical mass.Rerouting power \nfrom engines to defensive \nmeasures.";
			m_pGamestate->NotifyGuiForSubtitle(true);
			m_fSubtitleCurrent = 0.0f;
			m_pTheEngines->SetStopped(true);
			m_fdialogCurrent = 0.0f;

			m_sObjective.clear();
			m_sObjective = "Destroy all threats";
			ResetObjectiveFlash();
			m_bInfiniteTrigger = false;
			m_fInfiniteTimer = 0;
		}
		else if ((healthCheck <= m_fPlayerHealthEvent || true == m_bInfiniteTrigger) && false == m_bSaboteurEventTriggered)
		{
			m_nSpawnEvent = eSpawnType::eSpawnSaboteur;
			m_ePhase = eBeforeEvent;
			m_bChangeTo = true;
			m_bEventSelected = true;
			m_bSaboteurEventTriggered = true;

			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_2D_WAIFU);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_WAIFU_BEFOREEVENT);
			m_sSubtitle = "Attention!Hostiles nearing \ncritical mass.Rerouting power \nfrom engines to defensive \nmeasures.";
			m_pGamestate->NotifyGuiForSubtitle(true);
			m_fSubtitleCurrent = 0.0f;
			m_pTheEngines->SetStopped(true);
			m_fdialogCurrent = 0.0f;
			m_bShowSubtitle = true;
			m_sObjective.clear();
			m_sObjective = "Destroy all threats";
			ResetObjectiveFlash();
			m_bInfiniteTrigger = false;
			m_fInfiniteTimer = 0;
		}
	}
	else if (eDirectorPhase::eWaitUntilKillAll != m_ePhase && false == m_bEventSelected)
	{
		FinalSpawn();
		m_fInfiniteTimer += fDt;
	}

	if (true == m_bHunterEventTriggered && true == m_bSaboteurEventTriggered &&
		true == m_bCreepEventTriggered) // && true == m_bSpammerEventTriggered)
	{
		m_bHunterEventTriggered = false;
		m_bSaboteurEventTriggered = false;
		m_bCreepEventTriggered = false;
		//m_bSpammerEventTriggered = false;
	}

	if (m_fInfiniteTimer > INFINITEMODEEVENTTIMER)
	{
		m_bInfiniteTrigger = true;
	}
}

