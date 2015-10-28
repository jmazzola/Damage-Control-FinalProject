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

struct GSOutput
{
	float4 PositionCS			: SV_Position;
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
// Determines the extents of a minimum-area quad fit to a spherical light volume
//-------------------------------------------------------------------------------------------------
void CalcLightQuad(in float3 lightPosVS, in float lightRange, out float2 topLeft, out float2 bottomRight)
{
	// Fit a bounding sphere to the light, where the center is the light position and the radius
	// is the light's range.
	float4 lightCenterVS = float4(lightPosVS, 1.0f);
	float radius = lightRange;

	// Figure out the four points at the top, bottom, left, and right of the sphere
	float4 topVS = lightCenterVS + float4(0.0f, radius, 0.0f, 0.0f);
	float4 bottomVS = lightCenterVS - float4(0.0f, radius, 0.0f, 0.0f);
	float4 leftVS = lightCenterVS - float4(radius, 0.0f, 0.0f, 0.0f);
	float4 rightVS = lightCenterVS + float4(radius, 0.0f, 0.0f, 0.0f);

	// Figure out whether we want to use the top and right from quad
	// tangent to the front of the sphere, or the back of the sphere
	leftVS.z = leftVS.x < 0.0f ? leftVS.z - radius : leftVS.z + radius;
	rightVS.z = rightVS.x < 0.0f ? rightVS.z + radius : rightVS.z - radius;
	topVS.z = topVS.y < 0.0f ? topVS.z + radius : topVS.z - radius;
	bottomVS.z = bottomVS.y < 0.0f ? bottomVS.z - radius : bottomVS.z + radius;

	// Clamp the z coordinate to the clip planes
	leftVS.z = clamp(leftVS.z, ClipPlanes.x, ClipPlanes.y);
	rightVS.z = clamp(rightVS.z, ClipPlanes.x, ClipPlanes.y);
	topVS.z = clamp(topVS.z, ClipPlanes.x, ClipPlanes.y);
	bottomVS.z = clamp(bottomVS.z, ClipPlanes.x, ClipPlanes.y);

	// Figure out the rectangle in clip-space by applying the perspective transform.
	// We assume that the perspective transform is symmetrical with respect to X and Y.
	float rectLeftCS = leftVS.x * ProjMatrix[0][0] / leftVS.z;
	float rectRightCS = rightVS.x * ProjMatrix[0][0] / rightVS.z;
	float rectTopCS = topVS.y * ProjMatrix[1][1] / topVS.z;
	float rectBottomCS = bottomVS.y * ProjMatrix[1][1] / bottomVS.z;

	// clamp the rectangle to the screen extents
	rectTopCS = clamp(rectTopCS, -1.0f, 1.0f);
	rectBottomCS = clamp(rectBottomCS, -1.0f, 1.0f);
	rectLeftCS = clamp(rectLeftCS, -1.0f, 1.0f);
	rectRightCS = clamp(rectRightCS, -1.0f, 1.0f);

	topLeft = float2(rectLeftCS, rectTopCS);
	bottomRight = float2(rectRightCS, rectBottomCS);
}

//-------------------------------------------------------------------------------------------------
// Geometry shader entry point. This shader emits a quad, whose extents are determined by finding
// the screen space min and max of a view space-aligned bounding box fit to the light volume.
// For directional lights, a full-screen quad is emitted instead.
//-------------------------------------------------------------------------------------------------
[maxvertexcount(4)]
void main(point VSOutput input[1], inout TriangleStream<GSOutput> QuadStream)
{
	GSOutput output;

	// Pass along light properties
	output.LightColor = input[0].LightColor;

	#if ( POINTLIGHT || SPOTLIGHT )
		output.LightPosVS = input[0].LightPosVS;
		output.LightRange = input[0].LightRange;
	#endif
	
	#if ( DIRECTIONALLIGHT || SPOTLIGHT )
		output.LightDirectionVS = input[0].LightDirectionVS;
	#endif
	
	#if SPOTLIGHT
		output.SpotlightAngles = input[0].SpotlightAngles;
	#endif
	
	#if ( POINTLIGHT || SPOTLIGHT )				
		// Figure out a depth for the quad, by using the point on the sphere 
		// furthest from the camera. We combine this with "greater-than" depth testing,
		// so that any pixels where the sphere is "floating in air" will be culled and only
		// pixels where the volume intersects with geometry will be lit. This is based on 
		// the fact that our scene is mostly an empty volume, with walls on the far side.
		// However we'll clamp to the far plane if the light intersects with it, so that
		// we don't inadvertantly clip pixels that still need to be lit.
		float lightDepth = input[0].LightPosVS.z + input[0].LightRange;
		if (input[0].LightPosVS.z - input[0].LightRange < ClipPlanes.y)
			lightDepth = min(lightDepth, ClipPlanes.y);
	
		// Project the light depth
		lightDepth = mul(float4(0, 0, lightDepth, 1.0f), ProjMatrix).z / lightDepth;
	
		// Figure out the quad extents
		float2 topLeft, bottomRight;
		CalcLightQuad(input[0].LightPosVS, input[0].LightRange, topLeft, bottomRight);
	
		float4 quadVerts[4] =
		{
			float4(topLeft.x, topLeft.y, lightDepth, 1.0f),
			float4(bottomRight.x, topLeft.y, lightDepth, 1.0f),
			float4(topLeft.x, bottomRight.y, lightDepth, 1.0f),
			float4(bottomRight.x, bottomRight.y, lightDepth, 1.0f)
		};
	#elif DIRECTIONALLIGHT
		float4 quadVerts[4] =
		{
			float4(-1, 1, 1, 1),
			float4(1, 1, 1, 1),
			float4(-1, -1, 1, 1),
			float4(1, -1, 1, 1)
		};
	#endif

	// Emit 4 new verts, and 2 new triangles
	for (int i = 0; i < 4; i++)
	{
		output.PositionCS = quadVerts[i];

		// Calculate the view ray by unprojecting the vertex position into view space.
		// For a quad we can clamp in the vertex shader, since we only interpolate in the XY direction.
		float3 positionVS = mul(quadVerts[i], InvProjMatrix).xyz;
		output.ViewRay = float3(positionVS.xy / positionVS.z, 1.0f);

		QuadStream.Append(output);
	}

	QuadStream.RestartStrip( );
}