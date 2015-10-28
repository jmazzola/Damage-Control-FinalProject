/***********************************************
* Filename:			Collision.cpp
* Date:      		05/12/2015
* Mod. Date:		05/13/2015
* Mod. Initials:	BL
* Author:    		Brandon Lucier
* Purpose:
************************************************/

#include "Collision.h"
#include "../Renderer/DebugRenderer.h"

using namespace MathHelper;

/*****************************************************************
* CCollision():
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
CCollision::CCollision( )
{
}


/*****************************************************************
* ~CCollision():
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
CCollision::~CCollision( )
{
}

bool CCollision::SphereToSphere(TSphere* tA, TSphere* tB)
{
	float dX = (tA->vCenter.x - tB->vCenter.x) * (tA->vCenter.x - tB->vCenter.x);
	//float dY = (tA->Center.y - tB->Center.y) * (tA->Center.y - tB->Center.y);
	float dZ = (tA->vCenter.z - tB->vCenter.z) * (tA->vCenter.z - tB->vCenter.z);
	float distance = dX  /*+ dY*/ + dZ;
	float AddedRadii = tA->fRadius + tB->fRadius;

	if (distance < (AddedRadii * AddedRadii))
	{
		return true;
	}

	return false;
}

bool CCollision::SphereToAABB(TSphere* tSphere, TAABB* tBox, DirectX::XMFLOAT3& ClosestPoint_out)
{
	DirectX::XMFLOAT3 ClosestPoint;

	if (tSphere->vCenter.x < tBox->vMin.x)
	{
		ClosestPoint.x = tBox->vMin.x;
	}
	else if (tSphere->vCenter.x > tBox->vMax.x)
	{
		ClosestPoint.x = tBox->vMax.x;
	}
	else
	{
		ClosestPoint.x = tSphere->vCenter.x;
	}

	if (tSphere->vCenter.y < tBox->vMin.y)
	{
		ClosestPoint.y = tBox->vMin.y;
	}
	else if (tSphere->vCenter.y > tBox->vMax.y)
	{
		ClosestPoint.y = tBox->vMax.y;
	}
	else
	{
		ClosestPoint.y = tSphere->vCenter.y;
	}

	if (tSphere->vCenter.z < tBox->vMin.z)
	{
		ClosestPoint.z = tBox->vMin.z;
	}
	else if (tSphere->vCenter.z > tBox->vMax.z)
	{
		ClosestPoint.z = tBox->vMax.z;
	}
	else
	{
		ClosestPoint.z = tSphere->vCenter.z;
	}

	float dX = (tSphere->vCenter.x - ClosestPoint.x) * (tSphere->vCenter.x - ClosestPoint.x);
	//float dY = (sphere.Center.y - ClosestPoint.y) * (sphere.Center.y - ClosestPoint.y);
	float dZ = (tSphere->vCenter.z - ClosestPoint.z) * (tSphere->vCenter.z - ClosestPoint.z);

	float distance = dX /*+ dY*/ + dZ;

	if (distance < (tSphere->fRadius * tSphere->fRadius))
	{
		ClosestPoint_out = ClosestPoint;
		return true;
	}

	return false;
}

bool CCollision::SphereToAABB3D(TSphere* tSphere, TAABB* tBox, DirectX::XMFLOAT3& ClosestPoint_out)
{
	DirectX::XMFLOAT3 ClosestPoint;

	if (tSphere->vCenter.x < tBox->vMin.x)
	{
		ClosestPoint.x = tBox->vMin.x;
	}
	else if (tSphere->vCenter.x > tBox->vMax.x)
	{
		ClosestPoint.x = tBox->vMax.x;
	}
	else
	{
		ClosestPoint.x = tSphere->vCenter.x;
	}

	if (tSphere->vCenter.y < tBox->vMin.y)
	{
		ClosestPoint.y = tBox->vMin.y;
	}
	else if (tSphere->vCenter.y > tBox->vMax.y)
	{
		ClosestPoint.y = tBox->vMax.y;
	}
	else
	{
		ClosestPoint.y = tSphere->vCenter.y;
	}

	if (tSphere->vCenter.z < tBox->vMin.z)
	{
		ClosestPoint.z = tBox->vMin.z;
	}
	else if (tSphere->vCenter.z > tBox->vMax.z)
	{
		ClosestPoint.z = tBox->vMax.z;
	}
	else
	{
		ClosestPoint.z = tSphere->vCenter.z;
	}

	float dX = (tSphere->vCenter.x - ClosestPoint.x) * (tSphere->vCenter.x - ClosestPoint.x);
	float dY = (tSphere->vCenter.y - ClosestPoint.y) * (tSphere->vCenter.y - ClosestPoint.y);
	float dZ = (tSphere->vCenter.z - ClosestPoint.z) * (tSphere->vCenter.z - ClosestPoint.z);

	float distance = dX + dY + dZ;
	ClosestPoint_out = ClosestPoint;

	if (distance < (tSphere->fRadius * tSphere->fRadius))
	{
		return true;
	}

	return false;
}


int CCollision::PlaneToSphere(TPlane* tPlane, TSphere* tSphere)
{
	if (Dot(tSphere->vCenter, tPlane->vNormal) - tPlane->fOffset > tSphere->fRadius)
	{
		return 1;
	}
	else if (Dot(tSphere->vCenter, tPlane->vNormal) - tPlane->fOffset < -tSphere->fRadius)
	{
		return 2;
	}
	else
	{
		return 3;
	}
}

int CCollision::PlaneToAABB(TPlane* tPlane, TAABB* tBox)
{
	DirectX::XMFLOAT3 center;
	center.x = (tBox->vMax.x + tBox->vMin.x) / 2;
	center.y = (tBox->vMax.y + tBox->vMin.y) / 2;
	center.z = (tBox->vMax.z + tBox->vMin.z) / 2;
	DirectX::XMFLOAT3 extents = tBox->vMax - center;
	DirectX::XMFLOAT3 absNorm = { abs(tPlane->vNormal.x), abs(tPlane->vNormal.y), abs(tPlane->vNormal.z) };
	float radius = Dot(extents, absNorm);

	TSphere sphere;
	sphere.vCenter = center;
	sphere.fRadius = radius;

	return PlaneToSphere(tPlane, &sphere);
}

void CCollision::BuildFrustum(float fFov, float fNearDist, float fFarDist, float fRatio, DirectX::XMFLOAT4X4& rCamXform, TFrustum& rtFrustum_out)
{
	/* parameters:
	frustum - the thing we're filling out
	fov - Vertical field of view
	nearDist - distance from camera to near plane
	farDist - distance from camera to far plane
	ratio - is the ViewWidth divided ViewHeight
	camXform - World matrix of the camera object
	*/

	// Calculating the center point of the near & far plane
	DirectX::XMFLOAT3 Position = XMMatrixPosition(rCamXform);
	DirectX::XMFLOAT3 Zaxis = XMMatrixForward(rCamXform);
	DirectX::XMFLOAT3 nearPlane_Center = Position - Zaxis * fNearDist;
	DirectX::XMFLOAT3 farPlane_Center = Position - Zaxis * fFarDist;

	// Calculating the Width and Height
	float nearPlane_Height = 2 * tan(fFov / 2) * fNearDist;
	float farPlane_Height = 2 * tan(fFov / 2) * fFarDist;
	float nearPlane_Width = nearPlane_Height * fRatio;
	float farPlane_Width = farPlane_Height * fRatio;

	// calculating the corners the add/sub will change based on what point we want
	// TL + then -
	// BL - then -
	// BR - then +
	// TR + then +
	DirectX::XMFLOAT3 YAxis = MathHelper::XMMatrixUp(rCamXform);
	DirectX::XMFLOAT3 XAxis = { rCamXform._11, rCamXform._21, rCamXform._31 };
	rtFrustum_out.vCorners[FTL] = farPlane_Center + YAxis * (farPlane_Height / 2) - XAxis * (farPlane_Width / 2);
	rtFrustum_out.vCorners[FBL] = farPlane_Center - YAxis * (farPlane_Height / 2) - XAxis * (farPlane_Width / 2);
	rtFrustum_out.vCorners[FBR] = farPlane_Center - YAxis * (farPlane_Height / 2) + XAxis * (farPlane_Width / 2);
	rtFrustum_out.vCorners[FTR] = farPlane_Center + YAxis * (farPlane_Height / 2) + XAxis * (farPlane_Width / 2);
	rtFrustum_out.vCorners[NTL] = nearPlane_Center + YAxis * (nearPlane_Height / 2) - XAxis * (nearPlane_Width / 2);
	rtFrustum_out.vCorners[NTR] = nearPlane_Center + YAxis * (nearPlane_Height / 2) + XAxis * (nearPlane_Width / 2);
	rtFrustum_out.vCorners[NBR] = nearPlane_Center - YAxis * (nearPlane_Height / 2) + XAxis * (nearPlane_Width / 2);
	rtFrustum_out.vCorners[NBL] = nearPlane_Center - YAxis * (nearPlane_Height / 2) - XAxis * (nearPlane_Width / 2);

	ComputePlane(rtFrustum_out.vCorners[NBR], rtFrustum_out.vCorners[NBL], rtFrustum_out.vCorners[NTL], rtFrustum_out.tPlanes[NEAR_PLANE]);
	ComputePlane(rtFrustum_out.vCorners[FTL], rtFrustum_out.vCorners[FBL], rtFrustum_out.vCorners[FBR], rtFrustum_out.tPlanes[FAR_PLANE]);
	ComputePlane(rtFrustum_out.vCorners[NBL], rtFrustum_out.vCorners[FBL], rtFrustum_out.vCorners[FTL], rtFrustum_out.tPlanes[LEFT_PLANE]);
	ComputePlane(rtFrustum_out.vCorners[FTR], rtFrustum_out.vCorners[FBR], rtFrustum_out.vCorners[NBR], rtFrustum_out.tPlanes[RIGHT_PLANE]);
	ComputePlane(rtFrustum_out.vCorners[FTL], rtFrustum_out.vCorners[FTR], rtFrustum_out.vCorners[NTR], rtFrustum_out.tPlanes[TOP_PLANE]);
	ComputePlane(rtFrustum_out.vCorners[FBR], rtFrustum_out.vCorners[FBL], rtFrustum_out.vCorners[NBL], rtFrustum_out.tPlanes[BOTTOM_PLANE]);


}

void CCollision::ComputePlane(DirectX::XMFLOAT3& rPointA, DirectX::XMFLOAT3& rPointB, DirectX::XMFLOAT3& rPointC, TPlane& rPlane_out)
{
	DirectX::XMFLOAT3 edge1 = rPointB - rPointA;
	DirectX::XMFLOAT3 edge2 = rPointC - rPointB;

	DirectX::XMFLOAT3 TheNormal;
	DirectX::XMStoreFloat3(&TheNormal, DirectX::XMVector3Cross(XMLoadFloat3(&edge1), XMLoadFloat3(&edge2)));

	rPlane_out.vNormal = Normalize(TheNormal);

	rPlane_out.fOffset = Dot(rPlane_out.vNormal, rPointA);
}

bool CCollision::FrustumToSphere(TFrustum* rtFrustum, TSphere* tSphere)
{
	if (PlaneToSphere(&rtFrustum->tPlanes[NEAR_PLANE], tSphere) == 2)
	{
		return false;
	}
	if (PlaneToSphere(&rtFrustum->tPlanes[FAR_PLANE], tSphere) == 2)
	{
		return false;
	}
	if (PlaneToSphere(&rtFrustum->tPlanes[LEFT_PLANE], tSphere) == 2)
	{
		return false;
	}
	if (PlaneToSphere(&rtFrustum->tPlanes[RIGHT_PLANE], tSphere) == 2)
	{
		return false;
	}
	if (PlaneToSphere(&rtFrustum->tPlanes[TOP_PLANE], tSphere) == 2)
	{
		return false;
	}
	if (PlaneToSphere(&rtFrustum->tPlanes[BOTTOM_PLANE], tSphere) == 2)
	{
		return false;
	}

	return true;
}

bool CCollision::FrustumToAABB(TFrustum* rtFrustum, TAABB* tAabb)
{
	if (PlaneToAABB(&rtFrustum->tPlanes[NEAR_PLANE], tAabb) == 2)
	{
		return false;
	}
	if (PlaneToAABB(&rtFrustum->tPlanes[FAR_PLANE], tAabb) == 2)
	{
		return false;
	}
	if (PlaneToAABB(&rtFrustum->tPlanes[LEFT_PLANE], tAabb) == 2)
	{
		return false;
	}
	if (PlaneToAABB(&rtFrustum->tPlanes[RIGHT_PLANE], tAabb) == 2)
	{
		return false;
	}
	if (PlaneToAABB(&rtFrustum->tPlanes[TOP_PLANE], tAabb) == 2)
	{
		return false;
	}
	if (PlaneToAABB(&rtFrustum->tPlanes[BOTTOM_PLANE], tAabb) == 2)
	{
		return false;
	}

	return true;
}

bool CCollision::CapsuleToAABB3D(TCapsule* tCapsule, TAABB* tAabb)
{
	//Center of the AABB
	DirectX::XMFLOAT3 xmBoxCenter = (tAabb->vMax + tAabb->vMin) * 0.5f;
	//Find the closest point on the capsule's line
	DirectX::XMFLOAT3 xmSegDir = tCapsule->tSegment.m_End - tCapsule->tSegment.m_Start;
	DirectX::XMFLOAT3 xmSegNorm = Normalize(xmSegDir);
	DirectX::XMFLOAT3 xmToBox = xmBoxCenter - tCapsule->tSegment.m_Start;
	float fDot = Dot(xmSegNorm, xmToBox);
	xmSegNorm = xmSegNorm * fDot;

	if (fDot > Length(xmSegDir) || fDot < 0)
	{
		return false;
	}

	TSphere tTemp;
	tTemp.type = TCollider::eSphere;
	tTemp.vCenter = tCapsule->tSegment.m_Start + xmSegNorm;
	tTemp.fRadius = tCapsule->fRadius;

	//Is the closest point on the line within the radius of the closest point in the box
	if (SphereToAABB3D(&tTemp, tAabb, xmBoxCenter))
	{
		return true;
	}

	return false;
}

bool CCollision::CapsuleToAABB3D(TSegment* tSegment, float fRadius, TAABB* tAabb)
{
	//Construct a capsule
	TCapsule tCapsule;
	tCapsule.type = TCollider::eCapsule;
	tCapsule.tSegment = *tSegment;
	tCapsule.fRadius = fRadius;

	//Test it
	return CapsuleToAABB3D(&tCapsule, tAabb);
}

bool CCollision::CapsuleToAABB3D(DirectX::XMFLOAT3 xmStart, DirectX::XMFLOAT3 xmEnd, float fRadius, TAABB* tAabb)
{
	//Construct some spheres
	TSegment tSegment;
	tSegment.m_End = xmEnd;
	tSegment.m_Start = xmStart;

	//Test it
	return CapsuleToAABB3D(&tSegment, fRadius, tAabb);
}

bool CCollision::CapsuleToSphere(TCapsule* tCapsule, TSphere* tSphere)
{
	//Test the spherical parts of the tCapsule
	TSphere temp;
	temp.type = TCollider::eSphere;
	temp.fRadius = tCapsule->fRadius;
	temp.vCenter = tCapsule->tSegment.m_Start;
	if (SphereToSphere(&temp, tSphere))
		return true;
	temp.vCenter = tCapsule->tSegment.m_End;
	if (SphereToSphere(&temp, tSphere))
		return true;

	//Could still collide at the cyllinder...
	//Find closest edge of cyllinder
	DirectX::XMFLOAT3 neutSphere = tSphere->vCenter;
	neutSphere.y = tCapsule->tSegment.m_Start.y;
	DirectX::XMFLOAT3 cylStart = neutSphere - tCapsule->tSegment.m_Start;
	Normalize(cylStart);
	cylStart = tCapsule->tSegment.m_Start + cylStart * tCapsule->fRadius;
	neutSphere.y = tCapsule->tSegment.m_End.y;
	DirectX::XMFLOAT3 cylEnd = neutSphere - tCapsule->tSegment.m_End;
	Normalize(cylStart);
	cylEnd = tCapsule->tSegment.m_End + cylEnd * tCapsule->fRadius;

	//Get the line segment between the two points
	DirectX::XMFLOAT3 segment = cylEnd - cylStart;
	float length = Length(segment);
	Normalize(segment);
	DirectX::XMFLOAT3 toSphere = tSphere->vCenter - cylStart;
	//Find distance along the line
	float distance = Dot(segment, toSphere);
	//If the ends are the closest points, we already know there's no collision
	if (distance <= 0 || distance >= length)
		return false;
	//Found the closest point, what's the distance?
	DirectX::XMFLOAT3 closest = cylStart + segment * distance;
	toSphere = closest - tSphere->vCenter;
	if (SquareLength(toSphere) <= tSphere->fRadius * tSphere->fRadius)
		return true;

	return false;
}

bool CCollision::CapsuleToSphere(TSegment* tSegment, float fRadius, TSphere* tSphere)
{
	//Construct a capsule
	TCapsule tCapsule;
	tCapsule.type = TCollider::eCapsule;
	tCapsule.tSegment = *tSegment;
	tCapsule.fRadius = fRadius;

	//Test it
	return CapsuleToSphere(&tCapsule, tSphere);
}

bool CCollision::CapsuleToSphere(DirectX::XMFLOAT3 xmStart, DirectX::XMFLOAT3 xmEnd, float fRadius, TSphere* tSphere)
{
	//Construct some spheres
	TSegment tSegment;
	tSegment.m_End = xmEnd;
	tSegment.m_Start = xmStart;

#ifdef _DEBUG
	DirectX::XMFLOAT2 xmRadius = DirectX::XMFLOAT2(fRadius, fRadius);
	DirectX::XMFLOAT4 xmColor = DirectX::XMFLOAT4(1, 0.5f, 0.0f, 1);
	CDebugRenderer::GetReference( ).CircleRenderer(xmStart, xmRadius, xmColor);
	CDebugRenderer::GetReference( ).CircleRenderer(xmEnd, xmRadius, xmColor);
#endif
	//Test it
	return CapsuleToSphere(&tSegment, fRadius, tSphere);

}

bool CCollision::ColliderToCollider(TCollider* pCol1, TCollider* pCol2)
{
	TAABB* AABB1 = nullptr, *AABB2 = nullptr;
	TPlane* Plane1 = nullptr, *Plane2 = nullptr;
	TSphere* Sphere1 = nullptr, *Sphere2 = nullptr;
	TCapsule* Capsule1 = nullptr, *Capsule2 = nullptr;

	switch (pCol1->type)
	{
		case TCollider::ePlane:
			{
				Plane1 = static_cast<TPlane*>(pCol1);
			}
			break;
		case TCollider::eAABB:
			{
				AABB1 = static_cast<TAABB*>(pCol1);
			}
			break;
		case TCollider::eSphere:
			{
				Sphere1 = static_cast<TSphere*>(pCol1);
			}
			break;
		case TCollider::eCapsule:
			{
				Capsule1 = static_cast<TCapsule*>(pCol1);
			}
			break;
		default:
			return false;
	}

	switch (pCol2->type)
	{
		case TCollider::ePlane:
			{
				Plane2 = static_cast<TPlane*>(pCol2);
			}
			break;
		case TCollider::eAABB:
			{
				AABB2 = static_cast<TAABB*>(pCol2);
			}
			break;
		case TCollider::eSphere:
			{
				Sphere2 = static_cast<TSphere*>(pCol2);
			}
			break;
		case TCollider::eCapsule:
			{
				Capsule2 = static_cast<TCapsule*>(pCol2);
			}
			break;
		default:
			return false;
	}

	if (Sphere1 != nullptr && Sphere2 != nullptr)
	{
		return CCollision::SphereToSphere(Sphere1, Sphere2);
	}
	else if (Plane1 != nullptr && AABB2 != nullptr)
	{
		return (CCollision::PlaneToAABB(Plane1, AABB2) == 3);
	}
	else if (AABB1 != nullptr && Plane2 != nullptr)
	{
		return (CCollision::PlaneToAABB(Plane2, AABB1) == 3);
	}
	else if (Plane1 != nullptr && Sphere2 != nullptr)
	{
		return (CCollision::PlaneToSphere(Plane1, Sphere2) == 3);
	}
	else if (Sphere1 != nullptr && Plane2 != nullptr)
	{
		return (CCollision::PlaneToSphere(Plane2, Sphere1) == 3);
	}
	else if (Sphere1 != nullptr && AABB2 != nullptr)
	{
		DirectX::XMFLOAT3 point = { 0, 0, 0 };
		return CCollision::SphereToAABB(Sphere1, AABB2, point);
	}
	else if (AABB1 != nullptr && Sphere2 != nullptr)
	{
		DirectX::XMFLOAT3 point = { 0, 0, 0 };
		return CCollision::SphereToAABB(Sphere2, AABB1, point);
	}


	return false;

}
