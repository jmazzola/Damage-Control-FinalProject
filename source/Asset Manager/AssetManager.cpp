/***********************************************
* Filename:			AssetManager.h
* Date:      		05/12/2015
* Mod. Date:		05/14/2015
* Mod. Initials:	JPM
* Author:    		Justin Mazzola, David Goad
* Purpose:			Class to handle/manage assets such
*					as textures, models, scripts.
*					This class also acts as a loader
*					for FBX files.
************************************************/

#include "AssetManager.h"
#include "..\Renderer\Renderer.h"
#include "..\Animation Manager\Deformer.h"

#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include "../Entity Manager/Entity.h"
#include "../Entity Manager/Scenery/Workstation.h"
#include "../Entity Manager/Entities/Subsystems/Engines.h"
#include "../Entity Manager/Entities/Subsystems/Reactor.h"
#include "../Entity Manager/Entities/Subsystems/LifeSupport.h"
#include "../Entity Manager/AI/MonsterSpawner.h"
#include "../State Machine/GameplayState.h"
#include "../Entity Manager/EntityManager.h"
#include "../Entity Manager/Entities/Subsystems/Subsystem.h"
#include "../Entity Manager/Level.h"
#include "../Entity Manager/Scenery/FloorGuide.h"
#include "../Entity Manager/Scenery/RotaryLights.h"

#include "../Math/Collision.h"

#include <algorithm>
#include <fstream>

using namespace MathHelper;

struct Triangle
{
	int index[3];
};

CAssetManager::~CAssetManager( )
{
	for (auto& i : m_mModels)
	{
		if (i.second)
		{
			SafeRelease(i.second->m_pIndexBuffer);
			SafeRelease(i.second->m_pVertexBuffer);
			//SafeRelease(i.second->m_tTex.m_pDiffuseTexture);
			//SafeRelease(i.second->m_tTex.m_pNormalTexture);
			//SafeRelease(i.second->m_tTex.m_pMiscTexture);
		}

		delete i.second;
	}

	m_mModels.clear( );

	for (auto& i : m_mAnimations)
	{
		if (i.second)
		{
			SafeRelease(i.second->m_pModel->m_pIndexBuffer);
			SafeRelease(i.second->m_pModel->m_pVertexBuffer);
			delete (i.second->m_pModel);
			i.second->m_pModel = nullptr;
			//SafeRelease(i.second->m_pModel->m_tTex.m_pNormalTexture);
			//SafeRelease(i.second->m_pModel->m_tTex.m_pMiscTexture);
		}

		delete i.second;
		i.second = nullptr;
	}

	m_mAnimations.clear( );
}

CAssetManager::TModel::TModel(const TModel& rhs)
{
	m_pVertexBuffer = rhs.m_pVertexBuffer;
	m_pIndexBuffer = rhs.m_pIndexBuffer;
	m_unIndexCount = rhs.m_unIndexCount;

	m_cMesh = CMesh(rhs.m_cMesh);

	for (int i = 0; i < _countof(m_tTex.m_pDiffuseTexture); i++)
	{
		m_tTex.m_pDiffuseTexture[i] = rhs.m_tTex.m_pDiffuseTexture[i];
	}
	m_tTex.m_pNormalTexture = rhs.m_tTex.m_pNormalTexture;
	m_tTex.m_pMiscTexture = rhs.m_tTex.m_pMiscTexture;
}

void CAssetManager::UnloadModel(std::string szTag)
{
	//Make sure it exists without adding a new entry to the map
	if (m_mModels.find(szTag) != m_mModels.end())
	{
		//Make sure it's not nullptr
		if (m_mModels[szTag])
		{
			SafeRelease(m_mModels[szTag]->m_pIndexBuffer);
			SafeRelease(m_mModels[szTag]->m_pVertexBuffer);

			delete m_mModels[szTag];
			m_mModels[szTag] = nullptr;
		}
		m_mModels.erase(m_mModels.find(szTag));
	}
}

bool CAssetManager::InitializeSdkObjects(FbxManager*& prFbxManager, FbxScene*& prScene)
{
	prFbxManager = FbxManager::Create( );

	if (!prFbxManager)
	{
		// cout << "[InitializeSdkObjects] Creating the FbxManager FAILED\n";
		FBXSDK_printf("[InitializeSdkObjects] Creating the FbxManager FAILED\n");
		return false;
	}
	else
	{
		// cout << "[InitializeSdkObjects] Creating the FbxManager CREATED\n" << prFbxManager->GetVersion() << "\n";
		//FBXSDK_printf("[InitializeSdkObjects] FbxManager Created: Version: %s\n", prFbxManager->GetVersion( ));
	}

	FbxIOSettings* ios = FbxIOSettings::Create(prFbxManager, IOSROOT);

	if (!ios)
	{
		// cout << "[InitializeSdkObjects] Creating the FbxIOSettings FAILED.\n";
		FBXSDK_printf("[InitializeSdkObjects] Creating the FbxIOSettings FAILED.\n");
		return false;
	}

	prFbxManager->SetIOSettings(ios);

	// Create the FBX Scene
	prScene = FbxScene::Create(prFbxManager, "MyScene");

	if (!prScene)
	{
		// cout << "[InitializeSdkObjects] Creating the FbxScene FAILED.\n";
		FBXSDK_printf("[InitializeSdkObjects] Creating the FbxScene FAILED.\n");
		return false;
	}

	return true;
}

bool CAssetManager::LoadLocalModelFBX(std::string szFileName, std::string szTag)
{
	std::string localFile = PathFindFileNameA(szFileName.c_str( ));
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str( )));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str( )), ".fbxbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".fbxbin"));

	if (!PathFileExistsA(fileName.c_str( )))
		return false;

	// Load the local fbxbin
	CMesh cMesh;
	m_mModels[szTag] = new TModel;
	cMesh.GetName( ) = szTag;

	std::fstream file(fileName, std::ios_base::in | std::fstream::binary);

	if (file.is_open( ) && file.good( ))
	{
		//FBXSDK_printf("Reading %s's model information\n", fileName.c_str( ));

		FbxLocalTime timeStamp;
		file.read((char*)&timeStamp, sizeof(FbxLocalTime));

		// Check for the .fbx following this file
		std::string fbxFile = fileName;
		CustomPathFuncs::RemoveSubstringInString(fbxFile, "bin");

		FbxLocalTime fbxTimeStamp;
		ZeroMemory(&fbxTimeStamp, sizeof(FbxLocalTime));

#ifdef CHECK_FBX
		// Create a FBX Manager
		FbxManager* fbxManager = NULL;
		FbxScene* fbxScene = NULL;

		ImportFBXData(fbxManager, fbxScene, szFileName, &fbxTimeStamp);
		CleanUpFBX(fbxScene, fbxManager);
#endif

		//We found te fbx file, make sure we're up to date
		if (fbxTimeStamp.mYear != 0)
		{
			// Because fuck overloading operators am I right? Probably not.
			if ((fbxTimeStamp.mDay != timeStamp.mDay) || (fbxTimeStamp.mHour != timeStamp.mHour) || (fbxTimeStamp.mSecond != timeStamp.mSecond) ||
				(fbxTimeStamp.mMinute != timeStamp.mMinute) || (fbxTimeStamp.mMonth != timeStamp.mMonth) || (fbxTimeStamp.mYear != timeStamp.mYear))
			{
				FBXSDK_printf("The timestamp for %s didn't match up with the fbx file. Creating a new one.\n", fileName.c_str( ));
				delete m_mModels[szTag];
				m_mModels[szTag] = nullptr;
				return false;
			}
		}
		//We didn't find the fbx file, assume we're up to date
		/*else
		{

		}
		*/
		long offsetForAnim;
		file.read((char*)&offsetForAnim, sizeof(long));

		// Read index count
		file.read((char*)&m_mModels[szTag]->m_unIndexCount, sizeof(unsigned int));

		// Read indices

		cMesh.GetIndices( ).resize(m_mModels[szTag]->m_unIndexCount);

		unsigned int indexCount = cMesh.GetIndices( ).size( );
		//for (unsigned int i = 0; i < indexCount; i++)
		{
			file.read((char*)&cMesh.GetIndices( )[0], indexCount * sizeof(unsigned int));
		}

		// Read vertex count
		unsigned int vertCount = cMesh.GetVertices( ).size( );
		file.read((char*)&vertCount, sizeof(unsigned int));

		cMesh.GetVertices( ).resize(vertCount);

		// Read vertices
		//for (unsigned int i = 0; i < vertCount; i++)
		{
			file.read((char*)&cMesh.GetVertices( )[0], vertCount * sizeof(CMesh::TUniqueVertex));
		}

		// Read number of textures of this particular mesh
		unsigned int numOfTextures = m_mTextureNames[cMesh.GetName( )].size( );
		file.read((char*)&numOfTextures, sizeof(unsigned int));

		m_mTextureNames[cMesh.GetName( )].resize(numOfTextures);

		// Read the texture names/name length
		for (unsigned int i = 0; i < numOfTextures; i++)
		{
			unsigned int nameLen;// = m_mTextureNames[cMesh.GetName()][i].length();
			file.read((char*)&nameLen, sizeof(unsigned int));
			m_mTextureNames[cMesh.GetName( )][i].resize(nameLen);
			file.read(const_cast<char*>(m_mTextureNames[cMesh.GetName( )][i].c_str( )), nameLen);

			//Load in textures from filenames
			CComPtr<ID3D11ShaderResourceView> pSRV = NULL;
			LoadTexture(m_mTextureNames[cMesh.GetName( )][i].c_str( ), &pSRV);

			std::string objectTag = m_mTextureNames[cMesh.GetName( )][i];
			transform(objectTag.begin( ), objectTag.end( ), objectTag.begin( ), toupper);

			if ((objectTag.find("DIFFUSE") != std::string::npos))
			{
				for (int texture_index = 0; texture_index < _countof(m_mModels[szTag]->m_tTex.m_pDiffuseTexture); texture_index++)
				{
					if (m_mModels[szTag]->m_tTex.m_pDiffuseTexture[texture_index] == NULL)
					{
						m_mModels[szTag]->m_tTex.m_pDiffuseTexture[texture_index] = pSRV;
						break;
					}
				}
			}
			else if ((objectTag.find("NORMAL") != std::string::npos))
			{
				m_mModels[szTag]->m_tTex.m_pNormalTexture = pSRV;
			}
			else
			{
				m_mModels[szTag]->m_tTex.m_pMiscTexture = pSRV;
			}
		}

		file.clear( );
		file.close( );
	}

	CreateBuffer(CRenderer::GetReference( ).GetDevice( ), cMesh.GetVertices( ), D3D11_BIND_VERTEX_BUFFER, &m_mModels[szTag]->m_pVertexBuffer);
	CreateBuffer(CRenderer::GetReference( ).GetDevice( ), cMesh.GetIndices( ), D3D11_BIND_INDEX_BUFFER, &m_mModels[szTag]->m_pIndexBuffer);

	m_mModels[szTag]->m_cMesh = cMesh;

	return true;
}

bool CAssetManager::LoadLocalAnimFBX(std::string szFileName, std::string szTag, TModel& tModel)
{
	std::string localFile = PathFindFileNameA(szFileName.c_str( ));
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str( )));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str( )), ".fbxbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".fbxbin"));

	if (!PathFileExistsA(fileName.c_str( )))
		return false;

	// Load the local animation information

	std::fstream file(fileName, std::ios_base::in | std::ios_base::binary);

	if (file.is_open( ) && file.good( ))
	{
		/*file.seekg(0, ios::end);
		file.seekp()*/

		/*file.seekg(0, ios::end);
		auto fuckyou = file.tellg();
		fuckyou -= 4;

		file.seekg(fuckyou);

		char footer[5] = { 0 };
		file.read((char*)&footer, 5);

		if (file.eof())
		{
		file.clear();
		file.close();
		return false;
		}*/

		FbxLocalTime timeStamp;
		file.read((char*)&timeStamp, sizeof(FbxLocalTime));

		long offset;
		file.read((char*)&offset, sizeof(long));

		file.seekg(offset);

		char shit = 'Z';
		file.read(&shit, 1);

		if (file.eof( ))
			return false;

		//FBXSDK_printf("Reading %s's anim information\n", fileName.c_str( ));

		delete m_mModels[szTag];
		m_mModels[szTag] = nullptr;

		m_mAnimations[szTag] = new TAnimation( );
		m_mAnimations[szTag]->m_pModel = new CAssetManager::TModel(tModel);

		CMesh* pMesh = &m_mAnimations[szTag]->m_pModel->m_cMesh;

		pMesh->GetInfluences( ).resize(pMesh->GetVertices( ).size( ));

		for (size_t vert = 0; vert < pMesh->GetVertices( ).size( ); vert++)
		{
			int num_influences = 0;
			file.read((char*)&num_influences, sizeof(int));

			pMesh->GetInfluences( )[vert].resize(num_influences);

			for (int i = 0; i < num_influences; i++)
			{
				file.read((char*)&pMesh->GetInfluences( )[vert][i].m_unJointIndex, sizeof(int));
				file.read((char*)&pMesh->GetInfluences( )[vert][i].m_fWeight, sizeof(float));
			}
		}

		unsigned int numJoints = 0;
		file.read((char*)&numJoints, sizeof(unsigned int));

		pMesh->GetJoints( ).resize(numJoints);

		// Write out joints
		for (size_t i = 0; i < numJoints; i++)
		{
			CMesh::TJoint temp;

			file.read((char*)&temp.m_mWorldBindPoseTransform, sizeof(temp.m_mWorldBindPoseTransform));

			pMesh->GetJoints( )[i] = temp;
		}

		// Read the animation duration
		file.read((char*)&m_mAnimations[szTag]->m_pAnim.m_fDuration, sizeof(float));

		// Read the channel count
		unsigned int numChannels = 0;
		file.read((char*)&numChannels, sizeof(unsigned int));

		m_mAnimations[szTag]->m_pAnim.m_vChannels.resize(numChannels);

		for (size_t channel = 0; channel < numChannels; channel++)
		{
			unsigned int numKeys = 0;
			file.read((char*)&numKeys, sizeof(unsigned int));

			m_mAnimations[szTag]->m_pAnim.m_vChannels[channel].m_vKeys.resize(numKeys);

			for (unsigned int key = 0; key < numKeys; key++)
			{
				file.read((char*)&m_mAnimations[szTag]->m_pAnim.m_vChannels[channel].m_vKeys[key].m_fTime, sizeof(float));
				file.read((char*)&m_mAnimations[szTag]->m_pAnim.m_vChannels[channel].m_vKeys[key].m_Node, sizeof(DirectX::XMFLOAT4X4));

				/*file.read((char*)&m_mAnimations[szTag]->m_pAnim.m_vChannels[channel].m_vKeys[key].m_vQuaternion, sizeof(DirectX::XMFLOAT4));
				file.read((char*)&m_mAnimations[szTag]->m_pAnim.m_vChannels[channel].m_vKeys[key].m_vPosition, sizeof(DirectX::XMFLOAT3));*/
			}
		}

		// Read the keyframe count
		unsigned int keyframeCount = 0;
		file.read((char*)&keyframeCount, sizeof(unsigned int));

		m_mAnimations[szTag]->m_pAnim.m_vKeyframes.resize(keyframeCount);

		for (size_t i = 0; i < keyframeCount; i++)
		{
			file.read((char*)&m_mAnimations[szTag]->m_pAnim.m_vKeyframes[i].m_fTime, sizeof(float));

			unsigned int numNodes = 0;
			file.read((char*)&numNodes, sizeof(unsigned int));
			m_mAnimations[szTag]->m_pAnim.m_vKeyframes[i].m_vNodes.resize(numNodes);

			//for (unsigned int node = 0; node < numNodes; node++)
			{
				file.read((char*)&m_mAnimations[szTag]->m_pAnim.m_vKeyframes[i].m_vNodes[0], numNodes * sizeof(DirectX::XMFLOAT4X4));
			}
		}
	}

	SafeRelease(m_mAnimations[szTag]->m_pModel->m_pVertexBuffer);
	//SafeRelease(m_mModels[szTag]->m_pVertexBuffer);

	return true;
}

void CAssetManager::CleanUpFBX(FbxScene* pScene, FbxManager* pManager)
{
	if (pScene)
	{
		pScene->Destroy(true);
		pScene = nullptr;
	}
	if (pManager)
	{
		pManager->GetIOSettings( )->Destroy(true);
		pManager->Destroy( );
		pManager = nullptr;
	}
}

bool CAssetManager::SaveLocalModelFBX(std::string szFileName, std::string szTag, CMesh& cMesh)
{
	std::string localFile = PathFindFileNameA(szFileName.c_str( ));
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str( )));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str( )), ".fbxbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".fbxbin"));

	// Create the local fbxbin to allow loading times to be faster
	std::fstream file(fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	if (file.is_open( ) && file.good( ))
	{
		if (!m_mModels[szTag])
		{
			file.close( );
			return true;
		}

		//FBXSDK_printf("Creating %s's model info\n", fileName.c_str( ));

		// Create a FBX Manager
		FbxManager* fbxManager = NULL;
		FbxScene* fbxScene = NULL;

		if (!ImportFBXData(fbxManager, fbxScene, szFileName))
		{
			return false;
		}

		FbxImporter* fbxImporter = NULL;

		fbxImporter = FbxImporter::Create(fbxManager, "");

		if (!fbxImporter)
		{
			// cout << "[Load] Creating the FbxImporter has FAILED.\n";
			FBXSDK_printf("[Load] Creating the FbxImporter has FAILED.\n");

			fbxImporter->Destroy( );
			fbxImporter = nullptr;

			CleanUpFBX(fbxScene, fbxManager);

			return false;
		}

		if (!fbxImporter->Initialize(szFileName.c_str( ), -1, fbxManager->GetIOSettings( )))
		{
			FbxStatus status = fbxImporter->GetStatus( );

			// cout << "[Load] Initializing the FBXImporter has FAILED.\n";
			FBXSDK_printf("[Load] Initializing the FBXImporter has FAILED.\n");

			fbxImporter->Destroy( );
			fbxImporter = nullptr;

			CleanUpFBX(fbxScene, fbxManager);

			return false;
		}

		// Grab the timestamp
		FbxIOFileHeaderInfo* headerInfo = fbxImporter->GetFileHeaderInfo( );
		file.write((char*)&headerInfo->mCreationTimeStamp, sizeof(FbxLocalTime));

		CleanUpFBX(fbxScene, fbxManager);

		char* temp = "TEMP";
		file.write(temp, 4);

		// Write index count
		file.write((char*)&m_mModels[szTag]->m_unIndexCount, sizeof(unsigned int));

		// Write indices
		unsigned int indexCount = cMesh.GetIndices( ).size( );
		for (unsigned int i = 0; i < indexCount; i++)
		{
			file.write((char*)&cMesh.GetIndices( )[i], sizeof(unsigned int));
		}

		// Write vertex count
		unsigned int vertCount = cMesh.GetVertices( ).size( );
		file.write((char*)&vertCount, sizeof(unsigned int));

		// Write vertices
		for (unsigned int i = 0; i < vertCount; i++)
		{
			file.write((char*)&cMesh.GetVertices( )[i], sizeof(CMesh::TUniqueVertex));
		}

		// Write number of textures of this particular mesh
		unsigned int numOfTextures = m_mTextureNames[cMesh.GetName( )].size( );
		file.write((char*)&numOfTextures, sizeof(unsigned int));

		// Write the texture names/name length
		for (unsigned int i = 0; i < numOfTextures; i++)
		{
			unsigned int nameLen = m_mTextureNames[cMesh.GetName( )][i].length( );
			file.write((char*)&nameLen, sizeof(unsigned int));
			file.write(m_mTextureNames[cMesh.GetName( )][i].c_str( ), nameLen);
		}

		/*char* footer = "ANIM";
		file.write(footer, strlen(footer));*/

		// Get our current position
		long positionforAnim = (long)file.tellp( );

		file.seekg(0x1C, std::ios::beg);
		file.write((char*)&positionforAnim, sizeof(long));
		file.close( );
	}
	else
		return false;

	return true;
}

bool CAssetManager::SaveLocalAnimFBX(std::string szFileName, std::string szTag, JointList& uniqueJoints)
{
	std::string localFile = PathFindFileNameA(szFileName.c_str( ));
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str( )));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str( )), ".fbxbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".fbxbin"));

	std::fstream file(fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::app);

	// Travel to the end of the file

	if (file.is_open( ) && file.good( ))
	{
		file.seekp(0, file.end);
		char f = 'F';
		file.write(&f, 1);

		//FBXSDK_printf("Creating %s's animation info\n", fileName.c_str( ));

		// Write out JointInfluences
		CMesh* pMesh = &m_mAnimations[szTag]->m_pModel->m_cMesh;
		for (size_t vert = 0; vert < pMesh->GetVertices( ).size( ); vert++)
		{
			int num_influences = pMesh->GetInfluences( )[vert].size( );
			file.write((char*)&num_influences, sizeof(int));

			for (int i = 0; i < num_influences; i++)
			{
				file.write((char*)&pMesh->GetInfluences( )[vert][i].m_unJointIndex, sizeof(int));
				file.write((char*)&pMesh->GetInfluences( )[vert][i].m_fWeight, sizeof(float));
			}
		}

		// Write out number of joints
		unsigned int numJoints = pMesh->GetJoints( ).size( );
		file.write((char*)&numJoints, sizeof(unsigned int));

		// Write out joints
		for (size_t i = 0; i < numJoints; i++)
		{
			CMesh::TJoint temp = pMesh->GetJoints( )[i];

			/*unsigned int nameSize = pMesh->GetJoints()[i].m_szName.length();
			file.write((char*)&nameSize, pMesh->GetJoints()[i].m_szName.length());
			file.write((char*)pMesh->GetJoints()[i].m_szName.c_str(), nameSize);*/

			/*file.write((char*)&temp.m_unParent, sizeof(temp.m_unParent));

			file.write((char*)&temp.m_vunChildren.size(), sizeof(size_t));

			for (size_t child = 0; j < pMesh->GetJoints()[i].m_vunChildren.size(); j++)
			{
			file.write((char*)&pMesh->GetJoints()[i].m_vunChildren[child], sizeof(unsigned int));
			}*/

			file.write((char*)&temp.m_mWorldBindPoseTransform, sizeof(temp.m_mWorldBindPoseTransform));
		}

		// Write out the animation's duration
		float duration = m_mAnimations[szTag]->m_pAnim.GetDuration( );
		file.write((char*)&duration, sizeof(float));

		// Write out the number of channels (they should be the same as the unique joints, but for now, let's have this here.)
		unsigned int numChannels = m_mAnimations[szTag]->m_pAnim.GetChannels( ).size( );
		file.write((char*)&numChannels, sizeof(unsigned int));

		for (size_t channel = 0; channel < numChannels; channel++)
		{
			CAnimation<DirectX::XMFLOAT4X4>::TChannel temp = m_mAnimations[szTag]->m_pAnim.GetChannels( )[channel];

			unsigned int numKeys = temp.m_vKeys.size( );
			file.write((char*)&numKeys, sizeof(unsigned int));

			for (unsigned int key = 0; key < numKeys; key++)
			{
				file.write((char*)&temp.m_vKeys[key].m_fTime, sizeof(float));

				file.write((char*)&temp.m_vKeys[key].m_Node, sizeof(DirectX::XMFLOAT4X4));

				/*file.write((char*)&temp.m_vKeys[key].m_vQuaternion, sizeof(DirectX::XMFLOAT4));
				file.write((char*)&temp.m_vKeys[key].m_vPosition, sizeof(DirectX::XMFLOAT3));*/
			}
		}

		// Write out the number of keyframes
		unsigned int numKeyframes = m_mAnimations[szTag]->m_pAnim.GetKeyFrames( ).size( );
		file.write((char*)&numKeyframes, sizeof(unsigned int));

		for (size_t i = 0; i < numKeyframes; i++)
		{
			CAnimation<DirectX::XMFLOAT4X4>::TKeyFrame keyframe = m_mAnimations[szTag]->m_pAnim.GetKeyFrames( )[i];
			file.write((char*)&keyframe.m_fTime, sizeof(float));

			unsigned int numNodes = keyframe.m_vNodes.size( );
			file.write((char*)&numNodes, sizeof(unsigned int));

			for (unsigned int node = 0; node < numNodes; node++)
			{
				file.write((char*)&keyframe.m_vNodes[node], sizeof(DirectX::XMFLOAT4X4));
			}
		}
	}

	return true;
}

#pragma region LoadFBX() Helpers

bool CAssetManager::ImportFBXData(FbxManager*& prFbxManager, FbxScene*& prScene, std::string szFileName, FbxLocalTime* pTimeStamp)
{
	if (!PathFileExistsA(szFileName.c_str( )))
	{
		return false;
	}

#pragma region Prepare the FBXSDK
	if (!InitializeSdkObjects(prFbxManager, prScene))
	{
		// cout << "[Load] Initializing the SDK Objects has FAILED.\n";
		FBXSDK_printf("[Load] Intializing the SDK Objects has FAILED.\n");

		prScene->Destroy(true);
		prScene = nullptr;
		prFbxManager->GetIOSettings( )->Destroy(true);
		prFbxManager->Destroy( );
		prFbxManager = nullptr;

		return false;
	}
#pragma endregion

#pragma region Create an FBXImporter
	FbxImporter* fbxImporter = NULL;

	fbxImporter = FbxImporter::Create(prFbxManager, "");

	if (!fbxImporter)
	{
		// cout << "[Load] Creating the FbxImporter has FAILED.\n";
		FBXSDK_printf("[Load] Creating the FbxImporter has FAILED.\n");

		fbxImporter->Destroy( );
		fbxImporter = nullptr;

		prScene->Destroy(true);
		prScene = nullptr;
		prFbxManager->GetIOSettings( )->Destroy(true);
		prFbxManager->Destroy( );
		prFbxManager = nullptr;

		return false;
	}
#pragma endregion

#pragma region Initialize the FBXImporter
	if (!fbxImporter->Initialize(szFileName.c_str( ), -1, prFbxManager->GetIOSettings( )))
	{
		FbxStatus status = fbxImporter->GetStatus( );

		DebugPrint(
			std::string(std::string("[Load] Initializing the FBXImporter has FAILED.\n") +
			std::string(".....") +
			std::string(status.GetErrorString( )) +
			": " + szFileName + "\n" + "\tCheck the name?\n").c_str( ), ConsoleColor::Yellow);

		fbxImporter->Destroy( );
		fbxImporter = nullptr;

		prScene->Destroy(true);
		prScene = nullptr;
		prFbxManager->GetIOSettings( )->Destroy(true);
		prFbxManager->Destroy( );
		prFbxManager = nullptr;

		return false;
	}
#pragma endregion

#pragma region Load the contents from the file
	if (!fbxImporter->Import(prScene))
	{
		// cout << "[Load] Importing the FBXScene has FAILED.\n";
		FBXSDK_printf("[Load] Importing the FBXScene has FAILED.\n");

		fbxImporter->Destroy( );
		fbxImporter = nullptr;

		prScene->Destroy(true);
		prScene = nullptr;
		prFbxManager->GetIOSettings( )->Destroy(true);
		prFbxManager->Destroy( );
		prFbxManager = nullptr;

		return false;
	}
#pragma endregion
	//Get the time stamp if requested
	if (pTimeStamp)
	{
		FbxIOFileHeaderInfo* pHeader = fbxImporter->GetFileHeaderInfo( );
		*pTimeStamp = pHeader->mCreationTimeStamp;
	}
	// Destroy the importer because we don't need it anymore
	fbxImporter->Destroy( );

	FbxNode* rootNode = prScene->GetRootNode( );
	if (!rootNode)
	{
		// cout << "[Load] The FbxScene's root node is NULL!\n";
		FBXSDK_printf("[Load] The FbxScene's root node is NULL!\n");

		fbxImporter->Destroy( );
		fbxImporter = nullptr;

		prScene->Destroy(true);
		prScene = nullptr;
		prFbxManager->GetIOSettings( )->Destroy(true);
		prFbxManager->Destroy( );
		prFbxManager = nullptr;

		return false;
	}

	return true;
}

void CAssetManager::StoreModel(CMesh* pMesh, std::string szFileName, std::string szTag)
{
	m_mModels[szTag]->m_unIndexCount = pMesh->GetIndices( ).size( );
	m_mModels[szTag]->m_cMesh = (*pMesh);

	CreateBuffer(CRenderer::GetReference( ).GetDevice( ), pMesh->GetVertices( ), D3D11_BIND_VERTEX_BUFFER, &m_mModels[szTag]->m_pVertexBuffer);
	CreateBuffer(CRenderer::GetReference( ).GetDevice( ), pMesh->GetIndices( ), D3D11_BIND_INDEX_BUFFER, &m_mModels[szTag]->m_pIndexBuffer);

	// Create the local .fbxbin to make loading times supah fast.
	if (szFileName != "")
	{
		SaveLocalModelFBX(szFileName, szTag, *pMesh);
	}
}

bool CAssetManager::StoreAnimation(CMesh& cMesh, JointList& uniqueJoints, FbxScene* pScene, std::string szFileName, std::string szTag)
{
	//if (m_mAnimations[szTag] != nullptr)
	//{
	//	// We're an animation
	//	delete m_mModels[szTag];
	//	m_mModels[szTag] = nullptr;

	//	return false;
	//}

	m_mAnimations[szTag] = new TAnimation( );
	m_mAnimations[szTag]->m_pModel = new TModel;

	for (int i = 0; i < _countof(m_mModels[szTag]->m_tTex.m_pDiffuseTexture); i++)
	{
		m_mAnimations[szTag]->m_pModel->m_tTex.m_pDiffuseTexture[i] = m_mModels[szTag]->m_tTex.m_pDiffuseTexture[i];
	}

	m_mAnimations[szTag]->m_pModel->m_cMesh = cMesh;

	delete m_mModels[szTag];
	m_mModels[szTag] = nullptr;

	// Get the animation stacks
	int numOfAnimStacks = pScene->GetSrcObjectCount<FbxAnimStack>( );

	FbxAnimStack* animStack = NULL;
	for (int animStackNum = 0; animStackNum < numOfAnimStacks; animStackNum++)
	{
		// Get the current animstack
		animStack = pScene->GetSrcObject<FbxAnimStack>(animStackNum);

		// Grab the name
		FbxString animName = animStack->GetName( );
		std::string s = animName;
		m_mAnimations[szTag]->m_pAnim.m_szName = s;

		// Get the animation layers
		int numOfAnimLayers = pScene->GetSrcObjectCount<FbxAnimLayer>( );

		FbxAnimLayer* animLayer = NULL;
		FbxDocument* pDoc = NULL;

		for (int layer = 0; layer < numOfAnimLayers; layer++)
		{
			animLayer = animStack->GetMember<FbxAnimLayer>(layer);
			pDoc = static_cast<FbxDocument*>(pScene);

			LoadAnim(animLayer, m_mAnimations[szTag]->m_pAnim, m_mModels[szTag], uniqueJoints);

			/*
			FbxArray<FbxString*> takeArray;

			if (pDoc)
			pDoc->FillAnimStackNameArray(takeArray);

			for (int take = 0; take < takeArray.GetCount(); take++)
			{
			FbxString* takeName = takeArray.GetAt(take);

			pScene->SetCurrentAnimationStack(animStack);

			FbxTakeInfo* curTakeInfo = pScene->GetTakeInfo(FbxString(takeName->Buffer()));

			FbxTime start, stop;

			if (curTakeInfo)
			{
			start = curTakeInfo->mLocalTimeSpan.GetStart();
			stop = curTakeInfo->mLocalTimeSpan.GetStop();
			}
			else
			{
			FbxTimeSpan timeSpan;
			pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timeSpan);

			start = timeSpan.GetStart();
			stop = timeSpan.GetStop();
			}

			//unsigned int numFrames = (unsigned int)stop.GetFrameCount(FbxTime::eFrames24) - (unsigned int)start.GetFrameCount(FbxTime::eFrames24) + 1;
			//m_mAnimations[szTag]->m_pAnim.m_fDuration = float((stop - start).GetSecondDouble());

			takeName->Clear();
			} */

			// Load duration information from FBX File
			float duration = -1.0f;
			CAssetManager::TAnimation* pAnim = m_mAnimations[szTag];
			for (size_t channel = 0; channel < pAnim->m_pAnim.m_vChannels.size( ); channel++)
			{
				for (size_t key = 0; key < pAnim->m_pAnim.m_vChannels[channel].m_vKeys.size( ); key++)
				{
					if (pAnim->m_pAnim.m_vChannels[channel].m_vKeys[key].m_fTime > duration)
					{
						duration = pAnim->m_pAnim.m_vChannels[channel].m_vKeys[key].m_fTime;
					}
				}
			}

			pAnim->m_pAnim.m_fDuration = duration;

			animLayer->Destroy(true);

			/*for (int i = 0; i < takeArray.GetCount(); i++)
			{
			FbxFree(takeArray[i]);
			}

			takeArray.Clear();*/
		}

		//since above you always say animStack = somethingelse, you overwrite the pointer
		//thus getting ride of like everything [just in case anything new is created]
		animStack->Destroy( );
	}

	//m_mAnimations[szTag]->m_pDeformer.SetMesh(&m_mAnimations[szTag]->m_pModel->m_cMesh);

	m_mAnimations[szTag]->m_pModel->m_unIndexCount = cMesh.GetIndices( ).size( );

	//CreateBuffer(CRenderer::GetReference().GetDevice(), m_mAnimations[szTag]->m_pDeformer.GetSkinVerts(),
	//			 D3D11_BIND_VERTEX_BUFFER,
	//			 &m_mAnimations[szTag]->m_pModel->m_pVertexBuffer);
	CreateBuffer(CRenderer::GetReference( ).GetDevice( ), cMesh.GetIndices( ), D3D11_BIND_INDEX_BUFFER, &m_mAnimations[szTag]->m_pModel->m_pIndexBuffer);

	SaveLocalAnimFBX(szFileName, szTag, uniqueJoints);

	return true;
}

#pragma endregion

bool CAssetManager::LoadFBX(std::string szFileName, std::string szTag)
{
	if (m_mModels.find(szTag) != m_mModels.end() || m_mModels[szTag] != nullptr)
	{
		return false;
	}

	// Check if we already have a local .fbxbin file, if we do, load it and GET OUUUT.
	if (LoadLocalModelFBX(szFileName, szTag))
	{
		//this pointer is LOCAL
		CAssetManager::TModel model = CAssetManager::TModel(*m_mModels[szTag]);

		if (LoadLocalAnimFBX(szFileName, szTag, model))
		{
			delete m_mModels[szTag];
			m_mModels[szTag] = nullptr;

			return true;
		}

		return false;
	}

	// Create a FBX Manager
	FbxManager* fbxManager = NULL;
	FbxScene* fbxScene = NULL;

	if (!ImportFBXData(fbxManager, fbxScene, szFileName))
	{
		return false;
	}

	// Grab the root node of the scene
	FbxNode* rootNode = fbxScene->GetRootNode( );

	// List of unique joints
	JointList uniqueJoints;

	CMesh cMesh;
	m_mModels[szTag] = new TModel;
	cMesh.GetName( ) = szTag;

	// Traverse the scene
	if (!ProcessNode(rootNode, cMesh, m_mModels[szTag], uniqueJoints))
	{
		// cout << "[Load] Processing the root node and/or it's children FAILED.\n";
		FBXSDK_printf("[Load] Processing the root node and/or it's children FAILED.\n");

		CleanUpFBX(fbxScene, fbxManager);

		return false;
	}

	StoreModel(&cMesh, szFileName, szTag);

	if (uniqueJoints.size( ) > 0)
	{
		StoreAnimation(cMesh, uniqueJoints, fbxScene, szFileName, szTag);
	}

	CleanUpFBX(fbxScene, fbxManager);

	return true;
}

bool CAssetManager::LoadMesh(FbxMesh* pMesh, CMesh& cMesh)
{

	int polygonCount = pMesh->GetPolygonCount( );

	FbxVector4* controlPoints = pMesh->GetControlPoints( );

	if (!controlPoints)
	{
		// cout << "[LoadMesh] The current mesh doesn't have any control points/vertices. What did you export?\n";
		FBXSDK_printf("[LoadMesh] The current mesh doesn't have any control points/vertices. What did you export?\n");
		return false;
	}

	int vertexID = 0;

	for (int polyCounter = 0; polyCounter < polygonCount; ++polyCounter)
	{
		int polyVertTotal = pMesh->GetPolygonSize(polyCounter);

		if (polyVertTotal != 3)
		{
			// cout << "[LoadMesh] The current mesh is NOT triangulated. Please triangulate for better results.\n";
			FBXSDK_printf("[LoadMesh] The current mesh is NOT triangulated. Please triangulate for better results.\n");
		}

		for (int polyVertCounter = 0; polyVertCounter < polyVertTotal; ++polyVertCounter)
		{
			CMesh::TUniqueVertex uniqueVert;

			int cpIndex = pMesh->GetPolygonVertex(polyCounter, polyVertCounter);

			// Get the control point index & position
			uniqueVert.m_nControlPointIndex = cpIndex;
			uniqueVert.m_vPosition.x = (float)controlPoints[cpIndex].mData[0];
			uniqueVert.m_vPosition.y = (float)controlPoints[cpIndex].mData[1];
			uniqueVert.m_vPosition.z = (float)controlPoints[cpIndex].mData[2] * -1.0f;

			// Get the UVs
			int elementUVCount = pMesh->GetElementUVCount( );
			for (int uvCounter = 0; uvCounter < elementUVCount; ++uvCounter)
			{
				FbxGeometryElementUV* geoUV = pMesh->GetElementUV(uvCounter);

				FbxLayerElement::EMappingMode mapMode = geoUV->GetMappingMode( );
				FbxLayerElement::EReferenceMode refMode = geoUV->GetReferenceMode( );

				int directIndex = -1;

				if (mapMode == FbxGeometryElement::eByControlPoint)
				{
					if (refMode == FbxGeometryElement::eDirect)
					{
						directIndex = cpIndex;
					}
					else if (refMode == FbxGeometryElement::eIndexToDirect)
					{
						directIndex = geoUV->GetIndexArray( ).GetAt(cpIndex);
					}
				}
				else if (mapMode == FbxGeometryElement::eByPolygonVertex)
				{
					if (refMode == FbxGeometryElement::eDirect || refMode == FbxGeometryElement::eIndexToDirect)
					{
						directIndex = pMesh->GetTextureUVIndex(polyCounter, polyVertCounter);
					}
				}

				if (directIndex != -1)
				{
					FbxVector2 uv = geoUV->GetDirectArray( ).GetAt(directIndex);

					uniqueVert.m_vTexCoord.x = (float)uv.mData[0];
					uniqueVert.m_vTexCoord.y = 1.0f - (float)uv.mData[1];
				}
			}

			// Get the normals
			int elemNormCount = pMesh->GetElementNormalCount( );
			for (int normCounter = 0; normCounter < elemNormCount; ++normCounter)
			{
				FbxGeometryElementNormal* geoNorm = pMesh->GetElementNormal(normCounter);

				FbxLayerElement::EMappingMode mapMode = geoNorm->GetMappingMode( );
				FbxLayerElement::EReferenceMode refMode = geoNorm->GetReferenceMode( );

				int directIndex = -1;

				if (mapMode == FbxGeometryElement::eByControlPoint)
				{
					if (refMode == FbxGeometryElement::eDirect)
					{
						FbxVector4 norm = geoNorm->GetDirectArray( ).GetAt(cpIndex);

						uniqueVert.m_vNormal.x = (float)norm[0];
						uniqueVert.m_vNormal.y = (float)norm[1];
						uniqueVert.m_vNormal.z = (float)norm[2] * -1.0f;
					}
					else if (refMode == FbxGeometryElement::eIndexToDirect)
					{
						int id = geoNorm->GetIndexArray( ).GetAt(cpIndex);
						FbxVector4 norm = geoNorm->GetDirectArray( ).GetAt(id);

						uniqueVert.m_vNormal.x = (float)norm[0];
						uniqueVert.m_vNormal.y = (float)norm[1];
						uniqueVert.m_vNormal.z = (float)norm[2] * -1.0f;
					}
				}
				else if (mapMode == FbxGeometryElement::eByPolygonVertex)
				{
					if (refMode == FbxGeometryElement::eDirect)
					{
						FbxVector4 norm = geoNorm->GetDirectArray( ).GetAt(vertexID);

						uniqueVert.m_vNormal.x = (float)norm[0];
						uniqueVert.m_vNormal.y = (float)norm[1];
						uniqueVert.m_vNormal.z = (float)norm[2] * -1.0f;
					}
					else if (refMode == FbxGeometryElement::eIndexToDirect)
					{
						int id = geoNorm->GetIndexArray( ).GetAt(vertexID);
						FbxVector4 norm = geoNorm->GetDirectArray( ).GetAt(id);

						uniqueVert.m_vNormal.x = (float)norm[0];
						uniqueVert.m_vNormal.y = (float)norm[1];
						uniqueVert.m_vNormal.z = (float)norm[2] * -1.0f;
					}
				}
			}

			// Get the binormals/bitangents
			int elemBinormalCount = pMesh->GetElementBinormalCount( );

			for (int binormCounter = 0; binormCounter < elemBinormalCount; binormCounter++)
			{
				FbxGeometryElementBinormal* geoBinormal = pMesh->GetElementBinormal(binormCounter);

				FbxLayerElement::EMappingMode mapMode = geoBinormal->GetMappingMode( );
				FbxLayerElement::EReferenceMode refMode = geoBinormal->GetReferenceMode( );

				int directIndex = -1;

				if (mapMode == FbxGeometryElement::eByControlPoint)
				{
					if (refMode == FbxGeometryElement::eDirect)
					{
						FbxVector4 binormal = geoBinormal->GetDirectArray( ).GetAt(cpIndex);

						uniqueVert.m_vBinormal.x = (float)binormal[0];
						uniqueVert.m_vBinormal.y = (float)binormal[1];
						uniqueVert.m_vBinormal.z = (float)binormal[2] * -1.0f;
					}
					else if (refMode == FbxGeometryElement::eIndexToDirect)
					{
						int id = geoBinormal->GetIndexArray( ).GetAt(cpIndex);
						FbxVector4 binormal = geoBinormal->GetDirectArray( ).GetAt(id);

						uniqueVert.m_vBinormal.x = (float)binormal[0];
						uniqueVert.m_vBinormal.y = (float)binormal[1];
						uniqueVert.m_vBinormal.z = (float)binormal[2] * -1.0f;
					}
				}
				else if (mapMode == FbxGeometryElement::eByPolygonVertex)
				{
					if (refMode == FbxGeometryElement::eDirect)
					{
						FbxVector4 binormal = geoBinormal->GetDirectArray( ).GetAt(vertexID);

						uniqueVert.m_vBinormal.x = (float)binormal[0];
						uniqueVert.m_vBinormal.y = (float)binormal[1];
						uniqueVert.m_vBinormal.z = (float)binormal[2] * -1.0f;
					}
					else if (refMode == FbxGeometryElement::eIndexToDirect)
					{
						int id = geoBinormal->GetIndexArray( ).GetAt(vertexID);
						FbxVector4 binormal = geoBinormal->GetDirectArray( ).GetAt(id);

						uniqueVert.m_vBinormal.x = (float)binormal[0];
						uniqueVert.m_vBinormal.y = (float)binormal[1];
						uniqueVert.m_vBinormal.z = (float)binormal[2] * -1.0f;
					}
				}
			}

			// Get the tangents.
			int elemTangentCount = pMesh->GetElementTangentCount( );

			for (int tangCounter = 0; tangCounter < elemTangentCount; tangCounter++)
			{
				FbxGeometryElementTangent* geoTang = pMesh->GetElementTangent(tangCounter);

				FbxLayerElement::EMappingMode mapMode = geoTang->GetMappingMode( );
				FbxLayerElement::EReferenceMode refMode = geoTang->GetReferenceMode( );

				int directIndex = -1;

				if (mapMode == FbxGeometryElement::eByControlPoint)
				{
					if (refMode == FbxGeometryElement::eDirect)
					{
						FbxVector4 tang = geoTang->GetDirectArray( ).GetAt(cpIndex);

						uniqueVert.m_vTangents.x = (float)tang[0];
						uniqueVert.m_vTangents.y = (float)tang[1];
						uniqueVert.m_vTangents.z = (float)tang[2] * -1.0f;
						uniqueVert.m_vTangents.w = (float)tang[3];
					}
					else if (refMode == FbxGeometryElement::eIndexToDirect)
					{
						int id = geoTang->GetIndexArray( ).GetAt(cpIndex);
						FbxVector4 tang = geoTang->GetDirectArray( ).GetAt(id);

						uniqueVert.m_vTangents.x = (float)tang[0];
						uniqueVert.m_vTangents.y = (float)tang[1];
						uniqueVert.m_vTangents.z = (float)tang[2] * -1.0f;
						uniqueVert.m_vTangents.w = (float)tang[3];
					}
				}
				else if (mapMode == FbxGeometryElement::eByPolygonVertex)
				{
					if (refMode == FbxGeometryElement::eDirect)
					{
						FbxVector4 tang = geoTang->GetDirectArray( ).GetAt(vertexID);

						uniqueVert.m_vTangents.x = (float)tang[0];
						uniqueVert.m_vTangents.y = (float)tang[1];
						uniqueVert.m_vTangents.z = (float)tang[2] * -1.0f;
						uniqueVert.m_vTangents.w = (float)tang[3];
					}
					else if (refMode == FbxGeometryElement::eIndexToDirect)
					{
						int id = geoTang->GetIndexArray( ).GetAt(vertexID);
						FbxVector4 tang = geoTang->GetDirectArray( ).GetAt(id);

						uniqueVert.m_vTangents.x = (float)tang[0];
						uniqueVert.m_vTangents.y = (float)tang[1];
						uniqueVert.m_vTangents.z = (float)tang[2] * -1.0f;
						uniqueVert.m_vTangents.w = (float)tang[3];
					}
				}
			}

			std::vector<CMesh::TUniqueVertex>& uniqueVerts = cMesh.GetVertices( );

			size_t size = uniqueVerts.size( );
			size_t i;
			for (i = 0; i < size; ++i)
			{
				if (uniqueVert == uniqueVerts[i])
				{
					break;
				}
			}

			if (i == size)
			{
				uniqueVerts.push_back(uniqueVert);
			}

			cMesh.GetIndices( ).push_back(i);

			++vertexID;
		}
	}

//auto GetLayerTangent = [&] (FbxLayer * _layer, std::vector<CMesh::TUniqueVertex> &_verts) -> void
	//{
	//	FbxLayerElementTangent* pLayerTangents = _layer->GetTangents( );
	//	if (nullptr == pLayerTangents)
	//	{
	//		OutputDebugStringA("No Tangents were exported in this file\n");
	//		return;
	//	}
	//
	//	int maxPolys = pMesh->GetPolygonCount( );
	//	for (int polyIndex = 0; polyIndex < maxPolys; ++polyIndex)
	//	{
	//		int maxVerts = pMesh->GetPolygonSize(polyIndex);
	//
	//		for (int vertIndex = 0; vertIndex < maxVerts; ++vertIndex)
	//		{
	//			int vertID = polyIndex * 3 + vertIndex;
	//
	//			FbxLayerElementArrayTemplate<FbxVector4> &tanArray = pLayerTangents->GetDirectArray( );
	//			FbxVector4 tan = tanArray.GetAt(_verts.size( ));
	//			_verts[vertID].m_vTangents.x = static_cast<float>(tan[0]);
	//			_verts[vertID].m_vTangents.y = static_cast<float>(tan[1]);
	//			_verts[vertID].m_vTangents.z = -static_cast<float>(tan[2]);
	//		}
	//	}
	//};
	//
	//int maxLayers = pMesh->GetLayerCount( );
	//for (int layerIndex = 0; layerIndex < maxLayers; ++layerIndex)
	//{
	//	//indexes.push_back(mtIndicies);
	//	//verts.push_back(mtVerts);
	//	FbxLayer * pLayer = pMesh->GetLayer(layerIndex);
	//	if (pLayer == NULL)
	//		continue;
	//	//GetVertFromLayer(pLayer, indexes, verts);
	//	GetLayerTangent(pLayer, cMesh.GetVertices( ));
	//}

	using namespace DirectX;

	auto CalculateTangentArray = [&] (long vertexCount, long triangleCount, const Triangle *triangle, CMesh::TUniqueVertex* verts)
	{
		DirectX::XMFLOAT3 *tan1 = new DirectX::XMFLOAT3[vertexCount * 2];
		DirectX::XMFLOAT3 *tan2 = tan1 + vertexCount;
		ZeroMemory(tan1, vertexCount * sizeof(DirectX::XMFLOAT3) * 2);

		for (long a = 0; a < triangleCount; a++)
		{
			long i1 = triangle->index[0];
			long i2 = triangle->index[1];
			long i3 = triangle->index[2];

			const DirectX::XMFLOAT3& v1 = (&verts[i1])->m_vPosition;
			const DirectX::XMFLOAT3& v2 = (&verts[i2])->m_vPosition;
			const DirectX::XMFLOAT3& v3 = (&verts[i3])->m_vPosition;

			const DirectX::XMFLOAT2& w1 = (&verts[i1])->m_vTexCoord;
			const DirectX::XMFLOAT2& w2 = (&verts[i2])->m_vTexCoord;
			const DirectX::XMFLOAT2& w3 = (&verts[i3])->m_vTexCoord;

			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;

			float s1 = w2.x - w1.x;
			float s2 = w3.x - w1.x;
			float t1 = w2.y - w1.y;
			float t2 = w3.y - w1.y;

			float r = 1.0F / (s1 * t2 - s2 * t1);
			DirectX::XMFLOAT3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
								   (t2 * z1 - t1 * z2) * r);
			DirectX::XMFLOAT3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
								   (s1 * z2 - s2 * z1) * r);

			tan1[i1] = tan1[i1] + sdir;
			tan1[i2] = tan1[i2] + sdir;
			tan1[i3] = tan1[i3] + sdir;

			tan2[i1] = tan2[i1] + tdir;
			tan2[i2] = tan2[i2] + tdir;
			tan2[i3] = tan2[i3] + tdir;

			triangle++;
		}

		for (long a = 0; a < vertexCount; a++)
		{
			const DirectX::XMFLOAT3& n = (&verts[a])->m_vNormal;
			const DirectX::XMFLOAT3& t = tan1[a];

			// Gram-Schmidt orthogonalize
			{
				auto _t = XMLoadFloat3(&t);
				auto _n = XMLoadFloat3(&n);
				auto _dot = DirectX::XMVector3Dot(_t, _n);
				auto _normalize = DirectX::XMVector3Normalize((_t - _n * DirectX::XMVectorGetX(_dot)));
				DirectX::XMStoreFloat4(&(&verts[a])->m_vTangents, _normalize);// = (t - n * Dot(n, t)).Normalize( );
			}

			// Calculate handedness
			{
				auto _t = XMLoadFloat3(&t);
				auto _n = XMLoadFloat3(&n);
				auto _tan2 = XMLoadFloat3(&tan2[a]);
				auto _cross = DirectX::XMVector3Cross(_n, _t);
				auto _dot = DirectX::XMVector3Dot(_cross, _tan2);
				float _w = DirectX::XMVectorGetX(_dot);
				(&verts[a])->m_vTangents.w = (_w < 0.0F) ? -1.0f : 1.0f;// = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
			}
		}

		delete[ ] tan1;
	};

	std::vector<Triangle> tris;

	for (size_t v = 0; v < cMesh.GetIndices( ).size( ); v += 3)
	{
		Triangle tri =
		{
			cMesh.GetIndices( )[v + 0],
			cMesh.GetIndices( )[v + 1],
			cMesh.GetIndices( )[v + 2]
		};
		tris.push_back(tri);
	}

	CalculateTangentArray(cMesh.GetVertices( ).size( ), tris.size( ), &tris[0], &cMesh.GetVertices( )[0]);


	return true;
}

bool CAssetManager::LoadSkin(FbxMesh* pMesh, CMesh& cMesh, JointList& vUniqueJoints)
{
	cMesh.GetInfluences( ).resize(cMesh.GetVertices( ).size( ));

	size_t deformer_count = pMesh->GetDeformerCount( );
	for (size_t deformer_count_index = 0; deformer_count_index < deformer_count; deformer_count_index++)
	{
		FbxSkin* skin = (FbxSkin*)pMesh->GetDeformer(deformer_count_index, FbxDeformer::eSkin);

		size_t skin_count = skin->GetClusterCount( );
		for (size_t skin_count_index = 0; skin_count_index < skin_count; skin_count_index++)
		{
			FbxCluster* cluster = skin->GetCluster(skin_count_index);
			FbxNode* joint = cluster->GetLink( );

			//FBXLoader::LoadSkeletonNode
			size_t index;
			CAssetManager::LoadSkelNode(joint, cMesh, vUniqueJoints, index);

			FbxAMatrix cluster_transform;
			cluster->GetTransformMatrix(cluster_transform);

			FbxAMatrix cluster_link_transform;
			cluster->GetTransformLinkMatrix(cluster_link_transform);

			DirectX::XMFLOAT4X4 world_bind_pose;
			ConvertMatrix((cluster_transform * cluster_link_transform), world_bind_pose);

			cMesh.GetJoints( )[index].m_mWorldBindPoseTransform = world_bind_pose;

			int *control_point_indices = cluster->GetControlPointIndices( );
			double *control_point_weights = cluster->GetControlPointWeights( );

			int control_point_indices_count = cluster->GetControlPointIndicesCount( );
			for (int control_point = 0; control_point < control_point_indices_count; control_point++)
			{
				int control_point_index = control_point_indices[control_point];
				float control_point_weight = (float)control_point_weights[control_point];

				CMesh::TJointInfluence influence;
				influence.m_unJointIndex = index;
				influence.m_fWeight = control_point_weight;

				size_t mesh_vertices_count = cMesh.GetVertices( ).size( );
				for (size_t mesh_vertex = 0; mesh_vertex < mesh_vertices_count; mesh_vertex++)
				{
					if (cMesh.GetVertices( )[mesh_vertex].m_nControlPointIndex == control_point_index)
					{
						cMesh.GetInfluences( )[mesh_vertex].push_back(influence);
					}
				}
			}
		}
	}
	return true;
}

bool CAssetManager::LoadSkelNode(FbxNode* pNode, CMesh& cMesh, JointList& vUniqueJoints, unsigned int& unIndex)
{
	if (!pNode->GetSkeleton( ))
	{
		return false;
	}

	auto pair = std::make_pair(pNode, pNode->GetNodeAttribute( ));

	for (size_t i = 0; i < vUniqueJoints.size( ); i++)
	{
		if (vUniqueJoints[i] == pair)
		{
			unIndex = i;
			return true;
		}
	}

	vUniqueJoints.push_back(pair);

	CMesh::TJoint joint;
	joint.m_unParent = 0;
	joint.m_szName = pNode->GetName( );
	cMesh.GetJoints( ).push_back(joint);

	unIndex = cMesh.GetJoints( ).size( ) - 1;

	size_t parentIndex;
	if (LoadSkelNode(pNode->GetParent( ), cMesh, vUniqueJoints, parentIndex))
	{
		cMesh.GetJoints( )[unIndex].m_unParent = parentIndex;
	}

	size_t children = pNode->GetChildCount( );
	for (size_t i = 0; i < children; i++)
	{
		size_t childIndex;
		if (LoadSkelNode(pNode->GetChild(i), cMesh, vUniqueJoints, childIndex))
		{
			cMesh.GetJoints( )[unIndex].m_vunChildren.push_back(childIndex);
		}
	}

	return true;
}

bool CAssetManager::ProcessNode(FbxNode* pNode, CMesh& cMesh, TModel*& prModel, JointList& vUniqueJoints)
{
	size_t attributeCount = pNode->GetNodeAttributeCount( );

	for (size_t i = 0; i < attributeCount; i++)
	{
		FbxNodeAttribute* fbxAttr = pNode->GetNodeAttributeByIndex(i);

		if (fbxAttr->GetAttributeType( ) && fbxAttr->GetAttributeType( ) == FbxNodeAttribute::eMesh)
		{
			LoadMesh(pNode->GetMesh( ), cMesh);
			LoadSkin(pNode->GetMesh( ), cMesh, vUniqueJoints);
			LoadTexture(pNode->GetMesh( ), cMesh, prModel);
		}
	}

	size_t childCount = pNode->GetChildCount( );
	for (size_t i = 0; i < childCount; i++)
	{
		if (!ProcessNode(pNode->GetChild(i), cMesh, prModel, vUniqueJoints))
		{
			// cout << "Processing the node's children " << i << " has FAILED.\n";
			FBXSDK_printf("Processing the node's children: %i has FAILED\n", i);
			return false;
		}
	}

	return true;
}

bool CAssetManager::LoadTexture(FbxMesh* pMesh, CMesh& cMesh, TModel*& prModel)
{
	FbxProperty fbxProperty;

	if (!pMesh->GetNode( ))
	{
		// cout << "[LoadTexture] The mesh's node was null!\n";
		FBXSDK_printf("[LoadTexture] The mesh's node was null!\n");
		return false;
	}

	int matCount = pMesh->GetNode( )->GetSrcObjectCount<FbxSurfaceMaterial>( );

	for (int material = 0; material < matCount; material++)
	{
		FbxSurfaceMaterial* surfaceMat = pMesh->GetNode( )->GetSrcObject<FbxSurfaceMaterial>(material);

		if (!surfaceMat)
		{
			continue;
		}

		int textureIndex;

		FBXSDK_FOR_EACH_TEXTURE(textureIndex)
		{
			fbxProperty = surfaceMat->FindProperty(FbxLayerElement::sTextureChannelNames[textureIndex]);

			if (!fbxProperty.IsValid( ))
			{
				continue;
			}

			int textureCount = fbxProperty.GetSrcObjectCount<FbxTexture>( );

			for (int i = 0; i < textureCount; i++)
			{
				FbxTexture* fbxTexture = fbxProperty.GetSrcObject<FbxTexture>(i);

				if (!fbxTexture)
				{
					continue;
				}

				FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(fbxTexture);

				if (!fileTexture)
				{
					continue;
				}

				std::string texName = fileTexture->GetFileName( );
				texName = PathFindFileNameA(texName.c_str( ));

				if (texName == "")
					continue;

				std::string s = TEXTURESPATH + texName;

				//Make sure we're not adding doubles
				{
					unsigned int uTextureCount = m_mTextureNames[cMesh.GetName( )].size( );
					bool bOriginal = true;
					for (size_t tex = 0; tex < uTextureCount; tex++)
					{
						if (m_mTextureNames[cMesh.GetName( )][tex] == s)
						{
							bOriginal = false;
							break;
						}
					}
					if (!bOriginal)
					{
						continue;
					}
				}
				CComPtr<ID3D11ShaderResourceView> pSRV = NULL;
				LoadTexture(s.c_str( ), &pSRV);

				std::string objectTag = texName;
				transform(objectTag.begin( ), objectTag.end( ), objectTag.begin( ), toupper);

				if ((objectTag.find("DIFFUSE") != std::string::npos))
				{
					for (int i = 0; i < _countof(prModel->m_tTex.m_pDiffuseTexture); i++)
					{
						if (prModel->m_tTex.m_pDiffuseTexture[i] == NULL)
						{
							prModel->m_tTex.m_pDiffuseTexture[i] = pSRV;
							break;
						}
					}
				}
				else if ((objectTag.find("NORMAL") != std::string::npos))
				{
					prModel->m_tTex.m_pNormalTexture = pSRV;
				}
				else
				{
					prModel->m_tTex.m_pMiscTexture = pSRV;
				}

				m_mTextureNames[cMesh.GetName( )].push_back(s);
			}
		}
	}

	return true;
}

bool CAssetManager::LoadAnim(FbxAnimLayer* pLayer, CAnimation<DirectX::XMFLOAT4X4>& cAnim, TModel*& prModel, JointList& vUniqueJoints)
{
	cAnim.m_vChannels.resize(vUniqueJoints.size( ));

	std::vector<float> keyTimes;

	for (size_t i = 0; i < vUniqueJoints.size( ); i++)
	{
		FbxNode* node = vUniqueJoints[i].first;

		keyTimes.clear( );

		LoadAnimCurve(node->LclTranslation.GetCurve(pLayer), keyTimes);
		LoadAnimCurve(node->LclRotation.GetCurve(pLayer), keyTimes);
		LoadAnimCurve(node->LclScaling.GetCurve(pLayer), keyTimes);

		for (size_t keytime = 1; keytime < keyTimes.size( ); keytime++)
		{
			CAnimation<DirectX::XMFLOAT4X4>::TKey key;

			FbxTime time;
			time.SetSecondDouble(keyTimes[keytime]);
			key.m_fTime = (float)time.GetSecondDouble( );

			FbxMatrix gTransform = node->EvaluateGlobalTransform(time);
			ConvertMatrix(gTransform, key.m_Node);

			/*DirectX::XMVECTOR scale, rotQuat, trans;
			DirectX::XMMATRIX matrix = DirectX::XMLoadFloat4x4(&key.m_Node);
			DirectX::XMMatrixDecompose(&scale, &rotQuat, &trans, matrix);

			DirectX::XMStoreFloat4(&key.m_vQuaternion, rotQuat);
			DirectX::XMStoreFloat3(&key.m_vPosition, trans);*/

			cAnim.m_vChannels[i].m_vKeys.push_back(key);
		}
	}

	return true;
}

bool CAssetManager::LoadAnimCurve(FbxAnimCurve* pCurve, std::vector<float>& vKeyTimes)
{
	if (!pCurve)
	{
		//FBXSDK_printf("[LoadAnimCurve] This current translation does not have an animation curve\n");
		return false;
	}

	size_t keyCount = pCurve->KeyGetCount( );

	for (size_t i = 0; i < keyCount; i++)
	{
		float time = (float)pCurve->KeyGetTime(i).GetSecondDouble( );

		if (find(vKeyTimes.begin( ), vKeyTimes.end( ), time) == vKeyTimes.end( ))
		{
			vKeyTimes.push_back(time);
		}
	}

	return true;
}

bool CAssetManager::LoadMapFromFBX(std::string szFileName, CGameplayState* pGameplay, int ePiece)
{
	// Create a FBX Manager
	FbxManager* fbxManager = NULL;
	FbxScene* fbxScene = NULL;
	// Grab the root node of the scene
	FbxLocalTime fbxTimeStamp;
	ZeroMemory(&fbxTimeStamp, sizeof(fbxTimeStamp));

#ifdef CHECK_FBX
	ImportFBXData(fbxManager, fbxScene, szFileName, &fbxTimeStamp);
#endif
	if (ePiece & EMAP_COLLISION && LoadCollision(szFileName, pGameplay, fbxTimeStamp))
	{
		//Don't load this in later
		ePiece = ePiece ^ EMAP_COLLISION;
	}
	if (ePiece & EMAP_RENDER && LoadMap(szFileName, pGameplay, fbxTimeStamp))
	{
		//Don't load this later
		ePiece = ePiece ^ EMAP_RENDER;
	}
	if (ePiece & EMAP_SCENERY && LoadScenery(szFileName, pGameplay, fbxTimeStamp))
	{
		//Don't load this later
		ePiece = ePiece ^ EMAP_SCENERY;
	}
	if (ePiece & EMAP_LIGHTS && LoadLights(szFileName, pGameplay->GetLevelManager(), fbxTimeStamp))
	{
		//Don't load this later
		ePiece = ePiece ^ EMAP_LIGHTS;
	}
	if (ePiece & EMAP_INANIMATE && LoadInanimates(szFileName, pGameplay, fbxTimeStamp))
	{
		//Don't load this later
		ePiece = ePiece ^ EMAP_INANIMATE;
	}

	if (ePiece == 0)
	{
		CleanUpFBX(fbxScene, fbxManager);
		return true;
	}
#ifndef CHECK_FBX
	//If it's release and somthing isn't found, make sure we can make it
	else
	{
		ImportFBXData(fbxManager, fbxScene, szFileName, &fbxTimeStamp);
	}
#endif

	//The Year would only be 0 if we failed to import the fbx
	if (fbxTimeStamp.mYear != 0)
	{

		// Grab the root node of the scene
		FbxNode* rootNode = fbxScene->GetRootNode( );

		// Traverse the scene
		ProcessMap(rootNode, pGameplay, pGameplay->GetLevelManager(), szFileName, ePiece, fbxTimeStamp);

		rootNode->Destroy(true);
		rootNode = nullptr;

		//this is two
		CleanUpFBX(fbxScene, fbxManager);

		return true;
	}

	CleanUpFBX(fbxScene, fbxManager);
	return false;
}

namespace{
	bool LoadAABB(FbxMesh* pMesh, TAABB* pCollider)
	{
		if (pCollider == nullptr)
		{
			return false;
		}
		int polygonCount = pMesh->GetPolygonCount();

		FbxVector4* controlPoints = pMesh->GetControlPoints();

		if (!controlPoints)
		{
			// cout << "[LoadMesh] The current mesh doesn't have any control points/vertices. What did you export?\n";
			FBXSDK_printf("[LoadMesh] The current mesh doesn't have any control points/vertices. What did you export?\n");
			return false;
		}

		//Store the vertices
		std::vector<CMesh::TUniqueVertex> uniqueVerts;
		int vertexID = 0;

		for (int polyCounter = 0; polyCounter < polygonCount; ++polyCounter)
		{
			int polyVertTotal = pMesh->GetPolygonSize(polyCounter);

			for (int polyVertCounter = 0; polyVertCounter < polyVertTotal; ++polyVertCounter)
			{
				CMesh::TUniqueVertex uniqueVert;

				int cpIndex = pMesh->GetPolygonVertex(polyCounter, polyVertCounter);

				uniqueVert.m_vPosition.x = (float)controlPoints[cpIndex].mData[0];
				uniqueVert.m_vPosition.y = (float)controlPoints[cpIndex].mData[1];
				uniqueVert.m_vPosition.z = (float)controlPoints[cpIndex].mData[2] * -1.0f;

				size_t size = uniqueVerts.size();
				size_t i;
				for (i = 0; i < size; ++i)
				{
					if (uniqueVert == uniqueVerts[i])
					{
						break;
					}
				}

				if (i == size)
				{
					uniqueVerts.push_back(uniqueVert);
				}

				++vertexID;
			}
		}

		//We have all the verts, now lets make an Aabb out of them
		pCollider->type = TCollider::eAABB;
		//Not using XMFLOATs everywhere was a huge mistake - Gergar
		pCollider->vMax.x = pCollider->vMin.x = uniqueVerts[0].m_vPosition.x;
		pCollider->vMax.y = pCollider->vMin.y = uniqueVerts[0].m_vPosition.y;
		pCollider->vMax.z = pCollider->vMin.z = uniqueVerts[0].m_vPosition.z;

		for (size_t i = 1; i < uniqueVerts.size(); i++)
		{
			pCollider->vMax.x = max(pCollider->vMax.x, uniqueVerts[i].m_vPosition.x);
			pCollider->vMax.y = max(pCollider->vMax.y, uniqueVerts[i].m_vPosition.y);
			pCollider->vMax.z = max(pCollider->vMax.z, uniqueVerts[i].m_vPosition.z);

			pCollider->vMin.x = min(pCollider->vMin.x, uniqueVerts[i].m_vPosition.x);
			pCollider->vMin.y = min(pCollider->vMin.y, uniqueVerts[i].m_vPosition.y);
			pCollider->vMin.z = min(pCollider->vMin.z, uniqueVerts[i].m_vPosition.z);
		}

		return true;
	}

	bool LoadAABB(CMesh& cMesh, TAABB* pCollider)
	{
		if (pCollider == nullptr)
		{
			return false;
		}

		//Store the vertices
		std::vector<CMesh::TUniqueVertex>& uniqueVerts = cMesh.GetVertices();

		//We have all the verts, now lets make an Aabb out of them
		pCollider->type = TCollider::eAABB;
		//Not using XMFLOATs everywhere was a huge mistake - Gergar
		pCollider->vMax.x = pCollider->vMin.x = uniqueVerts[0].m_vPosition.x;
		pCollider->vMax.y = pCollider->vMin.y = uniqueVerts[0].m_vPosition.y;
		pCollider->vMax.z = pCollider->vMin.z = uniqueVerts[0].m_vPosition.z;

		for (size_t i = 1; i < uniqueVerts.size(); i++)
		{
			pCollider->vMax.x = max(pCollider->vMax.x, uniqueVerts[i].m_vPosition.x);
			pCollider->vMax.y = max(pCollider->vMax.y, uniqueVerts[i].m_vPosition.y);
			pCollider->vMax.z = max(pCollider->vMax.z, uniqueVerts[i].m_vPosition.z);

			pCollider->vMin.x = min(pCollider->vMin.x, uniqueVerts[i].m_vPosition.x);
			pCollider->vMin.y = min(pCollider->vMin.y, uniqueVerts[i].m_vPosition.y);
			pCollider->vMin.z = min(pCollider->vMin.z, uniqueVerts[i].m_vPosition.z);
		}

		return true;
	}

	void CenterOfMesh(FbxMesh* pMesh, DirectX::XMFLOAT3& xmCenter)
	{
		TAABB tAABB;
		LoadAABB(pMesh, &tAABB);
		xmCenter = (tAABB.vMax + tAABB.vMin) * 0.5f;
	}

	DirectX::XMMATRIX ExtractWorldMatrix(FbxNode* pNode)
	{
		double* temp = pNode->LclScaling.Get().mData;
		float x = (float)temp[0], y = (float)temp[1], z = (float)temp[2];
		DirectX::XMMATRIX xmWorldMat = DirectX::XMMatrixScaling(float(temp[0]), float(temp[1]), float(temp[2]));
		temp = pNode->LclRotation.Get().mData;
		x = -(float)temp[0]; y = -(float)temp[1]; z = (float)temp[2];
		xmWorldMat = DirectX::XMMatrixMultiply(xmWorldMat, DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(x), DirectX::XMConvertToRadians(y), DirectX::XMConvertToRadians(z)));
		temp = pNode->LclTranslation.Get().mData;
		x = (float)temp[0]; y = (float)temp[1]; z = (float)temp[2];
		xmWorldMat = DirectX::XMMatrixMultiply(xmWorldMat, DirectX::XMMatrixTranslation(float(temp[0]), float(temp[1]), float(-temp[2])));

		return xmWorldMat;
		/*
			DirectX::XMMATRIX xmWorldMat = DirectX::XMMatrixIdentity( );
			double* temp = pNode->LclTranslation.Get( ).mData;
			//Switches from right handed to left
			xmWorldMat = DirectX::XMMatrixMultiply(xmWorldMat, DirectX::XMMatrixTranslation(float(temp[0]), float(temp[1]), float(-temp[2])));
			temp = pNode->LclRotation.Get( ).mData;
			xmWorldMat = DirectX::XMMatrixMultiply(xmWorldMat, DirectX::XMMatrixRotationRollPitchYaw(float(temp[0]), float(temp[1]), float(temp[2])));
			temp = pNode->LclScaling.Get( ).mData;
			xmWorldMat = DirectX::XMMatrixMultiply(xmWorldMat, DirectX::XMMatrixScaling(float(temp[0]), float(temp[1]), float(temp[2])));

			return xmWorldMat;

			*/
	}
}
bool CAssetManager::ProcessMapNode(FbxNode* pNode, CGameplayState* pGameplay, std::vector<CEntity*>& vAddedEntities)
{
	size_t attributeCount = pNode->GetNodeAttributeCount( );
	//Get the data out of this guy
	for (size_t i = 0; i < attributeCount; i++)
	{
		FbxNodeAttribute* fbxAttr = pNode->GetNodeAttributeByIndex(i);
		//Look for specific pieces
		std::string nodeName = pNode->GetName( );

		{
			JointList useless;
			CMesh cTerrainGroup;
			cTerrainGroup.GetName( ) = nodeName;
			m_mModels[nodeName] = new TModel( );
			CEntity* pTerrain = new CEntity( );
			pTerrain->SetTag(nodeName);
			pTerrain->SetEntityType(CEntity::eMAP);
			DirectX::XMStoreFloat4x4(&pTerrain->GetLocalMatrix( ), ExtractWorldMatrix(pNode));
			//Load normal terrain
			for (int j = 0; j < pNode->GetChildCount( ); j++)
			{
				ProcessNode(pNode->GetChild(j), cTerrainGroup, m_mModels[nodeName], useless);
			}
			//Load invisiwalls
			if (nodeName.find("Inv") != std::string::npos)
			{
				//Make the collider for them
				TAABB* pCollider = new TAABB( );
				LoadAABB(cTerrainGroup, pCollider);
				pTerrain->SetCollider(pCollider);
			}

			//Save the model
			StoreModel(&cTerrainGroup, "", nodeName);
			vAddedEntities.push_back(pTerrain);
			pGameplay->GetEntityManager( )->AddToManager(pTerrain);
			return true;
		}
	}
	//size_t childCount = pNode->GetChildCount();
	//for (size_t i = 0; i < childCount; i++)
	//{
	//	if (!ProcessMapNode(pNode->GetChild(i), pGameplay, cMesh))
	//	{
	//		// cout << "Processing the node's children " << i << " has FAILED.\n";
	//		FBXSDK_printf("Processing the node's children: %i has FAILED\n", i);
	//		return false;
	//	}
	//}

	return false;
}

void CAssetManager::ProcessMap(FbxNode* pNode, CGameplayState* pGameplay, CLevel* pLevel, std::string szFileName, int nPiece, const FbxLocalTime& tTimeStamp)
{
	//Check the children
	size_t childCount = pNode->GetChildCount( );
	for (size_t i = 0; i < childCount; i++)
	{
		FbxNode* pChild = pNode->GetChild(i);
		std::string szName = pChild->GetName( );
		std::vector<CEntity*> vCreatedEntities;
		//Handle the walls and floors
		if (nPiece & EMAP_RENDER && szName == "Render")
		{
			for (int j = 0; j < pChild->GetChildCount( ); j++)
			{
				ProcessMapNode(pChild->GetChild(j), pGameplay, vCreatedEntities);
			}

			//Save out info in binary
			SaveMap(vCreatedEntities, szFileName, tTimeStamp);

			//Add the map tags to the level dude
			for (size_t j = 0; j < vCreatedEntities.size(); j++)
			{
				pLevel->AddTag(vCreatedEntities[i]->GetTag());
			}
		}
		//Load collision of walls
		else if (nPiece & EMAP_COLLISION && szName == "Collide")
		{
			std::vector<TAABB*> vColliders;
			ProcessCollisionNode(pChild, vColliders);
			for (size_t j = 0; j < vColliders.size( ); j++)
			{
				pGameplay->GetEntityManager( )->AddToManager(vColliders[j]);
			}

			//Save out info in binary
			SaveCollision(vColliders, szFileName, tTimeStamp);
		}
		//Load in the other things (sub-systems, monster spawners, etc.)
		else if (nPiece & EMAP_SCENERY && szName == "Scenery")
		{
			std::vector<CEntity*> vCreatedEntities;
			ProcessScenery(pChild, pGameplay, vCreatedEntities);

			//Save out info in binary
			SaveScenery(vCreatedEntities, pGameplay, szFileName, tTimeStamp);
		}
		else if (nPiece & EMAP_LIGHTS && szName == "Lights")
		{
			bool bFoundRotary = false;
			//Load lights from fbx
			for (int j = 0; j < pChild->GetChildCount( ); j++)
			{
				FbxNode* pGrandChild = pChild->GetChild(j);
				//-------I know it's messy, but there's not much time------
				//Load in the rotary lights
				szName = pGrandChild->GetName();
				if (!bFoundRotary && szName == "RotaryLights")
				{
					bFoundRotary = true;
					for (int k = 0; k < pGrandChild->GetChildCount(); k++)
					{
						FbxNode* pGreatGrandChild = pGrandChild->GetChild(k);
						std::string szRotName = pGreatGrandChild->GetName();
						DirectX::XMFLOAT3 xmPos;
						DirectX::XMFLOAT4X4 xmRotMat;
						DirectX::XMStoreFloat4x4(&xmRotMat, ExtractWorldMatrix(pGreatGrandChild));
						xmPos.x = xmRotMat._41;
						xmPos.y = xmRotMat._42;
						xmPos.z = xmRotMat._43;
						pLevel->AddRotaryLight( new CRotaryLights(xmPos, (szRotName.find("X") != std::string::npos)));
					}
				}
				//Load in the regular lights
				else if (pGrandChild->GetNodeAttribute( ) && pGrandChild->GetNodeAttribute( )->GetAttributeType( ) == fbxsdk_2015_1::FbxNodeAttribute::EType::eLight)
				{
					FbxLight* pFBXLight = pGrandChild->GetLight( );
					DirectX::XMFLOAT4X4 xmMat;
					DirectX::XMStoreFloat4x4(&xmMat, ExtractWorldMatrix(pGrandChild));
					LightParams* pLight = new LightParams( );

					//Extract the position/direction from the node
					pLight->Position = DirectX::XMFLOAT3(xmMat._41, xmMat._42, xmMat._43);
					pLight->Direction = DirectX::XMFLOAT3(xmMat._31, xmMat._32, xmMat._33);

					//Extract the color from the light
					pLight->Color.x = (float)pFBXLight->Color.Get( )[0] * (float)pFBXLight->Intensity.Get( ) * 0.0375f;
					pLight->Color.y = (float)pFBXLight->Color.Get( )[1] * (float)pFBXLight->Intensity.Get( ) * 0.0375f;
					pLight->Color.z = (float)pFBXLight->Color.Get( )[2] * (float)pFBXLight->Intensity.Get( ) * 0.0375f;

					//Find the light type
					if (pFBXLight->LightType.Get( ) == fbxsdk_2015_1::FbxLight::EType::ePoint)
					{
						pLight->Type = LightType::Point;
					}
					else if (pFBXLight->LightType.Get( ) == fbxsdk_2015_1::FbxLight::EType::eSpot)
					{
						pLight->Type = LightType::Spot;
					}
					else
					{
						pLight->Type = LightType::Directional;
					}

					//Find the range
					pLight->Range = (float)pFBXLight->Intensity.Get( ) * 4;

					//Find the angles
					pLight->SpotInnerAngle = (float)pFBXLight->InnerAngle.Get( );
					pLight->SpotOuterAngle = (float)pFBXLight->OuterAngle.Get( );

					pLevel->AddLight(pLight);
				}

			}

			//Save lights as a binary
			SaveLights(pLevel, szFileName, tTimeStamp);
		}
		//Any objects in the terrain that have unique models that will be loaded in separately
		else if (nPiece & EMAP_INANIMATE && szName == "Inanimates")
		{
			std::vector<CEntity*> vEntities;
			//Extract the tags, matrixes, etc from the FBX
			ProcessInanimates(pChild, pGameplay, vEntities);
			//Add the map tags to the level dude
			for (size_t j = 0; j < vEntities.size(); j++)
			{
				pLevel->AddTag(vEntities[i]->GetTag());
			}
			//Save it to a binary
			SaveInanimates(vEntities, szFileName, tTimeStamp);
		}
	}
}

void CAssetManager::ProcessScenery(FbxNode* pNode, CGameplayState* pGameplay, std::vector<CEntity*>& vAddedEntities)
{
	//Check the children
	size_t childCount = pNode->GetChildCount( );
	for (size_t i = 0; i < childCount; i++)
	{
		FbxNode* pChild = pNode->GetChild(i);
		std::string szName = pChild->GetName( );
		//Upgrade shop
		if (szName == "Workstation" || szName == "Reactor" || szName == "Engine" || szName == "LifeSupport")
		{
			//Make the entity
			CEntity* pWork;
			//if (szName.find("Work") != std::string::npos)
			//{
			//	pWork = new CWorkStation(pGameplay, eStateChange::STATE_CH_SHOP, "Press E to access SHOP");
			//}
			if (szName.find("Rea") != std::string::npos)
			{
				pWork = new CReactor( );
			}
			else if (szName.find("Eng") != std::string::npos)
			{
				pWork = new CEngines( );
			}
			else if (szName.find("Lif") != std::string::npos)
			{
				pWork = new CLifeSupport( );
			}
			else
			{
				pWork = nullptr;
			}

			DirectX::XMStoreFloat4x4(&(pWork->GetLocalMatrix( )), ExtractWorldMatrix(pChild));
			pWork->SetPosition(pWork->GetPosition());
			//pWork->SetTag(szName);
			CMesh cMesh;
			cMesh.GetName( ) = szName;
			//m_mModels[szName] = new TModel( );
			//JointList useless;
			for (int j = 0; j < pChild->GetChildCount( ); j++)
			{
				FbxNode* pGrandChild = pChild->GetChild(j);
				std::string szGCName = pGrandChild->GetName( );
				//Load the collider
				if (szGCName == "pCollider")
				{
					TAABB* pCollider = new TAABB( );
					LoadAABB(pGrandChild->GetMesh( ), pCollider);
					pCollider->vMax = pCollider->vMax + pWork->GetPosition( );
					pCollider->vMin = pCollider->vMin + pWork->GetPosition( );
					//if (szName.find("Work") != std::string::npos)
					//{
					//	pWork->SetCollider(pCollider);
					//}
					//else
					{
						delete pWork->GetCollider( );
						static_cast<CSubSystem*>(pWork)->SetCollider(pCollider);
					}
				}
				////Load the zone
				//else if (szGCName == "pWorkPlane")
				//{
				//	if (szName.find("Work") != std::string::npos)
				//	{
				//		TAABB* pCollider = new TAABB( );
				//		LoadAABB(pGrandChild->GetMesh( ), pCollider);
				//		pCollider->vMax = pCollider->vMax + pWork->GetPosition( );
				//		pCollider->vMin = pCollider->vMin + pWork->GetPosition( );
				//		dynamic_cast<CScenery*>(pWork)->SetZone(pCollider);
				//	}
				//	//Get this for the mesh as well
				//	ProcessNode(pGrandChild, cMesh, m_mModels[szName], useless);
				//}
				////Load the mesh
				//else if (szGCName == "Renderable")
				//{
				//	ProcessNode(pGrandChild, cMesh, m_mModels[szName], useless);
				//}
			}
			cMesh.GetInfluences( ).clear( );
			//StoreModel(&cMesh, "", szName);
			//Add to entity manager
			if (szName.find("Rea") != std::string::npos)
			{
				pGameplay->GetEntityManager( )->AddToManager((CReactor*)pWork, CEntityManager::eREACTOR);
				((CReactor*)pWork)->SetSystemType(eSubsystemtype::eSystemReactor);
			}
			else if (szName.find("Eng") != std::string::npos)
			{
				pGameplay->GetEntityManager( )->AddToManager((CEngines*)pWork, CEntityManager::eENGINES);
				((CEngines*)pWork)->SetSystemType(eSubsystemtype::eSystemEngine);
			}
			else if (szName.find("Lif") != std::string::npos)
			{
				pGameplay->GetEntityManager( )->AddToManager((CLifeSupport*)pWork, CEntityManager::eLIFESUPPORT);
				((CLifeSupport*)pWork)->SetSystemType(eSubsystemtype::eSystemLife);
			}
			else
			{
				pGameplay->GetEntityManager( )->AddToManager((CScenery*)pWork);
			}
			vAddedEntities.push_back(pWork);
		}
		//Player spawn
		else if (szName.find("Play"/*"Player"*/) != std::string::npos)
		{
			DirectX::XMFLOAT3 xmSpawn;
			CenterOfMesh(pChild->GetMesh( ), xmSpawn);
			DirectX::XMStoreFloat3(&xmSpawn, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&xmSpawn), ExtractWorldMatrix(pChild)));
			//Store
			pGameplay->SetPlayerStartPos(xmSpawn);
		}
		else if (szName == "FloorGuides")
		{
			ProcessFloorGuides(pChild, pGameplay);
		}
		//Monster spawners
		//else if (szName.find("Spawner") != std::string::npos)
		//{
		//	for (int j = 0; j < pChild->GetChildCount( ); j++)
		//	{
		//		FbxNode* pGrandChild = pChild->GetChild(j);
		//		std::string szGrandName = pGrandChild->GetName( );
		//		//Make a spawner
		//		CMonsterSpawner* pSpawner = nullptr;
		//		//Make it constant
		//		if (szGrandName.find("Const") != std::string::npos)
		//		{
		//			pSpawner = new CMonsterSpawner(float(25 + rand( ) % 5), float(25 + rand( ) % 10), 8, true);
		//		}
		//		//Make it waved
		//		else
		//		{
		//			//Used to keep the spawners out of sync
		//			static int s_nSpawnerCount;

		//			if (s_nSpawnerCount == 0)
		//				pSpawner = new CMonsterSpawner(float(35 + rand( ) % 14),
		//				float(50 + rand( ) % 10),
		//				(45 + rand( ) % 13), false);
		//			else
		//				pSpawner = new CMonsterSpawner(float(65 + rand( ) % 13),
		//				float(50 + rand( ) % 27),
		//				(80 + rand( ) % 13), false);

		//			s_nSpawnerCount++;
		//		}
		//		//Get the position of the spawner
		//		DirectX::XMFLOAT4X4 xmWorldMat;
		//		DirectX::XMStoreFloat4x4(&xmWorldMat, ExtractWorldMatrix(pGrandChild));
		//		pSpawner->SetPosition(DirectX::XMFLOAT3(xmWorldMat._41, 0, xmWorldMat._43));
		//		//Hook it up with the spawner
		//		pSpawner->SetEntityManager(pGameplay->GetEntityManager( ));
		//		pGameplay->GetEntityManager( )->AddToManager(pSpawner);
		//	}
		//Make it waved
		//else
		//{
		//	//Used to keep the spawners out of sync
		//	static int s_nSpawnerCount = 0;
		//
		//	CWaveSpawner* waveSpawner = new CWaveSpawner();
		//
		//	if (s_nSpawnerCount == 0)
		//	{
		//		waveSpawner->ReadXml("Assets//XML//WaveSpawner1.xml");
		//	}
		//	else
		//	{
		//		waveSpawner->ReadXml("Assets//XML//WaveSpawner2.xml");
		//	}
		//
		//	DirectX::XMFLOAT4X4 xmWorldMat;
		//	DirectX::XMStoreFloat4x4(&xmWorldMat, ExtractWorldMatrix(pGrandChild));
		//	waveSpawner->SetPosition(DirectX::XMFLOAT3(xmWorldMat._41, 0, xmWorldMat._43));
		//	//Hook it up with the spawner
		//	waveSpawner->SetEntityManager(pGameplay->GetEntityManager());
		//	pGameplay->GetEntityManager()->AddToManager( waveSpawner);
		//
		//	s_nSpawnerCount++;
		//}
		//}
		//}
		/*//Doors
		else if (szName.find("Door") != std::string::npos)
		{
		}*/
	}
}

void CAssetManager::ProcessFloorGuides(FbxNode* pNode, CGameplayState* pGameplay)
{
	unsigned int uChildCount = pNode->GetChildCount( );
	for (size_t i = 0; i < uChildCount; i++)
	{
		FbxNode* pChild = pNode->GetChild(i);
		DirectX::XMFLOAT4X4 xMat;
		DirectX::XMStoreFloat4x4(&xMat, ExtractWorldMatrix(pChild));
		DirectX::XMFLOAT3 xmPos;
		xmPos.x = xMat._41; xmPos.y = xMat._42; xmPos.z = xMat._43;
		float fArrowRot[3];
		for (size_t j = 0; j < 3; j++)
		{
			FbxNode* pGrandChild = pChild->GetChild(j);
			//pGrandChild->RotationOffset.Get();
			fArrowRot[j] = -DirectX::XMConvertToRadians((float)(pGrandChild->LclRotation.Get( )[1]));
		}
		pGameplay->GetEntityManager( )->GetFloorGuide( )->AddGuide(xmPos, fArrowRot[0], fArrowRot[1], fArrowRot[2]);

	}
}

bool CAssetManager::ProcessCollisionNode(FbxNode* pNode, std::vector<TAABB*>& vColliders)
{
	size_t attributeCount = pNode->GetNodeAttributeCount( );

	for (size_t i = 0; i < attributeCount; i++)
	{
		FbxNodeAttribute* fbxAttr = pNode->GetNodeAttributeByIndex(i);

		if (fbxAttr->GetAttributeType( ) && fbxAttr->GetAttributeType( ) == FbxNodeAttribute::eMesh)
		{
			LoadCollider(pNode->GetMesh( ), vColliders);
		}
	}

	size_t childCount = pNode->GetChildCount( );
	for (size_t i = 0; i < childCount; i++)
	{
		if (!ProcessCollisionNode(pNode->GetChild(i), vColliders))
		{
			// cout << "Processing the node's children " << i << " has FAILED.\n";
			FBXSDK_printf("Processing the node's children: %i has FAILED\n", i);
			return false;
		}
	}

	return true;
}

bool CAssetManager::LoadCollider(FbxMesh* pMesh, std::vector<TAABB*>& vColliders)
{
	TAABB* pCollider = new TAABB( );

	//Create a collider from the vertices
	if (!LoadAABB(pMesh, pCollider))
	{
		delete pCollider;
		return false;
	}
	//Add the collider to our list
	vColliders.push_back(pCollider);

	return true;
}

void CAssetManager::LoadTexture(const char* _szFile, ID3D11ShaderResourceView** _ppSRV)
{
	wchar_t widename[MAX_PATH * 4];
	int ret = MultiByteToWideChar(CP_UTF8, 0, _szFile, -1, widename, MAX_PATH * 4);

	LoadTexture(widename, _ppSRV);
}

void CAssetManager::LoadTexture(const wchar_t* _szFile, ID3D11ShaderResourceView** _ppSRV)
{
	std::wstring path = PathFindExtensionW(_szFile);
	transform(path.begin( ), path.end( ), path.begin( ), toupper);

	ID3D11Resource* resource = NULL;
	if (path.find(L".DDS") != std::string::npos)
	{
		HRESULT hr = CreateDDSTextureFromFile(CRenderer::GetReference( ).GetDevice( ), _szFile, &resource, _ppSRV);
		HR(hr);
	}
	else
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(CRenderer::GetReference( ).GetDevice( ), _szFile, &resource, _ppSRV);

		if (HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr)
		{
			std::wstring file = _szFile;

			DebugPrint(std::string("File Not Found: " + std::string(file.begin( ), file.end( )) + "\n").c_str( ),
					   ConsoleColor::Cyan);
		}
		else
		{
			HR(hr);
		}
	}

	if (_ppSRV)
	{
		//SetD3DName((*_ppSRV), std::string("Object Created At Line: " + std::to_string(__LINE__) + " File: " + __FILE__).c_str( ));
		//SetD3DName(resource, std::string("Object Created At Line: " + std::to_string(__LINE__) + " File: " + __FILE__).c_str( ));
	}

	SafeRelease(resource);
}

#pragma region MAPBIN

//Save the renderable information as a mapbin
void CAssetManager::SaveMap(std::vector<CEntity*>& vEntities, std::string szFileName, const FbxLocalTime& tTimeStamp)
{
	std::string localFile = PathFindFileNameA(szFileName.c_str( ));
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str()));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".mapbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".mapbin"));

	// Create the local fbxbin to allow loading times to be faster
	std::fstream file(fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	if (file.is_open( ) && file.good( ))
	{
		//Save the header
		{
			char temp[10] = "RenderMap";
			file.write(temp, 10);
		}
		//Save the time stamp
		file.write((char*)&tTimeStamp, sizeof(FbxLocalTime));

		//How many meshes are here
		unsigned int uNumMeshes = vEntities.size( );
		file.write((char*)&uNumMeshes, sizeof(int));
		for (size_t j = 0; j < uNumMeshes; j++)
		{
			//Write the tag for this model
			unsigned int nameLen = vEntities[j]->GetTag( ).size( );
			file.write((char*)&nameLen, sizeof(int));
			file.write(vEntities[j]->GetTag( ).c_str( ), nameLen);

			// Write index count
			file.write((char*)&m_mModels[vEntities[j]->GetTag( )]->m_unIndexCount, sizeof(unsigned int));

			// Write indices
			unsigned int indexCount = m_mModels[vEntities[j]->GetTag( )]->m_cMesh.GetIndices( ).size( );
			for (unsigned int i = 0; i < indexCount; i++)
			{
				file.write((char*)&m_mModels[vEntities[j]->GetTag( )]->m_cMesh.GetIndices( )[i], sizeof(unsigned int));
			}

			// Write vertex count
			unsigned int vertCount = m_mModels[vEntities[j]->GetTag( )]->m_cMesh.GetVertices( ).size( );
			file.write((char*)&vertCount, sizeof(unsigned int));

			// Write vertices
			for (unsigned int i = 0; i < vertCount; i++)
			{
				file.write((char*)&m_mModels[vEntities[j]->GetTag( )]->m_cMesh.GetVertices( )[i], sizeof(CMesh::TUniqueVertex));
			}

			// Write number of textures of this particular mesh
			unsigned int numOfTextures = m_mTextureNames[m_mModels[vEntities[j]->GetTag( )]->m_cMesh.GetName( )].size( );
			file.write((char*)&numOfTextures, sizeof(unsigned int));

			// Write the texture names/name length
			for (unsigned int i = 0; i < numOfTextures; i++)
			{
				unsigned int nameLen = m_mTextureNames[m_mModels[vEntities[j]->GetTag( )]->m_cMesh.GetName( )][i].length( );
				file.write((char*)&nameLen, sizeof(unsigned int));
				file.write(m_mTextureNames[m_mModels[vEntities[j]->GetTag( )]->m_cMesh.GetName( )][i].c_str( ), nameLen);
			}

		}
	}
	file.close( );
}
//Load in the mapbin
bool CAssetManager::LoadMap(std::string szFileName, CGameplayState* pGameplay, const FbxLocalTime& tTimeStamp)
{
	if (pGameplay == nullptr)
	{
		DebugPrint("[Asset Manager] Gameplay Invalid!\n");
		return false;
	}
	std::string localFile = PathFindFileNameA(szFileName.c_str( ));
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str()));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".mapbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".mapbin"));

	// Create the local fbxbin to allow loading times to be faster
	std::fstream file(fileName, std::ios_base::in | std::ios_base::binary);

	if (file.is_open( ) && file.good( ))
	{
		//Does the header match?
		{
			char temp[10];
			file.read(temp, 10);
			if (strcmp(temp, "RenderMap"))
			{
				//Header didn't match, we're not supposed to be here
				file.close( );
				return false;
			}
		}
		//Does the timestamp match?
		{
			FbxLocalTime temp;
			file.read((char*)&temp, sizeof(FbxLocalTime));
			if (tTimeStamp.mYear != 0 && ((temp.mDay != tTimeStamp.mDay) || (temp.mHour != tTimeStamp.mHour) || (temp.mSecond != tTimeStamp.mSecond) ||
				(temp.mMinute != tTimeStamp.mMinute) || (temp.mMonth != tTimeStamp.mMonth) || (temp.mYear != tTimeStamp.mYear)))
			{
				//Map is outdated, get another one
				DebugPrint((fileName + " outdated, generating new file...\n").c_str( ));
				file.close( );
				return false;
			}
		}

		//Number of models here
		unsigned int uMeshCount;
		file.read((char*)&uMeshCount, sizeof(int));
		char szBuffer[256];

		for (size_t i = 0; i < uMeshCount; i++)
		{
			//Get the name of the mesh
			unsigned int uNameLen;
			file.read((char*)&uNameLen, sizeof(int));
			file.read(szBuffer, uNameLen);
			szBuffer[uNameLen] = '\0';
			std::string szTag = szBuffer;
			//Create the mesh
			m_mModels[szTag] = new TModel( );
			CMesh cMesh;
			cMesh.GetName( ) = szTag;

			//Get the indices
			file.read((char*)&m_mModels[szTag]->m_unIndexCount, sizeof(int));
			cMesh.GetIndices( ).resize(m_mModels[szTag]->m_unIndexCount);
			for (size_t j = 0; j < m_mModels[szTag]->m_unIndexCount; j++)
			{
				file.read((char*)&cMesh.GetIndices( )[j], sizeof(int));
			}
			//Get the vertices
			unsigned int uVertCount;
			file.read((char*)&uVertCount, sizeof(int));
			cMesh.GetVertices( ).resize(uVertCount);
			for (size_t j = 0; j < uVertCount; j++)
			{
				file.read((char*)&cMesh.GetVertices( )[j], sizeof(CMesh::TUniqueVertex));
			}

			// Read number of textures of this particular mesh
			unsigned int numOfTextures = m_mTextureNames[cMesh.GetName( )].size( );
			file.read((char*)&numOfTextures, sizeof(unsigned int));

			m_mTextureNames[cMesh.GetName( )].resize(numOfTextures);

			// Read the texture names/name length
			for (unsigned int j = 0; j < numOfTextures; j++)
			{
				unsigned int nameLen;// = m_mTextureNames[cMesh.GetName()][j].length();
				file.read((char*)&nameLen, sizeof(unsigned int));
				m_mTextureNames[cMesh.GetName( )][j].resize(nameLen);
				file.read(const_cast<char*>(m_mTextureNames[cMesh.GetName( )][j].c_str( )), nameLen);

				//Load in textures from filenames
				CComPtr<ID3D11ShaderResourceView> pSRV = NULL;
				LoadTexture(m_mTextureNames[cMesh.GetName( )][j].c_str( ), &pSRV);
				//cMesh.GetTextures().push_back(pSRV);

				std::string objectTag = m_mTextureNames[cMesh.GetName( )][j];
				transform(objectTag.begin( ), objectTag.end( ), objectTag.begin( ), toupper);

				if ((objectTag.find("DIFFUSE") != std::string::npos))
				{
					for (int texture_index = 0; texture_index < _countof(m_mModels[szTag]->m_tTex.m_pDiffuseTexture); texture_index++)
					{
						if (m_mModels[szTag]->m_tTex.m_pDiffuseTexture[texture_index] == NULL)
						{
							m_mModels[szTag]->m_tTex.m_pDiffuseTexture[texture_index] = pSRV;
							break;
						}
					}
				}
				else if ((objectTag.find("NORMAL") != std::string::npos))
				{
					m_mModels[szTag]->m_tTex.m_pNormalTexture = pSRV;
				}
				else
				{
					m_mModels[szTag]->m_tTex.m_pMiscTexture = pSRV;
				}
			}
			CreateBuffer(CRenderer::GetReference( ).GetDevice( ), cMesh.GetVertices( ), D3D11_BIND_VERTEX_BUFFER, &m_mModels[szTag]->m_pVertexBuffer);
			CreateBuffer(CRenderer::GetReference( ).GetDevice( ), cMesh.GetIndices( ), D3D11_BIND_INDEX_BUFFER, &m_mModels[szTag]->m_pIndexBuffer);

			m_mModels[szTag]->m_cMesh = cMesh;
			CEntity* pEntity = new CEntity( );
			pEntity->SetTag(szTag);
			pEntity->SetEntityType(CEntity::eMAP);
			if (szTag.find("Inv") != std::string::npos)
			{
				TAABB* pCol = new TAABB( );
				LoadAABB(cMesh, pCol);
				pEntity->SetCollider(pCol);
			}
			pGameplay->GetEntityManager( )->AddToManager(pEntity);
			pGameplay->GetLevelManager()->AddTag(pEntity->GetTag());
		}

		return true;
	}
	return false;
}

//Save the scenery information as a mapbin
void CAssetManager::SaveScenery(std::vector<CEntity*>& vObjects, CGameplayState* pGameplay, std::string szFileName, const FbxLocalTime& tTimeStamp)
{
	std::string localFile = PathFindFileNameA(szFileName.c_str());
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str()));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".snrbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".snrbin"));

	// Create the local fbxbin to allow loading times to be faster
	std::fstream file(fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	if (file.is_open() && file.good())
	{
		//Save the header
		{
			char temp[11] = "SceneryMap";
			file.write(temp, 11);
		}
		//Save the time stamp
		file.write((char*)&tTimeStamp, sizeof(FbxLocalTime));

		////How many meshes are here
		//unsigned int uNumMeshes = vObjects.size( );
		//file.write((char*)&uNumMeshes, sizeof(int));
		//for (size_t j = 0; j < uNumMeshes; j++)
		//{
		//
		//	//Write the tag for this model
		//	unsigned int nameLen = vObjects[j]->GetTag( ).size( );
		//	file.write((char*)&nameLen, sizeof(int));
		//	file.write(vObjects[j]->GetTag( ).c_str( ), nameLen);
		//
		//	// Write index count
		//	file.write((char*)&m_mModels[vObjects[j]->GetTag( )]->m_unIndexCount, sizeof(unsigned int));
		//
		//	// Write indices
		//	unsigned int indexCount = m_mModels[vObjects[j]->GetTag( )]->m_cMesh.GetIndices( ).size( );
		//	for (unsigned int i = 0; i < indexCount; i++)
		//	{
		//		file.write((char*)&m_mModels[vObjects[j]->GetTag( )]->m_cMesh.GetIndices( )[i], sizeof(unsigned int));
		//	}
		//
		//	// Write vertex count
		//	unsigned int vertCount = m_mModels[vObjects[j]->GetTag( )]->m_cMesh.GetVertices( ).size( );
		//	file.write((char*)&vertCount, sizeof(unsigned int));
		//
		//	// Write vertices
		//	for (unsigned int i = 0; i < vertCount; i++)
		//	{
		//		file.write((char*)&m_mModels[vObjects[j]->GetTag( )]->m_cMesh.GetVertices( )[i], sizeof(CMesh::TUniqueVertex));
		//	}
		//
		//	// Write number of textures of this particular mesh
		//	unsigned int numOfTextures = m_mTextureNames[m_mModels[vObjects[j]->GetTag( )]->m_cMesh.GetName( )].size( );
		//	file.write((char*)&numOfTextures, sizeof(unsigned int));
		//
		//	// Write the texture names/name length
		//	for (unsigned int i = 0; i < numOfTextures; i++)
		//	{
		//		unsigned int nameLen = m_mTextureNames[m_mModels[vObjects[j]->GetTag( )]->m_cMesh.GetName( )][i].length( );
		//		file.write((char*)&nameLen, sizeof(unsigned int));
		//		file.write(m_mTextureNames[m_mModels[vObjects[j]->GetTag( )]->m_cMesh.GetName( )][i].c_str( ), nameLen);
		//	}
		//
		//}
		//How many objects were created
		unsigned int uObjCount = vObjects.size();
		file.write((char*)&uObjCount, sizeof(int));
		for (size_t i = 0; i < uObjCount; i++)
		{
			//What is this object?
			std::string szTempTag;
			if (vObjects[i]->GetTag().find("Reactor") != std::string::npos)
			{
				szTempTag = "Reactor";
			}
			else if (vObjects[i]->GetTag().find("LifeSupport") != std::string::npos)
			{
				szTempTag = "LifeSupport";
			}
			else if (vObjects[i]->GetTag().find("Engine") != std::string::npos)
			{
				szTempTag = "Engine";
			}
			else
			{
				szTempTag = vObjects[i]->GetTag();
			}

			unsigned int uTagLen = szTempTag.size();
			file.write((char*)&uTagLen, sizeof(int));
			file.write(szTempTag.c_str(), uTagLen);

			//Where is it
			DirectX::XMFLOAT4X4 xmMatrix = vObjects[i]->GetWorldMatrix();
			file.write((char*)&xmMatrix, sizeof(DirectX::XMFLOAT4X4));

			//Where is its collider
			file.write((char*)vObjects[i]->GetCollider(), sizeof(TAABB));

			//Where is its active zone?
			if (dynamic_cast<CWorkStation*>(vObjects[i]))
			{

				file.write((char*)(((CWorkStation*)vObjects[i])->GetZone()), sizeof(TAABB));
			}
		}
		//Write the FloorGuides
		FloorGuides::CFloorGuide* pFloorGuide = pGameplay->GetEntityManager()->GetFloorGuide();
		unsigned int uGuideCnt = pFloorGuide->GetMarkerCount();
		file.write((char*)&uGuideCnt, sizeof(int));
		//Save each guide's info
		for (unsigned int i = 0; i < uGuideCnt; i++)
		{
			//Save the position
			DirectX::XMFLOAT3 pos = pFloorGuide->GetMarker(i)->m_cDisplay.GetPosition();
			file.write((char*)&pos, sizeof(DirectX::XMFLOAT3));
			//Save the rotations
			file.write((char*)(&pFloorGuide->GetMarker(i)->m_fReactorRotation), sizeof(float));
			file.write((char*)(&pFloorGuide->GetMarker(i)->m_fLifeSupportRotation), sizeof(float));
			file.write((char*)(&pFloorGuide->GetMarker(i)->m_fEngineRotation), sizeof(float));
		}


		//Write the player spawn point
		DirectX::XMFLOAT3 xmPlayerPos = pGameplay->GetPlayerStartPos();
		file.write((char*)&xmPlayerPos, sizeof(DirectX::XMFLOAT3));
	}
	file.close();
}
//Load in the mapbin
bool CAssetManager::LoadScenery(std::string szFileName, CGameplayState* pGameplay, const FbxLocalTime& tTimeStamp)
{
	std::string localFile = PathFindFileNameA(szFileName.c_str());
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str()));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".snrbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".snrbin"));

	// Create the local fbxbin to allow loading times to be faster
	std::fstream file(fileName, std::ios_base::in | std::ios_base::binary);

	if (file.is_open() && file.good())
	{
		//Does the header match?
		{
			char temp[11];
			file.read(temp, 11);
			if (strcmp(temp, "SceneryMap"))
			{
				//Header didn't match, we're not supposed to be here
				file.close();
				return false;
			}
		}
		//Does the timestamp match?
		{
			FbxLocalTime temp;
			file.read((char*)&temp, sizeof(FbxLocalTime));
			if (tTimeStamp.mYear != 0 && ((temp.mDay != tTimeStamp.mDay) || (temp.mHour != tTimeStamp.mHour) || (temp.mSecond != tTimeStamp.mSecond) ||
				(temp.mMinute != tTimeStamp.mMinute) || (temp.mMonth != tTimeStamp.mMonth) || (temp.mYear != tTimeStamp.mYear)))
			{
				//Map is outdated, get another one
				DebugPrint((fileName + " outdated, generating new file...\n").c_str());
				file.close();
				return false;
			}
		}
		char szBuffer[256];

		////Number of models here
		//unsigned int uMeshCount;
		//file.read((char*)&uMeshCount, sizeof(int));
		//
		//for (size_t i = 0; i < uMeshCount; i++)
		//{
		//	//Get the name of the mesh
		//	unsigned int uNameLen;
		//	file.read((char*)&uNameLen, sizeof(int));
		//	file.read(szBuffer, uNameLen);
		//	szBuffer[uNameLen] = '\0';
		//	std::string szTag = szBuffer;
		//	//Create the mesh
		//	m_mModels[szTag] = new TModel( );
		//	CMesh cMesh;
		//	cMesh.GetName( ) = szTag;
		//
		//	//Get the indices
		//	file.read((char*)&m_mModels[szTag]->m_unIndexCount, sizeof(int));
		//	cMesh.GetIndices( ).resize(m_mModels[szTag]->m_unIndexCount);
		//	for (size_t j = 0; j < m_mModels[szTag]->m_unIndexCount; j++)
		//	{
		//		file.read((char*)&cMesh.GetIndices( )[j], sizeof(int));
		//	}
		//	//Get the vertices
		//	unsigned int uVertCount;
		//	file.read((char*)&uVertCount, sizeof(int));
		//	cMesh.GetVertices( ).resize(uVertCount);
		//	for (size_t j = 0; j < uVertCount; j++)
		//	{
		//		file.read((char*)&cMesh.GetVertices( )[j], sizeof(CMesh::TUniqueVertex));
		//	}
		//
		//	// Read number of textures of this particular mesh
		//	unsigned int numOfTextures = m_mTextureNames[cMesh.GetName( )].size( );
		//	file.read((char*)&numOfTextures, sizeof(unsigned int));
		//
		//	m_mTextureNames[cMesh.GetName( )].resize(numOfTextures);
		//
		//	// Read the texture names/name length
		//	for (unsigned int j = 0; j < numOfTextures; j++)
		//	{
		//		unsigned int nameLen;// = m_mTextureNames[cMesh.GetName()][j].length();
		//		file.read((char*)&nameLen, sizeof(unsigned int));
		//		m_mTextureNames[cMesh.GetName( )][j].resize(nameLen);
		//		file.read(const_cast<char*>(m_mTextureNames[cMesh.GetName( )][j].c_str( )), nameLen);
		//
		//		//Load in textures from filenames
		//		CComPtr<ID3D11ShaderResourceView> pSRV = NULL;
		//		LoadTexture(m_mTextureNames[cMesh.GetName( )][j].c_str( ), &pSRV);
		//
		//		std::string objectTag = m_mTextureNames[cMesh.GetName( )][j];
		//		transform(objectTag.begin( ), objectTag.end( ), objectTag.begin( ), toupper);
		//
		//		if ((objectTag.find("DIFFUSE") != std::string::npos))
		//		{
		//			for (int texture_index = 0; texture_index < _countof(m_mModels[szTag]->m_tTex.m_pDiffuseTexture); texture_index++)
		//			{
		//				if (m_mModels[szTag]->m_tTex.m_pDiffuseTexture[texture_index] == NULL)
		//				{
		//					m_mModels[szTag]->m_tTex.m_pDiffuseTexture[texture_index] = pSRV;
		//					break;
		//				}
		//			}
		//		}
		//		else if ((objectTag.find("NORMAL") != std::string::npos))
		//		{
		//			m_mModels[szTag]->m_tTex.m_pNormalTexture = pSRV;
		//		}
		//		else
		//		{
		//			m_mModels[szTag]->m_tTex.m_pMiscTexture = pSRV;
		//		}
		//	}
		//	CreateBuffer(CRenderer::GetReference( ).GetDevice( ), cMesh.GetVertices( ), D3D11_BIND_VERTEX_BUFFER, &m_mModels[szTag]->m_pVertexBuffer);
		//	CreateBuffer(CRenderer::GetReference( ).GetDevice( ), cMesh.GetIndices( ), D3D11_BIND_INDEX_BUFFER, &m_mModels[szTag]->m_pIndexBuffer);
		//
		//	m_mModels[szTag]->m_cMesh = cMesh;
		//}

		//How many objects are we supposed to load?
		unsigned int uObjCount;
		file.read((char*)&uObjCount, sizeof(int));
		for (size_t i = 0; i < uObjCount; i++)
		{
			unsigned int uTagLen;
			file.read((char*)&uTagLen, sizeof(int));
			file.read(szBuffer, uTagLen);
			szBuffer[uTagLen] = '\0';
			string szTag = szBuffer;
			CEntity* pEntity = nullptr;
			//Load in the different dudes
			//if (szTag == "Workstation")
			//{
			//	pEntity = new CWorkStation(pGameplay, eStateChange::STATE_CH_SHOP, "Press E to access SHOP");
			//}
			if (szTag == "Reactor")
			{
				pEntity = new CReactor();
			}
			else if (szTag == "Engine")
			{
				pEntity = new CEngines();
			}
			else if (szTag == "LifeSupport")
			{
				pEntity = new CLifeSupport();
			}
			else
			{
				continue;
			}
			if (pEntity)
			{
				//pEntity->SetTag(szTag);
				//Where is this thing
				DirectX::XMFLOAT4X4 xmMatrix;
				file.read((char*)&xmMatrix, sizeof(DirectX::XMFLOAT4X4));
				pEntity->GetLocalMatrix() = xmMatrix;
				pEntity->SetPosition(pEntity->GetPosition());
				//Where is its collider?

				//if (dynamic_cast<CWorkStation*>(pEntity))
				//{
				//	TAABB* pAABB = new TAABB();
				//	file.read((char*)pAABB, sizeof(TAABB));
				//	pEntity->SetCollider(pAABB);
				//
				//	TAABB* pCol = new TAABB();
				//	file.read((char*)pCol, sizeof(TAABB));
				//	((CWorkStation*)pEntity)->SetZone(pCol);
				//
				//	pGameplay->GetEntityManager()->AddToManager((CWorkStation*)pEntity);
				//}
				if (dynamic_cast<CSubSystem*>(pEntity))
				{
					//TAABB* pAABB = new TAABB();
					file.read((char*)((CSubSystem*)pEntity)->GetCollider(), sizeof(TAABB));
					//((CSubSystem*)pEntity)->SetCollider(pAABB);

					if (dynamic_cast<CReactor*>(pEntity))
					{
						((CSubSystem*)pEntity)->SetCollider(((CSubSystem*)pEntity)->GetCollider());
						pGameplay->GetEntityManager()->AddToManager((CSubSystem*)pEntity, 0);
					}
					else if (dynamic_cast<CLifeSupport*>(pEntity))
					{
						pGameplay->GetEntityManager()->AddToManager((CSubSystem*)pEntity, 1);
					}
					else if (dynamic_cast<CEngines*>(pEntity))
					{
						pGameplay->GetEntityManager()->AddToManager((CSubSystem*)pEntity, 2);
					}
				}
			}
		}

		//Load in floor guides
		unsigned int uGuideCnt = 0;
		file.read((char*)&uGuideCnt, sizeof(int));
		for (unsigned int i = 0; i < uGuideCnt; i++)
		{
			DirectX::XMFLOAT3 xmPos;
			float fReactor, fLifeSupport, fEngine;
			file.read((char*)&xmPos, sizeof(DirectX::XMFLOAT3));
			file.read((char*)&fReactor, sizeof(float));
			file.read((char*)&fLifeSupport, sizeof(float));
			file.read((char*)&fEngine, sizeof(float));
			pGameplay->GetEntityManager()->GetFloorGuide()->AddGuide(xmPos, fReactor, fLifeSupport, fEngine);
		}

		//Find the player spawn point
		DirectX::XMFLOAT3 xmPlayerPos;
		file.read((char*)&xmPlayerPos, sizeof(DirectX::XMFLOAT3));
		pGameplay->SetPlayerStartPos(xmPlayerPos);

		return true;
	}

	return false;
}

//Save the cillideable information as a colbin
void CAssetManager::SaveCollision(std::vector<TAABB*>& vColliders, std::string szFileName, const FbxLocalTime& tTimeStamp)
{
	std::string localFile = PathFindFileNameA(szFileName.c_str( ));
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str()));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".colbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".colbin"));

	// Create the local fbxbin to allow loading times to be faster
	std::fstream file(fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	if (file.is_open( ) && file.good( ))
	{
		//Save the header
		{
			char temp[11] = "CollideMap";
			file.write(temp, 11);
		}
		//Save the time stamp
		file.write((char*)&tTimeStamp, sizeof(FbxLocalTime));

		file.clear( );
		//Write how many AABBs you've got
		unsigned int uSize = vColliders.size( );
		file.write((const char*)&uSize, sizeof(int));
		//Save each aabb out
		for (size_t i = 0; i < uSize; i++)
		{
			file.write((char*)(vColliders[i]), sizeof(TAABB));
		}
	}
	file.close( );
}
//Load in the mapbin
bool CAssetManager::LoadCollision(std::string szFileName, CGameplayState* pGameplay, const FbxLocalTime& tTimeStamp)
{
	if (pGameplay == nullptr)
	{
		DebugPrint("[Asset Manager] Gameplay Invalid!\n");
		return false;
	}
	std::string localFile = PathFindFileNameA(szFileName.c_str( ));
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str()));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".colbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".colbin"));

	// Create the local fbxbin to allow loading times to be faster
	std::fstream file(fileName, std::ios_base::in | std::ios_base::binary);

	if (file.is_open( ) && file.good( ))
	{
		//Does the header match?
		{
			char temp[11];
			file.read(temp, 11);
			if (strcmp(temp, "CollideMap"))
			{
				//Header didn't match, we're not supposed to be here
				file.close( );
				return false;
			}
		}
		//Does the timestamp match?
		{
			FbxLocalTime temp;
			file.read((char*)&temp, sizeof(FbxLocalTime));
			if (tTimeStamp.mYear != 0 && ((temp.mDay != tTimeStamp.mDay) || (temp.mHour != tTimeStamp.mHour) || (temp.mSecond != tTimeStamp.mSecond) ||
				(temp.mMinute != tTimeStamp.mMinute) || (temp.mMonth != tTimeStamp.mMonth) || (temp.mYear != tTimeStamp.mYear)))
			{
				//Map is outdated, get another one
				DebugPrint((fileName + " outdated, generating new file...\n").c_str( ));
				file.close( );
				return false;
			}
		}

		unsigned int uColCount = 0;
		file.read((char *)&uColCount, sizeof(int));

		for (size_t i = 0; i < uColCount; i++)
		{
			TAABB* pCollider = new TAABB( );
			file.read((char*)pCollider, sizeof(TAABB));
			pGameplay->GetEntityManager( )->AddToManager(pCollider);
		}

		file.close( );
		return true;
	}
	DebugPrint(("[Asset Manager] Failed to Load Collision data from " + fileName + "\n").c_str( ));
	return false;
}

//Save the light info in a binary file
void CAssetManager::SaveLights(CLevel* pLevel, std::string szFileName, const FbxLocalTime& tTimeStamp)
{
	std::string localFile = PathFindFileNameA(szFileName.c_str( ));
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str()));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".colbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".litbin"));

	// Create the local fbxbin to allow loading times to be faster
	std::fstream file(fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	if (file.is_open( ) && file.good( ))
	{
		file.clear( );

		//Write a header
		{
			char temp[10] = "MapLights";
			file.write(temp, 10);
		}
		//Save the time stamp
		file.write((char*)&tTimeStamp, sizeof(FbxLocalTime));

		//How many lights do we have
		unsigned int uLightCount = pLevel->GetLightCount( );
		file.write((char*)&uLightCount, sizeof(int));
		//Write out each light
		for (size_t i = 0; i < uLightCount; i++)
		{
			file.write((char*)pLevel->GetLight(i), sizeof(LightParams));
		}
		//How many rotary lights do we have
		uLightCount = pLevel->GetRotaryLightCount();
		file.write((char*)&uLightCount, sizeof(int));
		//Save out each rotary light
		for (size_t i = 0; i < uLightCount; i++)
		{
			bool bRotateX = pLevel->GetRotaryLight(i)->GetRotateX();
			DirectX::XMFLOAT3 xmPos = pLevel->GetRotaryLight(i)->GetPosition();
			file.write((char*)&bRotateX, sizeof(bool));
			file.write((char*)&xmPos, sizeof(DirectX::XMFLOAT3));
		}
	}
	file.close( );
}
//Load Lights from binary
bool CAssetManager::LoadLights(std::string szFileName, CLevel* pLevel, const FbxLocalTime& tTimeStamp)
{
	if (pLevel == nullptr)
	{
		DebugPrint("[Asset Manager] Gameplay Invalid!\n");
		return false;
	}
	std::string localFile = PathFindFileNameA(szFileName.c_str( ));
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str()));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".colbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".litbin"));

	// Create the local fbxbin to allow loading times to be faster
	std::fstream file(fileName, std::ios_base::in | std::ios_base::binary);

	if (file.is_open( ) && file.good( ))
	{
		//Does the header match?
		{
			char temp[10];
			file.read(temp, 10);
			if (strcmp(temp, "MapLights"))
			{
				//Header didn't match, we're not supposed to be here
				file.close( );
				return false;
			}
		}
		//Does the timestamp match?
		{
			FbxLocalTime temp;
			file.read((char*)&temp, sizeof(FbxLocalTime));
			if (tTimeStamp.mYear != 0 && ((temp.mDay != tTimeStamp.mDay) || (temp.mHour != tTimeStamp.mHour) || (temp.mSecond != tTimeStamp.mSecond) ||
				(temp.mMinute != tTimeStamp.mMinute) || (temp.mMonth != tTimeStamp.mMonth) || (temp.mYear != tTimeStamp.mYear)))
			{
				//Map is outdated, get another one
				DebugPrint((fileName + " outdated, generating new file...\n").c_str( ));
				file.close( );
				return false;
			}
		}
		//How many lights
		unsigned int uLightCount;
		file.read((char*)&uLightCount, sizeof(int));
		//Load each light
		for (size_t i = 0; i < uLightCount; i++)
		{
			LightParams* pLight = new LightParams( );
			file.read((char*)pLight, sizeof(LightParams));

			pLevel->AddLight(pLight);
		}
		//How many rotary lights
		file.read((char*)&uLightCount, sizeof(int));
		//Read each rotary light
		for (size_t i = 0; i < uLightCount; i++)
		{
			bool bRotateX = false;
			DirectX::XMFLOAT3 xmPos;
			file.read((char*)&bRotateX, sizeof(bool));
			file.read((char*)&xmPos, sizeof(DirectX::XMFLOAT3));

			pLevel->AddRotaryLight(new CRotaryLights(xmPos, bRotateX));
		}


		file.close( );
		return true;
	}
	DebugPrint(("[Asset Manager] Failed to Load Light data from " + fileName + "\n").c_str( ));
	return false;
}

//Save all inanimate information in a binary file
void CAssetManager::SaveInanimates(std::vector<CEntity*>& vObjects, std::string szFileName, const FbxLocalTime& tTimeStamp)
{
	std::string localFile = PathFindFileNameA(szFileName.c_str());
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str()));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".colbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".inabin"));

	// Create the local fbxbin to allow loading times to be faster
	std::fstream file(fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	if (file.is_open() && file.good())
	{
		file.clear();

		//Write a header
		{
			char temp[10] = "Inanimate";
			file.write(temp, 10);
		}
		//Save the time stamp
		file.write((char*)&tTimeStamp, sizeof(FbxLocalTime));

		//How many objects have we got
		unsigned int uObjectCount = vObjects.size();
		file.write((char*)&uObjectCount, sizeof(int));

		//Store each
		std::string szCurrTag = "Bleh";
		for (size_t i = 0; i < uObjectCount; i++)
		{
			//Make a section for this tag
			if (vObjects[i]->GetTag() != szCurrTag)
			{
				szCurrTag = vObjects[i]->GetTag();
				//How long is this new tag
				unsigned int uTagLen = szCurrTag.size();
				file.write((char*)&uTagLen, sizeof(int));
				//What is this new tag?
				file.write(szCurrTag.c_str(), uTagLen);
				//How many more dudes use this tag?
				unsigned int uTagUsers = 1;
				while (uObjectCount > i + uTagUsers && vObjects[i + uTagUsers]->GetTag() == szCurrTag)
				{
					uTagUsers++;
				}
				file.write((char*)&uTagUsers, sizeof(int));
			}
			//Write the object's transform
			DirectX::XMFLOAT4X4 xmMat = vObjects[i]->GetWorldMatrix();
			file.write((char*)&xmMat, sizeof(DirectX::XMFLOAT4X4));

			//Are we transparent
			{
				bool bTransparent = vObjects[i]->GetRObjectType() == CRObject::ROBJECT_TRANSPARENT;
				file.write((char*)&bTransparent, sizeof(bool));
			}
			//Are we an invisiwall?
			{
				bool bHasCollider = vObjects[i]->GetCollider() != nullptr;
				file.write((char*)&bHasCollider, sizeof(bool));

				if (bHasCollider)
				{
					file.write((char*)vObjects[i]->GetCollider(), sizeof(TAABB));
				}
			}
		}
	}
	file.close();
}
//Load all inanimate information from a binary file
bool CAssetManager::LoadInanimates(std::string szFileName, CGameplayState* pGameplay, const FbxLocalTime& tTimeStamp)
{
	if (pGameplay == nullptr)
	{
		DebugPrint("[Asset Manager] Gameplay Invalid!\n");
		return false;
	}
	std::string localFile = PathFindFileNameA(szFileName.c_str());
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str()));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".colbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".inabin"));

	// Create the local fbxbin to allow loading times to be faster
	std::fstream file(fileName, std::ios_base::in | std::ios_base::binary);

	if (file.is_open() && file.good())
	{
		//Does the header match?
		{
			char temp[10];
			file.read(temp, 10);
			if (strcmp(temp, "Inanimate"))
			{
				//Header didn't match, we're not supposed to be here
				file.close();
				return false;
			}
		}
		//Does the timestamp match?
		{
			FbxLocalTime temp;
			file.read((char*)&temp, sizeof(FbxLocalTime));
			if (tTimeStamp.mYear != 0 && ((temp.mDay != tTimeStamp.mDay) || (temp.mHour != tTimeStamp.mHour) || (temp.mSecond != tTimeStamp.mSecond) ||
				(temp.mMinute != tTimeStamp.mMinute) || (temp.mMonth != tTimeStamp.mMonth) || (temp.mYear != tTimeStamp.mYear)))
			{
				//Map is outdated, get another one
				DebugPrint((fileName + " outdated, generating new file...\n").c_str());
				file.close();
				return false;
			}
		}
		//How many objects
		unsigned int uObjectCount;
		file.read((char*)&uObjectCount, sizeof(int));
		//How many objects use this tag?
		unsigned int uTagCount = 0;
		std::string szCurrTag;
		for (size_t i = 0; i < uObjectCount; i++, uTagCount--)
		{
			//Load in a new string
			if (uTagCount == 0)
			{
				//How long is the Tag
				unsigned int uTagLen;
				file.read((char*)&uTagLen, sizeof(int));
				//What is the tag
				char szBuffer[256];
				file.read(szBuffer, uTagLen);
				szBuffer[uTagLen] = '\0';
				//How many objects use this tag
				szCurrTag = szBuffer;
				file.read((char*)&uTagCount, sizeof(int));
			}
			//Prep the entity
			CEntity* pTemp = new CEntity();
			pTemp->SetTag(szCurrTag);
			{
				//What's it's matrix?
				DirectX::XMFLOAT4X4 xmMat;
				file.read((char*)&xmMat, sizeof(DirectX::XMFLOAT4X4));
				pTemp->GetLocalMatrix() = xmMat;
			}
			{
				//Is it transparent?
				bool bTransparent;
				file.read((char*)&bTransparent, sizeof(bool));
				if (bTransparent)
				{
					pTemp->SetRObjectType(CRObject::ROBJECT_TRANSPARENT);
				}
			}
			{
				//Are we an invisiwall
				bool bInvisiWall;
				file.read((char*)&bInvisiWall, sizeof(bool));
				if (bInvisiWall)
				{
					pTemp->SetCollider(new TAABB());
					file.read((char*)pTemp->GetCollider(), sizeof(TAABB));
				}
			}
			pGameplay->GetEntityManager()->AddToManager(pTemp);
		}

		file.close();
		return true;
	}
	DebugPrint(("[Asset Manager] Failed to Load Inanimate data from " + fileName + "\n").c_str());
	return false;
}


#pragma endregion

void CAssetManager::ProcessInanimates(FbxNode* pNode, CGameplayState* pGameplay, std::vector<CEntity*>& vAddedEntities)
{
	//pNode should hold all Inanimate groups
	size_t uGroupCount = pNode->GetChildCount();

	//Process each group
	for (size_t i = 0; i < uGroupCount; i++)
	{
		FbxNode* pGroupParent = pNode->GetChild(i);
		std::string szGroupName = pGroupParent->GetName();
		//Special cases
#pragma region LSTanks
		if (szGroupName == "LSTanks")
		{
			std::vector<CEntity*> vGlasses;
			std::vector<CEntity*> vCadavers;
			//Cycle through each tank
			size_t uTankCount = pGroupParent->GetChildCount();
			for (size_t j = 0; j < uTankCount; j++)
			{
				//Should we invisiwall?
				std::string szTemp = pGroupParent->GetChild(j)->GetName();
				bool bInvisiWalls = false;
				bInvisiWalls = (szTemp.find("INV") != std::string::npos);

				//Find the transform of the object
				DirectX::XMFLOAT4X4 xmMat;
				DirectX::XMStoreFloat4x4(&xmMat, ExtractWorldMatrix(pGroupParent->GetChild(j)));
				//Every tank has a frame
				CEntity* pTemp = new CEntity();
				pTemp->GetLocalMatrix() = xmMat;
				pTemp->SetTag("LSFrame");
				vAddedEntities.push_back(pTemp);
				//Invisi-walls
				if (bInvisiWalls)
				{
					TModel* pModel = GetModelByTag(pTemp->GetTag());
					if (pModel)
					{
						TAABB* pCollider = new TAABB();
						LoadAABB(pModel->m_cMesh, pCollider);
						pTemp->SetCollider(pCollider);
					}
				}
				//Some tanks have glass
				if (pGroupParent->GetChild(j)->GetChildCount() > 1)
				{
					pTemp = new CEntity();
					pTemp->GetLocalMatrix() = xmMat;
					pTemp->SetTag("LSGlass");
					pTemp->SetRObjectType(CRObject::ROBJECT_TRANSPARENT);
					vGlasses.push_back(pTemp);
					//Invisi-walls
					if (bInvisiWalls)
					{
						TModel* pModel = GetModelByTag(pTemp->GetTag());
						if (pModel)
						{
							TAABB* pCollider = new TAABB();
							LoadAABB(pModel->m_cMesh, pCollider);
							pTemp->SetCollider(pCollider);
						}
					}
				}
				//Very few tanks have a cadaver
				if (pGroupParent->GetChild(j)->GetChildCount() > 2)
				{
					pTemp = new CEntity();
					pTemp->GetLocalMatrix() = xmMat;
					pTemp->SetTag("LSCadaver");
					vCadavers.push_back(pTemp);
					//Invisi-walls
					if (bInvisiWalls)
					{
						TModel* pModel = GetModelByTag(pTemp->GetTag());
						if (pModel)
						{
							TAABB* pCollider = new TAABB();
							LoadAABB(pModel->m_cMesh, pCollider);
							//Will probably have to scale them by world matrix
							pTemp->SetCollider(pCollider);
						}
					}
				}
			}
			//Add the glasses in proper order
			for (size_t i = 0; i < vGlasses.size(); i++)
			{
				vAddedEntities.push_back(vGlasses[i]);
			}
			//Add the cadavers in proper order
			for (size_t i = 0; i < vCadavers.size(); i++)
			{
				vAddedEntities.push_back(vCadavers[i]);
			}
		}
#pragma endregion
		//Normal shit
		else
		{
			//Cycle through each tank
			size_t uInstanceCount = pGroupParent->GetChildCount();
			for (size_t j = 0; j < uInstanceCount; j++)
			{
				//Should we invisiwall?
				std::string szTemp = pGroupParent->GetChild(j)->GetName();
				bool bInvisiWalls = (szTemp.find("INV") != std::string::npos);
				bool bTransparent = (szTemp.find("trans") != std::string::npos);

				//Find the transform of the object
				DirectX::XMFLOAT4X4 xmMat;
				DirectX::XMStoreFloat4x4(&xmMat, ExtractWorldMatrix(pGroupParent->GetChild(j)));
				//Every tank has a frame
				CEntity* pTemp = new CEntity();
				pTemp->GetLocalMatrix() = xmMat;
				pTemp->SetTag(szGroupName);
				vAddedEntities.push_back(pTemp);
				//Invisi-walls
				if (false && bInvisiWalls)
				{
					TModel* pModel = GetModelByTag(pTemp->GetTag());
					if (pModel)
					{
						TAABB* pCollider = new TAABB();
						LoadAABB(pModel->m_cMesh, pCollider);
						pTemp->SetCollider(pCollider);
					}
				}
				if (bTransparent)
				{
					pTemp->SetRObjectType(CRObject::ROBJECT_TRANSPARENT);
				}
			}
		}
	}
	//Add each entity to the entity manager
	size_t uObjectCount = vAddedEntities.size();
	for (size_t i = 0; i < uObjectCount; i++)
	{
		pGameplay->GetEntityManager()->AddToManager(vAddedEntities[i]);
	}
}
