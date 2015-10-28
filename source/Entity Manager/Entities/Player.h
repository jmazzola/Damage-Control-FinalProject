/***********************************************
* Filename:			Player.h
* Date:      		05/15/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:			The player represents the user
*					in the game
************************************************/
#ifndef _PLAYER_H
#define _PLAYER_H

#include "../LivingEntites.h"
#include "../../Asset Manager/AssetManager.h"
#include "../../Transform.h"

#include "../../Math/Collision.h"

#include "../../Game Stats/GameStats.h"

class CIWeapon;
class CPickUp;
class CDroneBuddy;
class CModBank;
class CRadar;
enum EGunType : signed char;

class CPlayer : public CLivingEntities
{
public:

	CPlayer(void);
	virtual ~CPlayer(void);

	virtual void TakeDamage(float fAmount);
	virtual void Update(float dt) override;

	/*****************************************************************
	* Restore() - restores the player to default status
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
	void Reset(void);

	/*****************************************************************
	* HandlePickUp() - Reads PickUp's info and buffs player, kills PickUp
	*
	* Ins:			CPickUp*
	*
	* Outs:			void
	*
	* Returns:		void
	*
	* Mod. Date:		    05/28/2015
	* Mod. Initials:		MJG
	*****************************************************************/
	void HandlePickUp(CPickUp* pPickUp);

	void MaxUpgrades(void);

	bool RegisterAnimation(void);

	//Switches the current weapon
	bool SwitchWeapon(EGunType eWhichWeapon);

	/**************************
	* Mutators
	**************************/
	void SetAvailableDirs(bool* Directions)
	{
		AvailableMoveDirs[0] = Directions[0];
		AvailableMoveDirs[1] = Directions[1];
		AvailableMoveDirs[2] = Directions[2];
		AvailableMoveDirs[3] = Directions[3];
	}

	void SetCreepColliding(bool YesNo)
	{
		isCreepColliding = YesNo; m_fHinderedTimer = 0.5f;
	}
	void SetHunterColliding(bool YesNo)
	{
		isHunterColliding = YesNo; m_fHinderedTimer = 0.5f;
	}
	void SetSabotuerColliding(bool YesNo)
	{
		isSaboteurColliding = YesNo; m_fHinderedTimer = 0.5f;
	}
	void SetSpammerColliding(bool YesNo)
	{
		isSpammerColliding = YesNo; m_fHinderedTimer = 0.5f;
	}
	void SetScrapCount(int nScrapAmount)
	{
		m_nScrapAmount = nScrapAmount;
	}
	void SetTotalScrapCount(int nScrapAmount)
	{
		m_nTotalScrapAmount = nScrapAmount;
	}
	void SetOxygen(float fOxygen)
	{
		m_fOxygen = fOxygen;
	}
	void SetAnimation(CAssetManager::TAnimation* _pAnimation)
	{
		m_pAnimation = _pAnimation;
	}
	void SetCurrentHealth(float _currHealth)
	{
		m_fCurrentHealth = _currHealth;
	}

	void AddScrap(int nScrap)
	{
		m_nScrapAmount += nScrap; m_nTotalScrapAmount += nScrap;
	}
	void SubScrap(int nScrap)
	{
		m_pGameStats->m_nTotalScrapSpent += nScrap;
		m_nScrapAmount -= nScrap;
	}

	/**************************
	* Accessors
	***************************/
	//CModBank* GetCargo(void) const
	//{
	//	return m_pCargo;
	//}
	int GetScrapCount(void) const
	{
		return m_nScrapAmount;
	}
	int GetTotalScrapCount(void) const
	{
		return m_nTotalScrapAmount;
	}
	TSphere* GetCollider( )
	{
		return tCollider;
	}
	CIWeapon* GetCurrentWeapon( ) const
	{
		return m_pCurrentWeapon;
	}
	CIWeapon* GetPistol( ) const
	{
		return m_pPistol;
	}
	CIWeapon* GetMachineGun( ) const
	{
		return m_pMachineGun;
	}
	CIWeapon* GetShotgun( ) const
	{
		return m_pShotgun;
	}
	CIWeapon* GetDrone( )
	{
		return m_pDroneBuddy;
	}
	float GetOxygen(void) const
	{
		return m_fOxygen;
	}
	DirectX::XMFLOAT4X4& GetWeaponBone( )
	{
		return MyWorld.GetWorld( );
	}
	CDroneBuddy* GetDroneBuddy(void) const
	{
		return (CDroneBuddy*)m_pDroneBuddy;
	}

	CGameStats* GetGameStats() const
	{
		return m_pGameStats;
	}

	DirectX::XMFLOAT4X4 GetModelWorld( ) override
	{
		return ModelTransform.GetWorld( );
	}

	DirectX::XMFLOAT3 GetBindPosition( ) const
	{
		return m_XMBindPos;
	}

	DirectX::XMFLOAT3 GetMousePosition( )
	{
		return m_XMMousePos;
	}

private:
	void SplitCircle(float _fDT);
	void SplitSquare(float _fDT);

	bool m_bGodMode = false;
	bool m_bSprinting = false;
	Transform ModelTransform;

	//The currently equipped weapon
	CIWeapon* m_pCurrentWeapon = nullptr;
	//The default locations of guns
	CIWeapon* m_pPistol = nullptr;
	CIWeapon* m_pMachineGun = nullptr;
	CIWeapon* m_pShotgun = nullptr;
	CIWeapon* m_pDroneBuddy = nullptr;

	//Holds unused mods
	//CModBank* m_pCargo = nullptr;

	//How long until the next step sound plays
	float m_fStepTimer = 0.0f;

	float m_fOxygen;

	TSphere* tCollider;
	float m_fMoveSpeed;
	bool AvailableMoveDirs[4];
	//bool AvailableMoves[4];
	bool isCreepColliding = false;
	bool isHunterColliding = false;
	bool isSaboteurColliding = false;
	bool isSpammerColliding = false;
	float m_fHinderedTimer = 0.0f;
	void Turn(POINT mouse);
	void Input(float dt);


	bool m_bTabShow;

	int m_nScrapAmount;
	int m_nTotalScrapAmount;

	DirectX::XMFLOAT3 m_XMBindPos;

	DirectX::XMFLOAT3 m_XMMousePos;

	DirectX::XMFLOAT2 m_XMBindRadius;

	CGameStats* m_pGameStats;
	
	float m_fDeployTimer;
};

#endif
