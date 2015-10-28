#include "stdafx.h"
#include "PrimitiveTopologyManager.h"

#include "Renderer.h"

CPrimitiveTopologyManager *CPrimitiveTopologyManager::instancePtr = 0;

CPrimitiveTopologyManager::CPrimitiveTopologyManager(void)
{
	// purposely invalid option
	m_tCurrentTopology = TOPOLOGY_COUNT;
}

CPrimitiveTopologyManager::~CPrimitiveTopologyManager(void)
{
}

CPrimitiveTopologyManager &CPrimitiveTopologyManager::GetReference( )
{
	if (!instancePtr)
	{
		instancePtr = new CPrimitiveTopologyManager;
	}
	return *instancePtr;
}

void CPrimitiveTopologyManager::DeleteInstance( )
{
	delete instancePtr;
	instancePtr = nullptr;
}

bool CPrimitiveTopologyManager::ApplyTopology(TTopology topology)
{
	if (topology >= TOPOLOGY_COUNT)
	{
		m_tCurrentTopology = topology;
		return false;
	}

	if (m_tCurrentTopology == topology)
		return false;

	switch (topology)
	{
		case CPrimitiveTopologyManager::TOPOLOGY_TRIANGLELIST:
			{
				CRenderer::GetReference().GetContext( )->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			}
			break;

		case CPrimitiveTopologyManager::TOPOLOGY_POINTLIST:
			{
				CRenderer::GetReference().GetContext( )->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			}
			break;

		case CPrimitiveTopologyManager::TOPOLOGY_LINELIST:
			{
				CRenderer::GetReference().GetContext( )->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
			}
	}

	m_tCurrentTopology = topology;

	return true;
}

void CPrimitiveTopologyManager::ClearState( )
{
	ApplyTopology(TTopology::TOPOLOGY_COUNT);
}