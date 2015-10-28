#ifndef _PARTICLEMANAGER_H
#define _PARTICLEMANAGER_H

#include <list>

class CParticleSystem;

class CParticleManager
{
public:
	~CParticleManager( );

	static CParticleManager* GetInstance( );
	static void DeleteInstance( );

	void AddParticleSystem(CParticleSystem* _pSystem);

	void Initialize( );

	void Update(float _fDT);
	void Render( );

	void DeactivateAll();

private:
	CParticleManager(void);
	CParticleManager(const CParticleManager &) = delete;
	CParticleManager &operator=(const CParticleManager &) = delete;

	static CParticleManager* m_pInstance;

	std::list<CParticleSystem*> m_particleSystems;
};

#endif