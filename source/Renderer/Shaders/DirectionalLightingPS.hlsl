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
struct PSInput
{
	float4 ScreenPos			: SV_Position;
	float3 ViewRay				: VIEWRAY;

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
// Textures
//-------------------------------------------------------------------------------------------------
Texture2DMS<float4> GBufferTexture : register(t0);
Texture2DMS<float> DepthTexture : register(t1);

//-------------------------------------------------------------------------------------------------
// Decodes a spheremap-encoded normal
//-------------------------------------------------------------------------------------------------
float3 SpheremapDecode(float2 encoded)
{
	float4 nn = float4(encoded, 1, -1);
	float l = dot(nn.xyz, -nn.xyw);
	nn.z = l;
	nn.xy *= sqrt(l);
	return nn.xyz * 2 + float3(0, 0, -1);
}

//-------------------------------------------------------------------------------------------------
// Converts a depth buffer value to view-space position
//-------------------------------------------------------------------------------------------------
float3 PositionFromDepth(in float zBufferDepth, in float3 viewRay)
{
	// For a quad we already clamped in the vertex shader
	viewRay = viewRay.xyz;

	// Convert to a linear depth value using the projection matrix
	float linearDepth = ProjMatrix[3][2] / (zBufferDepth - ProjMatrix[2][2]);
	return viewRay * linearDepth;
}

//-------------------------------------------------------------------------------------------------
// Helper function for extracting G-Buffer attributes
//-------------------------------------------------------------------------------------------------
void GetGBufferAttributes(in float2 screenPos, in float3 viewRay, in uint subSampleIndex,
						  out float3 normal, out float3 position, out float specularPower)
{
	// Determine our coordinate for sampling the texture based on the current screen position
	int2 sampleCoord = int2(screenPos.xy);

	float4 gBuffer = GBufferTexture.Load(sampleCoord, subSampleIndex);
	normal = SpheremapDecode(gBuffer.xy);
	position = PositionFromDepth(DepthTexture.Load(sampleCoord, subSampleIndex).x, viewRay);
	specularPower = gBuffer.z;
}

//-------------------------------------------------------------------------------------------------
// Calculates the lighting term for a single G-Buffer texel
//-------------------------------------------------------------------------------------------------
float4 CalcLighting(in float3 normal, in float3 position, in float specularPower, in PSInput input)
{
	// Hack the ambience
	#if DIRECTIONALLIGHT
		float3 ambience = float3(.1f, .1f, .1f);
		float3 light = input.LightColor;
		float3 final = light * ambience;
		return float4(final, 0.0f);
	#endif

	// Calculate the diffuse term
	float3 L = 0;
	float attenuation = 1.0f;
	#if POINTLIGHT || SPOTLIGHT
		// Base the the light vector on the light position
		L = input.LightPosVS - position;
	
		// Calculate attenuation based on distance from the light source
		float dist = length(L);
		attenuation = max(0, 1.0f - (dist / input.LightRange));
	
		L /= dist;
	#elif DIRECTIONALLIGHT
		// Light direction is explicit for directional lights
		L = -input.LightDirectionVS;
	#endif
	
	#if SPOTLIGHT
		// Also add in the spotlight attenuation factor
		float3 L2 = input.LightDirectionVS;
		float rho = dot(-L, L2);
		attenuation *= saturate((rho - input.SpotlightAngles.y) / (input.SpotlightAngles.x - input.SpotlightAngles.y));
	#endif

	float nDotL = saturate(dot(normal, L));
	float3 diffuse = nDotL * input.LightColor * attenuation;

	// In view space camera position is (0, 0, 0)
	float3 camPos = float3(0.0f, 0.0f, 0.0f);

	// Calculate the specular term
	float3 V = camPos - position;
	float3 H = normalize(L + V);
	float specNormalizationFactor = ((specularPower + 8.0f) / (8.0f * 3.14159265f));
	float specular = pow(saturate(dot(normal, H)), specularPower) * specNormalizationFactor * nDotL * attenuation;

	// Final value is diffuse RGB + mono specular
	return float4(diffuse, specular);
}

//-------------------------------------------------------------------------------------------------
// Pixel shader entry point. This shader runs per-pixel.
//-------------------------------------------------------------------------------------------------
float4 main(in PSInput input) : SV_Target0
{
	float3 normal;
	float3 position;
	float specularPower;

	// Get the G-Buffer values for the first sub-sample, and calculate the lighting
	GetGBufferAttributes(input.ScreenPos.xy, input.ViewRay, 0, normal, position, specularPower);

	return CalcLighting(normal, position, specularPower, input);
}
