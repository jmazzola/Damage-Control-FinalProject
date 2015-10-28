/***********************************************
* Filename:			AIDirector.h
* Date:      		07/05/2015
* Mod. Date:		08/05/2015
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:			Director class to handle the enemy spawns and
					control the pacing of the game
************************************************/

#ifndef _CAI_DIRECTOR_H_
#define _CAI_DIRECTOR_H_

#include "../Entity.h"
class CEngines;
class CPlayer;
class CLifeSupport;
class CReactor;
class CEntityManager;
class CGameplayState;

enum eDirectorPhase {
	eOff, //the director is the progress is 0
	eBuildUp, eRelax, ePeakFade, //different phases to handle the spawns
	//events
	eSpawnEvent, // spawns one type enemies 
	eBeforeEvent, // gives time to the player to clear the level before a event and plays the sound of event type
	eEventCooldown, //this phase is called to create a forced safe situation to give the player the chance to recover if is in a really bad situation
	//events end
	eWaitUntilKillAll, //director waits until all enemies are destroyed to start the engines once again-> THIS IS FOR THE WIN STATE
	eBeforeTutorial,
	eTutorialPhase,
	eFinalCountDown, // the final WOW
	eDisabled // is to disable the director and never turn it on again
};

enum eSpawnType { eSpawnCreep, /*eSpawnSpammer,*/ eSpawnHunter, eSpawnSaboteur };

class CAIDirector :
	public CEntity
{
public:
	/*****************************************************************
	* CAIDirector(): default constructor.
	*
	* Ins: N/A
	*
	* Outs: N/A
	*
	* Returns: N/A
	*
	* Mod. Date:		07/05/2015
	* Mod. Initials:	JM
	*****************************************************************/
	CAIDirector();

	/*****************************************************************
	* ~CAIDirector(): default destructor.
	*
	* Ins: N/A
	*
	* Outs: N/A
	*
	* Returns: N/A
	*
	* Mod. Date:		07/05/2015
	* Mod. Initials:	JM
	*****************************************************************/
	~CAIDirector();

	/*****************************************************************
	* MonitorStress(): The core of the director to manage the player's stress in a given 
	moment and act according to that value.
	*
	* Ins: void
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		07/05/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void			MonitorStress();

	/*****************************************************************
	* BuildUpPhase(): Upon starting the engines, the Director enters Build Up mode. In this mode, 
	*				monsters will spawn normally.
	*
	* Ins: float fDt - Delta Time
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		07/05/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void			BuildUpPhase(float fDt);

	/*****************************************************************
	* PeakPhase(): The Director reaches "Peak" when the player is at maximum Intensity. It usually occurs at the peak of combat. 
	*				monsters stop spawning at this point.
	*
	* Ins: float fDt - Delta Time
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		07/05/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void			PeakFadePhase(float fDt);

	/*****************************************************************
	* RelaxPhase(): This phase gives a time frame to recuperate and buy/repair/heal
	*
	* Ins: float fDt - delta time
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		07/05/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void			RelaxPhase(float fDt);

	/*****************************************************************
	* FinalCountDownPhase(): our final wow moment. Enemies constantly spawn 
	*
	* Ins: void
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		07/26/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void			FinalCountDownPhase();

	/*****************************************************************
	* EvaluateIfEvent(float fDt): checks if the director needs to create special event
	*
	*
	* Ins: float Delta Time - time slice
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		07/05/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void			EvaluateIfEvent(float fDt);

	/*****************************************************************
	* EvaluateIfEventInfinite(float fDt): checks if the director needs to create special event in the new game mode
	*
	*
	* Ins: float Delta Time - time slice
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		08/14/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void			EvaluateIfEventInfinite(float fDt);

	/*****************************************************************
	* EventCoolDown(): gives the player time to clear the level before a certain event
	*
	* Ins: float - Delta Time
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:			07/19/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void		EventCoolDown(float fDt);

	/*****************************************************************
	* FinalSpawn(): Handles the final spawn event
	*
	* Ins: float - Delta Time
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:			07/27/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void		FinalSpawn();


	/*****************************************************************
	* BeforeEvent(): waits until the special event is completed
	*
	*
	* Ins: bool - to know the phase to go
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		07/19/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void			BeforeEvent(float fDt);

	/*****************************************************************
	* TutorialPhase(): handles the tutorial update
	*
	*
	* Ins: float delta time
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		07/22/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void			TutorialPhase(float fDt);

	/*****************************************************************
	* BeforeTutorialPhase(): handles the tutorial update
	*
	*
	* Ins: float delta time
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		07/24/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void			BeforeTutorialPhase(float fDt);

	/*****************************************************************
	* Reset(): Sets director to default values
	*
	*
	* Ins: void
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		07/05/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void			Reset();

	/*****************************************************************
	* IncreaseMaxEnemyAmount(): increases spawned max enemy amount.
	*
	* Ins: void
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		07/17/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void			IncreaseMaxEnemyAmount();

	/*****************************************************************
	* DecaySTress(): relaxes the stress over time
	*
	*
	* Ins: void
	*
	* Outs: N/A
	*
	* Returns: void
	*
	* Mod. Date:		07/09/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void DecaySTress();

	/*****************************************************************
	* SpawnSpecial(): spawns a monster due a special event 
	*				(f.e: if player stays too long in shop area when the director is ON)
	*
	*
	* Ins: int Type - spawn type depending on the event
	*
	* Outs: N/A
	*
	* Returns: bool - if we spawned successfully
	*
	* Mod. Date:		07/05/2015
	* Mod. Initials:	JM
	*****************************************************************/
	bool			SpawnSpecial(eSpawnType Type, eEnemyVariant variant = eNoVariant);

	/*****************************************************************
	* FindValidLocation(): checks against the frustum to spawn enemies outside the player's line of sight
	*
	*
	* Ins: void
	*
	* Outs: N/A
	*
	* Returns: DIRECTX::XMFLOAT3
	*
	* Mod. Date:		07/05/2015
	* Mod. Initials:	JM
	*****************************************************************/
	DirectX::XMFLOAT3			FindValidLocation();

	/*****************************************************************
	* ReadXml(): initialization.
	*
	* Ins: const char* filename
	*
	* Outs:
	*
	* Returns: bool - success or failure
	*
	* Mod. Date:			06/20/2015
	* Mod. Initials:		JM
	*****************************************************************/
	bool ReadXml(const char* filename);

	//override from base class
	virtual void	Update(float fDt) override;

	/*****************************************************************
	* ReadXml(): WaitUntilKillAll.
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: bool - if all enemies are dead
	*
	* Mod. Date:			07/18/2015
	* Mod. Initials:		JM
	*****************************************************************/
	bool		WaitUntilKillAll();

	/*****************************************************************
	* SpawnEvent(): initialization.
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: bool - success or failure
	*
	* Mod. Date:			07/18/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void		SpawnEvent();

	/*****************************************************************
	* Render(): print current objective.
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:			07/18/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void		Render();

	/*****************************************************************
	* ChangeObjective(); It handles the objective changed sound and the text flashing
	*
	* Ins: float fDt - delta time
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:			07/25/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void ChangeObjective(float fDt);

	/*****************************************************************
	* ResetObjectiveFlash(); function to reset the timers of the objective flashing
	*
	* Ins: void
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:			07/25/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void ResetObjectiveFlash();

	/*****************************************************************
	* HandleSoundAndSubtitles(): function that chooses what waifu message to play and sets the subtitle.
	*
	* Ins: float fDt
	*
	* Outs: void
	*
	* Returns: void
	*
	* Mod. Date:			08/05/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void HandleSoundAndSubtitles(float fDt);

	/*********************
	* Mutators
	*********************/
	void SetEngines(CEngines* myEng)					{ m_pTheEngines = myEng; }
	void SetLifeSupport(CLifeSupport* myLS)				{ m_pTheLifeSupport = myLS; }
	void SetReactor(CReactor* myRea)					{ m_pTheReactor = myRea; }
	void SetPlayer(CPlayer* myPlay)						{ m_pPlayer = myPlay; }
	void SetEntityManager(CEntityManager* myEnt)		{ m_pEntityManager = myEnt; }
	void SetSubtitle(std::string message);
	void SetWaifuOn(bool value) { m_bWaifuEnabled = value; }

	void SetGameplay(CGameplayState* these)				{ m_pGamestate = these; }
	void SetInfiniteMode(bool mode) { m_bInfiniteMode = mode; }
	void UpdateScore(int score) { m_nScore += score; m_bFlashScore = true; }
	void SetIsPaused(bool isit){ m_bIsPaused = isit; }

	/***************
	*  Accessors
	***************/
	int GetScore()const { return m_nScore; }
	bool GetPlayerDead()const{ return m_bPlayerDead; }

private:
	//pointers
	CEngines*						m_pTheEngines;
	CLifeSupport*					m_pTheLifeSupport;
	CReactor*						m_pTheReactor;
	CPlayer*						m_pPlayer;
	CEntityManager*					m_pEntityManager;
	CGameplayState*					m_pGamestate;

	//For monitorring
	float							m_fSystemDownWeight;
	float							m_fEnginesWeight;
	float							m_fLifeSupportWeight;
	float							m_fReactorWeight;
	float							m_fScrapWeight;
	float							m_fPlayerDamageWeight;
	float							m_fDistanceWeight;
	float							m_fDecayOverTime;
	
	float							m_fStressLevel;
	float							m_fMinimumPeak;
	float							m_fMaximumPeak;

	float							m_fCurrentMinimumPeak;
	float							m_fCurrentMaximumPeak;

	float							m_fMonitorDelayTimer;
	float							m_fCurrentDelay;
	bool							m_bBuildDone;
	eDirectorPhase					m_ePhase;

	float							m_fEventTimer;
	float							m_fBuildUpSPlit;
	float							m_fCurrentSplit;
	bool							m_bSplitted;

	float							m_fUpgradeWeight;

	float							m_fCurrentProgress;
	float							m_fProgressPercentCheck;

	int								m_nMaxAmountCreep;
	int								m_nMaxAmountHunter;
	int								m_nMaxAmountSaboteur;
	//int								m_nMaxAmountSpammer;
	//int								m_nCurrentMaxAmountSpammer;
	int								m_nCurrentMaxAmountCreep;
	int								m_nCurrentMaxAmountHunter;
	int								m_nCurrentMaxAmountSaboteur;

	int								m_nCreepAmountIncrement;
	//int								m_nSpammerAmountIncrement;
	int								m_nHunterAmountIncrement;
	int								m_nSaboteurAmountIncrement;

	float							m_fCreepEventDistance;
	//float							m_fSpammerEventDistance;
	float							m_fHunterEventDistance;
	float							m_fSaboteurEventDistance;
	float							m_fPlayerHealthEvent;

	bool							m_bCreepEventTriggered;
	//bool							m_bSpammerEventTriggered;
	bool							m_bHunterEventTriggered;
	bool							m_bSaboteurEventTriggered;

	//For Spawning monsters
	float							m_fCreepCost;
	//float							m_fSpammerCost;
	float							m_fHunterCost;
	float							m_fSaboteurCost;
	bool							m_bSpawnLimit;

	float							m_fSaboteurChance;
	float							m_fHunterChance;
	//float							m_fSpammerChance;
	//float							m_fMaxSpammerChance;
	float							m_fMaxSaboteurChance;
	float							m_fMaxHunterChance;

	float							m_fTimerToSpawn; // time to cooldown the spawn in build up phase
	float							m_fCurrentToSpawn;
	float							m_fTimerRelax; // time to cooldown the spawn in build up phase
	float							m_fRelaxSpawn;

	float							m_fHunterChanceModifier;
	float							m_fSaboteurChanceModifier;
	//float							m_fSpammerChanceModifier;
	
	eSpawnType						m_nSpawnEvent;
	bool							m_bEventSelected;
	bool							m_bChangeTo;

	//float							m_fSpawnEventMaxTime;
	//float							m_fSpawnEventCurrent;

	float							m_fdialogTimer;
	float							m_fdialogCurrent;

	bool							m_bTutorialCreep;
	bool							m_bTutorialHunter;
	//bool							m_bTutorialSpammer;
	bool							m_bTutorialSpawn;
	bool							m_bTutorialTipSaid;
	bool							m_bTutorialSpawned;
	bool							m_bIntro;

	int								m_nSpawnAmount;

	float							m_fSpawnIncrementTimer;
	float							m_fDelay;

	std::string						m_sObjective;
	std::string						m_sSubtitle;
	float							m_fSubtitleCurrent;
	float							m_fObjectiveFlashing;
	float							m_fObjectiveTotalFlashTime;
	DirectX::XMFLOAT4				m_ObjectiveFlashColor;
	DirectX::XMFLOAT4				m_ObjectiveBaseColor;
	DirectX::XMFLOAT4				m_ObjectiveCurrentColor;
	bool							m_bIsBaseColor;

	bool							m_bIsObjectiveChanged;

	float							m_fIntroWait;
	bool							m_bFinalObjectiveShowes;
	float							m_fVariantChance;
	float							m_fVariantIncrement;

	bool							m_bWaifuEnabled;
	bool							m_bInfiniteMode;

	int								m_nScore;
	float							m_fEnginesCurrentProgress;
	int								m_nPlayerCurrentScrap;

	//DirectX::XMFLOAT4				m_xmScoreBase;
	//DirectX::XMFLOAT4				m_xmScoreDiff;
	DirectX::XMFLOAT4				m_xmScoreColor;

	bool							m_bFlashScore;
	bool							m_bShowSubtitle;
	//float							m_fScoreFlashTimer;

	float							m_fInfiniteTimer;
	bool							m_bInfiniteTrigger;
	bool							m_bPlayerDead;

	bool							m_bShowRemainding;
	int								m_nRemainding;
	bool							m_bIsPaused;
};
#endif //_CAI_DIRECTOR_H_
