#include "SamplerStateManager.h"

#include "Renderer.h"

CSamplerStateManager *CSamplerStateManager::instancePtr = 0;

CSamplerStateManager::CSamplerStateManager(void)
{
	currentState = SS_COUNT;
}

CSamplerStateManager::~CSamplerStateManager(void)
{
	for (int i = 0; i < SamplerState::SS_COUNT; i++)
	{
		samplerStates[i] = nullptr;
	}
}

CSamplerStateManager &CSamplerStateManager::GetReference( )
{
	if (!instancePtr)
	{
		instancePtr = new CSamplerStateManager;
		instancePtr->CreateStates( );
	}
	return *instancePtr;
}

void CSamplerStateManager::DeleteInstance( )
{
	delete instancePtr;
	instancePtr = nullptr;
}

bool CSamplerStateManager::ApplyState(SamplerState state)
{
	if (state >= SS_COUNT)
	{
		currentState = state;
		return false;
	}

	if (currentState == state)
		return false;

	CRenderer::GetReference( ).GetContext( )->PSSetSamplers(0, 1, &samplerStates[state].p);
	//CRenderer::GetReference( ).GetContext( )->VSSetSamplers(0, 1, &samplerStates[state].p);

	currentState = state;
	return true;
}

void CSamplerStateManager::ClearState( )
{
	ApplyState(SamplerState::SS_COUNT);
}

void CSamplerStateManager::CreateStates( )
{
	CD3D11_SAMPLER_DESC samplerStateDesc(D3D11_DEFAULT);

	CRenderer::GetReference( ).GetDevice( )->CreateSamplerState(&samplerStateDesc, &samplerStates[SS_Default]);
	//SetD3DName(samplerStates[SS_Default], std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	samplerStateDesc.AddressU = samplerStateDesc.AddressV = samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	CRenderer::GetReference( ).GetDevice( )->CreateSamplerState(&samplerStateDesc, &samplerStates[SS_CLAMP]);
	//SetD3DName(samplerStates[SS_CLAMP], std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());

	samplerStateDesc.AddressU = samplerStateDesc.AddressV = samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	CRenderer::GetReference( ).GetDevice( )->CreateSamplerState(&samplerStateDesc, &samplerStates[SS_WRAP]);
	//SetD3DName(samplerStates[SS_WRAP], std::string("Object Created At Line: " + to_string(__LINE__) + " File: " + __FILE__).c_str());


	// Create a sampler state
	D3D11_SAMPLER_DESC sampDesc;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sampDesc.MinLOD = 0.0f;
	sampDesc.MipLODBias = 0.0f;
	CRenderer::GetReference( ).GetDevice( )->CreateSamplerState(&sampDesc, &samplerStates[SS_ANISOTROPIC]);

}
