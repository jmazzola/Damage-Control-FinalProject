/***********************************************
* Filename:			AchieveTracker.h
* Date:      		08/20/2015
* Mod. Date:		08/20/2015
* Mod. Initials:	BL
* Author:    		Brandon Lucier
* Purpose:			Tracks all the achievements 
*					that the player has (un)locked					
************************************************/
#ifndef _CACHIEVEMENTTRACKER_H_
#define _CACHIEVEMENTTRACKER_H_

class CTexture2D;

class CAchieveTracker
{
public:
	CAchieveTracker();
	~CAchieveTracker();

	enum eAchievements { eKiller, eSurvivor, eCampaigner, eLifeLover, eRectorLover, 
						 eEnginesLover, eBuyMachGun, eBuyShotGun, eBuyDrone, eAchieveCount };

	static CAchieveTracker* GetInstance();
	static void Terminate() { SafeDelete(InstancePtr) };

	void UnlockAchievement(eAchievements Achieve, float Dt = 0.0f);
	void RenderAchievement();

	void Update(float dt);

	void Load();
	void LoadDefault();
	void Save();

	/***************
	* Accessors
	****************/

	bool GetNotification() { return m_bNotifyPlayer; }

	unsigned int GetEnemiesKilled() { return m_nEnemiesKilled; }

	bool GetSurvivor() { return m_bSurvive10mins; }
	bool GetCampaignComplete() { return m_bCampaignComplete; }
	bool GetLifeSuppLover() { return m_bdidLifeSuppLive; }
	bool GetReactorLover() { return m_bdidReactorLive; }
	bool GetEnginesLover() { return m_bdidEnginesLive; }
	bool GetBoughtMachineGun() { return m_bBoughtMG; }
	bool GetBoughtShotgun() { return m_bBoughtSG; }
	bool GetBoughtDrone() { return m_bBoughtDB; }

	/***************
	* Mutators
	****************/
	void SetIsExit(bool isIt){ m_bExit = isIt; }

private:
	static CAchieveTracker* InstancePtr;

	// Base texture for displaying achievements
	CTexture2D* m_pTexture = nullptr;
	std::string m_sText;

	float m_fTextureTimer;

	// For knowing when to tell the player they got one
	bool m_bNotifyPlayer = false;

	// numbers to keep track of certain achievements
	unsigned int m_nEnemiesKilled = 0;
	float m_fSurvivorTimer = 0.0f;

	// Bools determining whether or not we got the achievement
	bool m_bKiller = false;
	bool m_bSurvive10mins = false;
	bool m_bCampaignComplete = false;

	bool m_bdidLifeSuppLive = false;
	bool m_bdidReactorLive = false;
	bool m_bdidEnginesLive = false;

	bool m_bBoughtMG = false;
	bool m_bBoughtSG = false;
	bool m_bBoughtDB = false;

	bool m_bExit = false;
};

#endif