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

#ifndef _CASSETMANAGER_H_
#define _CASSETMANAGER_H_

#include "..\Core\stdafx.h"
#include "..\Core\EnumerationHolder.h"

#include <fbxsdk.h>

#include "..\Animation Manager\AnimationManager.h"
#include "..\Animation Manager\Animation.h"
#include "..\Animation Manager\Deformer.h"
#include "..\Animation Manager\Interpolator.h"
#include "..\Animation Manager\Mesh.h"

typedef std::vector<std::pair<FbxNode*, FbxNodeAttribute*>> JointList;

struct TCollider;
struct TAABB;

class CEntity;
class CGameplayState;
class CLevel;

class CAssetManager
{
public:
	CAssetManager( )
	{
	}
	~CAssetManager( );

	/*****************************************************************
	* TTexture:				Holds onto ID3D11ShaderResourceViews for each
	*						texture (diffuse, normal, misc, etc)
	*****************************************************************/
	struct TTexture
	{
		TTexture( )
		{
			for (int i = 0; i < _countof(m_pDiffuseTexture); i++)
			{
				m_pDiffuseTexture[i] = NULL;
			}
		}

		CComPtr<ID3D11ShaderResourceView> m_pDiffuseTexture[3];
		CComPtr<ID3D11ShaderResourceView> m_pNormalTexture = NULL;
		CComPtr<ID3D11ShaderResourceView> m_pMiscTexture = NULL;
	};

	/*****************************************************************
	* TModel:				Holds onto the vertex and index buffers
	*						as well as the index count and texture
	*						information to pass to the renderer
	*****************************************************************/
	struct TModel
	{
		ID3D11Buffer* m_pVertexBuffer = NULL;
		ID3D11Buffer* m_pIndexBuffer = NULL;
		size_t m_unIndexCount;
		TTexture m_tTex;
		CMesh m_cMesh;

		TModel( ) = default;

		TModel(const TModel& rhs);
	};

	struct TAnimation
	{
		CAnimation<DirectX::XMFLOAT4X4> m_pAnim;
		//CMesh m_pMesh;
		TModel* m_pModel;
		//CDeformer m_pDeformer;
		//CInterpolator<DirectX::XMFLOAT4X4> m_pInterpolator;
	};

	/*****************************************************************
	* LoadFBX():		Blackboxed function to loading in a .fbx file
	*					and storing it into the asset manager while
	*					loading in all the information needed to use
	*					in game, and giving the current .fbx file a tag
	*					to be associated with.
	*
	* Ins:				szFileName
	*					szTag
	*
	* Returns:			bool
	*
	* Mod. Date:		05/15/2015
	* Mod. Initials:	JPM
	*****************************************************************/
	bool LoadFBX(std::string szFileName,
				 std::string szTag);

	void UnloadModel(std::string szTag);

	void CleanUpFBX(FbxScene* pScene,
					FbxManager* pManager);

	/*****************************************************************
	* LocalLocalModelFBX():		Blackboxed function to loading in a local .fbxbin
	*							file for meshes/models and speed up loading times
	*
	* Ins:				szFileName
	*					szTag
	*
	* Returns:			bool
	*
	* Mod. Date:		06/09/2015
	* Mod. Initials:	JPM
	*****************************************************************/
	bool LoadLocalModelFBX(std::string szFileName, std::string szTag);

	/*****************************************************************
	* LocalLocalAnimFBX():		Blackboxed function to loading in a local .fbxbin
	*							file fora animations for faster loading times
	*
	* Ins:				szFileName
	*					szTag
	*
	* Returns:			bool
	*
	* Mod. Date:		06/09/2015
	* Mod. Initials:	JPM
	*****************************************************************/
	bool LoadLocalAnimFBX(std::string szFileName, std::string szTag, TModel& tModel);

	/*****************************************************************
	* SaveLocalModelFBX():	Blackboxed function to creating and writing to a
	*							local .fbxbin file for models/meshes to speed up
	*							loading times.
	*
	* Ins:				szFileName
	*					szTag
	*
	* Returns:			bool
	*
	* Mod. Date:		06/09/2015
	* Mod. Initials:	JPM
	*****************************************************************/
	bool SaveLocalModelFBX(std::string szFileName, std::string szTag, CMesh& cMesh);

	/*****************************************************************
	* SaveLocalAnimFBX():	Blackboxed function to creating and writing to a
	*							local .fbxbin file for animations and their animation
	*							information to improve loading times
	*
	* Ins:				szFileName
	*					szTag
	*
	* Returns:			bool
	*
	* Mod. Date:		06/09/2015
	* Mod. Initials:	JPM
	*****************************************************************/
	bool SaveLocalAnimFBX(std::string szFileName, std::string szTag, JointList& uniqueJoints);

	/*****************************************************************
	* LoadMapFromFBX():	Blackboxed function to loading in a map and
	*							everything related to it
	*							from an fbx file to where it needs to go.
	*
	* Ins:				szFileName
	*
	* Outs:				pGame
	*
	* Returns:			bool
	*
	* Mod. Date:		06/11/2015
	* Mod. Initials:	MJG
	*****************************************************************/
	bool LoadMapFromFBX(std::string szFileName, CGameplayState* pGame, int ePiece = EMAP_ALL);

	/*****************************************************************
	* GetModelByTag():		Fetches the TModel* needed from the
	*						unordered map via a std::string (name) or tag
	*
	* Ins:					szTag
	*
	* Returns:				TModel*
	*
	* Mod. Date:			05/15/2015
	* Mod. Initials:		JPM
	*****************************************************************/
	__forceinline TModel* GetModelByTag(std::string szTag)
	{
		return m_mModels[szTag];
	}

	__forceinline TAnimation* GetAnimByTag(std::string szTag)
	{
		return m_mAnimations[szTag];
	}

	/*****************************************************************
	* CreateBuffer():		A templated helper function that assists in
	*						creating D3D11 Buffers.
	*
	* Ins:					pDevice
	*						data
	*						bindFlags
	*
	* Outs:					ppBuffer
	*
	* Returns:				bool
	*
	* Mod. Date:			05/15/2015
	* Mod. Initials:		JPM
	*****************************************************************/
	template<typename T>
	static void CreateBuffer(_In_ ID3D11Device* pDevice, T const& data, D3D11_BIND_FLAG bindFlags, _Outptr_ ID3D11Buffer** ppBuffer)
	{
		assert(ppBuffer != 0);

		D3D11_BUFFER_DESC bufferDesc = { 0 };

		bufferDesc.ByteWidth = (UINT)data.size( ) * sizeof(T::value_type);
		bufferDesc.BindFlags = bindFlags;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA dataDesc = { 0 };

		dataDesc.pSysMem = &data.front( );

		HR(pDevice->CreateBuffer(&bufferDesc, &dataDesc, ppBuffer));

		//SetD3DName((*ppBuffer), std::string("Object Created At Line: " + std::to_string(__LINE__) + " File: " + __FILE__).c_str( ));
	}

	static void LoadTexture(const char* _szFile, ID3D11ShaderResourceView** _ppSRV);
	static void LoadTexture(const wchar_t* _szFile, ID3D11ShaderResourceView** _ppSRV);

private:

	__forceinline void ConvertMatrix(const FbxMatrix fbx_matrix, DirectX::XMFLOAT4X4& float4x4)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				float4x4.m[i][j] = (float)fbx_matrix.Get(i, j);
			}
		}

		float4x4._31 = -float4x4._31;
		float4x4._32 = -float4x4._32;
		float4x4._34 = -float4x4._34;

		float4x4._13 = -float4x4._13;
		float4x4._23 = -float4x4._23;
		float4x4._43 = -float4x4._43;
	}

	/*****************************************************************
	* ImportFBXData():		A helper function that loads the fbx data into
	*						the pFBXScene for you
	*
	* Outs:							prFbxManager
	*								prScene
	*
	* Ins:							szFileName
	*
	* Returns:						bool
	*
	* Mod. Date:					06/11/2015
	* Mod. Initials:				MJG
	*****************************************************************/
	bool ImportFBXData(FbxManager*& prFbxManager, FbxScene*& prScene, std::string szFileName, FbxLocalTime* pTimeStamp = nullptr);

	/*****************************************************************
	* StoreModel():				Stores the model in memory as vert/index
	*						buffers. Helper for LoadFBX and such
	*
	* Ins:							pMesh, szFileName, szTag
	*
	* Returns:						bool
	*
	* Mod. Date:					06/11/2015
	* Mod. Initials:				MJG
	*****************************************************************/
	void StoreModel(CMesh* pMesh, std::string szFileName, std::string szTag);

	/*****************************************************************
	* StoreAnimation():				Stores the animation in memory
	*						Helper for LoadFBX and such
	*
	* Ins:							pMesh, vJoints, pScene, szFileName, szTag
	*
	* Returns:						bool
	*
	* Mod. Date:					06/11/2015
	* Mod. Initials:				MJG
	*****************************************************************/
	bool StoreAnimation(CMesh& cMesh, JointList& vJoints, FbxScene* pScene, std::string szFileName, std::string szTag);

	/*****************************************************************
	* InitializeSdkObjects():		Creates and setups the fbx manager
	*								and scene needed to traverse/load
	*								in a .fbx file.
	*
	* Outs:							prFbxManager
	*								prScene
	*
	* Returns:						bool
	*
	* Mod. Date:					05/13/2015
	* Mod. Initials:				JPM
	*****************************************************************/
	bool InitializeSdkObjects(FbxManager*& prFbxManager,
							  FbxScene*& prScene);

	/*****************************************************************
	* ProcessNode():		A recursive function that processes/loads
	*						in a node's mesh, skin, texture and anything
	*						else needed that's attached to itself and it's
	*						children.
	*
	* Ins:					pNode
	*
	* Outs:					cMesh
	*						prModel
	*						vUniqueJoints
	*
	* Returns:				bool
	*
	* Mod. Date:			05/15/2015
	* Mod. Initials:		JPM
	*****************************************************************/
	bool ProcessNode(FbxNode* pNode,
					 CMesh& cMesh,
					 TModel*& prModel,
					 JointList& vUniqueJoints);

	/*****************************************************************
	* ProcessCollissionNode():		A recursive function that processes/loads
	*						in a node's mesh as colliders/entities
	*
	* Ins:					pNode
	*
	* Outs:					vColliders
	*
	* Returns:				bool
	*
	* Mod. Date:			05/15/2015
	* Mod. Initials:		MJG
	*****************************************************************/
	bool ProcessCollisionNode(FbxNode* pNode,
							  std::vector<TAABB*>& vColliders);

	/*****************************************************************
	* ProcessMap():		A recursive function that processes the
	*						high level map nodes
	*
	* Ins:					pNode
	*
	* Outs:					pGameplay
	*
	* Returns:				void
	*
	* Mod. Date:			06/13/2015
	* Mod. Initials:		MJG
	*****************************************************************/
	void ProcessMap(FbxNode* pNode, CGameplayState* pGameplay, CLevel* pLevel,
		std::string szFileName, int nPiece, const FbxLocalTime& tTimeStamp);

	//Save the renderable information as a mapbin
	void SaveMap(std::vector<CEntity*>& vEntities, std::string szFileName, const FbxLocalTime& tTimeStamp);
	//Load in the mapbin
	bool LoadMap(std::string szFileName, CGameplayState* pGameplay, const FbxLocalTime& tTimeStamp);

	//Save the scenery information as a mapbin
	void SaveScenery(std::vector<CEntity*>& vObjects, CGameplayState* pGameplay, std::string szFileName, const FbxLocalTime& tTimeStamp);
	//Load in the mapbin
	bool LoadScenery(std::string szFileName, CGameplayState* pGameplay, const FbxLocalTime& tTimeStamp);

	//Save the cillideable information as a colbin
	void SaveCollision(std::vector<TAABB*>& vColliders, std::string szFileName, const FbxLocalTime& tTimeStamp);
	//Load in the mapbin
	bool LoadCollision(std::string szFileName, CGameplayState* pGameplay, const FbxLocalTime& tTimeStamp);

	//Save the light info in a binary file
	void SaveLights(CLevel* pLevel, std::string szFileName, const FbxLocalTime& tTimeStamp);
	//Load Lights from binary
	bool LoadLights(std::string szFileName, CLevel* pLevel, const FbxLocalTime& tTimeStamp);

	//Save all inanimate information in a binary file
	void SaveInanimates(std::vector<CEntity*>& vObjects, std::string szFileName, const FbxLocalTime& tTimeStamp);
	//Load all inanimate information from a binary file
	bool LoadInanimates(std::string szFileName, CGameplayState* pGameplay, const FbxLocalTime& tTimeStamp);

	/*****************************************************************
	* ProcessScenery():		A function that processes the
	*						scenery in the map
	*
	* Ins:					pNode
	*
	* Outs:					pGameplay
	*
	* Returns:				void
	*
	* Mod. Date:			06/13/2015
	* Mod. Initials:		MJG
	*****************************************************************/
	void ProcessScenery(FbxNode* pNode, CGameplayState* pGameplay, std::vector<CEntity*>& vAddedEntities);

	void ProcessInanimates(FbxNode* pNode, CGameplayState* pGameplay, std::vector<CEntity*>& vAddedEntities);

	//Loads in the Floor Guides
	void ProcessFloorGuides(FbxNode* pNode, CGameplayState* pGameplay);

	/*****************************************************************
	* ProcessMapNode():		A recursive function that processes the
	*						high level map nodes
	*
	* Ins:					pNode
	*
	* Outs:					vInvisiWalls
	*
	* Returns:				bool
	*
	* Mod. Date:			06/13/2015
	* Mod. Initials:		MJG
	*****************************************************************/
	bool ProcessMapNode(FbxNode* pNode, CGameplayState* pGameplay,
						std::vector<CEntity*>& vAddedEntities);

	/*****************************************************************
	* LoadMesh():		Loads in vertices as a collider/entity
	*
	* Ins:					pMesh
	*
	* Outs:					vColliders
	*
	* Returns:				bool
	*
	* Mod. Date:		    05/15/2015
	* Mod. Initials:	    JM
	*****************************************************************/
	bool LoadCollider(FbxMesh* pMesh,
					  std::vector<TAABB*>& vColliders);

	/*****************************************************************
	* LoadMesh():		Loads in vertices, normals, UVs from a
	*					fbx file to a single mesh.
	*
	* Ins:					pMesh
	*
	* Outs:					cMesh
	*
	* Returns:				bool
	*
	* Mod. Date:		    05/13/2015
	* Mod. Initials:	    JM
	*****************************************************************/
	bool LoadMesh(FbxMesh* pMesh,
				  CMesh& cMesh);

	bool LoadSkin(FbxMesh* pMesh,
				  CMesh& cMesh,
				  JointList& vUniqueJoints);

	/*****************************************************************
	* LoadTexture():		Loads in the textures and texture names for
	*						diffuse/normal/misc.
	*
	* Ins:					pMesh
	*						cMesh
	*
	* Outs:					prModel
	*
	* Returns:				bool
	*
	* Mod. Date:		    05/15/2015
	* Mod. Initials:	    JM
	*****************************************************************/
	bool LoadTexture(FbxMesh* pMesh,
					 CMesh& cMesh,
					 TModel*& prModel);

	bool LoadSkelNode(FbxNode* pNode,
					  CMesh& cMesh,
					  JointList& vUniqueJoints,
					  unsigned int& unIndex);

	bool LoadAnim(FbxAnimLayer* pLayer,
				  CAnimation<DirectX::XMFLOAT4X4>& cAnim,
				  TModel*& prModel,
				  JointList& vUniqueJoints);

	bool LoadAnimCurve(FbxAnimCurve* pCurve,
					   std::vector<float>& vKeyTimes);

	std::unordered_map< std::string, TModel* > m_mModels;
	std::unordered_map< std::string, TAnimation*> m_mAnimations;
	std::unordered_map<std::string, std::vector<std::string>> m_mTextureNames;
};

#endif // _CASSETMANAGER_H_
