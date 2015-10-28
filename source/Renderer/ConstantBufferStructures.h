/***********************************************
* Filename// :  		ConstantBufferStructures.cpp
* Date// :      		05/12/2015
* Mod. Date// : 		05/28/2015
* Mod. Initials// :	DG
* Author// :    		David Goad
* Purpose// :   		Container for shader structs
************************************************/

#ifndef _CONSTANT_BUFFER_STRUCTURES_H_
#define _CONSTANT_BUFFER_STRUCTURES_H_

#include "..\Core\stdafx.h"

//structures base on FSGDGameEngine

const static int NUM_BONES = 100;
const static int NUM_INSTANCE = 10;

struct TPerObject // : register(b0)
{
	// The world matrix of the object to be rendered.
	DirectX::XMFLOAT4X4	gM;

	// the world view matrix of the object to be rendered
	DirectX::XMFLOAT4X4	gMV;

	// The concatenated world view projection matrices
	DirectX::XMFLOAT4X4	gMVP;

	static const int register_slot = 0;
};

struct TPerSkinnedObject // : register(b0)
{
	// The world matrix of the object to be rendered.
	DirectX::XMFLOAT4X4	gSkinnedM;

	// the world view matrix of the object to be rendered
	DirectX::XMFLOAT4X4	gSkinnedMV;

	// The concatenated world view projection matrices
	DirectX::XMFLOAT4X4	gSkinnedMVP;

	// The bone matrices of the object to be animated.
	DirectX::XMFLOAT4X4	gSkinMatrices[NUM_BONES];

	float scaling;

	DirectX::XMFLOAT3 fPad;

	static const int register_slot = 0;
};

struct TInstanced // : register(b1)
{
	DirectX::XMFLOAT4X4	gWorldInstanced[NUM_INSTANCE];
	static const int register_slot = 1;
};

struct TPerCamera // : register(b2)
{
	/// the view matrix
	DirectX::XMFLOAT4X4	gView;
	/// the projection matrix
	DirectX::XMFLOAT4X4	gProj;

	/// The current cameras current inverse projection matrix
	DirectX::XMFLOAT4X4	gInvProj;

	DirectX::XMFLOAT2A gClipPlanes;

	///// The current cameras current view projection concatenated matrix
	//DirectX::XMFLOAT4X4	gViewProj;

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

	static const int register_slot = 2;
};

struct TColor // : register(b0)
{
	DirectX::XMFLOAT4 colorMod;

	static const int register_slot = 0;
};

struct TPerParticle
{
	DirectX::XMFLOAT4X4 mWorld;
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 gProj;
	DirectX::XMFLOAT4X4 gInvView;
	DirectX::XMFLOAT4 g_size;

	static const int register_slot = 1;
};

//struct TTexCoords // : register(b1)
//{
//	DirectX::XMFLOAT4 UVs[4];
//
//	static const int register_slot = 1;
//}

struct TTime // : register(b10)
{
	DirectX::XMFLOAT2A Translate;

	static const int register_slot = 10;
};

#endif //_CONSTANT_BUFFER_STRUCTURES_H_
