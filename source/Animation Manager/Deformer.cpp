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

#include "Deformer.h"

#include "../State Machine/GameplayState.h"
#include "../Asset Manager/AssetManager.h"

CDeformer::CDeformer()
{
}

void CDeformer::LoadVerts()
{
	int num_skin_verts = m_pMesh->GetVertices().size();
	for (int vert = 0; vert < num_skin_verts; vert++)
	{
		int num_influences = m_pMesh->GetInfluences()[vert].size();

		//XMINT4 jointIndices = XMINT4(0, 0, 0, 0);
		int indices[4] = { 0, };
		float weights[4] = { 0 };
		for (int i = 0; i < num_influences; i++)
		{
			memcpy(&indices[i], &m_pMesh->GetInfluences()[vert][i].m_unJointIndex, sizeof(int));
			memcpy(&weights[i], &m_pMesh->GetInfluences()[vert][i].m_fWeight, sizeof(float));
		}
		m_vJointIndices.push_back(DirectX::XMINT4(indices[0], indices[1], indices[2], indices[3]));
		m_vWeights.push_back(DirectX::XMFLOAT4(weights[0], weights[1], weights[2], weights[3]));

		CMesh::TSkinnedVertex vertex;
		vertex.tVertex = m_pMesh->GetVertices()[vert];
		vertex.m_anBone = m_vJointIndices[vert];
		vertex.m_afWeight = m_vWeights[vert];
		m_vSkinVerts.push_back(vertex);

		//DirectX::XMFLOAT4 weights;
		//weights.x = m_pMesh->GetInfluences()[vert][0].m_fWeight;
		//weights.y = m_pMesh->GetInfluences()[vert][1].m_fWeight;
		//weights.z = m_pMesh->GetInfluences()[vert][2].m_fWeight;
		//weights.w = m_pMesh->GetInfluences()[vert][3].m_fWeight;
	}
}

void CDeformer::Process(std::vector<DirectX::XMFLOAT4X4>& vPose)
{
	if (!m_pMesh || !m_pMesh->GetVertices().size())
	{
		return;
	}

	if (!m_vSkinnedMatrices.size()/* > 0*/)
	{
		m_vSkinnedMatrices.clear();
		m_vSkinnedMatrices.resize(m_pMesh->GetJoints().size());
	}

	m_vSkinVerts.resize(m_pMesh->GetVertices().size());

	int numSkinVerts = m_pMesh->GetVertices().size();

	DirectX::XMFLOAT4X4 invBindPose;

	for (size_t jointNum = 0; jointNum < m_pMesh->GetJoints().size(); jointNum++)
	{
		//B inverse
		DirectX::XMFLOAT4X4 worldBindPose = m_pMesh->GetJoints()[jointNum].m_mWorldBindPoseTransform;

		//OrthoNormalInverse(worldBindPose, invBindPose);
		DirectX::XMMATRIX bindPose;
		bindPose = DirectX::XMLoadFloat4x4(&worldBindPose);

		bindPose = DirectX::XMMatrixInverse(NULL, bindPose);

		DirectX::XMMATRIX m1, m2;
		m1 = bindPose;
		m2 = DirectX::XMLoadFloat4x4(&vPose[jointNum]);

		DirectX::XMStoreFloat4x4(&m_vSkinnedMatrices[jointNum], (DirectX::XMMatrixMultiply(m1,m2)));

		// invbindpose * vPose

		////C(B-inverse * current vertex)
		//DirectX::XMVECTOR CBV;
		//MatrixMultVector(invXpos, vPose[m_pMesh->GetInfluences()[], CBV);
	}



	/*DirectX::XMFLOAT3 fCBV;

	CBV *= weight;
	DirectX::XMStoreFloat3(&fCBV, CBV);*/


	/*m_vSkinVerts[vert].x += fCBV.x;
	m_vSkinVerts[vert].y += fCBV.y;
	m_vSkinVerts[vert].z += fCBV.z;*/
}
//}