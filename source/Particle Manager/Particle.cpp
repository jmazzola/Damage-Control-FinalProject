/***********************************************
* Filename:			Particle.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#include "Particle.h"
#include "../Renderer/Scene.h"
using namespace DirectX;

/*****************************************************************
* CParticle():
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
CParticle::CParticle()
{
}

/*****************************************************************
* ~CParticle():
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
CParticle::~CParticle()
{
}

/*****************************************************************
* Update():
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/18/2015
* Mod. Initials:
*****************************************************************/
void CParticle::Update(float dt)
{
	if (m_fLifeTime < 0.f)
		return;

	m_vVelocity = m_vVelocity + (DirectX::XMFLOAT3)(m_vAcceleration) * dt;
	m_vPosition = m_vPosition + m_vVelocity * dt;
	m_fRotation += m_fRotationSpeed * dt;
	m_fLifeTime -= dt;

	m_blendColor.x = Lerp(m_startColor.x, m_endColor.x, 1.f - (m_fLifeTime / m_fMaxLifeTime));
	m_blendColor.y = Lerp(m_startColor.y, m_endColor.y, 1.f - (m_fLifeTime / m_fMaxLifeTime));
	m_blendColor.z = Lerp(m_startColor.z, m_endColor.z, 1.f - (m_fLifeTime / m_fMaxLifeTime));
	m_blendColor.w = Lerp(m_startColor.w, m_endColor.w, 1.f - (m_fLifeTime / m_fMaxLifeTime));
}

/*****************************************************************
* Update():
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/19/2015
* Mod. Initials:
*****************************************************************/
float CParticle::Lerp(float min, float max, float weight)
{
	float range = max - min;
	return min + range * weight;
}

bool CParticle::operator<(CParticle& other)
{
	DirectX::XMFLOAT3 xmCamPoint = CScene::GetCamera()->Position();
	DirectX::XMFLOAT3 camPos = { xmCamPoint.x, xmCamPoint.y, xmCamPoint.z };

	DirectX::XMFLOAT3 toA = camPos - m_vPosition;
	float distanceA;
	DirectX::XMStoreFloat(&distanceA, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&toA)));

	DirectX::XMFLOAT3 toB = camPos - other.m_vPosition;
	float distanceB;
	DirectX::XMStoreFloat(&distanceB, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&toB)));

	return distanceA < distanceB;
}