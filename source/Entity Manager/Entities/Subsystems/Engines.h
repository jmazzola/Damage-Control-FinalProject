#ifndef _ENGINES_H
#define _ENGINES_H
/***********************************************
* Filename:			Engines.h
* Date:      		05/22/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:
************************************************/
#include "Subsystem.h"

class CEngines : public CSubSystem
{
	// the progress the engines have made towards the goal
	float		m_fProgress;

	// the ammount of progress per second
	float		m_fSpeed;

	bool		m_bStopped;
	//Is the holo display dimming or brightening?
	bool m_bWaxing = false;
	//Blink progress
	float m_fBlinkProgress = 0.0f;

	// the total distance to the goal
	float		m_fDestination;
	float		m_fCheckDistance;
	//bool		m_bAllDestroyed;

	//How close to the goal do we have to be before the final rush
	float m_fFinalEventDistance = 28.0f;
	//What distance do we start at?
	float m_fStartDistance = 0.0f;

	CRObject* m_pHoloDisplay = nullptr;

public:
	/*****************************************************************
	* CEngines()
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:
	*****************************************************************/
	CEngines(void);

	/*****************************************************************
	* ~CEngines() - Sets the position of the engines
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:
	*****************************************************************/
	virtual ~CEngines(void);

	/*****************************************************************
	* Update(): Updates the engines
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void Update(float dt) override;

	/*****************************************************************
	* SetPosition() - Sets the position of the engines
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:
	*****************************************************************/
	virtual void SetPosition(DirectX::XMFLOAT3& position) override;

	void TakeDamage(float fDamage) override;

	void Reset(void);
	
	/***************
	*  Accessors
	***************/
	CRObject* GetHoloDisplay(void) const { return m_pHoloDisplay; }
	// returns true if the ship has made it to its destination
	bool	IsComplete(void) const { return m_fProgress >= m_fDestination; }
	//bool	GetAllDestroyed(void) const { return m_bAllDestroyed; }

	// returns the current progress towards the goal
	float GetProgress(void) const { return m_fProgress; }

	// gets the distance to the goal
	float GetGoalDistance(void) const { return m_fDestination; }
	float GetGoalCheck() const;
	bool GetStopped()const{return m_bStopped; }
	float GetHealthRatio()const { return (m_fCurrentHealth / m_fMaxHealth); }

	/***************
	* Mutators
	***************/
	void SetProgress(float progress) { m_fProgress = progress; }
	//void SetAllDestroyed(bool bAllDestroyed) { m_bAllDestroyed = bAllDestroyed; }
	void SetStopped(bool stop) { m_bStopped = stop; }
	void SetGoalDistance(float distance) { m_fDestination = distance; }

protected:
	virtual bool LoadStats(void) override;

};

#endif