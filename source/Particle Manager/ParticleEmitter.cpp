/***********************************************
* Filename:			ParticleEmitter.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#include "ParticleEmitter.h"
#include "ParticleSystem.h"
#include "Particle.h"
using namespace DirectX;

/*****************************************************************
* CParticleEmitter():
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
CParticleEmitter::CParticleEmitter(CParticleSystem* system, CParticle& flyweight)
{
	m_pParent = system;
	m_pParent->AddEmitter(this);
	m_flyweight = flyweight;


	m_unAmmoutPerEmit = MAX_PARTICLES;
}

/*****************************************************************
* ~CParticleEmitter():
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
CParticleEmitter::~CParticleEmitter()
{
}

/*****************************************************************
* Emit() - emits 'numParticles' ammount of particles into the 
*		   particle system
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/18/2015
* Mod. Initials:
*****************************************************************/
void CParticleEmitter::Emit()
{
	CParticle* particles = &m_pParent->m_particles[0];
	unsigned int found = 0;
	for (unsigned int i = 0; i < m_unAmmoutPerEmit; i++)
	{
		bool foundParticle = false;
		for (unsigned int j = 0; j < MAX_PARTICLES; j++)
		{
			if (particles[j].m_fLifeTime <= 0.0f)
			{
				foundParticle = true;

				SetParticleValues(particles + j);
				found++;
				/*if (found >= i)
				{
					break;
				}*/
			}
		}
		// if no particle was found, take the oldest particle and recycle it
		/*if (!foundParticle)
		{
			for (unsigned int j = 0; j < MAX_PARTICLES; j++)
			{
				if (particles[j].m_fLifeTime < particles[j].m_fMaxLifeTime / 2.f)
				{
					foundParticle = true;

					SetParticleValues(particles + j);
				}
			}
		}*/
	}
}

void CParticleEmitter::SetParticleValues(CParticle* p)
{
	p->m_fLifeTime = p->m_fMaxLifeTime = fRand.next(m_fMinLifeTime, m_fMaxLifeTime);
	p->m_vVelocity = fRand.nextVector();
	float fTemp = fRand.next(m_fMinSpeed, m_fMaxSpeed);
	p->m_vVelocity.x *= fTemp;
	p->m_vVelocity.y *= fTemp;
	p->m_vVelocity.z *= fTemp;
	p->m_startColor = m_flyweight.m_startColor;
	p->m_endColor = m_flyweight.m_endColor;
	p->m_fSize = fRand.next(m_fMinSize, m_fMaxSize);
	p->m_fRotation = fRand.next(m_fMinRotation, m_fMaxRotation);
	p->m_fRotationSpeed = m_flyweight.m_fRotationSpeed;
	p->m_vPosition.x = p->m_vPosition.y = p->m_vPosition.z = 0.f;
}
