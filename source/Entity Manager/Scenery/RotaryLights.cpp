#include "stdafx.h"
#include "RotaryLights.h"

#include "../../Renderer/Scene.h"
#include "../../Core/StructureHolder.h"

#define DEFAULT_ROTATION_SPEED 100.0f
//Speed will be default + or - this
#define DEFAULT_ROTATION_RANGE 12

CRotaryLights::CRotaryLights(DirectX::XMFLOAT3 _XMPositon, bool _bRotateX)
{
	XMStoreFloat4x4(&t.Local, DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&_XMPositon)));

	m_pSpot_Down = new LightParams( );
	m_pSpot_Up = new LightParams( );

	m_pSpot_Up->Color = m_pSpot_Down->Color = { (204 / 255.0f) * 15.0f, (17 / 255.0f) * 15.0f, 0 };
	m_pSpot_Up->Direction = m_pSpot_Down->Direction = { 0, -1, 0 };
	m_pSpot_Up->Position = m_pSpot_Down->Position = { 0, 0, 0 };
	m_pSpot_Up->Range = m_pSpot_Down->Range = 1000;
	m_pSpot_Up->SpotOuterAngle = m_pSpot_Down->SpotOuterAngle = .92f;
	m_pSpot_Up->SpotInnerAngle = m_pSpot_Down->SpotInnerAngle = .95f;
	m_pSpot_Up->Type = m_pSpot_Down->Type = Spot;

	m_bRotateX = _bRotateX;

	//Give a slightly different spin speed to each one
	m_fSpinSpeed = DEFAULT_ROTATION_SPEED;
	m_fSpinSpeed += (rand() % DEFAULT_ROTATION_RANGE * 2) - DEFAULT_ROTATION_RANGE;
}


CRotaryLights::~CRotaryLights( )
{
	SafeDelete(m_pSpot_Up)
		SafeDelete(m_pSpot_Down)
}

void CRotaryLights::Update(float _fDT)
{
	t.Local = t.GetWorld( );
	m_pSpot_Up->Position = m_pSpot_Down->Position = XMMatrixPosition(t.Local);

	DirectX::XMFLOAT3 up = { t.Local.m[1][0], t.Local.m[1][1], t.Local.m[1][2] };

	m_pSpot_Up->Direction = up;
	m_pSpot_Down->Direction = up * -1.0f;

	if (m_bRotateX)
	{
		DirectX::XMStoreFloat4x4(&t.Local, DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(_fDT * m_fSpinSpeed)) * DirectX::XMLoadFloat4x4(&t.Local));
	}
	else
	{
		DirectX::XMStoreFloat4x4(&t.Local, DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(_fDT * m_fSpinSpeed)) * DirectX::XMLoadFloat4x4(&t.Local));
	}
}

void CRotaryLights::AddLight(CScene* _pScene)
{
	_pScene->AddLight(*m_pSpot_Up);
	_pScene->AddLight(*m_pSpot_Down);
}