#pragma once

#include "..\Core\stdafx.h"

namespace EDRendererD3D
{
	class BlendStateManager
	{
	public:
		enum BStates
		{
			BS_Default = 0,
			BS_Alpha,
			BS_Additive,

			BS_Lighting_Alpha,

			BS_COUNT,
		};

		~BlendStateManager(void);

		static BlendStateManager &GetReference( );

		static void DeleteInstance( );

		bool ApplyState(BStates state);

		BStates GetCurrentState( )
		{
			return currentState;
		}

		void ClearState( );

	private:
		BlendStateManager(void);
		BlendStateManager(const BlendStateManager &)
		{
		}
		BlendStateManager &operator=(const BlendStateManager &)
		{
			return *this;
		}

		static BlendStateManager *instancePtr;

		CComPtr<ID3D11BlendState > blendStates[BS_COUNT];

		BStates currentState;
		void CreateStates( );
	};
}
