#pragma pack_matrix(row_major)

//-------------------------------------------------------------------------------------------------
// Constants
//-------------------------------------------------------------------------------------------------
cbuffer CameraParams : register(b0)
{
	matrix ViewMatrix;
	matrix ProjMatrix;
	matrix InvProjMatrix;
	float2 ClipPlanes;
}

// This indicates the number of MSAA samples
#define NUMSUBSAMPLES 4

//-------------------------------------------------------------------------------------------------
// Input/output structs
//-------------------------------------------------------------------------------------------------
struct VSInput
{
	//#if ( POINTLIGHT || SPOTLIGHT )
		float3 LightPosWS 		: POSITION;
		float LightRange : RANGE;
	//#endif
	
		float3 LightColor 		: COLOR;
	
	//#if ( DIRECTIONALLIGHT || SPOTLIGHT )
		float3 LightDirectionWS : DIRECTION;
	//#endif
	
	//#if SPOTLIGHT
		float2 SpotlightAngles	: SPOTANGLES;
	//#endif
};

struct VSOutput
{
	#if ( POINTLIGHT || SPOTLIGHT )
		float3 LightPosVS 		: POSITION;
		float LightRange : RANGE;
	#endif
	
		float3 LightColor 		: COLOR;
	
	#if ( DIRECTIONALLIGHT || SPOTLIGHT )
		float3 LightDirectionVS : DIRECTION;
	#endif
	
	#if SPOTLIGHT
		float2 SpotlightAngles	: SPOTANGLES;
	#endif
};

//-------------------------------------------------------------------------------------------------
// Vertex shader entry point. This shader passes along the light parameters to the geometry shader,
// and also transforms the light postition and direction to view space.
//-------------------------------------------------------------------------------------------------
VSOutput main(in VSInput input)
{
	VSOutput output;

	output.LightColor = input.LightColor;

	#if ( POINTLIGHT || SPOTLIGHT )
		output.LightPosVS = mul(float4(input.LightPosWS, 1.0f), ViewMatrix).xyz;
		output.LightRange = input.LightRange;
	#endif
	
	#if ( DIRECTIONALLIGHT || SPOTLIGHT )
		output.LightDirectionVS = mul(input.LightDirectionWS, (float3x3)ViewMatrix);
	#endif
	
	#if SPOTLIGHT
		output.SpotlightAngles = input.SpotlightAngles;
	#endif

	return output;
}