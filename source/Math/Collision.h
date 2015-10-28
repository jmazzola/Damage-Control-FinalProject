/***********************************************
* Filename:			Collision.h
* Date:      		05/12/2015
* Mod. Date:		05/13/2015
* Mod. Initials:	BL
* Author:    		Brandon Lucier
* Purpose:			Checks to see if 2 collidiers touched each other
************************************************/

#ifndef _CCOLLISION_H_
#define _CCOLLISION_H_

#include "..\Core\stdafx.h" 
#include "..\Core\StructureHolder.h"

class CCollision
{
public:
	CCollision();
	~CCollision();

	enum FrustumCorners{ FTL = 0, FBL, FBR, FTR, NTL, NTR, NBR, NBL };
	enum FrustumPlanes{ NEAR_PLANE = 0, FAR_PLANE, LEFT_PLANE, RIGHT_PLANE, TOP_PLANE, BOTTOM_PLANE };


	/*****************************************************************
	* ComputePlane():
	* Ins:					  DirectX::XMFLOAT3 pointA the position of a point for a plane
	*						  DirectX::XMFLOAT3 pointB the position of a point for a plane
	*						  DirectX::XMFLOAT3 pointC the position of a point for a plane
	* Outs:					  tPlane the plane that will be built by the above points
	* Returns:				  void
	* Mod. Date:		      05/14/2015
	* Mod. Initials:		  BL
	*****************************************************************/
	static void ComputePlane(DirectX::XMFLOAT3& rPointA,
							 DirectX::XMFLOAT3& rPointB,
							 DirectX::XMFLOAT3& rPointC,
							 TPlane& rtPlane_out);


	/*****************************************************************
	* BuildFrustum():
	* Ins:					  float fov (Field of View) - for the frustum
	*						  float nearDist - the distance to the near plane of the frustum
	*						  float farDist - the distance to the far plane of the frustum
	*						  float ratio - ViewWidth / ViewHeight
	*						  const matrix4f& - the world matrix for the frustum
	* Outs:					  tFrustum frustum_out - the built frustum for your pleasure
	* Returns:				  void
	* Mod. Date:		      05/14/2015
	* Mod. Initials:		  BL
	*****************************************************************/
	static void BuildFrustum(float fFov,
							 float fNearDist,
							 float fFarDist,
							 float fRatio,
							 DirectX::XMFLOAT4X4& rCamXform,
							 TFrustum& rtFrustum_out);

	/*****************************************************************
	* SphereToSphere():
	* Ins:					  tSphere a - the sphere used to check against the other
	*						  tSphere b - the sphere used to check against the other
	* Outs:					  
	* Returns:				  bool - true if they collide
	* Mod. Date:		      05/14/2015
	* Mod. Initials:		  BL
	*****************************************************************/
	static bool SphereToSphere(TSphere* tA,
							   TSphere* tB);

	/*****************************************************************
	* SphereToAABB():
	* Ins:					  tSphere a - the sphere used to check against the other
	*						  tAABB b - the AABB used to check against the other
	* Outs:
	* Returns:				  bool - true if they collide
	* Mod. Date:		      05/14/2015
	* Mod. Initials:		  BL
	*****************************************************************/
	static bool SphereToAABB(TSphere* tSphere,
							 TAABB* tBox,
							 DirectX::XMFLOAT3& ClosestPoint_Out);
	static bool SphereToAABB3D(TSphere* tSphere,
							TAABB* tBox,
							DirectX::XMFLOAT3& ClosestPoint_Out);

	/*****************************************************************
	* PlaneToSphere():
	* Ins:					  tPlane plane - the plane used to check against the other
	*						  tSphere sphere - the sphere used to check against the other
	* Outs:
	* Returns:				  int
	*							1 if the sphere is in front of the plane
	*							2 if the sphere is behind the plane
	*							3 if the sphere is stradding the plane
	* Mod. Date:		      05/14/2015
	* Mod. Initials:		  BL
	*****************************************************************/
	static int PlaneToSphere(TPlane* tPlane,
							 TSphere* tSphere);

	/*****************************************************************
	* PlaneToAABB():
	* Ins:					  tPlane plane - the plane used to check against the other
	*						  tAABB aabb - the AABB used to check against the other
	* Outs:
	* Returns:				  int
	*							1 if the AABB is in front of the plane
	*							2 if the AABB is behind the plane
	*							3 if the AABB is stradding the plane
	* Mod. Date:		      05/14/2015
	* Mod. Initials:		  BL
	*****************************************************************/
	static int PlaneToAABB(TPlane* tPlane,
						   TAABB* tAabb);

	/*****************************************************************
	* FrustumToSphere():
	* Ins:					  tFrustum& frustum - the frustum used to check against the other
	*						  tSphere& sphere - the aabb used to check against the other
	* Outs:
	* Returns:				  bool - true if they collide
	* Mod. Date:		      05/14/2015
	* Mod. Initials:		  BL
	*****************************************************************/
	static bool FrustumToSphere(TFrustum* tFrustum,
								TSphere* tSphere);
	

	/*****************************************************************
	* FrustumToAABB():
	* Ins:					  tFrustum& frustum - the frustum used to check against the other
	*						  tAABB& aabb - the aabb used to check against the other
	* Outs:
	* Returns:				  bool - true if they collide
	* Mod. Date:		      05/14/2015
	* Mod. Initials:		  BL
	*****************************************************************/
	static bool FrustumToAABB(TFrustum* tFrustum,
							  TAABB* tAabb);

	static bool CapsuleToSphere(TCapsule* tCapsule, TSphere* tSphere);

	static bool CapsuleToSphere(TSegment* tSegment, float fRadius, TSphere* tSphere);

	static bool CapsuleToSphere(DirectX::XMFLOAT3 xmStart, DirectX::XMFLOAT3 xmEnd, float fRadius, TSphere* tSphere);

	/*****************************************************************
	* CapsuleToAABB():
	* Ins:					  TCapsule& tCapsule - the capsule that will collide
	*						  tAABB& aabb - the aabb used to check against the other
	* Outs:
	* Returns:				  bool - true if they collide
	* Mod. Date:		      06/08/2015
	* Mod. Initials:		  MJG
	*****************************************************************/
	static bool CapsuleToAABB3D(TCapsule* tCapsule, TAABB* tAabb);

	static bool CapsuleToAABB3D(TSegment* tSegment, float fRadius, TAABB* tAabb);

	static bool CapsuleToAABB3D(DirectX::XMFLOAT3 xmStart, DirectX::XMFLOAT3 xmEnd, float fRadius, TAABB* tAabb);

	static bool ColliderToCollider(TCollider* pCol1, TCollider* pCol2);
};
#endif // _CCOLLISION_H_
