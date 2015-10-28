#pragma once

#include "ICamera.h"

#define CameraMovementSpeed 500
#define CameraRotationSpeed 5

class CFirstPersonCamera : public CICamera
{
public:
	CFirstPersonCamera( );
	~CFirstPersonCamera( );

	void Update(DirectX::XMFLOAT3 _XMBindPos, float _fDT) override;

	void UpdateView( );
	void UpdateWorld(DirectX::XMFLOAT3 MoveVector);
	void Reset( );
	void ProcessInput(float time);

	void TurnTo(DirectX::XMFLOAT3 dest);
	void FlyTo(DirectX::XMFLOAT3 _XMDestination, float _fDT);

	DirectX::XMFLOAT2 GetAngle( )
	{
		return this->m_XMAngle;
	}

	void SetPosition(DirectX::XMFLOAT3 xmPosition)
	{
		m_XMPosition = xmPosition;
	}

	void SetAngle(DirectX::XMFLOAT2 val)
	{
		this->m_XMAngle = val;
	}

private:

	void BindAngles( );

	DirectX::XMFLOAT2 m_XMAngle;
	DirectX::XMFLOAT4X4 m_XMRotationMatrix;
	DirectX::XMFLOAT2 m_XMLastPointerPos;
	DirectX::XMFLOAT2 m_XMCurrentPointerPos;

};

