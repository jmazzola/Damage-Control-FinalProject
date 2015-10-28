/***********************************************
* Filename:			Mesh.h
* Date:      		05/12/2015
* Mod. Date:		05/13/2015
* Mod. Initials:	JM
* Author:    		Justin Mazzola
* Purpose:			This class will be responsible for
					handling the properties representing
					a single mesh (vertices, normals, textures, etc.)
************************************************/

#ifndef _CMESH_H_
#define _CMESH_H_

#include "..\Core\stdafx.h"

class CMesh
{
public:
	CMesh() {};
	~CMesh() {};

	CMesh(const CMesh& rhs)
	{
		m_szName = rhs.m_szName;

		m_vVertices = rhs.m_vVertices;
		m_vSkinnedVerts = rhs.m_vSkinnedVerts;
		m_vIndices = rhs.m_vIndices;
		m_vJoints = rhs.m_vJoints;
		m_vJointInfluences = rhs.m_vJointInfluences;
		m_vTextureNames = rhs.m_vTextureNames;
		m_vTextures = rhs.m_vTextures;
	}

	/***********************************************
	* Author:    		Justin Mazzola
	* Purpose:			A joint inside a mesh
	*					that stores the matrices for
	*					a pose and it's children.
	************************************************/
	struct TJoint
	{
		std::string m_szName;
		unsigned int m_unParent;
		std::vector<unsigned int> m_vunChildren;
		DirectX::XMFLOAT4X4 m_mLocalBindPoseTransform;
		DirectX::XMFLOAT4X4 m_mWorldBindPoseTransform;
	};

	/***********************************************
	* Author:    		Justin Mazzola
	* Purpose:			A joint influence holding
	*					on a weight for that joint
	************************************************/
	struct TJointInfluence
	{
		unsigned int m_unJointIndex;
		float m_fWeight;
	};

	/***********************************************
	* Author:    		Justin Mazzola
	* Purpose:			A unique vertex holding on
	*					the position, normal, and 
	*					Maya control point index info
	************************************************/
	struct TUniqueVertex
	{
		DirectX::XMFLOAT3 m_vPosition;
		DirectX::XMFLOAT2 m_vTexCoord;
		DirectX::XMFLOAT3 m_vNormal;
		DirectX::XMFLOAT4 m_vTangents;
		DirectX::XMFLOAT3 m_vBinormal;
		int m_nControlPointIndex;

		bool operator==(const TUniqueVertex& v) const
		{
			return  m_vPosition.x == v.m_vPosition.x &&
				m_vPosition.y == v.m_vPosition.y &&
				m_vPosition.z == v.m_vPosition.z &&
				m_vNormal.x == v.m_vNormal.x &&
				m_vNormal.y == v.m_vNormal.y &&
				m_vNormal.z == v.m_vNormal.z &&
				m_vTexCoord.x == v.m_vTexCoord.x &&
				m_vTexCoord.y == v.m_vTexCoord.y &&
				m_nControlPointIndex == v.m_nControlPointIndex;
		}
	};
	

	struct TSkinnedVertex
	{
		TUniqueVertex tVertex;
		DirectX::XMINT4 m_anBone;
		DirectX::XMFLOAT4 m_afWeight;
	};

	/*****************************************************************
	* LoadTexture():	Blackbox function to load in textures to apply
	*					to the mesh from an asset.
	*
	* Ins:					szFileName
	*
	* Returns:				void
	*
	* Mod. Date:		    05/13/2015
	* Mod. Initials:	    JM
	*****************************************************************/
	void LoadTexture(const char* szFileName);

	/***************
	*  Accessors
	*
	* Note: These accessors can also be used as mutators due to having a reference.
	***************/
	__forceinline std::string& GetName()
	{
		return m_szName;
	}

	__forceinline std::vector<TUniqueVertex >& GetVertices()
	{
		return m_vVertices;
	}

	__forceinline std::vector<unsigned int >& GetIndices( )
	{
		return m_vIndices;
	}

	__forceinline std::vector<TJoint>& GetJoints( )
	{
		return m_vJoints;
	}

	__forceinline std::vector<std::vector<TJointInfluence>>& GetInfluences( )
	{
		return m_vJointInfluences;
	}

	__forceinline std::vector<std::string>& GetTextureNames( )
	{
		return m_vTextureNames;
	}

	__forceinline std::vector<ID3D11ShaderResourceView*>& GetTextures( )
	{
		return m_vTextures;
	}

private:

	std::string m_szName;

	std::vector<TUniqueVertex> m_vVertices;
	std::vector<TSkinnedVertex> m_vSkinnedVerts;
	std::vector<unsigned int> m_vIndices;
	std::vector<TJoint> m_vJoints;
	std::vector<std::vector<TJointInfluence>> m_vJointInfluences;
	std::vector<std::string> m_vTextureNames;
	std::vector<ID3D11ShaderResourceView*> m_vTextures;
};

#endif //_CMESH_H_
