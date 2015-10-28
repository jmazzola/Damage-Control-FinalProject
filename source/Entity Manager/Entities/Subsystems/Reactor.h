#ifndef _REACTOR_H
#define _REACTOR_H
/***********************************************
* Filename:			Reactor.h
* Date:      		05/22/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:
************************************************/
#include "Subsystem.h"

class CAIDirector; 

class CReactor : public CSubSystem
{
public:
	/*****************************************************************
	* CReactor()
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
	CReactor();

	/*****************************************************************
	* ~CReactor()
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
	virtual ~CReactor(void);

	/*****************************************************************
	* SetPosition() - sets the position of the reactor
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

	virtual void SetCollider(TAABB* pCollider) override;

	/*****************************************************************
	* Update() - Updates the reactor
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

	void TakeDamage(float fDamage) override;

	/***************
	*  Accessors
	***************/
	CRObject* GetCore(void) const { return m_pCore; }
	CRObject* GetServos(void) const { return m_pServos; }
	CRObject* GetBase(void) const { return m_pBase; }

	// returns the current time left in the meltdown timer
	float GetMeltdownTimer(void) const { return m_fMeltdownTimer; }

	float GetMaxMeltdownTimer(void) const { return m_fMaxMeltdownTime; }
	/***************
	* Mutators
	***************/

protected:
	virtual bool LoadStats(void) override;

private:
	//Handles all the moving parts
	void Animate(float fDt);

	//"Animation" values
	signed char m_cRising = 1;// 1 or -1
	signed char m_cStretching = 1;//1 or -1
	signed char m_cBrightening = 1;//1 or -1
	float m_fCoreScale = 0;
	float m_fCoreHeight = 0;
	float m_fCoreRotation = 0;
	float m_fServoRotation = 0;
	float m_fBaseRotation = 0;

	// timer for the meltdown sequence
	float m_fMeltdownTimer;

	// the value the meltdown timer returns to after restoring power
	float m_fMaxMeltdownTime = 30.f;

	CRObject* m_pCore;
	CRObject* m_pServos;
	CRObject* m_pBase;

};

#endif