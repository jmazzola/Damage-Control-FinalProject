#pragma pack_matrix(row_major)

cbuffer TColor : register(b0)
{
	float3 Color;
};

struct PSInput
{
	float4 ScreenPos 	: SV_Position;
	float2 TexCoord		: TEXCOORD;
};

//-------------------------------------------------------------------------------------------------
// Textures
//-------------------------------------------------------------------------------------------------
Texture2D       	  	DiffuseMap : register(t0);
Texture2DMS<float4, 4> 	LightTexture : register(t1);
SamplerState    		AnisoSampler : register(s0);

//-------------------------------------------------------------------------------------------------
// Pixel shader entry point. This shader runs per-pixel.
//-------------------------------------------------------------------------------------------------
float4 main(in PSInput input, in uint coverageMask : SV_Coverage) : SV_Target0
{
	// Determine our coordinate for sampling the texture based on the current screen position
	int2 sampleCoord = int2(input.ScreenPos.xy);

	float3 diffuse = 0;
	float3 specular = 0;
	const uint NumMSAASamples = 4;
	float numSamplesApplied = 0.0f;

	// Loop through the MSAA samples and modulate diffuse and specular lighting by the albedo
	for (uint i = 0; i < NumMSAASamples; ++i)
	{
		// We only want to apply a lighting sample if the geometry we've rasterized passed the
		// coverage test for the corresponding MSAA sample point
		if (coverageMask & (1 << i))
		{
			// Sample the light target for the current sub-sample
			float4 lighting = LightTexture.Load(sampleCoord, i);

				// Seperate into diffuse and specular components
				diffuse += lighting.xyz;
			specular += lighting.www;

			++numSamplesApplied;
		}
	}

	// Apply the diffuse normalization factor
	const float DiffuseNormalizationFactor = 1.0f / 3.14159265f;
	diffuse *= DiffuseNormalizationFactor;

	// Apply the albedos
	float3 diffuseAlbedo = DiffuseMap.Sample(AnisoSampler, input.TexCoord).rgb * Color;
		float3 specularAlbedo = float3(0.7f, 0.7f, 0.7f);
		diffuse *= diffuseAlbedo;
	specular *= specularAlbedo;

	// Final output is the sum of diffuse + specular divided by number of samples covered
	float3 output = (diffuse + specular) / numSamplesApplied;
		return float4(output, 1.0f);
}
