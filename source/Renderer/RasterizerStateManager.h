#pragma once

#include "..\Core\stdafx.h"

namespace EDRendererD3D
{
	class RasterizerStateManager
	{
		struct RasterizerStateConfig : public D3D11_RASTERIZER_DESC
		{
			RasterizerStateConfig( )
			{
				SetDefaults( );
			}

		private:

			void SetDefaults( )
			{
				FillMode = D3D11_FILL_SOLID;
				CullMode = D3D11_CULL_BACK;
				FrontCounterClockwise = false;
				DepthBias = 0;
				SlopeScaledDepthBias = 0.0f;
				DepthBiasClamp = 0.0f;
				DepthClipEnable = true;
				ScissorEnable = false;
				MultisampleEnable = false;
				AntialiasedLineEnable = false;
			}
		};

	public:
		enum RasterStates
		{
			RS_Default_CW,
			RS_Default_CCW,

			RS_Wireframe_CW,
			RS_Wireframe_CCW,

			RS_Default_MSAA_CW,
			RS_Default_MSAA_CCW,

			RS_NoCull,
			RS_NoCull_MSAA,

			RS_COUNT
		};

		~RasterizerStateManager(void);

		static RasterizerStateManager &GetReference( );

		static void DeleteInstance( );

		bool ApplyState(RasterStates state);

		RasterStates GetCurrentState( )
		{
			return currentState;
		}

		void ClearState( );

	private:
		RasterizerStateManager(void);
		RasterizerStateManager(const RasterizerStateManager &)
		{
		}
		RasterizerStateManager &operator=(const RasterizerStateManager &)
		{
			return *this;
		}

		static RasterizerStateManager *instancePtr;

		CComPtr<ID3D11RasterizerState> rasterStates[RS_COUNT];

		RasterStates currentState;
		void CreateStates( );
	};
}
