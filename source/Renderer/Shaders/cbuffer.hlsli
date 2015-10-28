#ifndef _CBUFFER_HLSLI_
#define _CBUFFER_HLSLI_

#include "../../Particle Manager/Particle.h"

const static int NUM_BONES = 100;
const static int NUM_INSTANCE = 10;

struct TPerSkinnedObject
{
	// The world matrix of the object to be rendered.
	float4x4	gSkinnedM;

	// the world view matrix of the object to be rendered
	float4x4	gSkinnedMV;

	// The concatenated world view projection matrices
	float4x4	gSkinnedMVP;

	// The bone matrices of the object to be animated.
	float4x4	gSkinMatrices[NUM_BONES];

};

cbuffer TPerObject : register(b0)
{
	// The world matrix of the object to be rendered.
	float4x4	gM;

	// the world view matrix of the object to be rendered
	float4x4	gMV;

	// The concatenated world view projection matrices
	float4x4	gMVP;

	//static const int register_slot = 0;
};

cbuffer TPerSkinnedObject : register(b0)
{
	TPerSkinnedObject perSkinned;
	//static const int register_slot = 0;
};

cbuffer TAnimationInstanced : register(b1)
{
	//TPerSkinnedObject instances[NUM_INSTANCE];
	//static const int register_slot = 0;
};

cbuffer TPerCamera : register(b2)
{
	/// the view matrix
	float4x4	gView;

	/// the projection matrix
	float4x4	gProj;

	/// The current cameras current inverse view-projection matrix
	float4x4	gInvProj;

	float2 gClipPlanes;

	///// The current cameras current view projection concatenated matrix
	//float4x4	gViewProj;

	// The position of the camera, or eye.
	//DirectX::XMFLOAT3 gCameraPos;
	// The distance of the far clip plane
	//float gFarDist;
	// The size of the main render target in pixels
	//DirectX::XMFLOAT2 gScreenSize;
	// Precomputed value used to convert depth to actual distance.
	// gPlaneX = -far / (far - near)
	// distance = gPlaneY / (gPlaneX + depth)
	//float gPlaneX;
	// Precomputed value used to convert depth to actual distance.
	// gPlaneY = -far * near/ (far - near)
	// distance = gPlaneY / (gPlaneX + depth)
	//float gPlaneY;
	//DirectX::XMFLOAT3 gCameraDir;

	//static const int register_slot = 2;
};

cbuffer TColor : register(b0)
{
	float4 colorMod;

	//static const int register_slot = 0;
};

cbuffer TPerParticle : register(b1)
{
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
	float4x4 gInvView;
	float4 g_size;
}

//cbuffer TTexCoords : register(b1)
//{
//	float4 UVs[4];
//
//	//static const int register_slot = 2;
//}

cbuffer TTime : register(b10)
{
	float2 Translate;

	//static const int register_slot = 10;
};

#endif //_CBUFFER_HLSLI_
