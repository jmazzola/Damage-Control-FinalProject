/***********************************************
* Filename:			CDeformer.h
* Date:      		05/12/2015
* Mod. Date:		05/15/2015
* Mod. Initials:	JPM
* Author:    		Justin Mazzola
* Purpose:			This class will be the deformer
for a mesh, handling skinning
vertices.
************************************************/

#ifndef _CDEFORMER_H_
#define _CDEFORMER_H_

#include "..\Core\stdafx.h"
#include "..\Animation Manager\Mesh.h"

class CMesh;

class CDeformer
{
public:
	CDeformer();
	~CDeformer() {}

	/*****************************************************************
	* Process():	Blackbox function to process the skin verts and pass
	*				them to the GPU
	*
	* Returns:		void
	*
	* Mod. Date		05/18/2015
	* Mod. Initials:JPM
	*****************************************************************/
	void Process(std::vector<DirectX::XMFLOAT4X4>& vPose);


	void LoadVerts();


	/***************
	*  Accessors
	***************/

	__forceinline CMesh* GetMesh() const
	{
		return m_pMesh;
	}

	__forceinline std::vector<CMesh::TSkinnedVertex>& GetSkinVerts()
	{
		return m_vSkinVerts;
	}

	__forceinline std::vector<DirectX::XMFLOAT4X4>& GetSkinnedMatrices()
	{
		return m_vSkinnedMatrices;
	}

	__forceinline std::vector<DirectX::XMINT4>& GetJointIndices()
	{
		return m_vJointIndices;
	}

	__forceinline std::vector<DirectX::XMFLOAT4>& GetWeights()
	{
		return m_vWeights;
	}


	/***************
	*  Mutators
	***************/

	__forceinline void SetMesh(CMesh* pMesh)
	{
		m_pMesh = pMesh;
		LoadVerts();
	}


private:


	/*****************************************************************
	* OrthoNormalInverse():	Math helper function to return a orthonormal
	*						inverse matrix with the given parameter.
	*
	* Ins:					float4x4
	*
	* Outs:					return4x4
	* Returns:				void
	*
	* Mod. Date				05/18/2015
	* Mod. Initials:		JPM
	*****************************************************************/
	__forceinline void OrthoNormalInverse(DirectX::XMFLOAT4X4& float4x4, DirectX::XMFLOAT4X4& return4x4)
	{
		return4x4._11 = float4x4._11;
		return4x4._12 = float4x4._21;
		return4x4._13 = float4x4._31;
		return4x4._14 = float4x4._41;

		return4x4._21 = float4x4._12;
		return4x4._22 = float4x4._22;
		return4x4._23 = float4x4._32;
		return4x4._24 = float4x4._42;

		return4x4._31 = float4x4._13;
		return4x4._32 = float4x4._23;
		return4x4._33 = float4x4._33;
		return4x4._34 = float4x4._43;

		return4x4._44 = 1.0f;

		// Annoying DirectX::XMFLOAT3 -> DirectX::XMVECTOR conversion
		DirectX::XMFLOAT4 axis = DirectX::XMFLOAT4(float4x4._41, float4x4._42, float4x4._43, float4x4._44);

		DirectX::XMVECTOR axis_w = DirectX::XMLoadFloat4(&axis);
		DirectX::XMVECTOR axis_x = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(float4x4._11, float4x4._12, float4x4._13, float4x4._14));
		DirectX::XMVECTOR axis_y = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(float4x4._21, float4x4._22, float4x4._23, float4x4._24));
		DirectX::XMVECTOR axis_z = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(float4x4._31, float4x4._32, float4x4._33, float4x4._34));

		DirectX::XMFLOAT4 dotProduct;
		DirectX::XMStoreFloat4(&dotProduct, DirectX::XMVector4Dot(axis_w, axis_x));
		return4x4._41 = -dotProduct.x;

		DirectX::XMStoreFloat4(&dotProduct, DirectX::XMVector4Dot(axis_w, axis_y));
		return4x4._42 = -dotProduct.x;

		DirectX::XMStoreFloat4(&dotProduct, DirectX::XMVector4Dot(axis_w, axis_z));
		return4x4._43 = -dotProduct.x;
	}

	/*****************************************************************
	* MatrixMultVector():	Math helper function to multiply a vector and
	*						float4x4 matrix to an output vector
	*
	* Ins:					vector
	*						float4x4
	*
	* Outs:					vecOut
	* Returns:				void
	*
	* Mod. Date				05/18/2015
	* Mod. Initials:		JPM
	*****************************************************************/
	__forceinline void MatrixMultVector(DirectX::XMVECTOR& vector, DirectX::XMFLOAT4X4& float4x4, DirectX::XMVECTOR& vecOut)
	{
		/*
			r.x = (m.axis_x.x * v.x) + (m.axis_y.x * v.y) + (m.axis_z.x * v.z) + (m.axis_w.x * 1);
			r.y = (m.axis_x.y * v.x) + (m.axis_y.y * v.y) + (m.axis_z.y * v.z) + (m.axis_w.y * 1);
			r.z = (m.axis_x.z * v.x) + (m.axis_y.z * v.y) + (m.axis_z.z * v.z) + (m.axis_w.z * 1);
			*/

		DirectX::XMFLOAT3 dotProduct;
		DirectX::XMStoreFloat3(&dotProduct, DirectX::XMVector3Dot(vector, DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(float4x4._11, float4x4._21, float4x4._31))));
		float x = dotProduct.x + float4x4._41;

		DirectX::XMStoreFloat3(&dotProduct, DirectX::XMVector3Dot(vector, DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(float4x4._12, float4x4._22, float4x4._32))));
		float y = dotProduct.x + float4x4._42;

		DirectX::XMStoreFloat3(&dotProduct, DirectX::XMVector3Dot(vector, DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(float4x4._13, float4x4._23, float4x4._33))));
		float z = dotProduct.x + float4x4._43;

		vecOut = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(x, y, z)));
	}



	CMesh* m_pMesh;
	std::vector<CMesh::TSkinnedVertex> m_vSkinVerts;
	std::vector<DirectX::XMFLOAT4X4> m_vSkinnedMatrices;
	std::vector<DirectX::XMINT4> m_vJointIndices;
	std::vector<DirectX::XMFLOAT4> m_vWeights;
};

#endif // _CDEFORMER_H_
