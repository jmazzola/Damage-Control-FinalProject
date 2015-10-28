/***********************************************
* Filename:  		ThirdPersonCamera.cpp
* Date:				05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		Chase Camera class that follows
*						the player
************************************************/

#include "ThirdPersonCamera.h"

using namespace DirectX;
using namespace MathHelper;

/// <summary>
/// Physics coefficient which controls the influence of the camera's m_XMPosition
/// over the spring force. The stiffer the spring, the closer it will stay to
/// the chased object.
/// </summary>

/// <summary>
/// Physics coefficient which approximates internal friction of the spring.
/// Sufficient m_fDamping will prevent the spring from oscillating infinitely.
/// </summary>

/// <summary>
/// Mass of the camera body. Heaver objects require stiffer springs with less
/// m_fDamping to move at the same rate as lighter objects.
/// </summary>

//3,000 looks okay
//i like 5,000
//the snep-beck effect gives it that little extra something.
CThirdPersonCamera::CThirdPersonCamera( ) : m_fStiffness(4000.0f), m_fDamping(600.0f), m_fMass(50.0f), m_XMLookAtOffset(DirectX::XMFLOAT3(0, 90, 0)), m_XMVelocity(DirectX::XMFLOAT3(10, 10, 10))
{
	this->m_XMDesiredPositionOffset = DirectX::XMFLOAT3(LEFTRIGHT, UPDOWN, ZINDEX);
	this->m_XMPosition = this->m_XMDesiredPositionOffset;//if y'all want some spring, do = this->m_XMLookAtOffset;

	UpdateView( );

	DirectX::XMFLOAT4X4 VP;
	DirectX::XMStoreFloat4x4(&VP, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&m_XMView), DirectX::XMLoadFloat4x4(&m_XMProjection)));
	ComputeFrustumPlanes(VP);
}

void CThirdPersonCamera::Update(DirectX::XMFLOAT3 _XMBindPos,
								float _fDT)
{

	/*if (GetAsyncKeyState(VK_LCONTROL))
	{
		if (GetAsyncKeyState(VK_UP))
		{
			m_XMDesiredPositionOffset.y++;
		}
		if (GetAsyncKeyState(VK_DOWN))
		{
			m_XMDesiredPositionOffset.y--;
		}
		if (GetAsyncKeyState(VK_LEFT))
		{
			m_XMDesiredPositionOffset.z++;
		}
		if (GetAsyncKeyState(VK_RIGHT))
		{
			m_XMDesiredPositionOffset.z--;
		}
	}*/

	//DebugPrint(std::string(
	//	std::to_string(m_XMDesiredPositionOffset.x) + "|" +
	//	std::to_string(m_XMDesiredPositionOffset.y) + "|" +
	//	std::to_string(m_XMDesiredPositionOffset.z) + "\n").c_str( ));

	DirectX::XMFLOAT3 stretch;
	DirectX::XMFLOAT3 force;
	DirectX::XMFLOAT3 acceleration;

	const float Step = _fDT;// .016f;

	DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity( );

	DirectX::XMStoreFloat3(&this->m_XMDesiredPosition, DirectX::XMLoadFloat3(&_XMBindPos) +
						   DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_XMDesiredPositionOffset), identity));

	//DirectX::XMStoreFloat3(&stretch, DirectX::XMLoadFloat3(&this->m_XMPosition) - DirectX::XMLoadFloat3(&this->m_XMDesiredPosition));
	//DirectX::XMStoreFloat3(&force, (-this->m_fStiffness * DirectX::XMLoadFloat3(&stretch)) - (this->m_fDamping * DirectX::XMLoadFloat3(&this->m_XMVelocity)));
	//DirectX::XMStoreFloat3(&acceleration, DirectX::XMLoadFloat3(&force) / this->m_fMass);
	//
	//DirectX::XMStoreFloat3(&this->m_XMVelocity, DirectX::XMLoadFloat3(&this->m_XMVelocity) + DirectX::XMLoadFloat3(&acceleration) * Step);
	//DirectX::XMStoreFloat3(&this->m_XMPosition, DirectX::XMLoadFloat3(&this->m_XMPosition) + DirectX::XMLoadFloat3(&this->m_XMVelocity) * Step);

	m_XMPosition = _XMBindPos + m_XMDesiredPositionOffset;

	DirectX::XMStoreFloat3(&this->m_XMLookAt, DirectX::XMLoadFloat3(&_XMBindPos) + DirectX::XMLoadFloat3(&m_XMLookAtOffset));

	CICamera::UpdateView( );
}

void CThirdPersonCamera::Reset( )
{
	this->m_XMDesiredPositionOffset = DirectX::XMFLOAT3(LEFTRIGHT, UPDOWN, ZINDEX);
	CICamera::UpdateView( );
}
