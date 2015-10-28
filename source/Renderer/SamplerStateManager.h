#pragma once

#include "..\Core\stdafx.h"

class CSamplerStateManager
{
public:
	enum SamplerState
	{
		SS_Default = 0, SS_CLAMP, SS_WRAP, SS_ANISOTROPIC, SS_COUNT
	};

	~CSamplerStateManager(void);

	static CSamplerStateManager &GetReference( );

	static void DeleteInstance( );

	bool ApplyState(SamplerState state);

	SamplerState GetCurrentState( )
	{
		return currentState;
	}

	void ClearState( );

private:
	CSamplerStateManager(void);
	CSamplerStateManager(const CSamplerStateManager &)
	{
	}
	CSamplerStateManager &operator=(const CSamplerStateManager &)
	{
		return *this;
	}

	static CSamplerStateManager *instancePtr;

	CComPtr<ID3D11SamplerState> samplerStates[SS_COUNT];

	SamplerState currentState;
	void CreateStates( );
};
