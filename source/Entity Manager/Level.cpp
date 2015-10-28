
#include "Level.h"
#include "../Core/stdafx.h"
#include "../Renderer/Magic.h"
#include "../Core/StructureHolder.h"
#include "../Renderer/Scene.h"
#include "../Asset Manager/AssetManager.h"

#include "Scenery\RotaryLights.h"

#define FLICKER_OFF_THRESHOLD 0.3f
#define FLICKER_ON_THRESHOLD 0.7f

CLevel::CLevel( )
{
	Reset( );

	//m_vRotaryLights.push_back(new CRotaryLights({ 0, 180, 0 }, false));
	//m_vRotaryLights.push_back(new CRotaryLights({ 0, 180, 0 }, true));
}

CLevel::~CLevel( )
{
	Clear( );
}

//Frees everything we're holding onto
void CLevel::Clear(void)
{
	ClearLights( );
	ClearRotaryLights();
}

void CLevel::Reset(void)
{
	m_xmLightShade = { 1.0f, 1.0f, 1.0f };
	m_bLightSwitch = true;
}

//Deletes the lights
void CLevel::ClearLights(void)
{
	for (size_t i = 0; i < m_vLights.size( ); i++)
	{
		delete m_vLights[i].first;
		m_vLights[i].first = nullptr;
	}
	m_vLights.clear( );
}

void CLevel::ClearRotaryLights(void)
{
	for (size_t i = 0; i < m_vRotaryLights.size(); i++)
	{
		SafeDelete(m_vRotaryLights[i]);
	}
	m_vRotaryLights.clear();
}

void CLevel::AddLight(LightParams* pLight)
{
	if (pLight)
	{
		std::pair<LightParams*, DirectX::XMFLOAT3> temp;
		temp.first = pLight;
		temp.second = pLight->Color;
		m_vLights.push_back(temp);
	}
}

void CLevel::AddRotaryLight(CRotaryLights* pLight)
{
	if (pLight)
	{
		m_vRotaryLights.push_back(pLight);
	}
}

void CLevel::AddLightsToScene(CScene* pScene)
{
	if (pScene)
	{
		if (m_bLightSwitch)
		{
			for (size_t i = 0; i < m_vLights.size(); i++)
			{
				if (m_vLights[i].first)
				{
					pScene->AddLight(*m_vLights[i].first);
				}
			}
		}
		if (m_bEmergencyLightSwitch)
		{
			for (size_t i = 0; i < m_vRotaryLights.size(); i++)
			{
				if (m_vRotaryLights[i])
				{
					m_vRotaryLights[i]->AddLight(pScene);
				}
			}
		}
	}
}

const LightParams* CLevel::GetLight(unsigned int nLightIndex) const
{
	if (nLightIndex >= 0 && nLightIndex < m_vLights.size( ))
	{
		return m_vLights[nLightIndex].first;
	}

	return nullptr;
}

const CRotaryLights* CLevel::GetRotaryLight(unsigned int nLightIndex) const
{
	if (nLightIndex >= 0 && nLightIndex < m_vRotaryLights.size())
	{
		return m_vRotaryLights[nLightIndex];
	}

	return nullptr;
}


void CLevel::SetLightShade(DirectX::XMFLOAT3 xmColor)
{
	//Set the current shade
	m_xmLightShade = xmColor;
	//Adjust all the lights
	for (size_t i = 0; i < m_vLights.size( ); i++)
	{
		if (m_vLights[i].first)
		{
			m_vLights[i].first->Color.x = m_vLights[i].second.x * xmColor.x;
			m_vLights[i].first->Color.y = m_vLights[i].second.y * xmColor.y;
			m_vLights[i].first->Color.z = m_vLights[i].second.z * xmColor.z;
		}
	}
}

void CLevel::SetLightShade(float r, float g, float b)
{
	SetLightShade(DirectX::XMFLOAT3(r, g, b));
}

void CLevel::SetLightShade(float RGandB)
{
	SetLightShade(DirectX::XMFLOAT3(RGandB, RGandB, RGandB));
}


void CLevel::Flicker(float fDuration)
{
	m_fTotalFlickerTime = fDuration;
}

void CLevel::Update(float fDt)
{
	//Update the flicker timer
	if (m_fTotalFlickerTime)
	{
		m_fFlickerTimer += fDt;

		//What's our percent progress
		float fFlickerProgress = m_fFlickerTimer / m_fTotalFlickerTime;
		float fShade = 1;
		if (fFlickerProgress >= 1)
		{
			m_fFlickerTimer = m_fTotalFlickerTime = 0;
			fFlickerProgress = 1;
		}
		else if (fFlickerProgress < FLICKER_OFF_THRESHOLD)
		{
			fShade = fFlickerProgress / FLICKER_OFF_THRESHOLD;
		}
		else if (fFlickerProgress < FLICKER_ON_THRESHOLD)
		{
			fShade = 0.01f;
		}
		else
		{
			fShade = (fFlickerProgress - FLICKER_ON_THRESHOLD) / FLICKER_OFF_THRESHOLD;
		}
		SetLightShade({ fShade, fShade, fShade });
	}

	//Update the rotary Lights
	if (m_bEmergencyLightSwitch)
	{
		for (auto & i : m_vRotaryLights)
		{
			i->Update(fDt);
		}
	}
}

void CLevel::UnloadLevelGeometry(CAssetManager* pAssets)
{
	if (pAssets)
	{
		for (size_t i = 0; i < m_vLevelTags.size( ); i++)
		{
			pAssets->UnloadModel(m_vLevelTags[i]);
		}

		//We don't need these anymore
		m_vLevelTags.clear( );
	}
}