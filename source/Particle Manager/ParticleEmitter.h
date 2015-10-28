/***********************************************
* Filename:			ParticleEmitter.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#ifndef _CPARTICLEEMITTER_H_
#define _CPARTICLEEMITTER_H_


#include "..\Core\stdafx.h"
#include "Particle.h"
#include "../Math/fRand.h"

class CParticleSystem;

class CParticleEmitter
{
	friend class CParticleSystem;
	CParticleSystem* m_pParent;

	unsigned int m_unAmmoutPerEmit;

	float m_fMaxLifeTime;
	float m_fMinLifeTime;

	float m_fMinRotation;
	float m_fMaxRotation;

	float m_fMinSize;
	float m_fMaxSize;

	float m_fMinSpeed;
	float m_fMaxSpeed;

	//A FLYWEIGHT IS A POINTER
	//	THAT'S THE WHOLE POINT!
	CParticle m_flyweight;

	CFRand fRand;

	void SetParticleValues(CParticle* p);

public:
	CParticleEmitter(CParticleSystem* parent, CParticle& flywieght);
	~CParticleEmitter();

	void AddRef(void);
	void Release(void);

	void Emit();

	/***************
	*  Accessors
	***************/

	CParticleSystem* GetParent(void) const { return m_pParent; }

	/***************
	* Mutators
	***************/
	void SetStartColor(DirectX::XMFLOAT4 xmColor) { m_flyweight.m_startColor = xmColor; }
	void SetEndColor(DirectX::XMFLOAT4 xmColor) { m_flyweight.m_endColor = xmColor; }

};

#endif // _CPARTICLEEMITTER_H_
