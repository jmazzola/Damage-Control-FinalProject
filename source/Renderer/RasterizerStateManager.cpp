#include "RasterizerStateManager.h"
#include "Renderer.h"

namespace EDRendererD3D
{
	RasterizerStateManager *RasterizerStateManager::instancePtr = 0;

	RasterizerStateManager::RasterizerStateManager(void)
	{
		currentState = RS_COUNT;
	}

	RasterizerStateManager::~RasterizerStateManager(void)
	{
		for (int i = 0; i < RasterStates::RS_COUNT; i++)
		{
			rasterStates[i] = nullptr;
		}
	}

	RasterizerStateManager &RasterizerStateManager::GetReference( )
	{
		if (!instancePtr)
		{
			instancePtr = new RasterizerStateManager;
			instancePtr->CreateStates( );
		}
		return *instancePtr;
	}

	void RasterizerStateManager::DeleteInstance( )
	{
		delete instancePtr;
		instancePtr = nullptr;
	}

	bool RasterizerStateManager::ApplyState(RasterStates state)
	{
		if (state >= RS_COUNT)
		{
			currentState = state;
			return false;
		}

		if (currentState == state)
			return false;

		CRenderer::GetReference( ).GetContext( )->RSSetState(rasterStates[state]);
		currentState = state;
		return true;
	}

	void RasterizerStateManager::ClearState( )
	{
		ApplyState(RasterStates::RS_COUNT);
	}

	void RasterizerStateManager::CreateStates( )
	{
		ID3D11Device* pDevice = CRenderer::GetReference( ).GetDevice( );

		//default Rasterizer State [Clock Wise]
		{
			RasterizerStateConfig desc;
			pDevice->CreateRasterizerState(&desc, &rasterStates[RS_Default_CW].p);
		}

		//default Rasterizer State [Counter Clock Wise]
		{
			RasterizerStateConfig desc;
			desc.FrontCounterClockwise = true;
			pDevice->CreateRasterizerState(&desc, &rasterStates[RS_Default_CCW].p);
		}

		//wireframe Rasterizer State [Clock Wise]
		{
			RasterizerStateConfig desc;
			desc.FillMode = D3D11_FILL_WIREFRAME;
			pDevice->CreateRasterizerState(&desc, &rasterStates[RS_Wireframe_CW].p);
		}

		//wireframe Rasterizer State [Counter Clock Wise]
		{
			RasterizerStateConfig desc;
			desc.FillMode = D3D11_FILL_WIREFRAME;
			desc.FrontCounterClockwise = true;
			pDevice->CreateRasterizerState(&desc, &rasterStates[RS_Wireframe_CCW].p);
		}

		//default Rasterizer State [Clock Wise with MSAA]
		{
			RasterizerStateConfig desc;
			desc.MultisampleEnable = true;
			pDevice->CreateRasterizerState(&desc, &rasterStates[RS_Default_MSAA_CW].p);
		}

		//default Rasterizer State [Counter Clock Wise with MSAA]
		{
			RasterizerStateConfig desc;
			desc.MultisampleEnable = true;
			desc.FrontCounterClockwise = true;
			pDevice->CreateRasterizerState(&desc, &rasterStates[RS_Default_MSAA_CCW].p);
		}

		//no cull Rasterizer State
		{
			RasterizerStateConfig desc;
			desc.CullMode = D3D11_CULL_NONE;
			pDevice->CreateRasterizerState(&desc, &rasterStates[RS_NoCull].p);
		}

		//no cull Rasterizer State [MSAA]
		{
			RasterizerStateConfig desc;
			desc.CullMode = D3D11_CULL_NONE;
			desc.MultisampleEnable = true;
			desc.DepthBiasClamp = 1.0f;
			desc.DepthClipEnable = false;
			pDevice->CreateRasterizerState(&desc, &rasterStates[RS_NoCull_MSAA].p);
		}
	}
}
