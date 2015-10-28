/***********************************************
* Filename:			PathSearch.cpp
* Date:      		05/12/2015
* Mod. Date:		05/19/2015
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:			This class handles the NavMesh loading
setting up and pathfinding algorithm
************************************************/

#include "PathSearch.h"
#include "../../Renderer/DebugRenderer.h"
#include "../../State Machine/StateMachine.h"
#include <fstream>
#include <assert.h>
using namespace MathHelper;

CPathSearch* CPathSearch::m_gInstance = nullptr;

bool IsMyGreat(CPlannerNode* const& lhs, CPlannerNode* const& rhs)
{
	return (lhs->GetHeuristic() > rhs->GetHeuristic());
}

CPathSearch::CPathSearch() : m_myPriority(IsMyGreat)
{
}

void CPathSearch::DeleteInstance()
{
	if (m_gInstance != nullptr)
	{
		m_gInstance->Shutdown();

		delete m_gInstance;
		m_gInstance = nullptr;
	}
}

CPathSearch::~CPathSearch()
{
	m_tTriangles.clear();
	m_nIndices.clear();
	m_vVertex.clear();
	Shutdown();
}

//************************************ FBX ******************************

bool CPathSearch::InitializeSdkObjects(FbxManager*& prFbxManager, FbxScene*& prScene)
{
	prFbxManager = FbxManager::Create();

	if (!prFbxManager)
	{
//#ifdef _DEBUG
//		// cout << "[InitializeSdkObjects] Creating the FbxManager FAILED\n";
//		FBXSDK_printf("[PathSearch::InitializeSdkObjects] Creating the FbxManager FAILED\n");
//#endif
		return false;
	}


	FbxIOSettings* ios = FbxIOSettings::Create(prFbxManager, IOSROOT);

	if (!ios)
	{
//#ifdef _DEBUG
//		// cout << "[InitializeSdkObjects] Creating the FbxIOSettings FAILED.\n";
//		FBXSDK_printf("[PathSearch::InitializeSdkObjects] Creating the FbxIOSettings FAILED.\n");
//#endif
		return false;
	}

	prFbxManager->SetIOSettings(ios);

	// Create the FBX Scene
	prScene = FbxScene::Create(prFbxManager, "MyScene");

	if (!prScene)
	{
//#ifdef _DEBUG
//		// cout << "[InitializeSdkObjects] Creating the FbxScene FAILED.\n";
//		FBXSDK_printf("[PathSearch::InitializeSdkObjects] Creating the FbxScene FAILED.\n");
//#endif
		return false;
	}

	return true;
}

bool CPathSearch::LoadNavMesh(std::string szFileName)
{
	
	m_sModelName = PathFindFileNameA(szFileName.c_str());
	CustomPathFuncs::CustomPathRemoveExtension(m_sModelName);

	if (LoadLocalModelFBX(szFileName))
	{
		return true;
	}

	// Create a FBX Manager
	FbxManager* fbxManager = NULL;
	FbxScene* fbxScene = NULL;

	// Prepare the FBXSDK
	if (!InitializeSdkObjects(fbxManager, fbxScene))
	{
//#ifdef _DEBUG
//		// cout << "[Load] Initializing the SDK Objects has FAILED.\n";
//		FBXSDK_printf("[PathSearch::LoadNavMesh] Intializing the SDK Objects has FAILED.\n");
//#endif
		return false;
	}

	// Create an FBXImporter
	FbxImporter* fbxImporter = NULL;

	fbxImporter = FbxImporter::Create(fbxManager, "");

	if (!fbxImporter)
	{
//#ifdef _DEBUG
//		// cout << "[Load] Creating the FbxImporter has FAILED.\n";
//		FBXSDK_printf("[PathSearch::LoadNavMesh] Creating the FbxImporter has FAILED.\n");
//#endif
		return false;
	}

	// Initialize it
	if (!fbxImporter->Initialize(szFileName.c_str(), -1, fbxManager->GetIOSettings()))
	{
		FbxStatus status = fbxImporter->GetStatus();
//#ifdef _DEBUG
//		// cout << "[Load] Initializing the FBXImporter has FAILED.\n";
//		FBXSDK_printf("[PathSearch::LoadNavMesh] Initializing the FBXImporter has FAILED.\n");
//#endif
		return false;
	}

	// Load the contents from the file
	if (!fbxImporter->Import(fbxScene))
	{
//#ifdef _DEBUG
//		// cout << "[Load] Importing the FBXScene has FAILED.\n";
//		FBXSDK_printf("[PathSearch::LoadNavMesh] Importing the FBXScene has FAILED.\n");
//#endif
		return false;
	}

	// Destroy the importer because we don't need it anymore
	fbxImporter->Destroy();

	// Grab the root node of the scene
	FbxNode* rootNode = fbxScene->GetRootNode();

	if (!rootNode)
	{
		// cout << "[Load] The FbxScene's root node is NULL!\n";
		//FBXSDK_printf("[PathSearch::LoadNavMesh] The FbxScene's root node is NULL!\n");
		return false;
	}

	CMesh cMesh;

	// Traverse the scene
	if (!ProcessNode(rootNode, cMesh))
	{
//#ifdef _DEBUG
//		// cout << "[Load] Processing the root node and/or it's children FAILED.\n";
//		FBXSDK_printf("[PathSearch::LoadNavMesh] Processing the root node and/or it's children FAILED.\n");
//#endif
		return false;
	}

	// Create the local .fbxbin to make loading times supah fast.
	if (szFileName != "")
	{
		SaveLocalModelFBX(szFileName);
	}

	CleanUpFBX(fbxScene, fbxManager);

	return true;

}

bool CPathSearch::LoadMesh(FbxMesh* pMesh, CMesh& cMesh)
{
	// RTA code. Indices/DrawIndexed sure as hell doesn't work.
	int polygonCount = pMesh->GetPolygonCount();

	FbxVector4* controlPoints = pMesh->GetControlPoints();

	if (!controlPoints)
	{
		// cout << "[LoadMesh] The current mesh doesn't have any control points/vertices. What did you export?\n";
		//FBXSDK_printf("[PathSearch::LoadNavMesh] The current mesh doesn't have any control points/vertices. What did you export?\n");
		return false;
	}

	int vertexID = 0;

	for (int polyCounter = 0; polyCounter < polygonCount; ++polyCounter)
	{
		int polyVertTotal = pMesh->GetPolygonSize(polyCounter);

		if (polyVertTotal != 3)
		{
			// cout << "[LoadMesh] The current mesh is NOT triangulated. Please triangulate for better results.\n";
			//FBXSDK_printf("[PathSearch::LoadNavMesh] The current mesh is NOT triangulated. Please triangulate for better results. please\n");
		}

		for (int polyVertCounter = 0; polyVertCounter < polyVertTotal; ++polyVertCounter)
		{
			CMesh::TUniqueVertex uniqueVert;

			int cpIndex = pMesh->GetPolygonVertex(polyCounter, polyVertCounter);

			uniqueVert.m_vPosition.x = (float)controlPoints[cpIndex].mData[0];
			uniqueVert.m_vPosition.y = (float)controlPoints[cpIndex].mData[1];
			uniqueVert.m_vPosition.z = (float)controlPoints[cpIndex].mData[2] * -1.0f;


			int elementUVCount = pMesh->GetElementUVCount();
			for (int uvCounter = 0; uvCounter < elementUVCount; ++uvCounter)
			{
				FbxGeometryElementUV* geoUV = pMesh->GetElementUV(uvCounter);

				FbxLayerElement::EMappingMode mapMode = geoUV->GetMappingMode();
				FbxLayerElement::EReferenceMode refMode = geoUV->GetReferenceMode();

				int directIndex = -1;

				if (mapMode == FbxGeometryElement::eByControlPoint)
				{
					if (refMode == FbxGeometryElement::eDirect)
					{
						directIndex = cpIndex;
					}
					else if (refMode == FbxGeometryElement::eIndexToDirect)
					{
						directIndex = geoUV->GetIndexArray().GetAt(cpIndex);
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
					FbxVector2 uv = geoUV->GetDirectArray().GetAt(directIndex);

					uniqueVert.m_vTexCoord.x = (float)uv.mData[0];
					uniqueVert.m_vTexCoord.y = 1.0f - (float)uv.mData[1];
				}
			}

			int elemNormCount = pMesh->GetElementNormalCount();
			for (int normCounter = 0; normCounter < elemNormCount; ++normCounter)
			{
				FbxGeometryElementNormal* geoNorm = pMesh->GetElementNormal(normCounter);

				FbxLayerElement::EMappingMode mapMode = geoNorm->GetMappingMode();
				FbxLayerElement::EReferenceMode refMode = geoNorm->GetReferenceMode();

				int directIndex = -1;

				if (mapMode == FbxGeometryElement::eByControlPoint)
				{
					if (refMode == FbxGeometryElement::eDirect)
					{
						FbxVector4 norm = geoNorm->GetDirectArray().GetAt(cpIndex);

						uniqueVert.m_vNormal.x = (float)norm[0];
						uniqueVert.m_vNormal.y = (float)norm[1];
						uniqueVert.m_vNormal.z = (float)norm[2] * -1.0f;
					}
					else if (refMode == FbxGeometryElement::eIndexToDirect)
					{
						int id = geoNorm->GetIndexArray().GetAt(cpIndex);
						FbxVector4 norm = geoNorm->GetDirectArray().GetAt(id);

						uniqueVert.m_vNormal.x = (float)norm[0];
						uniqueVert.m_vNormal.y = (float)norm[1];
						uniqueVert.m_vNormal.z = (float)norm[2] * -1.0f;

					}
				}
				else if (mapMode == FbxGeometryElement::eByPolygonVertex)
				{
					if (refMode == FbxGeometryElement::eDirect)
					{
						FbxVector4 norm = geoNorm->GetDirectArray().GetAt(vertexID);

						uniqueVert.m_vNormal.x = (float)norm[0];
						uniqueVert.m_vNormal.y = (float)norm[1];
						uniqueVert.m_vNormal.z = (float)norm[2] * -1.0f;
					}
					else if (refMode == FbxGeometryElement::eIndexToDirect)
					{
						int id = geoNorm->GetIndexArray().GetAt(vertexID);
						FbxVector4 norm = geoNorm->GetDirectArray().GetAt(id);

						uniqueVert.m_vNormal.x = (float)norm[0];
						uniqueVert.m_vNormal.y = (float)norm[1];
						uniqueVert.m_vNormal.z = (float)norm[2] * -1.0f;
					}
				}
			}

			std::vector<CMesh::TUniqueVertex>& uniqueVerts = cMesh.GetVertices();

			size_t size = uniqueVerts.size();
			size_t i;
			for (i = 0; i < size; ++i)
			{
				if (uniqueVert == uniqueVerts[i])
					break;
			}

			if (i == size)
			{
				uniqueVerts.push_back(uniqueVert);
			}

			size_t mySiz = m_vVertex.size();
			size_t count;
			for (count = 0; count < mySiz; ++count)
			{
				if ((uniqueVert.m_vPosition.x == m_vVertex[count].x) &&
					(uniqueVert.m_vPosition.y == m_vVertex[count].y) &&
					(uniqueVert.m_vPosition.z == m_vVertex[count].z))
					break;
			}

			if (count == mySiz)
			{
				DirectX::XMFLOAT3 aux;
				//uniqueVert.m_nControlPointIndex = cpIndex;
				aux.x = uniqueVert.m_vPosition.x;
				aux.y = uniqueVert.m_vPosition.y;
				aux.z = uniqueVert.m_vPosition.z;

				m_vVertex.push_back(aux);
			}


			m_nIndices.push_back(count);
			cMesh.GetIndices().push_back(i);

			++vertexID;
		}
	}

	CreateCentroids();

	return true;
}

void CPathSearch::CreateCentroids()
{
	//build centroids

	int indexCount = (int)m_nIndices.size();
	//build triangles and centroids
	for (int currentIndex = 0; currentIndex < indexCount; currentIndex += 3)
	{
		DirectX::XMFLOAT3 centroid = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

		float checkX = (m_vVertex[m_nIndices[currentIndex]].x + m_vVertex[m_nIndices[currentIndex + 1]].x + m_vVertex[m_nIndices[currentIndex + 2]].x) / 3;
		float checkY = (m_vVertex[m_nIndices[currentIndex]].y + m_vVertex[m_nIndices[currentIndex + 1]].y + m_vVertex[m_nIndices[currentIndex + 2]].y) / 3;
		float checkZ = (m_vVertex[m_nIndices[currentIndex]].z + m_vVertex[m_nIndices[currentIndex + 1]].z + m_vVertex[m_nIndices[currentIndex + 2]].z) / 3;

		centroid.x = roundf(checkX * 100) / 100;
		centroid.y = 0.0f;
		centroid.z = roundf(checkZ * 100) / 100;

		TTriangle myTri;
		myTri.centroid = centroid;
		myTri.index[0] = m_nIndices[currentIndex];
		myTri.index[1] = m_nIndices[currentIndex + 1];
		myTri.index[2] = m_nIndices[currentIndex + 2];

		myTri.radius = abs(Length(m_vVertex[m_nIndices[currentIndex]] - centroid));

		m_tTriangles.push_back(myTri);

	}
}

bool CPathSearch::ProcessNode(FbxNode* pNode, CMesh& cMesh)
{
	size_t attributeCount = pNode->GetNodeAttributeCount();

	for (size_t i = 0; i < attributeCount; i++)
	{
		FbxNodeAttribute* fbxAttr = pNode->GetNodeAttributeByIndex(i);

		if (fbxAttr->GetAttributeType() && fbxAttr->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			LoadMesh(pNode->GetMesh(), cMesh);
		}
	}

	size_t childCount = pNode->GetChildCount();
	for (size_t i = 0; i < childCount; i++)
	{
		if (!ProcessNode(pNode->GetChild(i), cMesh))
		{
//#ifdef _DEBUG
//			// cout << "Processing the node's children " << i << " has FAILED.\n";
//			FBXSDK_printf("[PathSearch::ProcessNode] Processing the node's children: %i has FAILED\n", i);
//#endif
			return false;
		}
	}

	return true;
}

void CPathSearch::Initialize()
{
	int size = (int)m_tTriangles.size();
	for (int i = 0; i < size; i++)
	{
		TTile* myTile = new TTile();
		myTile->centroid = m_tTriangles[i].centroid;

		myTile->weight = 1;

		myTile->triangleIndex = i;

		TSearchNode* newNode = new TSearchNode();
		newNode->data = myTile;

		m_pathNodes[i] = newNode;
	}

	//find connections between centroids
	size_t maxTri = m_tTriangles.size();

	for (size_t curTri = 0; curTri < maxTri; curTri++)
	{
		TSearchNode* current = m_pathNodes.find(curTri)->second;

		unsigned int currT[3] = { m_tTriangles[curTri].index[0], m_tTriangles[curTri].index[1], m_tTriangles[curTri].index[2] };

		int counter = 0;
		for (size_t nextTri = 0; nextTri < maxTri; nextTri++)
		{
			if (curTri == nextTri)
				continue;
			bool check[3] = { false, false, false };

			unsigned int otherT[3] = { m_tTriangles[nextTri].index[0], m_tTriangles[nextTri].index[1], m_tTriangles[nextTri].index[2] };

			int sharedV[2] = { -1, -1 };

			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					if (currT[i] == otherT[j])
					{
						check[i] = true;
						if (sharedV[0] == -1)
							sharedV[0] = i;
						else
							sharedV[1] = i;
						break;
					}
				}
			}

			bool isOk = false;
			int count = 0;
			for (int i = 0; i < 3; i++)
			{
				if (check[i] == true)
					count++;
				if (count == 2)
				{
					isOk = true;
					break;
				}
			}


			//shared edges
			if (isOk == true)
			{

				auto finder = m_pathNodes.find(nextTri);

				TSearchNode* next = finder->second;

				TEdge* newEdge = new TEdge;
				newEdge->endPoint = next;

				//shared tho vertices
				DirectX::XMFLOAT3 hit0, hit1;
				hit0 = m_vVertex[m_tTriangles[curTri].index[sharedV[0]]];
				hit1 = m_vVertex[m_tTriangles[curTri].index[sharedV[1]]];

				//find which one is 
				DirectX::XMFLOAT3 p0, p1, p2;

				p0 = m_vVertex[m_tTriangles[curTri].index[0]];
				p1 = m_vVertex[m_tTriangles[curTri].index[1]];
				p2 = m_vVertex[m_tTriangles[curTri].index[2]];


				if ((EqualFloat(p0, hit0) && EqualFloat(p1, hit1)) || (EqualFloat(p0, hit1) && EqualFloat(p1, hit0)))
					newEdge->edgeEdge = 0;
				else if ((EqualFloat(p1, hit0) && EqualFloat(p2, hit1)) || (EqualFloat(p1, hit1) && EqualFloat(p2, hit0)))
					newEdge->edgeEdge = 1;
				else if((EqualFloat(p2, hit0) && EqualFloat(p0, hit1)) || (EqualFloat(p2, hit1) && EqualFloat(p0, hit0)))
					newEdge->edgeEdge = 2;

				newEdge->cost = 1.0f;
				current->connections.push_back(newEdge);
				counter++;

				if (counter == 3)
					break;
			}
		}
	}
//#ifdef _DEBUG
//	FBXSDK_printf("[PathSearch::initialize] %i nodes created in the NavMesh\n", m_pathNodes.size());
//	FBXSDK_printf("[PathSearch::initialize] Creating look up table\n");
//#endif
}

void CPathSearch::CleanUpFBX(FbxScene* pScene, FbxManager* pManager)
{
	if (pScene)
	{
		pScene->Destroy(true);
		pScene = nullptr;
	}
	if (pManager)
	{
		pManager->GetIOSettings()->Destroy(true);
		pManager->Destroy();
		pManager = nullptr;
	}
}

bool CPathSearch::ImportFBXData(FbxManager*& prFbxManager, FbxScene*& prScene, std::string szFileName)
{
#pragma region Prepare the FBXSDK
	if (!InitializeSdkObjects(prFbxManager, prScene))
	{
		// cout << "[Load] Initializing the SDK Objects has FAILED.\n";
		//FBXSDK_printf("[Load] Intializing the SDK Objects has FAILED.\n");

		prScene->Destroy(true);
		prScene = nullptr;
		prFbxManager->GetIOSettings()->Destroy(true);
		prFbxManager->Destroy();
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
		//FBXSDK_printf("[Load] Creating the FbxImporter has FAILED.\n");

		fbxImporter->Destroy();
		fbxImporter = nullptr;

		prScene->Destroy(true);
		prScene = nullptr;
		prFbxManager->GetIOSettings()->Destroy(true);
		prFbxManager->Destroy();
		prFbxManager = nullptr;

		return false;
	}
#pragma endregion

#pragma region Initialize the FBXImporter
	if (!fbxImporter->Initialize(szFileName.c_str(), -1, prFbxManager->GetIOSettings()))
	{
		FbxStatus status = fbxImporter->GetStatus();

		// cout << "[Load] Initializing the FBXImporter has FAILED.\n";
		//FBXSDK_printf("[Load] Initializing the FBXImporter has FAILED.\n");

		fbxImporter->Destroy();
		fbxImporter = nullptr;

		prScene->Destroy(true);
		prScene = nullptr;
		prFbxManager->GetIOSettings()->Destroy(true);
		prFbxManager->Destroy();
		prFbxManager = nullptr;

		return false;
	}
#pragma endregion

#pragma region Load the contents from the file
	if (!fbxImporter->Import(prScene))
	{
		// cout << "[Load] Importing the FBXScene has FAILED.\n";
		//FBXSDK_printf("[Load] Importing the FBXScene has FAILED.\n");

		fbxImporter->Destroy();
		fbxImporter = nullptr;

		prScene->Destroy(true);
		prScene = nullptr;
		prFbxManager->GetIOSettings()->Destroy(true);
		prFbxManager->Destroy();
		prFbxManager = nullptr;

		return false;
	}
#pragma endregion

	// Destroy the importer because we don't need it anymore
	fbxImporter->Destroy();

	FbxNode* rootNode = prScene->GetRootNode();
	if (!rootNode)
	{
		// cout << "[Load] The FbxScene's root node is NULL!\n";
		//FBXSDK_printf("[Load] The FbxScene's root node is NULL!\n");

		fbxImporter->Destroy();
		fbxImporter = nullptr;

		prScene->Destroy(true);
		prScene = nullptr;
		prFbxManager->GetIOSettings()->Destroy(true);
		prFbxManager->Destroy();
		prFbxManager = nullptr;

		return false;
	}

	return true;
}

bool CPathSearch::LoadLocalModelFBX(std::string szFileName)
{
	std::string localFile = PathFindFileNameA(szFileName.c_str());
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str()));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".fbxbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".fbxbin"));

	if (!PathFileExistsA(fileName.c_str()))
		return false;

	std::fstream file(fileName, std::ios_base::in | std::fstream::binary);

	if (file.is_open() && file.good())
	{
		//FBXSDK_printf("Reading %s's model information\n", fileName.c_str());

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

		if (ImportFBXData(fbxManager, fbxScene, szFileName))
		{
			FbxImporter* fbxImporter = NULL;

			fbxImporter = FbxImporter::Create(fbxManager, "");

			if (fbxImporter)
			{
				if (fbxImporter->Initialize(szFileName.c_str(), -1, fbxManager->GetIOSettings()))
				{
					// Grab the timestamp
					FbxIOFileHeaderInfo* headerInfo = fbxImporter->GetFileHeaderInfo();
					fbxTimeStamp = headerInfo->mCreationTimeStamp;
				}
				fbxImporter->Destroy();
				fbxImporter = nullptr;
			}
		}

		CleanUpFBX(fbxScene, fbxManager);
#endif

		//We found te fbx file, make sure we're up to date
		if (fbxTimeStamp.mYear != 0)
		{
			m_lDate = fbxTimeStamp;

			// Because fuck overloading operators am I right? Probably not.
			if ((fbxTimeStamp.mDay != timeStamp.mDay) || (fbxTimeStamp.mHour != timeStamp.mHour) || (fbxTimeStamp.mSecond != timeStamp.mSecond) ||
				(fbxTimeStamp.mMinute != timeStamp.mMinute) || (fbxTimeStamp.mMonth != timeStamp.mMonth) || (fbxTimeStamp.mYear != timeStamp.mYear))
			{
				//FBXSDK_printf("The timestamp for %s didn't match up with the fbx file. Creating a new one.\n", fileName.c_str());
				return false;
			}
		}
		//We didn't find the fbx file, assume we're up to date
		else
		{
			m_lDate = timeStamp;
		}

		long offsetForAnim;
		file.read((char*)&offsetForAnim, sizeof(long));

		unsigned int indexCount;
		// Read index count
		file.read((char*)&indexCount, sizeof(unsigned int));

		// Read indices

		m_nIndices.resize(indexCount);

		for (unsigned int i = 0; i < indexCount; i++)
		{
			file.read((char*)&m_nIndices[i], sizeof(unsigned int));
		}

		// Read vertex count
		unsigned int vertCount;
		file.read((char*)&vertCount, sizeof(unsigned int));

		m_vVertex.resize(vertCount);

		// Read vertices
		for (unsigned int i = 0; i < vertCount; i++)
		{
			file.read((char*)&m_vVertex[i], sizeof(DirectX::XMFLOAT3));
		}

		file.clear();
		file.close();
	}

	CreateCentroids();

	return true;
}

bool CPathSearch::SaveLocalModelFBX(std::string szFileName)
{
	std::string localFile = PathFindFileNameA(szFileName.c_str());
	//PathRemoveExtensionA(const_cast<char*>(localFile.c_str()));
	CustomPathFuncs::CustomPathRemoveExtension(localFile);

	std::string fileName = MODELSPATH + localFile;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".fbxbin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".fbxbin"));

	// Create the local fbxbin to allow loading times to be faster
	std::fstream file(fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	if (file.is_open() && file.good())
	{
		//FBXSDK_printf("Creating %s's model info\n", fileName.c_str());

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
			//FBXSDK_printf("[Load] Creating the FbxImporter has FAILED.\n");

			fbxImporter->Destroy();
			fbxImporter = nullptr;

			CleanUpFBX(fbxScene, fbxManager);

			return false;
		}

		if (!fbxImporter->Initialize(szFileName.c_str(), -1, fbxManager->GetIOSettings()))
		{
			FbxStatus status = fbxImporter->GetStatus();

			// cout << "[Load] Initializing the FBXImporter has FAILED.\n";
			//FBXSDK_printf("[Load] Initializing the FBXImporter has FAILED.\n");

			fbxImporter->Destroy();
			fbxImporter = nullptr;

			CleanUpFBX(fbxScene, fbxManager);

			return false;
		}

		// Grab the timestamp
		FbxIOFileHeaderInfo* headerInfo = fbxImporter->GetFileHeaderInfo();
		file.write((char*)&headerInfo->mCreationTimeStamp, sizeof(FbxLocalTime));

		CleanUpFBX(fbxScene, fbxManager);

		char temp[5] = "TEMP";

		file.write(temp, 4);
		//delete[] temp;

		unsigned int indexCount = m_nIndices.size();
		// Write index count
		file.write((char*)&indexCount, sizeof(unsigned int));

		// Write indices

		for (unsigned int i = 0; i < indexCount; i++)
		{
			file.write((char*)&m_nIndices[i], sizeof(unsigned int));
		}

		// Write vertex count
		unsigned int vertCount = m_vVertex.size();
		file.write((char*)&vertCount, sizeof(unsigned int));

		// Write vertices
		for (unsigned int i = 0; i < vertCount; i++)
		{
			file.write((char*)&m_vVertex[i], sizeof(DirectX::XMFLOAT3));
		}

		file.close();
	}
	else
		return false;

	return true;
}

//************************************ FBX END ******************************

int CPathSearch::Enter(int start, int end) // , int currentStart)
{
	pair<float, float> filter;
	//goal node

	//move to the closest centroid
	//int hello;
	//GetClosestTriangle(end, hello);

	auto finder = m_pathNodes.find(end);

	if (m_pGoalTile != nullptr)
	{
		delete m_pGoalTile;
		m_pGoalTile = nullptr;
	}

	//if (finder->second == nullptr)
	//return - 1;
	m_pGoalTile = new TSearchNode;
	m_pGoalTile->data = finder->second->data;
	m_pGoalTile->connections = finder->second->connections;

	//start node

	//GetClosestTriangle(start, hello);

	finder = m_pathNodes.find(start);

	if (m_pFirstTile != nullptr)
	{
		delete m_pFirstTile;
		m_pFirstTile = nullptr;
	}

	//if (finder->second == nullptr)
	//return;
	m_pFirstTile = new TSearchNode;
	m_pFirstTile->data = finder->second->data;
	m_pFirstTile->connections = finder->second->connections;
	CPlannerNode* firstNode = new CPlannerNode;
	firstNode->SetVertex(finder->second);
	firstNode->SetParent(nullptr);
	m_myPriority.push(firstNode);

	pair<float, float>  estimaS;
	estimaS.first = firstNode->GetVertex()->data->centroid.x;
	estimaS.second = firstNode->GetVertex()->data->centroid.z;
	m_pGoalHeuristic.first = m_pGoalTile->data->centroid.x;
	m_pGoalHeuristic.second = m_pGoalTile->data->centroid.z;
	float myHeu = Estimate(estimaS, m_pGoalHeuristic);
	firstNode->SetHeuristic(myHeu);
	firstNode->SetGiven(0);
	firstNode->SetFinalCost(firstNode->GetHeuristic() * m_fhWeight);

	m_bFinish = false;

	//add to visited
	m_Visited[finder->first] = firstNode;
	Update();

	int result = GetSolution();

	Exit();

	return result;
}

bool CPathSearch::IsDone() const
{

	if (m_bFinish == true)
		return true;
	return false;
}

void CPathSearch::Update()
{
	while (!m_myPriority.empty())
	{
		CPlannerNode* current = m_myPriority.front();

		if (current->GetVertex()->data->triangleIndex == m_pGoalTile->data->triangleIndex)
		{
			m_bFinish = true;
			break;
		}

		m_myPriority.pop();
		size_t mySiz = current->GetVertex()->connections.size();

		for (size_t i = 0; i < mySiz; i++)
		{
			TSearchNode* successor = current->GetVertex()->connections[i]->endPoint;

			float tempGiven = current->GetGiven() + (m_tTriangles[current->GetVertex()->data->triangleIndex].radius * 2);

			if (m_Visited[successor->data->triangleIndex] != NULL)
			{
				if (tempGiven < m_Visited[successor->data->triangleIndex]->GetGiven())
				{
					CPlannerNode* successorNode = m_Visited[successor->data->triangleIndex];
					m_myPriority.remove(successorNode);

					successor->data->tileEdge = current->GetVertex()->connections[i]->edgeEdge;
					//successor->data->edge2 = current->GetVertex()->edges[i]->exitEdge2;

					successorNode->SetGiven(tempGiven);
					successorNode->SetFinalCost(tempGiven + successorNode->GetHeuristic() * m_fhWeight);
					successorNode->SetParent(current);
					m_myPriority.push(successorNode);
				}
			}
			else
			{
				CPlannerNode* successorNode = new CPlannerNode(successor);
				successorNode->SetParent(current);
				successorNode->SetGiven(tempGiven);

				pair<float, float>  estimaS;
				estimaS.first = successor->data->centroid.x;
				estimaS.second = successor->data->centroid.z;
				float myHeu = Estimate(estimaS, m_pGoalHeuristic);

				successorNode->SetHeuristic(myHeu);
				successorNode->SetFinalCost(tempGiven + myHeu * m_fhWeight);

				successorNode->GetVertex()->data->tileEdge = current->GetVertex()->connections[i]->edgeEdge;
				//successorNode->GetVertex()->data->edge2 = current->GetVertex()->edges[i]->exitEdge2;

				m_Visited[successor->data->triangleIndex] = successorNode;
				m_myPriority.push(successorNode);
			}
			if (m_bShuttingDown == true)
				break;
		}
		if (m_bShuttingDown == true)
			break;
	}
}

void CPathSearch::Exit()
{
	if (!m_Visited.empty())
	{
		auto it = m_Visited.begin();
		while (it != m_Visited.end())
		{
			delete (it->second);
			++it;
			if (m_bShuttingDown == true)
				break;
		}
	}

	m_Visited.clear();
	m_myPriority.clear();
}

void CPathSearch::Shutdown()
{
	if (m_bShuttingDown == false)
	{
		m_bShuttingDown = true;
		Exit();
		if (!m_pathNodes.empty())
		{
			auto it = m_pathNodes.begin();
			while (it != m_pathNodes.end())
			{
				size_t edgeSize = it->second->connections.size();
				for (size_t i = 0; i < edgeSize; i++)
				{
					TEdge* aux = it->second->connections.back();
					it->second->connections.pop_back();
					delete aux;
				}
				TTile* helper = it->second->data;
				delete helper;
				delete (it->second);
				++it;
			}
		}
		m_pathNodes.clear();

		if (m_pGoalTile != nullptr)
			delete m_pGoalTile;
		m_pGoalTile = nullptr;

		if (m_pFirstTile != nullptr)
			delete m_pFirstTile;
		m_pFirstTile = nullptr;

		if (nullptr != m_gInstance)
		{
			delete m_gInstance;
		}
		m_gInstance = nullptr;
	}
}

int CPathSearch::GetSolution() const
{
	std::vector<TPathResult> temp;
	if (!m_myPriority.empty())
	{

		CPlannerNode* mym_pFirstTile = m_myPriority.front();
		while (mym_pFirstTile != nullptr)
		{
			TPathResult aux;
			aux.edgeIndex = mym_pFirstTile->GetVertex()->data->tileEdge;
			//aux.edge2 = mym_pFirstTile->GetVertex()->data->edge2;
			aux.triangleIndex = mym_pFirstTile->GetVertex()->data->triangleIndex;

			temp.push_back(aux);
			mym_pFirstTile = mym_pFirstTile->GetParent();
		}
	}
	
	return temp[temp.size() - 2].edgeIndex;
}

/*static*/ CPathSearch* CPathSearch::GetInstance(void)
{
	if (m_gInstance == nullptr)
		m_gInstance = new CPathSearch;
	return m_gInstance;

}

int CPathSearch::GetClosestVertex(DirectX::XMFLOAT3 myPoint)const
{
	size_t size = m_vVertex.size();
	int resulte;

	float smallest = FLT_MAX;
	for (size_t i = 0; i < size; i++)
	{
		float test = Length(m_vVertex[i] - myPoint);

		if (smallest > test)
		{
			smallest = test;
			resulte = i;
		}
	}
	return resulte;
}

float CPathSearch::Estimate(pair<float, float> start, pair<float, float> goal)
{
	float x, y;
	x = start.first - goal.first;
	y = start.second - goal.second;
	return (sqrt(x * x + y * y));
}

bool CPathSearch::PointInTriangle(DirectX::XMFLOAT3 myPosition, int &myTriangle)
{
	myTriangle = -1;
	DirectX::XMFLOAT3 p0, p1, p2;
	//OLD CODE
	//Had to remove this, the do while loop for finding a random point was an infinate loop using this code
	//if (myTriangle >= 0) //initial position
	//{

	//	p0 = m_vVertex[m_tTriangles[myTriangle].index[0]];
	//	p1 = m_vVertex[m_tTriangles[myTriangle].index[1]];
	//	p2 = m_vVertex[m_tTriangles[myTriangle].index[2]];

	//	//half space test with the current triangle
	//	if (PointToTriangleTest(myPosition, p0, p1, p2) == true)
	//	{
	//		return true;
	//	}
	//}
	// this determines that the point is inside that triangle
	//find in all triangles
	for (size_t i = 0; i < m_tTriangles.size(); i++)
	{
		//if (i == myTriangle)
		//	continue;

		p0 = m_vVertex[m_tTriangles[i].index[0]];
		p1 = m_vVertex[m_tTriangles[i].index[1]];
		p2 = m_vVertex[m_tTriangles[i].index[2]];

		//half space test with the current triangle
		if (PointToTriangleTest(myPosition, p0, p1, p2) == true)
		{
			myTriangle = i;
			return true;
		}
	}

	myTriangle = -1;
	return false;
}

float DistanceFromEdge(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 myPoint)
{
	DirectX::XMFLOAT3 closest;
	DirectX::XMFLOAT3 edge0 = p1 - p0;
	DirectX::XMFLOAT3 norm = Normalize(edge0);
	DirectX::XMFLOAT3 V = myPoint - p0;
	float dote = Dot(norm, V);

	if (dote < 0)
	{
		closest = p0;
	}
	else if (dote > Length(p1 - p0))
	{
		closest = p1;
	}
	else
	{
		closest = p0 + (norm * dote);
	}

	return Length(closest - myPoint);
}

void CPathSearch::GetClosestTriangle(DirectX::XMFLOAT3 myPoint, int& triangle)
{
	int help = -1;
	PointInTriangle(myPoint, help);

	if (help != -1)
	{
		triangle = help;
		return;
	}

	//if point is outside the navmesh

	size_t size = m_tTriangles.size();
	float smallest = FLT_MAX;
	for (size_t i = 0; i < size; i++)
	{
		DirectX::XMFLOAT3 p0, p1, p2;
		float d[3];
		p0 = m_vVertex[m_tTriangles[i].index[0]];
		p1 = m_vVertex[m_tTriangles[i].index[1]];
		p2 = m_vVertex[m_tTriangles[i].index[2]];

		d[0] = DistanceFromEdge(p0, p1, myPoint);
		d[1] = DistanceFromEdge(p1, p2, myPoint);
		d[2] = DistanceFromEdge(p2, p0, myPoint);

		for (int loop = 0; loop < 3; loop++)
		{
			if (smallest > d[loop])
			{
				smallest = d[loop];
				triangle = i;
			}
		}
	}
}

bool CPathSearch::PointToTriangleTest(DirectX::XMFLOAT3 myPosition, DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
{
	DirectX::XMVECTOR position = XMLoadFloat3(&myPosition);
	DirectX::XMVECTOR point0 = XMLoadFloat3(&p0);
	DirectX::XMVECTOR point1 = XMLoadFloat3(&p1);
	DirectX::XMVECTOR point2 = XMLoadFloat3(&p2);
	DirectX::XMVECTOR edge = DirectX::XMVectorSubtract(point1, point0);
	DirectX::XMVECTOR normal = DirectX::XMVector3Cross(edge, DirectX::XMVectorSubtract(point2, point0));

	// edge 0
	DirectX::XMVECTOR positionToPoint = DirectX::XMVectorSubtract(position, point0);
	if (DirectX::XMVectorGetW(DirectX::XMVector3Dot(normal, DirectX::XMVector3Cross(edge, positionToPoint))) < 0)
		return false; // P is on the right side

	// edge 1
	edge = DirectX::XMVectorSubtract(point2, point1);
	positionToPoint = DirectX::XMVectorSubtract(position, point1);
	if (DirectX::XMVectorGetW(DirectX::XMVector3Dot(normal, DirectX::XMVector3Cross(edge, positionToPoint))) < 0)
		return false; // P is on the right side

	// edge 2
	edge = DirectX::XMVectorSubtract(point0, point2);
	positionToPoint = DirectX::XMVectorSubtract(position, point2);
	if (DirectX::XMVectorGetW(DirectX::XMVector3Dot(normal, DirectX::XMVector3Cross(edge, positionToPoint))) < 0)
		return false; // P is on the right side;

	return true; // this ray hits the triangle
}

DirectX::XMFLOAT3 CPathSearch::RandomPoint()
{
	size_t check = m_tTriangles.size();
	if (0 == check)
	{
		return DirectX::XMFLOAT3{ 0, 0, 0 };
	}

	int number = rand() % check;
	return m_tTriangles[number].centroid;
}

void CPathSearch::CreateLookUpTable(void)
{
	//reserve all memory
	size_t size = m_tTriangles.size();
	m_vLookUpTable.resize(size);

	for (size_t i = 0; i < size; i++)
	{
		m_vLookUpTable[i].resize(size);

		for (size_t reset = 0; reset < size; reset++)
		{
			m_vLookUpTable[i][reset] = 4;
		}
	}

	//create stuff for A*
	m_fhWeight = 1.2f;
	Initialize();

	//try loading the table
	if (true == LoadLookUpTable())
	{
		return;
	}

	//start with the magic of creating the table
	for (size_t start = 0; start < size; start++)
	{
		for (size_t end = 0; end < size; end++)
		{
			if (start == end)
			{
				//m_vLookUpTable[start][end] = 4;
				continue;
			}

			m_vLookUpTable[start][end] = Enter(start, end);
		}
	}

	//save the table
	SaveLookUpTable();

}

bool CPathSearch::SaveLookUpTable(void)
{
	size_t size = m_tTriangles.size();
	std::ofstream saveFile;

	std::string fileName = TEXTFILESPATH + m_sModelName;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".bin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".bin"));

	saveFile.open(fileName, ios::binary | ios::out | ios::trunc);

	saveFile.write((char*)&(m_lDate), sizeof(FbxLocalTime));

	if (saveFile.is_open())
	{
		for (size_t start = 0; start < size; start++)
		{
			for (size_t end = 0; end < size; end++)
			{
				saveFile.write((char*)&(m_vLookUpTable[start][end]), sizeof(int));
			}
		}
		saveFile.close();
	}
	else
	{
		//printf("[CPathSearch::SaveLookUpTable] file didn't save \n");
		saveFile.close();
		return false;
	}

	return true;
}

bool CPathSearch::LoadLookUpTable(void)
{

	size_t size = m_tTriangles.size();
	std::ifstream loadFile;

	std::string fileName = TEXTFILESPATH + m_sModelName;
	//PathAddExtensionA(const_cast<char*>(fileName.c_str()), ".bin");
	CustomPathFuncs::CustomPathAddExtension(fileName, std::string(".bin"));

	loadFile.open(fileName, ios::binary | ios::in);

	if (loadFile.is_open())
	{
		FbxLocalTime help;
		loadFile.read((char*)&(help), sizeof(FbxLocalTime));

		if ((m_lDate.mDay != help.mDay) || (m_lDate.mHour != help.mHour) || (m_lDate.mSecond != help.mSecond) ||
			(m_lDate.mMinute != help.mMinute) || (m_lDate.mMonth != help.mMonth) || (m_lDate.mYear != help.mYear))
		{
			//FBXSDK_printf("The timestamp for %s didn't match up with the look up table file. Creating a new one.\n", fileName.c_str());
			return false;
		}

		for (size_t start = 0; start < size; start++)
		{
			for (size_t end = 0; end < size; end++)
			{
				loadFile.read((char*)&(m_vLookUpTable[start][end]), sizeof(int));
			}
		}
		loadFile.close();
	}
	else
	{
		//printf("[CPathSearch::loadLookUpTable] file didn't load \n");
		loadFile.close();
		return false;
	}

	return true;
}

CPlannerNode::CPlannerNode(TSearchNode* main)
{
	SetVertex(main);
	m_Parent = nullptr;
}

bool CPathSearch::EqualFloat(DirectX::XMFLOAT3 one, DirectX::XMFLOAT3 two)
{

	if (one.x == two.x && one.y == two.y && one.z == two.z)
		return true;
	return false;
}

int CPathSearch::FindExitEdge(int start, int end, DirectX::XMFLOAT3 startPos,  DirectX::XMFLOAT3 goalPos, TTriangleEdge& result)
{
	int nextEnd, nextStart;
	if (-1 == end)
		GetClosestTriangle(goalPos, nextEnd);
	else
		nextEnd = end;

	if (-1 == start)
		GetClosestTriangle(startPos, nextStart);
	else
		nextStart = start;

	int help = m_vLookUpTable[nextStart][nextEnd];

	if (help < 3 && help >= 0)
	{
		if (0 == help)
		{
			result.vertexOne = m_vVertex[m_tTriangles[nextStart].index[0]];
			result.vertexTwo = m_vVertex[m_tTriangles[nextStart].index[1]];

			result.edge = 0;
		}
		else if (1 == help)
		{
			result.vertexOne = m_vVertex[m_tTriangles[nextStart].index[1]];
			result.vertexTwo = m_vVertex[m_tTriangles[nextStart].index[2]];

			result.edge = 1;
		}
		else if(2 == help)
		{
			result.vertexOne = m_vVertex[m_tTriangles[nextStart].index[2]];
			result.vertexTwo = m_vVertex[m_tTriangles[nextStart].index[0]];

			result.edge = 2;
		}
	}

	return help;
}

void CPathSearch::RenderMyTriangle(int triangle)
{
	DirectX::XMFLOAT3 p0, p1, p2;

	if (0 <= triangle)
	{
		p0 = m_vVertex[m_tTriangles[triangle].index[0]];
		p1 = m_vVertex[m_tTriangles[triangle].index[1]];
		p2 = m_vVertex[m_tTriangles[triangle].index[2]];

		CDebugRenderer::GetReference().LineRenderer(p0, p1);
		CDebugRenderer::GetReference().LineRenderer(p1, p2);
		CDebugRenderer::GetReference().LineRenderer(p2, p0);
	}
}

void CPathSearch::HandleLoading(bool isCampaign)
{
	Exit();
	//clear the stuff just in case

	if (!m_pathNodes.empty())
	{
		auto it = m_pathNodes.begin();
		while (it != m_pathNodes.end())
		{
			size_t edgeSize = it->second->connections.size();
			for (size_t i = 0; i < edgeSize; i++)
			{
				TEdge* aux = it->second->connections.back();
				it->second->connections.pop_back();
				delete aux;
			}
			TTile* helper = it->second->data;
			delete helper;
			delete (it->second);
			++it;
		}
	}
	m_pathNodes.clear();

	if (m_pGoalTile != nullptr)
		delete m_pGoalTile;
	m_pGoalTile = nullptr;

	if (m_pFirstTile != nullptr)
		delete m_pFirstTile;
	m_pFirstTile = nullptr;

	if (true == isCampaign)
	{
		assert(LoadNavMesh(std::string(MODELSPATH + std::string("FinalKungPaoNavMesh.fbx"))) == true && "[CPathSearch] Navmesh hasn't been loaded!!");
	}
	else
	{
		assert(LoadNavMesh(std::string(MODELSPATH + std::string("FinalTeriyakiNavMesh.fbx"))) == true && "[CPathSearch] Navmesh hasn't been loaded!!");
	}
	CreateLookUpTable();
}