

#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <vector>
#include <DirectXMath.h>

struct LightParams;
class CScene;
class CAssetManager;
class CRotaryLights;

class CLevel
{
public:
	CLevel();
	~CLevel();

	//---------EmptyThis---------------
	//Frees everything we're holding onto
	void Clear(void);
	//Clears out our lights
	void ClearLights(void);
	void ClearRotaryLights(void);

	//---------FillThis-----------------
	void AddLight(LightParams* pLight);
	void AddRotaryLight(CRotaryLights* pLight);

	//-----DoSomethingWithThis----------
	void AddLightsToScene(CScene* pScene);
	void Reset(void);
	void Flicker(float fDuration = 0.35f);
	void Update(float dDt);

	void UnloadLevelGeometry(CAssetManager* pAssets);
	void AddTag(std::string szTag) { m_vLevelTags.push_back(szTag); }

	//-----------ACCESSORS----------------
	size_t GetLightCount(void) const { return m_vLights.size(); }
	const LightParams* GetLight(unsigned int nLightIndex) const;
	DirectX::XMFLOAT3 GetLightShade(void) const { return m_xmLightShade; }
	size_t GetRotaryLightCount(void) const { return m_vRotaryLights.size(); }
	const CRotaryLights* GetRotaryLight(unsigned int nLightIndex) const;

	//----------MUTATORS------------------
	void SetLightSwitch(bool bLightSwitch) { m_bLightSwitch = bLightSwitch; }
	void SetLightShade(DirectX::XMFLOAT3 xmColor);
	void SetLightShade(float r, float g, float b);
	void SetLightShade(float RGandB);
	void SetEmergencyLightSwitch(bool bLightSwitch) { m_bEmergencyLightSwitch = bLightSwitch; }

private:
	//-----LIGHTS-------
	//Are the lights on?
	bool m_bLightSwitch = true;
	//Are the rotary lights on
	bool m_bEmergencyLightSwitch = false;
	//What's our progress in the flicker
	float m_fFlickerTimer = 0.0f;
	//How long is this flicker supposed to last
	float m_fTotalFlickerTime = 0.0f;
	//What shade/How bright should the lights be (should probably be 0.0f to 1.0f)
	DirectX::XMFLOAT3 m_xmLightShade;
	//Holds all the fixed lights in the level
	//The light param is how it currently exists
	//The float is it's default color
	std::vector<std::pair<LightParams*, DirectX::XMFLOAT3>> m_vLights;
	//All the tags related to the level
	//Use this to unload the level
	std::vector<std::string> m_vLevelTags;

	std::vector<CRotaryLights*> m_vRotaryLights;
};



#endif