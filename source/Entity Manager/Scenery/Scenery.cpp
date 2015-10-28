/***********************************************
* Filename:			Scenery.cpp
* Date:      		06/14/2015
* Mod. Date:		06/14/2015
* Mod. Initials:	MJG
* Author:    		Michael Gergar
* Purpose:			Base class for all entities
*				that the player interacts with.
*				Currently excludes sub-systems
************************************************/
#include "Scenery.h"
#include "../../Math/Collision.h"
using namespace SceneryHelpers;

CScenery::CScenery()
{
	m_pInteractTest = DefaultSceneryTest;
	m_pAction = DefaultSceneryAction;
}

CScenery::CScenery(bool(*pInteractTest)(CEntity*), void(*pAction)(CScenery*))
{
	m_pInteractTest = pInteractTest;
	m_pAction = pAction;
}

CScenery::~CScenery()
{
	if (m_pInteractZone)
	{
		delete m_pInteractZone;
	}
}

void CScenery::Update(float fDt)
{
	CEntity::Update(fDt);
	if (GetInteract() && fDt > 0)
	{
		m_pAction(this);
	}
	if (GetActive())
	{
		m_bWasInteracting = m_bInteracting;
		m_bInteracting = false;
	}
}

bool CScenery::TestZoneCollision(CEntity* pOther)
{
	//Are we looking for stuff
	if (GetActive() && CCollision::ColliderToCollider(GetZone(), pOther->GetCollider()) && m_pInteractTest(pOther))
	{
		SetInteract(true);
		return true;
	}

	return false;
}

void CScenery::Enable(void)
{
	SetInteract(false);
	SetActive(true);
}

void CScenery::Disable(void)
{
	SetInteract(false);
	SetActive(false);
}

namespace SceneryHelpers
{
	//Default Test Function
	bool DefaultSceneryTest(CEntity* pOther)
	{
		if (pOther == nullptr)
		{
			return false;
		}
		return true;
	}
	//Default action function
	void DefaultSceneryAction(CScenery* pThis)
	{

	}

	//We only activate for the player
	bool PlayerSceneryTest(CEntity* pOther)
	{
		if (pOther && pOther->GetEntityType() == CEntity::ePLAYER)
		{
			return true;
		}
		return false;
	}
}