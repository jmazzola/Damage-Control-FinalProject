/***********************************************
* Filename:			ParticleSystem.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#ifndef _CPARTICLESYSTEM_H_
#define _CPARTICLESYSTEM_H_


#include "..\Core\stdafx.h"
#include "Particle.h"

class CParticleEmitter;

class CParticleSystem
{
	friend class CParticleEmitter;

	// minimum speed for all particles
	float m_fMinSpeed;
	float m_fMaxSpeed;

	// particle instance to copy data from
	CParticle m_copyParticle;
	CParticle m_particles[MAX_PARTICLES];

	// inilitizes data in the particle
	void SetupParticle(CParticle& p);
	
	std::vector<CParticleEmitter*> m_emitters;

	// vertex buffer for all points in the particle system
	CComPtr<ID3D11Buffer> m_pPointBuffer;

	CComPtr<ID3D11ShaderResourceView> m_pSrv;
	CComPtr<ID3D11ShaderResourceView> m_pTextureSrv;

	float3 m_position;
	
	std::vector<CParticle> m_liveList;

public:
	CParticleSystem();
	~CParticleSystem();

	// updates the particle system
	void Update(float dt);

	// renders the particle system to the screen
	void Render(void);

	

	// adds an emitter to the system
	void AddEmitter(CParticleEmitter* emitter) { m_emitters.push_back(emitter); }

	// retrives a particle emitter
	CParticleEmitter* GetEmitter(unsigned int index) { return m_emitters[index]; }

	static CParticleSystem* LoadFrom(const char* filename);

	/****************************
		Accessors
	****************************/
	float3 GetPosition(void) const { return m_position; }


	/****************************
	Accessors
	****************************/
	void SetPosition(float3& value) { m_position = value; }
};

#endif // _CPARTICLESYSTEM_H_
