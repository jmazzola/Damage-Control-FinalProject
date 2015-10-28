#include "../Core/stdafx.h"
#include "FirstPersonCamera.h"

#include "../Input Manager/InputManager.h"
#include "../Input Manager/eControls.h"
#include "../Input Manager/KeyBindings.h"
#include <string>

using namespace DirectX;

const XMFLOAT3 InitialPosition = XMFLOAT3(4.0f, 4.5f, -4.0f);
const XMFLOAT2 InitialAngle = XMFLOAT2(0.407f, -0.707f);
const float debounce = .1f;

CFirstPersonCamera::CFirstPersonCamera( ) :
m_XMAngle(InitialAngle)
{
	m_fNear = 1.0f;
	//m_fFar = 15.0f;
	m_fFOV = 90.0f;

	this->m_XMPosition = InitialPosition;

	CICamera::UpdateView( );
	CICamera::UpdateProjection(m_fFOV, m_fWidth / m_fHeight, m_fNear, m_fFar);
}

CFirstPersonCamera::~CFirstPersonCamera( )
{
}

void CFirstPersonCamera::UpdateView( )
{
	auto forward = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	auto _lookAt = XMLoadFloat3(&m_XMPosition) + XMVector3Transform(forward, XMLoadFloat4x4(&this->m_XMRotationMatrix));

	this->m_XMLookAt.x = XMVectorGetX(_lookAt);
	this->m_XMLookAt.y = XMVectorGetY(_lookAt);
	this->m_XMLookAt.z = XMVectorGetZ(_lookAt);

	CICamera::UpdateView( );
}

void CFirstPersonCamera::UpdateWorld(XMFLOAT3 MoveVector)
{
	auto vect = XMVector3Transform(XMLoadFloat3(&MoveVector), XMLoadFloat4x4(&this->m_XMRotationMatrix));

	this->m_XMPosition.x += XMVectorGetX(vect);
	this->m_XMPosition.y += XMVectorGetY(vect);
	this->m_XMPosition.z += XMVectorGetZ(vect);
}

void CFirstPersonCamera::Reset( )
{
	this->m_XMPosition = InitialPosition;
	this->m_XMAngle = InitialAngle;
	UpdateView( );
}

void CFirstPersonCamera::Update(DirectX::XMFLOAT3 _XMBindPos, float _fDT)
{
	XMStoreFloat4x4(&this->m_XMRotationMatrix,
					(XMMatrixRotationX(this->m_XMAngle.x) * XMMatrixRotationY(this->m_XMAngle.y)));
	ProcessInput(_fDT);
	UpdateView( );

//#ifdef _DEBUG
//	std::string strPos("X: " + std::to_string(m_XMPosition.x));
//	strPos.append(" Y: " + std::to_string(m_XMPosition.y));
//	strPos.append(" Z: " + std::to_string(m_XMPosition.z));
//	strPos.append(" aX: " + std::to_string(m_XMAngle.x));
//	strPos.append(" aY: " + std::to_string(m_XMAngle.y));
//	strPos.append("\n");
//	DebugPrint(strPos.c_str());
//#endif

}

void CFirstPersonCamera::ProcessInput(float _fDT)
{
	CInputManager* input = CInputManager::GetInstance( );

	bool ForceUpdate = false;
	this->m_XMLastPointerPos = this->m_XMCurrentPointerPos;
	this->m_XMCurrentPointerPos = XMFLOAT2(input->GetMousePosition( ).x, input->GetMousePosition( ).y);

	auto moveVect = XMFLOAT3(0, 0, 0);

//#pragma region Change camera rotation
//
//	if (input->IsButtonDown(eControls::MouseRight))
//	{
//		if (m_XMCurrentPointerPos.y - m_XMLastPointerPos.y > debounce)
//		{
//			this->m_XMAngle.x += _fDT * CameraRotationSpeed;
//		}
//		else if (m_XMCurrentPointerPos.y - m_XMLastPointerPos.y < -debounce)
//		{
//			this->m_XMAngle.x -= _fDT * CameraRotationSpeed;
//		}
//
//		if (m_XMCurrentPointerPos.x - m_XMLastPointerPos.x > debounce)
//		{
//			this->m_XMAngle.y += _fDT * CameraRotationSpeed;
//		}
//		else if (m_XMCurrentPointerPos.x - m_XMLastPointerPos.x < -debounce)
//		{
//			this->m_XMAngle.y -= _fDT * CameraRotationSpeed;
//		}
//
//		BindAngles( );
//		ForceUpdate = true;
//	}
//
//#pragma endregion
//
//#pragma region Change camera movements
//
//	if (input->IsButtonDown(eControls::D))
//	{
//		moveVect.x += _fDT * CameraMovementSpeed;
//		ForceUpdate = true;
//	}
//	else if (input->IsButtonDown(eControls::A))
//	{
//		moveVect.x -= _fDT * CameraMovementSpeed;
//		ForceUpdate = true;
//	}
//
//	if (input->IsButtonDown(eControls::W))
//	{
//		moveVect.z += _fDT * CameraMovementSpeed;
//		ForceUpdate = true;
//	}
//	else if (input->IsButtonDown(eControls::S))
//	{
//		moveVect.z -= _fDT * CameraMovementSpeed;
//		ForceUpdate = true;
//	}
//
//	if (input->IsButtonDown(eControls::Q))
//	{
//		moveVect.y += _fDT * CameraMovementSpeed;
//		ForceUpdate = true;
//	}
//	else if (input->IsButtonDown(eControls::Z))
//	{
//		moveVect.y -= _fDT * CameraMovementSpeed;
//		ForceUpdate = true;
//	}
//
//#pragma endregion

	if (ForceUpdate)
	{
		UpdateWorld(moveVect);
	}
}

void CFirstPersonCamera::TurnTo(DirectX::XMFLOAT3 dest)
{
	DirectX::XMFLOAT3 destVec = (dest - m_XMPosition);
	XMStoreFloat4x4(&m_XMView, DirectX::XMMatrixLookToLH(XMLoadFloat3(&m_XMPosition), XMLoadFloat3(&destVec), g_XMIdentityR1));
}

void CFirstPersonCamera::FlyTo(DirectX::XMFLOAT3 _XMDestination, float _fDT)
{
	// -x rotates up
	//	consequently, x rotates down.
	// -y rotates counter-clockwise
	//	you get the idea

	DirectX::XMFLOAT3 to_destination = _XMDestination - m_XMPosition;

	//dot = x1*x2 + y1*y2 + z1*z2
	//lenSq1 = x1*x1 + y1*y1 + z1*z1
	//lenSq2 = x2*x2 + y2*y2 + z2*z2
	//angle = acos(dot / sqrt(lenSq1 * lenSq2))

	auto dot = DirectX::XMVector3Dot(XMLoadFloat3(&to_destination), XMLoadFloat3(&m_XMLookAt));
	auto lengthA = DirectX::XMVector3Length(XMLoadFloat3(&to_destination));
	auto lengthB = DirectX::XMVector3Length(XMLoadFloat3(&m_XMLookAt));
	auto theta = std::acos(DirectX::XMVectorGetX(DirectX::XMVectorDivide(dot, DirectX::XMVectorMultiply(lengthA, lengthB))));

	dot = DirectX::XMVector3Dot(XMLoadFloat3(&to_destination), XMLoadFloat3(&XMFLOAT3(1, 0, 0)));
	theta = DirectX::XMVectorGetX(dot) > 0 ? theta : -theta;

	float degrees = DirectX::XMConvertToDegrees(theta);

	if (degrees < 180 && degrees >= .5f)
	{
		m_XMAngle.y += _fDT;
	}
	else if (degrees < 0 && degrees > -180)
	{
		m_XMAngle.y -= _fDT;
	}

	XMFLOAT3 move_vector = { 0, 0, 0 };
	UpdateWorld(move_vector);

	BindAngles( );
}

void CFirstPersonCamera::BindAngles( )
{

	if (this->m_XMAngle.x > 1.4)
	{
		this->m_XMAngle.x = 1.4f;
	}
	else if (this->m_XMAngle.x < -1.4)
	{
		this->m_XMAngle.x = -1.4f;
	}

	if (this->m_XMAngle.y > XM_PI)
	{
		this->m_XMAngle.y -= 2 * XM_PI;
	}
	else if (this->m_XMAngle.y < -XM_PI)
	{
		this->m_XMAngle.y += 2 * XM_PI;
	}
}
