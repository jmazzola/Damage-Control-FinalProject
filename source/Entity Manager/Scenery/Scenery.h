/***********************************************
* Filename:			Scenery.h
* Date:      		06/14/2015
* Mod. Date:		06/14/2015
* Mod. Initials:	MJG
* Author:    		Michael Gergar
* Purpose:			Base class for all entities
*				that the player interacts with.
*				Currently excludes sub-systems
************************************************/
#ifndef _SCENERY_H_
#define _SCENERY_H_

#include "../Entity.h"

class CScenery : public CEntity
{
public:
	//ctor
	//	Test will always pass
	CScenery();
	//Pass in its test for custom functionality
	CScenery(bool(*pInteractTest)(CEntity*), void(*pAction)(CScenery*));
	//copy ctor
	CScenery(const CScenery& cOther) = delete;
	//operator =
	CScenery& operator= (const CScenery& cOther) = delete;
	//dtor
	virtual ~CScenery();

	virtual void Update(float fDt) override;

	virtual void Enable(void);

	virtual void Disable(void);

	bool TestZoneCollision(CEntity* pOther);

	//---------Accessors-------------
	TCollider* GetZone(void) const { return m_pInteractZone; }
	bool GetInteract(void) const { return m_bInteracting; }
	bool GetWasInteract(void) const { return m_bWasInteracting; }
	bool GetActive(void) const { return m_bActive; }
	//Returns true if we weren't interacting last frame, but are this frame
	bool GetFirstFrame(void) const { return m_bInteracting && !m_bWasInteracting; }
	//---------Mutators-------------
	void SetZone(TCollider* pInteractZone)  { m_pInteractZone = pInteractZone; }
	void SetInteract(bool bInteracting) { m_bInteracting = bInteracting; }
	void SetActive(bool bActive) { m_bActive = bActive; }

protected:
	//If the player (or something else) enters this zone
	//	do something
	TCollider* m_pInteractZone = nullptr;
	//Is something in our zone
	//	Reset every frame
	bool m_bInteracting = false;
	//Was something in our zone?
	bool m_bWasInteracting = false;
	//Are we looking for dudes?
	bool m_bActive = true;
private:
	//Test objects in our zone
	//	Only after they pass the collision test
	bool (*m_pInteractTest)(CEntity*) = nullptr;
	//Act while objects are in our zone (once per frame)
	void (*m_pAction)(CScenery*) = nullptr;
};

namespace SceneryHelpers
{
	//Always returns true
	bool DefaultSceneryTest(CEntity* pOther);
	//Does nothing
	void DefaultSceneryAction(CScenery* pThis);

	//Returns true on player
	bool PlayerSceneryTest(CEntity* pOther);

}

#endif