/***********************************************
* Filename:			StructHolder.h
* Date:      		05/12/2015
* Mod. Date:		05/13/2015
* Mod. Initials:	BL
* Author:    		Jagoba Marcos
* Purpose:			Header to store all simple structures (without pointers)
*					and have an easy/clean way to access them.
************************************************/
#ifndef STRUCT_HOLDER_H
#define STRUCT_HOLDER_H

/**************************************
Shapes Region:	Here is where the shapes for Colliders are stored.
Used for entites and collision checks.
The base collider is called "tCollider".

Mod. Date:		05/13/2015
Mod. Initials:	BL
**************************************/
#pragma region Shapes
struct TCollider
{
	enum ColliderType
	{
		ePlane, eAABB, eSphere, eCapsule, eFrustum
	};
	int type;
};

struct TPlane : public TCollider
{
	DirectX::XMFLOAT3 vNormal;
	float fOffset;
};

struct TAABB : TCollider
{
	DirectX::XMFLOAT3 vMin;
	DirectX::XMFLOAT3 vMax;
};

struct TSegment
{
	DirectX::XMFLOAT3 m_Start;
	DirectX::XMFLOAT3 m_End;
};

struct TSphere : TCollider
{
	DirectX::XMFLOAT3 vCenter;
	float fRadius;
};

struct TCapsule : TCollider
{
	TSegment tSegment;
	float fRadius;
};

struct TFrustum : TCollider
{
	TPlane tPlanes[6];
	DirectX::XMFLOAT3 vCorners[8];
};
#pragma endregion

using namespace MathHelper;

// Light type
enum LightType
{
	Point = 0,
	Spot = 1,
	Directional = 2,

	NumLightTypes
};

// Light parameters
struct LightParams
{
	LightParams( ) : Position(0.0f, 0.0f, 0.0f), Color(1.0f, 1.0f, 1.0f), Direction(-1.0f, -1.0f, 1.0f),
		Range(2.0f), SpotInnerAngle(0), SpotOuterAngle(0), Type(Point)
	{
	}

	LightParams(DirectX::XMFLOAT3 _XMPosition, DirectX::XMFLOAT3 _XMColor, float _fRange)
		: Position(_XMPosition), Color(_XMColor), Range(_fRange), Type(Point)
	{
	}

	//outer ratio needs to fall between 
	//		[.70 -> .99]
	//inner ratio need to fall between
	//		[.71 -> 1.0]
	//
	//each inner/outer is a ratio between 0.0 & 1.0
	//	the inner ratio MUST be greater than the outer
	LightParams(DirectX::XMFLOAT3 _XMPosition, DirectX::XMFLOAT3 _XMColor, DirectX::XMFLOAT3 _XMDirection, float _fRange, float _fSpotInner, float _fSpotOuter)
		: Position(_XMPosition), Color(_XMColor), Range(_fRange), SpotInnerAngle(_fSpotInner), SpotOuterAngle(_fSpotOuter), Type(Spot)
	{
	}

	LightParams(DirectX::XMFLOAT3 _XMColor, DirectX::XMFLOAT3 _XMDirection)
		: Color(_XMColor), Direction(_XMDirection), Type(Directional)
	{
	}

	DirectX::XMFLOAT3 Position;
	float Range;
	DirectX::XMFLOAT3 Color;
	DirectX::XMFLOAT3 Direction;
	float SpotInnerAngle;
	float SpotOuterAngle;
	LightType Type;
};

struct TWeaponSpecials//union TModSpecials
{
	//struct
	//{
	signed char m_nBounceLevel;
	signed char m_nChainLevel;
	signed char m_nPierceLevel;
	signed char m_nExplodeLevel;
	signed char m_nFreezeLevel;
	signed char m_nPoisonLevel;
	signed char m_nSlowLevel;
	signed char m_nSecretLevel;
	//} m_tWeaponSpecials;
};

inline TWeaponSpecials operator + (TWeaponSpecials& tA, TWeaponSpecials& tB)
{
	TWeaponSpecials tC;
	tC.m_nBounceLevel = tA.m_nBounceLevel + tB.m_nBounceLevel;
	tC.m_nChainLevel = tA.m_nChainLevel + tB.m_nChainLevel;
	tC.m_nPierceLevel = tA.m_nPierceLevel + tB.m_nPierceLevel;
	tC.m_nExplodeLevel = tA.m_nExplodeLevel + tB.m_nExplodeLevel;
	tC.m_nBounceLevel = tA.m_nBounceLevel + tB.m_nBounceLevel;
	tC.m_nFreezeLevel = tA.m_nFreezeLevel + tB.m_nFreezeLevel;
	tC.m_nPoisonLevel = tA.m_nPoisonLevel + tB.m_nPoisonLevel;
	tC.m_nSlowLevel = tA.m_nSlowLevel + tB.m_nSlowLevel;
	tC.m_nSecretLevel = tA.m_nSecretLevel + tB.m_nSecretLevel;
	return tC;
}

struct TMsScores
{
	char name[4];
	int score;
};


#endif