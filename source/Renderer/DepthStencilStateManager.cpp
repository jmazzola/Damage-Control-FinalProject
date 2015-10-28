#include "DepthStencilStateManager.h"
#include "Renderer.h"

namespace EDRendererD3D
{
	DepthStencilStateManager *DepthStencilStateManager::instancePtr = 0;

	DepthStencilStateManager::DepthStencilStateManager(void)
	{
		// purposely invalid option
		currentState = DSS_COUNT;
	}

	DepthStencilStateManager::~DepthStencilStateManager(void)
	{
		for (int i = 0; i < DSStates::DSS_COUNT; i++)
		{
			dsStates[i] = nullptr;
		}
	}

	DepthStencilStateManager &DepthStencilStateManager::GetReference( )
	{
		if (!instancePtr)
		{
			instancePtr = new DepthStencilStateManager;
			instancePtr->CreateStates( );
		}
		return *instancePtr;
	}

	void DepthStencilStateManager::DeleteInstance( )
	{
		delete instancePtr;
		instancePtr = nullptr;
	}

	bool DepthStencilStateManager::ApplyState(DSStates state)
	{
		if (state >= DSS_COUNT)
		{
			currentState = state;
			return false;
		}

		if (currentState == state)
			return false;

		CRenderer::GetReference( ).GetContext( )->OMSetDepthStencilState(dsStates[state], stencilRefs[state]);
		currentState = state;
		return true;
	}

	void DepthStencilStateManager::ClearState( )
	{
		ApplyState(DSStates::DSS_COUNT);
	}

	void DepthStencilStateManager::CreateStates( )
	{
		D3D11_DEPTH_STENCIL_DESC dssDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT( ));

		{
			dssDesc.DepthEnable = FALSE;
			CRenderer::GetReference( ).GetDevice( )->CreateDepthStencilState(&dssDesc, &dsStates[DSS_NoDepth].p);
			stencilRefs[DSS_NoDepth] = 0;
		}

		{
			dssDesc.DepthEnable = TRUE;
			CRenderer::GetReference( ).GetDevice( )->CreateDepthStencilState(&dssDesc, &dsStates[DSS_Default].p);
			stencilRefs[DSS_Default] = 0;
		}

		{
			dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			CRenderer::GetReference( ).GetDevice( )->CreateDepthStencilState(&dssDesc, &dsStates[DSS_LessEqual].p);
			stencilRefs[DSS_LessEqual] = 0;
		}

		{
			dssDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT( ));
			dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			CRenderer::GetReference( ).GetDevice( )->CreateDepthStencilState(&dssDesc, &dsStates[DSS_Particles].p);
		}


		// Create a depth stencil state for the G-Buffer effect. We'll enable depth
		// writes and depth tests. We'll also enable stencil writes, and set
		// the stencil buffer to 1 for each pixel rendered.
		D3D11_DEPTH_STENCIL_DESC dsConfig;
		{
			dsConfig.DepthEnable = true;
			dsConfig.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsConfig.DepthFunc = D3D11_COMPARISON_LESS;
			dsConfig.StencilEnable = true;
			dsConfig.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			dsConfig.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			dsConfig.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsConfig.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			dsConfig.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsConfig.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsConfig.BackFace = dsConfig.FrontFace;

			CRenderer::GetReference( ).GetDevice( )->CreateDepthStencilState(&dsConfig, &dsStates[DSS_Deferred_Lighting_GBuffer].p);
			stencilRefs[DSS_Deferred_Lighting_GBuffer] = 1;
		}


		// Create a depth stencil state for the final pass effect. We'll disable
		// depth writes, but leave depth testing on. We'll also disable stencil testing
		{
			dsConfig.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			dsConfig.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			dsConfig.StencilEnable = false;
			dsConfig.StencilWriteMask = 0;
			dsConfig.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsConfig.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
			dsConfig.BackFace = dsConfig.FrontFace;

			CRenderer::GetReference( ).GetDevice( )->CreateDepthStencilState(&dsConfig, &dsStates[DSS_Deferred_Lighting_Final].p);
			stencilRefs[DSS_Deferred_Lighting_Final] = 0;
		}



		// Make a depth-stencil state used for generating the MSAA mask.
		{
			dsConfig.DepthEnable = FALSE;
			dsConfig.DepthFunc = D3D11_COMPARISON_LESS;
			dsConfig.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsConfig.StencilEnable = true;
			dsConfig.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			dsConfig.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			dsConfig.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsConfig.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsConfig.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
			dsConfig.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
			dsConfig.BackFace = dsConfig.FrontFace;

			CRenderer::GetReference( ).GetDevice( )->CreateDepthStencilState(&dsConfig, &dsStates[DSS_Deferred_Lighting_Mask].p);
			stencilRefs[DSS_Deferred_Lighting_Mask] = 1;
		}

		// Create a depth stencil state with no depth testing, and with stencil testing
		// enabled to make sure we only light pixels where we rendered to the G-Buffer
		{
			dsConfig.DepthEnable = FALSE;
			dsConfig.DepthFunc = D3D11_COMPARISON_LESS;
			dsConfig.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			dsConfig.StencilEnable = TRUE;
			dsConfig.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			dsConfig.StencilWriteMask = 0;
			dsConfig.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsConfig.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsConfig.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsConfig.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
			dsConfig.BackFace = dsConfig.FrontFace;

			CRenderer::GetReference( ).GetDevice( )->CreateDepthStencilState(&dsConfig, &dsStates[DSS_Deferred_Lighting_Disabled_1].p);
			stencilRefs[DSS_Deferred_Lighting_Disabled_1] = 1;
			CRenderer::GetReference( ).GetDevice( )->CreateDepthStencilState(&dsConfig, &dsStates[DSS_Deferred_Lighting_Disabled_2].p);
			stencilRefs[DSS_Deferred_Lighting_Disabled_2] = 2;
		}

		// Create a depth stencil state with greater-than-equal depth testing
		dsConfig.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
		CRenderer::GetReference( ).GetDevice( )->CreateDepthStencilState(&dsConfig, &dsStates[DSS_Deferred_Lighting_GreaterThanEqual_1].p);
		stencilRefs[DSS_Deferred_Lighting_GreaterThanEqual_1] = 1;
		CRenderer::GetReference( ).GetDevice( )->CreateDepthStencilState(&dsConfig, &dsStates[DSS_Deferred_Lighting_GreaterThanEqual_2].p);
		stencilRefs[DSS_Deferred_Lighting_GreaterThanEqual_2] = 2;

		dsConfig.DepthEnable = FALSE;
		dsConfig.DepthFunc = D3D11_COMPARISON_LESS;
		dsConfig.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsConfig.StencilEnable = FALSE;
		dsConfig.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dsConfig.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		dsConfig.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsConfig.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsConfig.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsConfig.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

		dsConfig.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsConfig.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsConfig.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsConfig.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

		CRenderer::GetReference( ).GetDevice( )->CreateDepthStencilState(&dsConfig, &dsStates[DSS_Deferred_Lighting_Final_Combine].p);
		stencilRefs[DSS_Deferred_Lighting_Final_Combine] = 0;
	}
}
