#include "stdafx.h"
#include "RenderTargetViewManager.h"


CRenderTargetViewManager *CRenderTargetViewManager::instancePtr = 0;

CRenderTargetViewManager::CRenderTargetViewManager(void)
{
	// purposely invalid option
	m_tCurrentRenderState = eRenderTargetView::COUNT;
}

CRenderTargetViewManager::~CRenderTargetViewManager(void)
{
}

CRenderTargetViewManager &CRenderTargetViewManager::GetReference( )
{
	if (!instancePtr)
	{
		instancePtr = new CRenderTargetViewManager;
	}
	return *instancePtr;
}

void CRenderTargetViewManager::DeleteInstance( )
{
	delete instancePtr;
	instancePtr = nullptr;
}

bool CRenderTargetViewManager::ApplyRenderTargetView(eRenderTargetView state)
{
	if (state >= COUNT)
	{
		m_tCurrentRenderState = state;
		return false;
	}

	if (m_tCurrentRenderState == state)
		return false;

	m_tCurrentRenderState = state;

	return true;
}

void CRenderTargetViewManager::ClearState( )
{
	ApplyRenderTargetView(eRenderTargetView::COUNT);
}