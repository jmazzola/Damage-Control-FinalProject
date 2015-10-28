/***********************************************
* Filename:			Particle.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#ifndef _CPARTICLE_H_
#define _CPARTICLE_H_

#if __cplusplus
#include "..\Core\stdafx.h"

typedef DirectX::XMFLOAT2 float2;
typedef DirectX::XMFLOAT3 float3;
typedef DirectX::XMFLOAT4 float4;
#define struct class
#define PACK _declspec(align(4))

#define POSITION_SEMANTIC
#define TEXCOORD_SEMANTIC
#define COLOR_SEMANTIC
#define INDEX_SEMANTIC
#else
#define PACK 

#define POSITION_SEMANTIC : SV_Position 
#define TEXCOORD_SEMANTIC : TEXCOORD 
#define COLOR_SEMANTIC : COLOR
#define INDEX_SEMANTIC : INDEX
#endif

#define MAX_PARTICLES 100

struct PACK CParticle
{
	float Lerp(float min, float max, float weight);
#if __cplusplus
public:
#endif
	float3 m_vPosition;
	float m_fSpeed;

	float m_fRotation;
	float m_fRotationSpeed;


	float3 m_vVelocity;
	float m_fSize;
	

	float3 m_vAcceleration;
	float m_fLifeTime;
	float m_fMaxLifeTime;

	float4 m_blendColor;
	float4 m_startColor;
	float4 m_endColor;
	
#if __cplusplus
	CParticle();
	~CParticle();

	void Update(float dt);

	bool operator<(CParticle& other);
#endif
};
#undef struct

struct TParticleOutput
{
	float4 position POSITION_SEMANTIC;
	float2 uv TEXCOORD_SEMANTIC;
	float4 color COLOR_SEMANTIC;
	int index INDEX_SEMANTIC;
};

#endif // _CPARTICLE_H_
