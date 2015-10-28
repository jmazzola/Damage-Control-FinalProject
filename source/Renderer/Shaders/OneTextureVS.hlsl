#pragma pack_matrix(row_major)

#include "VertexLayouts.hlsli"
#include "cbuffer.hlsli"

//cbuffer camera : register(b0)
//{
//	/// The current cameras current view projection concatenated matrix
//	matrix	gViewProj;
//	/// The current cameras current inverse view-projection matrix
//	matrix	gInvViewProj;
//};
//
//cbuffer object : register(b1)
//{
//	// The concatenated world view projection matrices
//	matrix	gMVP;
//	// The world matrix of the object to be rendered.
//	matrix	gWorld;
//};

//struct V_OUT
//{
//	float4 position : SV_POSITION;
//	float2 texCoord : TEXCOORD0;
//};

Output_PositionTexture main(Vertex_PositionNormalTexture input)
{
	Output_PositionTexture output = (Output_PositionTexture)0;

	float4 localH = float4(input.position, 1);
		localH = mul(localH, gMVP);
	//localH = mul(localH, View);
	//localH = mul(localH, Projection);

	output.position = localH;
	output.texCoord = input.texCoord;

	return output; // send projected vertex to the rasterizer stage
}
