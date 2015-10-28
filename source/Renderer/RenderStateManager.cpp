#include "stdafx.h"
#include "RenderStateManager.h"

#include "Renderer.h"

#include "BlendStateManager.h"
#include "DepthStencilStateManager.h"
//#include "RenderTargetViewManager.h"

#include "PrimitiveTopologyManager.h"
#include "RasterizerStateManager.h"
#include "SamplerStateManager.h"
#include "ShaderManager.h"
#include "VertexStructures.h"

using namespace EDRendererD3D;

CRenderStateManager *CRenderStateManager::instancePtr = 0;

CRenderStateManager::CRenderStateManager(void)
{
	// purposely invalid option
	m_tCurrentRenderState = eRenderState::COUNT;
}

CRenderStateManager::~CRenderStateManager(void)
{
}

CRenderStateManager &CRenderStateManager::GetReference( )
{
	if (!instancePtr)
	{
		instancePtr = new CRenderStateManager;
	}
	return *instancePtr;
}

void CRenderStateManager::DeleteInstance( )
{
	InputLayoutManager::DeleteInstance( );
	CShaderManager::DeleteInstance( );
	CSamplerStateManager::DeleteInstance( );
	CPrimitiveTopologyManager::DeleteInstance( );
	RasterizerStateManager::DeleteInstance( );
	DepthStencilStateManager::DeleteInstance( );
	BlendStateManager::DeleteInstance( );
	SafeDelete(instancePtr);
}

bool CRenderStateManager::ApplyRenderState(eRenderState state)
{
	if (state >= COUNT)
	{
		m_tCurrentRenderState = state;
		return false;
	}

	if (m_tCurrentRenderState == state)
		return false;

	BlendStateManager&			BlendState = BlendStateManager::GetReference( );
	DepthStencilStateManager&	DepthStencilState = DepthStencilStateManager::GetReference( );
	//CRenderTargetViewManager&	RenderTargetView = CRenderTargetViewManager::GetReference( );
	InputLayoutManager&			InputLayout = InputLayoutManager::GetReference( );
	CShaderManager&				ShaderManager = CShaderManager::GetReference( );
	CSamplerStateManager&		SamplerState = CSamplerStateManager::GetReference( );
	CPrimitiveTopologyManager&	PrimitiveTopology = CPrimitiveTopologyManager::GetReference( );
	RasterizerStateManager&		RasterizerState = RasterizerStateManager::GetReference( );

	BlendState.ApplyState(BlendStateManager::BStates::BS_Default);
	DepthStencilState.ApplyState(DepthStencilStateManager::DSStates::DSS_Default);
	//RenderTargetView.ApplyState( );

	SamplerState.ApplyState(CSamplerStateManager::SamplerState::SS_WRAP);

	PrimitiveTopology.ApplyTopology(CPrimitiveTopologyManager::TTopology::TOPOLOGY_TRIANGLELIST);

	RasterizerState.ApplyState(
		RasterizerStateManager::RasterStates::RS_Default_CCW);

	switch (state)
	{
#pragma region TwoD
		case CRenderStateManager::eRenderState::HUD:
			{
				Apply2D( );

				ShaderManager.ApplyPixelShader(CShaderManager::PixelShader::PS_TEXTURE);
				ShaderManager.ApplyVertexShader(CShaderManager::VertexShader::VS_HUD);
			}
			break;
		case CRenderStateManager::eRenderState::FONT:
			{
				Apply2D( );

				ShaderManager.ApplyPixelShader(CShaderManager::PixelShader::PS_FONT);
				ShaderManager.ApplyVertexShader(CShaderManager::VertexShader::VS_FONT);
			}
			break;
#pragma endregion

#pragma region Lighting
		case CRenderStateManager::eRenderState::GEOMETRY:
			{
				ShaderManager.ApplyPixelShader(CShaderManager::PixelShader::PS_GBUFFER);
				ShaderManager.ApplyVertexShader(CShaderManager::VertexShader::VS_GBUFFER);
			}
			break;
		case CRenderStateManager::eRenderState::LIGHTING:
			{
			}
			break;
		case CRenderStateManager::eRenderState::MATERIALS:
			{
				ShaderManager.ApplyPixelShader(CShaderManager::PixelShader::PS_TEXTURE);
				ShaderManager.ApplyVertexShader(CShaderManager::VertexShader::VS_TEST);
			}
			break;
#pragma endregion

#pragma region Transparent
		case CRenderStateManager::eRenderState::BACKFACE_TRANSPARENT:
			{
				ApplyTransparency( );

				RasterizerState.ApplyState(RasterizerStateManager::RasterStates::RS_Default_CW);
			}
			break;
		case CRenderStateManager::eRenderState::FRONTFACE_TRANSPARENT:
			{
				ApplyTransparency( );

				RasterizerState.ApplyState(RasterizerStateManager::RasterStates::RS_Default_CCW);
			}
			break;
#pragma endregion

		case CRenderStateManager::eRenderState::ANIMATION:
			{
				ShaderManager.ApplyPixelShader(CShaderManager::PixelShader::PS_TEXTURE);
				ShaderManager.ApplyVertexShader(CShaderManager::VertexShader::VS_SKINNING);
			}
			break;
		case CRenderStateManager::eRenderState::INSTANCE_ANIMATION:
			{
			}
			break;

		case CRenderStateManager::eRenderState::SKYBOX:
			{
				DepthStencilState.ApplyState(DepthStencilStateManager::DSStates::DSS_LessEqual);
				RasterizerState.ApplyState(RasterizerStateManager::RasterStates::RS_NoCull);
				SamplerState.ApplyState(CSamplerStateManager::SamplerState::SS_WRAP);

				ShaderManager.ApplyPixelShader(CShaderManager::PixelShader::PS_SKYBOX);
				ShaderManager.ApplyVertexShader(CShaderManager::VertexShader::VS_SKYBOX);
			}
			break;

		case CRenderStateManager::eRenderState::DEBUG_LINE:
			{
				RasterizerState.ApplyState(RasterizerStateManager::RasterStates::RS_NoCull);
				PrimitiveTopology.ApplyTopology(CPrimitiveTopologyManager::TTopology::TOPOLOGY_LINELIST);

				ShaderManager.ApplyPixelShader(CShaderManager::PixelShader::PS_DEBUG_LINE);
				ShaderManager.ApplyVertexShader(CShaderManager::VertexShader::VS_DEBUG_LINE);
			}
			break;

		case CRenderStateManager::eRenderState::PARTICLES:
			{
				RasterizerState.ApplyState(RasterizerStateManager::RasterStates::RS_NoCull);
				BlendState.ApplyState(BlendStateManager::BS_Alpha);
				DepthStencilState.ApplyState(DepthStencilStateManager::DSS_Particles);
				PrimitiveTopology.ApplyTopology(CPrimitiveTopologyManager::TTopology::TOPOLOGY_POINTLIST);

				ShaderManager.ApplyPixelShader(CShaderManager::PixelShader::PS_PARTICLES);
				ShaderManager.ApplyVertexShader(CShaderManager::VertexShader::VS_PARTICLES);
			}
			break;
	}

	m_tCurrentRenderState = state;

	return true;
}

void CRenderStateManager::ClearState( )
{
	BlendStateManager::GetReference( ).ClearState( );
	DepthStencilStateManager::GetReference( ).ClearState( );
	//CRenderTargetViewManager::GetReference( ).ClearState( );

	InputLayoutManager::GetReference( ).ClearState( );

	CShaderManager::GetReference( ).ClearPipeline( );

	CSamplerStateManager::GetReference( ).ClearState( );

	CPrimitiveTopologyManager::GetReference( ).ClearState( );

	RasterizerStateManager::GetReference( ).ClearState( );

	ApplyRenderState(eRenderState::COUNT);
}

void CRenderStateManager::ApplyTransparency( )
{
	BlendStateManager::GetReference( ).ApplyState(BlendStateManager::BStates::BS_Alpha);
	CShaderManager::GetReference( ).ApplyPixelShader(CShaderManager::PixelShader::PS_TEXTURE);
	CShaderManager::GetReference( ).ApplyVertexShader(CShaderManager::VertexShader::VS_TEST);
}
void CRenderStateManager::Apply2D( )
{
	DepthStencilStateManager::GetReference( ).ApplyState(DepthStencilStateManager::DSStates::DSS_NoDepth);
	RasterizerStateManager::GetReference( ).ApplyState(RasterizerStateManager::RasterStates::RS_NoCull);
	BlendStateManager::GetReference( ).ApplyState(BlendStateManager::BStates::BS_Alpha);
}
