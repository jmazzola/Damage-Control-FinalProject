/***********************************************
* Filename:			PathSearch.h
* Date:      		05/12/2015
* Mod. Date:		07/10/2015
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:			This class handles the NavMesh loading
setting up and pathfinding algorithm
************************************************/

#ifndef _CPATHSEARCH_H_
#define _CPATHSEARCH_H_

#include "..\..\Core\stdafx.h"
#include "..\..\Animation Manager\Mesh.h"
#include "..\..\Math\vec3.h"
#include "PriorityQueue.h"

#include <fbxsdk.h>
#include <map>
#include <set>
#include <deque>
using namespace std;

//struct TConnection
//{
//	//unsigned short	vertexIndex[2]; //not sure if I need this
//	unsigned int		triangleIndex;

//};

/**************************************
TTriangle:	middle level struct to store information from the fbx

Mod. Date:		05/28/2015
Mod. Initials:	JM
**************************************/
struct TTriangleEdge
{
	DirectX::XMFLOAT3	vertexOne;
	DirectX::XMFLOAT3	vertexTwo;

	int					edge;
};

/**************************************
TPathResult:	needed information for returning the path

Mod. Date:		07/10/2015
Mod. Initials:	JM
**************************************/
struct TPathResult
{
	//edge
	DirectX::XMFLOAT3 vertexOne;
	DirectX::XMFLOAT3 vertexTwo;

	int				edgeIndex;
	//triangle index
	unsigned int		triangleIndex;
};

/**************************************
TTriangle:	middle level struct to store information from the fbx

Mod. Date:		05/28/2015
Mod. Initials:	JM
**************************************/
struct TTriangle
{
	unsigned int			index[3];
	DirectX::XMFLOAT3		centroid;

	//TTriangleEdge			Edges[3];

	float					radius;
};

/**************************************
TTile:	smallest logical piece of the grid

Mod. Date:		05/28/2015
Mod. Initials:	JM
**************************************/
struct TTile
{
	DirectX::XMFLOAT3		centroid;
	unsigned int			weight;
	unsigned int			triangleIndex;

	/*DirectX::XMFLOAT3				vertex1;
	DirectX::XMFLOAT3				vertex2;*/

	int						tileEdge;

};

/**************************************
TSearchNode:	Basic structure to define walkable regions in the navmesh

Mod. Date:		06/09/2015
Mod. Initials:	JM
**************************************/
struct TSearchNode;

struct TEdge
{
	TSearchNode*			endPoint;
	float					cost;

	//DirectX::XMFLOAT3				exit1;
	//DirectX::XMFLOAT3				exit2;

	int						edgeEdge;
};

struct TSearchNode
{
	TTile*				data;
	vector<TEdge*>		connections;
};

/**************************************
KeyHash: needed struct in the unordered map

Mod. Date:		05/23/2015
Mod. Initials:	JM
**************************************/
struct KeyHash {
	 size_t operator()(const unsigned int k) const
	{
		return k;
	}
};

/**************************************
KeyEqual: needed struct in the unordered map

Mod. Date:		05/23/2015
Mod. Initials:	JM
**************************************/
struct KeyEqual {
	bool operator()(const unsigned int lhs, const unsigned int rhs) const
	{
		return lhs == rhs;
	}
};

class CPlannerNode
{
	TSearchNode*		m_Vertex;
	CPlannerNode*		m_Parent;

	//DirectX::XMFLOAT3			m_xmEdge1;
	//DirectX::XMFLOAT3			m_xmEdge2;

	int					m_nPlannerEdge;

	float				m_fHeuristicCost;
	float				m_fGivenCost;
	float				m_fFinalCost;

public:

	/*****************************************************************
	* CPlannerNode(): default constructor
	*
	* Ins:				void
	*
	* Returns:			void
	*
	* Mod. Date:		05/18/2015
	* Mod. Initials:	JM
	*****************************************************************/
	CPlannerNode(TSearchNode* main = nullptr);

	/***************
	* Mutators
	***************/
	void SetVertex(TSearchNode * theVertex)
	{
		m_Vertex = theVertex;
	}
	void SetParent(CPlannerNode * theParent)
	{
		m_Parent = theParent;
	}
	void SetHeuristic(float heuristic)
	{
		m_fHeuristicCost = heuristic;
	}
	void SetFinalCost(float cost)
	{
		m_fFinalCost = cost;
	}
	void SetGiven(float cost)
	{
		m_fGivenCost = cost;
	}
	/*void SetEdge1(DirectX::XMFLOAT3 edgeOne)
	{
	m_xmEdge1 = edgeOne;
	}
	void SetEdge2(DirectX::XMFLOAT3 edgeTwo)
	{
	m_xmEdge2 = edgeTwo;
	}*/

	void SetEdge(int edge)
	{
		m_nPlannerEdge = edge;
	}

	/***************
	*  Accessors
	***************/
	float GetHeuristic() const
	{
		return m_fHeuristicCost;
	}
	double GetFinalCost() const
	{
		return m_fFinalCost;
	}
	float GetGiven() const
	{
		return m_fGivenCost;
	}
	TSearchNode * GetVertex() const
	{
		return m_Vertex;
	}
	CPlannerNode* GetParent() const
	{
		return m_Parent;
	}
	/*DirectX::XMFLOAT3 GetEdge1()const
	{
	return m_xmEdge1;
	}
	DirectX::XMFLOAT3 GetEdge2()const
	{
	return m_xmEdge2;
	}*/
	int GetEdge() const
	{
		return m_nPlannerEdge;
	}
};

/*****************************************************************
* IsMyGreat(): neccesary for the priority queue. Compares two CPlannerNodes
*				and returns true if the heuristic from the left is greater than right
*
* Ins:				CPlannerNode*, CPlannerNode*
*
* Returns:			bool
*
* Mod. Date:		05/23/2015
* Mod. Initials:	JM
*****************************************************************/
bool IsMyGreat(CPlannerNode* const& lhs, CPlannerNode* const& rhs);

class CPathSearch
{
public:

	/*****************************************************************
	* CPathSearch(): default constructor
	*
	* Ins:				void
	*
	* Returns:			void
	*
	* Mod. Date:		05/18/2015
	* Mod. Initials:	JM
	*****************************************************************/
	CPathSearch();

	/*****************************************************************
	* ~CPathSearch():default destructor
	*
	* Ins:				void
	*
	* Returns:			void
	*
	* Mod. Date:		05/18/2015
	* Mod. Initials:	JM
	*****************************************************************/
	~CPathSearch();

	/*****************************************************************
	* Initialize(): build the Node grid to apply the search algorithm
	*
	* Ins:				void
	*
	* Returns:			void
	*
	* Mod. Date:		05/20/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void Initialize(void);

	/*****************************************************************
	* Enter(): entry point to the search loop to start traversing the navmesh
	*
	* Ins:				int start		- start point of the object
	*					int end		- goal point to stop the search
	*
	* Returns:			int		- exit edge
	*
	* Mod. Date:		07/10/2015
	* Mod. Initials:	JM
	*****************************************************************/
	int Enter(int start,
		int end); //,
	//int  currentStart);

	/*****************************************************************
	* IsDone(): end condition to stop searching
	*
	* Ins:				void
	*
	* Returns:			void
	*
	* Mod. Date:		05/20/2015
	* Mod. Initials:	JM
	*****************************************************************/
	bool IsDone(void) const;

	/*****************************************************************
	* Update():			Core of the search algorithm. this loop will evaluate and find the desired path
	*					(THIS BUILD HAS A*)
	*
	* Ins:				void
	*
	* Returns:			void
	*
	* Mod. Date:		05/20/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void Update();

	/*****************************************************************
	* EqualFloat():		check if two xmfloat3 are the same
	*
	* Ins:				DirectX::XMFLOAT3 , DirectX::XMFLOAT3
	*
	* Returns:			bool
	*
	* Mod. Date:		05/20/2015
	* Mod. Initials:	JM
	*****************************************************************/
	bool EqualFloat(DirectX::XMFLOAT3 one, DirectX::XMFLOAT3 two);

	/*****************************************************************
	* Exit():			Function to reset the algorithm
	*
	* Ins:				void
	*
	* Returns:			void
	*
	* Mod. Date:		05/20/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void Exit();

	/*****************************************************************
	* shutdown():		deletes all dynamic memory before the destructor and deletes the instance
	*
	* Ins:				void
	*
	* Returns:			void
	*
	* Mod. Date:		05/20/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void Shutdown();

	/*****************************************************************
	* GetExitEdge():	Returns the edge to exit
	*
	* Ins:				void
	*
	* Returns:			int
	*
	* Mod. Date:		07/13/2015
	* Mod. Initials:	JM
	*****************************************************************/
	int GetExitEdge() const;

	/*****************************************************************
	* getSolution():	Returns the path's solution
	*
	* Ins:				void
	*
	* Returns:			int
	*
	* Mod. Date:		05/20/2015
	* Mod. Initials:	JM
	*****************************************************************/
	int GetSolution()const;

	/*****************************************************************
	* GetClosestVertex():	returns the closest vertex to that point
	*
	* Ins:				position to test
	*
	* Returns:			int - triangle index
	*
	* Mod. Date:		06/11/2015
	* Mod. Initials:	JM
	*****************************************************************/
	int GetClosestVertex(DirectX::XMFLOAT3 myPoint)const;

	/*****************************************************************
	* GetClosestTriangle():	returns the triangle that is colliding with that point
	*
	* Ins:				position to test
	*
	* Returns:			DirectX::XMFLOAT3 closest centroid
	*
	* Mod. Date:		06/13/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void GetClosestTriangle(DirectX::XMFLOAT3 myPoint, int& triangle);

	/*****************************************************************
	* GetInstance():	if the instance is null, it creates a instance. It always returns an instance.
	*
	* Ins:				void
	*
	* Returns:			CPathSearch*, instance pointer
	*
	* Mod. Date:		05/21/2015
	* Mod. Initials:	JM
	*****************************************************************/
	static CPathSearch* GetInstance(void);

	/*****************************************************************
	* HandleLoading():	clears all the data and loads the correct navmesh
	* Ins:				bool
	*
	* Returns:			void
	*
	* Mod. Date:		08/23/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void HandleLoading(bool isCampaign);

	/*****************************************************************
	* Estimate():	Function to calculate the distance between two centroids
	*
	* Ins:					first, second
	*
	* Outs:
	*
	* Returns:				float -> the distance
	*
	* Mod. Date:			05/21/2015
	* Mod. Initials:		JM
	*****************************************************************/
	float Estimate(pair<float, float> first,
		pair<float, float> second);

	/*****************************************************************
	* PointInTriangle():	Function to control that the agents doesn't leave the navmesh
	*
	* Ins:					uint&, DirectX::XMFLOAT3
	*
	* Outs:					N/A
	*
	* Returns:				bool
	*
	* Mod. Date:			06/10/2015
	* Mod. Initials:		JM
	*****************************************************************/
	bool PointInTriangle(DirectX::XMFLOAT3 myPosition,
		int& myTriangle);

	/*****************************************************************
	* PointInTRiangle():	test if a point is inside or outside a triangle
	*
	* Ins:					 DirectX::XMFLOAT3, DirectX::XMFLOAT3, DirectX::XMFLOAT3, DirectX::XMFLOAT3
	*						the three points that define the triangle and the position to test
	*
	* Outs:					N/A
	*
	* Returns:				bool
	*
	* Mod. Date:			06/10/2015
	* Mod. Initials:		JM
	*****************************************************************/
	bool PointToTriangleTest(DirectX::XMFLOAT3 myPosition,
		DirectX::XMFLOAT3 p0,
		DirectX::XMFLOAT3 p1,
		DirectX::XMFLOAT3 p2);

	/*****************************************************************
	* DeleteInstance():		Delete the instance.
	*
	* Ins:				void
	*
	* Returns:			void
	*
	* Mod. Date:		06/25/2015
	* Mod. Initials:	JM
	*****************************************************************/
	static void DeleteInstance();

	DirectX::XMFLOAT3 RandomPoint();

	/*****************************************************************
	* FindExitEdge():	returns the edge where the agent needs to go
	*
	* Ins:					int start - start triangle
	*						int end - end triangle
	*						DirectX::XMFLOAT3 startPos
	*						DirectX::XMFLOAT3 goalPos,
	*
	* Outs:					TTriangleEdge& result
	*
	* Returns:				int
	*
	* Mod. Date:			07/10/2015
	* Mod. Initials:		JM
	*****************************************************************/
	int FindExitEdge(int start, int end, DirectX::XMFLOAT3 startPos, DirectX::XMFLOAT3 goalPos, TTriangleEdge& result);

	//debug
	/*****************************************************************
	* RenderMyTriangle():	debug function to draw the outlines of a given triangle
	*
	* Ins:					int triangle to draw
	*
	* Outs:					void
	*
	* Returns:				void
	*
	* Mod. Date:			07/10/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void RenderMyTriangle(int triangle);

	//instance
	static CPathSearch*						m_gInstance;

private:
	//================================= LOOK UP TABLE ===========================

	/*****************************************************************
	* CreateLookUpTable():	Initializes the lookUp Table
	*
	* Ins:					void
	*
	* Outs:					void
	*
	* Returns:				void
	*
	* Mod. Date:			07/10/2015
	* Mod. Initials:		JM
	*****************************************************************/
	void CreateLookUpTable(void);

	/*****************************************************************
	* SaveLookUpTable():	saves the look up table to a binary file
	*
	* Ins:					void
	*
	* Outs:					void
	*
	* Returns:				bool
	*
	* Mod. Date:			07/10/2015
	* Mod. Initials:		JM
	*****************************************************************/
	bool SaveLookUpTable(void);

	/*****************************************************************
	* LoadLookUpTable():	loads the look up table from a binary file
	*
	* Ins:					void
	*
	* Outs:					void
	*
	* Returns:				bool
	*
	* Mod. Date:			07/10/2015
	* Mod. Initials:		JM
	*****************************************************************/
	bool LoadLookUpTable(void);

	//================================= LOOK UP TABLE END ===========================

	/*****************************************************************
	* LoadFBX():		Blackboxed function to loading in a .fbx file
	*					and storing it into the asset manager while
	*					loading in all the information needed to use
	*					in game, and giving the current .fbx file a tag
	*					to be associated with.
	*
	* Ins:				szFileName
	*
	* Returns:			bool
	*
	* Mod. Date:		05/15/2015
	* Mod. Initials:	JPM
	*****************************************************************/
	bool LoadNavMesh(string szFileName);

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
		CMesh& cMesh);

	/*****************************************************************
	* SaveLocalModelFBX():	Blackboxed function to creating and writing to a
	*							local .fbxbin file for models/meshes to speed up
	*							loading times.
	*
	* Ins:				szFileName
	*
	* Returns:			bool
	*
	* Mod. Date:		06/09/2015
	* Mod. Initials:	JPM
	*****************************************************************/
	bool SaveLocalModelFBX( string szFileName);

	/*****************************************************************
	* LocalLocalModelFBX():		Blackboxed function to loading in a local .fbxbin
	*							file for meshes/models and speed up loading times
	*
	* Ins:				szFileName
	*
	* Returns:			bool
	*
	* Mod. Date:		06/09/2015
	* Mod. Initials:	JPM
	*****************************************************************/
	bool LoadLocalModelFBX( string szFileName);

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

	void CreateCentroids();

	void CleanUpFBX(FbxScene* pScene,
		FbxManager* pManager);

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
	bool ImportFBXData(FbxManager*& prFbxManager,
		FbxScene*& prScene,
		 string szFileName);

	 vector<unsigned int>										m_nIndices;			//store the indeces
	 vector<DirectX::XMFLOAT3>									m_vVertex;			//store the vertex position
	 vector<TTriangle>											m_tTriangles;		// polys

	//PATHFINDING
	unordered_map<unsigned int, TSearchNode*, KeyHash, KeyEqual>	m_pathNodes;		//A*
	map<unsigned int, CPlannerNode*>								m_Visited;
	bool															m_bFinish;
	TSearchNode*													m_pGoalTile;
	TSearchNode*													m_pFirstTile;
	PriorityQueue<CPlannerNode*>									m_myPriority;
	bool															m_bShuttingDown = false;
	pair<float, float>												m_pGoalHeuristic;
	float															m_fhWeight;
	//float															m_fRadius;

	 vector< vector<int>>									m_vLookUpTable;

	 std::string											m_sModelName;
	 FbxLocalTime											m_lDate;
};

#endif // _CPATHSEARCH_H_

