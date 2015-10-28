#pragma once

#include "../../Transform.h"
#include "../../Core/StructureHolder.h"

class CScene;
struct LightParams;

class CRotaryLights
{
public:

	//	_XMPositon is the positon of the light
	//	_bRotateX = true will make the lights rotate on the X axis
	//	_bRotateX =false will make the lights rotate on the Z axis
	CRotaryLights(DirectX::XMFLOAT3 _XMPositon, bool _bRotateX);
	virtual ~CRotaryLights( );

	void Update(float _fDT);
	void AddLight(CScene* _pScene);

	void SetSpinSpeed(float fSpeed) { m_fSpinSpeed = fSpeed; }

	//-----Accessors------
	bool GetRotateX(void) const { return m_bRotateX; }
	DirectX::XMFLOAT3 GetPosition(void) const { return {t.Local._41, t.Local._42, t.Local._43}; }

private:

	LightParams* m_pSpot_Up;
	LightParams* m_pSpot_Down;

	Transform t;

	float m_fSpinSpeed = 100.0f;
	bool m_bRotateX;
};

