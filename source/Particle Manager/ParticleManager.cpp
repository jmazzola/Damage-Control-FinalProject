#include "ParticleManager.h"
#include "ParticleSystem.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/ShaderManager.h"

CParticleManager* CParticleManager::m_pInstance = nullptr;

CParticleManager::CParticleManager( )
{

}

CParticleManager::~CParticleManager( )
{
	//for (auto i = m_particleSystems.begin(); i != m_particleSystems.end(); i++)
	//	delete (*i);

	for (auto& i : m_particleSystems)
	{
		SafeDelete(i);
	}
}

CParticleManager* CParticleManager::GetInstance( )
{
	if (!m_pInstance)
	{
		m_pInstance = new CParticleManager( );
		m_pInstance->Initialize( );
	}

	return m_pInstance;
}

void CParticleManager::DeleteInstance(void)
{
	SafeDelete(m_pInstance);
}

void CParticleManager::AddParticleSystem(CParticleSystem* pSystem)
{
	m_particleSystems.push_back(pSystem);
}

void CParticleManager::DeactivateAll()
{
	for (auto& i : m_particleSystems)
	{
		i->SetPosition(float3(0, -1000, 0));
	}
}

void CParticleManager::Update(float _fDT)
{
	//for (auto i = m_particleSystems.begin(); i != m_particleSystems.end(); i++)
	//	(*i)->Update(dt);

	for (auto& i : m_particleSystems)
	{
		i->Update(_fDT);
	}
}

void CParticleManager::Render(void)
{
	CShaderManager::GetReference( ).ApplyGeometryShader(CShaderManager::GeometryShader::GS_PARTICLES);

	//for (auto i = m_particleSystems.begin(); i != m_particleSystems.end(); i++)
	//	(*i)->Render();

	for (auto& i : m_particleSystems)
	{
		i->Render( );
	}

	CShaderManager::GetReference( ).ApplyGeometryShader(CShaderManager::GeometryShader::GS_NONE);
}

void CParticleManager::Initialize(void)
{

}
