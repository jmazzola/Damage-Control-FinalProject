#ifndef VERTEX_LAYOUTS_HLSLI
#define VERTEX_LAYOUTS_HLSLI

struct Vertex_Light
{
	float3 Position: POSITION;
	float Range : RANGE;
	float3 Color: COLOR;
	float3 Direction: DIRECTION;
	float2 SpotInnerOuterAngle: SPOTANGLES;
};

struct Vertex_Position
{
	float3 position : POSITION0;
};

struct Vertex_PositionTextureColor
{
	float3 position : POSITION0;
	float2 texCoord : TEXCOORD0;
	float4 color : COLOR0;
};

struct Vertex_PositionTexture
{
	float3 position : POSITION0;
	float2 texCoord : TEXCOORD0;
};

struct Vertex_PositionNormalTexture
{
	float3 position : POSITION0;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

struct Vertex_PositionNormalTextureBoneWeights
{
	float3 position : POSITION0;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL0;
	int4 bone : BONE0;
	float4 weights : WEIGHTS0;
};

struct Vertex_PositionNormalTextureTangentBiNormal
{
	float3 position : POSITION0;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 tangent : TANGENT0;
	float3 biNormal : BITANGENT;
};

struct Vertex_PositionNormalTextureTangentBiNormalSkinned
{
	float3 position : POSITION0;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 tangent : TANGENT0;
	float3 biNormal : BITANGENT;
	int cp : CONTROLPOINT0;
	int4 bone : BONE0;
	float4 weights : WEIGHTS0;
};

//OUTPUT VERTS
struct Output_PositionTexture
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};
struct Output_PositionTextureColor
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 color : COLOR0;
};
struct Output_SkyboxPositionTexture3
{
	float4 position : SV_POSITION;
	float3 texCoord : TEXCOORD0;
};
struct Output_PositionNormalTexture
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL0;
	float2 texCoord : TEXCOORD0;
};
struct Output_PositionNormalTextureTangentBiNormal
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL0;
	float2 texCoord : TEXCOORD0;
	float3 tangent : TANGENT0;
	float3 biNormal : BINORMAL0;
};

#endif //VERTEX_LAYOUTS_HLSLI
