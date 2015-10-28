#ifndef _SUBSYSTEM_H
#define _SUBSYSTEM_H

/***********************************************
* Filename:			Subsystem.h
* Date:      		05/22/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:			Base class for subsystems aboard
*					the ship
************************************************/

#include "../../LivingEntites.h"
#include "../../AI/AIDirector.h"

#define SUBSYSTEM_RECURRING_WARNING_TIMER 20.0f

class CGameplayState;
class CPlayer;
class CAssetManager;
class CParticleSystem;

enum eSubsystemtype { eSystemReactor, eSystemEngine, eSystemLife };

class CSubSystem : public CLivingEntities
{
public:



	/*****************************************************************
	* CSubSystem()
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
	CSubSystem(void);

	/*****************************************************************
	* ~CSubSystem()
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
	virtual ~CSubSystem(void);

	/*****************************************************************
	* Update() - Sets the position of the engines
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

	//Resets the subsystem to it's starting status
	virtual void Reset(void);

	/*****************************************************************
	* Kill() - kills the subsytem to fully destroyed status
	*
	* Ins:			void
	*
	* Outs:			void
	*
	* Returns:		void
	*
	* Mod. Date:		    06/20/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void Kill(void);

	/*****************************************************************
	* Restore() - Heals the subsystem by fAmount, will restore functionality
	*				at right health level
	*
	* Ins:			void
	*
	* Outs:			void
	*
	* Returns:		void
	*
	* Mod. Date:		    05/28/2015
	* Mod. Initials:		MJG
	*****************************************************************/
	virtual void Heal(float fAmount) override;

	void SetHealth(float fValue) { m_fCurrentHealth = fValue; }

	static CGameplayState* g_pGamePlayState;
	/***************
	 *  Accessors
	 ***************/
	bool GetIsDestroyed(void) const { return !m_bIsAlive; }
	bool GetIsRepairing(void) const { return m_bRepairing; }
	virtual TAABB* GetCollider() override { return tCollider; }
	eSubsystemtype	GetSystemType()const { return m_eMyType; }
	float GetRepairRange(void) const { return m_fRepairRange; }
	/***************
	 * Mutators
	 ***************/
	virtual void SetCollider(TAABB* pCollider) { tCollider = pCollider; }
	void	SetSystemType(eSubsystemtype myType) { m_eMyType = myType; }
	void SetRepairRange(float fRepairRange) { m_fRepairRange = fRepairRange; }
	void SetDirector(CAIDirector* director) { m_pAIDirector = director; }

protected:
	TAABB* tCollider;

	//std::vector<ID3D11ShaderResourceView*> m_healthTextures;

	// the ammount of time elapsed since the last output print
	float m_fPrintCounter;

	//Keep references to the player
	CPlayer* m_pPlayer = nullptr;

	//Being repaired by the player
	bool m_bRepairing = false;

	float			m_fCurrentDSoundTime;
	float			m_fDamageSoundTimer;
	float			m_fRecurringWarningTimer;
	float			m_fRepairRange = 300.0f;
	CAIDirector* m_pAIDirector;
private:
	//Keep references to the asset manager
	CAssetManager* m_pAssets = nullptr;

	CParticleSystem* m_pRepairEffect;
	CParticleSystem* m_pSmokeEffect;
	eSubsystemtype	m_eMyType;
	
};

#endif