#pragma pack_matrix(row_major)

#include "VertexLayouts.hlsli"
#include "cbuffer.hlsli"

Output_PositionTexture main(Vertex_PositionTexture input, uint vertexId : SV_VertexID)
{
	Output_PositionTexture output = (Output_PositionTexture)0;

	//output.position.xyz = input.position;
	//output.position.w = 1;

	float4 position = float4(input.position, 1);
	{
		position = mul(position, gM);

		//this matrix is, in this case, an Orthographic Matrix
		position = mul(position, gMVP);
	}

	output.position = position;
	output.texCoord = input.texCoord;// UVs[vertexId].xy;

	return output;
}
