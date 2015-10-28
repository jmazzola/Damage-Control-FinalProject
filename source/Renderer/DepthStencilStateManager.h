#pragma once

#include "..\Core\stdafx.h"

namespace EDRendererD3D
{
	class DepthStencilStateManager
	{
	public:
		enum DSStates
		{
			//default state bound most often
			DSS_Default = 0,

			//GBuffer state
			DSS_Deferred_Lighting_GBuffer,

			DSS_Deferred_Lighting_Final,

			DSS_Deferred_Lighting_Final_Combine,

			//GBuffer mask
			DSS_Deferred_Lighting_Mask,

			//Lighting no depth
			DSS_Deferred_Lighting_Disabled_1,
			DSS_Deferred_Lighting_Disabled_2,

			//Lighting greater than equal
			DSS_Deferred_Lighting_GreaterThanEqual_1,
			DSS_Deferred_Lighting_GreaterThanEqual_2,

			//Skybox state
			DSS_LessEqual,

			//2D state
			DSS_NoDepth,

			//special particles state
			DSS_Particles,

			DSS_COUNT
		};

		~DepthStencilStateManager(void);

		static DepthStencilStateManager &GetReference( );

		static void DeleteInstance( );

		bool ApplyState(DSStates state);

		DSStates GetCurrentState( )
		{
			return currentState;
		}

		void ClearState( );

	private:
		DepthStencilStateManager(void);
		DepthStencilStateManager(const DepthStencilStateManager &)
		{
		}
		DepthStencilStateManager &operator=(const DepthStencilStateManager &)
		{
			return *this;
		}

		static DepthStencilStateManager *instancePtr;

		CComPtr<ID3D11DepthStencilState > dsStates[DSS_COUNT];
		UINT stencilRefs[DSS_COUNT];

		DSStates currentState;
		void CreateStates( );
	};
}
