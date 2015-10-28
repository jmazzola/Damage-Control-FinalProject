/***********************************************
* Filename:			PickUp.cpp
* Date:      		06/3/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Gergar
* Purpose:			An entity that represents
*				objects that are destroyed when
*				colliding with the player that
*				bestow bonuses
************************************************/
#ifndef _PICKUP_H_
#define _PICKUP_H_

#include "../Entity.h"

class CPickUp : public CEntity
{
public:
	CPickUp(CEntity* pPlayer);
	~CPickUp();

	void Update(float fDt) override;
	void Enable(DirectX::XMFLOAT3 pos, eEnemyVariant variant = eEnemyVariant::eNoVariant) override;
	void Disable(void) override;

	//-------Accessors-----------
	int GetScrapValue(void) const { return m_nScrapValue; }
	bool GetAlive(void) const { return m_fLifeLived < m_fLifeTime; }
	//-------Mutators-----------
	void SetScrapValue(int nScrapValue) { m_nScrapValue = nScrapValue; }
private:
	int m_nScrapValue = 5;
	//If we need pick-ups for something other than scrap can add them here
	//	Or add a type enum and just have the value and type stored

	//Max time it will live before despawning
	float m_fLifeTime = 10.0f;
	//Time it has lived
	float m_fLifeLived = 100.0f;
	//Move towards the player
	CEntity* m_pPlayer = nullptr;
	float m_fPercentSpeed = 0.0f;
};

#endif