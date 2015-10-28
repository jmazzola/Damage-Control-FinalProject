#include "BlendStateManager.h"
#include "Renderer.h"

namespace EDRendererD3D
{
	BlendStateManager *BlendStateManager::instancePtr = 0;

	BlendStateManager::BlendStateManager(void)
	{
		currentState = BS_COUNT;
	}

	BlendStateManager::~BlendStateManager(void)
	{
		for (int i = 0; i < BStates::BS_COUNT; i++)
		{
			blendStates[i] = nullptr;
		}
	}

	BlendStateManager &BlendStateManager::GetReference( )
	{
		if (!instancePtr)
		{
			instancePtr = new BlendStateManager;
			instancePtr->CreateStates( );
		}
		return *instancePtr;
	}

	void BlendStateManager::DeleteInstance( )
	{
		delete instancePtr;
		instancePtr = nullptr;
	}

	bool BlendStateManager::ApplyState(BStates state)
	{
		if (state >= BS_COUNT)
		{
			currentState = state;
			return false;
		}

		if (currentState == state)
			return false;

		CRenderer::GetReference().GetContext( )->OMSetBlendState(blendStates[state], 0, 0xffffffff);
		currentState = state;
		return true;
	}

	void BlendStateManager::ClearState( )
	{
		ApplyState(BStates::BS_COUNT);
	}

	void BlendStateManager::CreateStates( )
	{
		D3D11_BLEND_DESC desc = CD3D11_BLEND_DESC(CD3D11_DEFAULT( ));
		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;
		desc.RenderTarget[0].BlendEnable = FALSE;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		CRenderer::GetReference().GetDevice( )->CreateBlendState(&desc, &blendStates[BS_Default].p);
		//SetD3DName(blendStates[BS_Default].p, std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

		desc.RenderTarget[0].BlendEnable = TRUE;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		//begin crap i added
		//
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		//
		//end crap i added

		CRenderer::GetReference().GetDevice( )->CreateBlendState(&desc, &blendStates[BS_Alpha].p);

		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		CRenderer::GetReference().GetDevice( )->CreateBlendState(&desc, &blendStates[BS_Additive].p);

		// Create a blend state for additive blending
		D3D11_BLEND_DESC blendConfig;
		blendConfig.AlphaToCoverageEnable = false;
		blendConfig.IndependentBlendEnable = false;
		for (int i = 0; i < 8; ++i)
		{
			blendConfig.RenderTarget[i].BlendEnable = TRUE;
			blendConfig.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			blendConfig.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
			blendConfig.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
			blendConfig.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendConfig.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendConfig.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
			blendConfig.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}

		CRenderer::GetReference( ).GetDevice( )->CreateBlendState(&blendConfig, &blendStates[BS_Lighting_Alpha].p);
	}
}
