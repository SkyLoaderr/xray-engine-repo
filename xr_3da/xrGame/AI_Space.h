// AI_Space.h: interface for the CAI_Space class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI_SPACE_H__58DA6D1C_2A38_4242_8327_A4EDF2D8EC0C__INCLUDED_)
#define AFX_AI_SPACE_H__58DA6D1C_2A38_4242_8327_A4EDF2D8EC0C__INCLUDED_
#pragma once

//#include "virtualvector.h"
#include "..\xrLevel.h"
#include "ai_a_star_search.h"
//#include "ai_a_star_nodes.h"

namespace AI {
	typedef struct tagSGraphEdge {
		u32		dwVertexNumber;
		float	fPathDistance;
	} SGraphEdge;

	#pragma pack(push,4)
	typedef struct tagSGraphVertex {
		Fvector				tPoint;
		u32					dwNodeID:24;
		u32					ucVertexType:8;
		u32					dwNeighbourCount;
		u32					dwEdgeOffset;
	} SGraphVertex;
	#pragma pack(pop)

	typedef struct tagSGraphHeader {
		u32					dwVersion;
		u32					dwVertexCount;
	} SGraphHeader;

	class	NodeEstimator
	{
	public:
		u32	BestNode;
		float	BestCost;
	public:
		MemberPlacement taMemberPositions;
		MemberNodes		taMemberNodes;
		MemberPlacement taDestMemberPositions;
		MemberNodes		taDestMemberNodes;
		EntityVec		*taMembers;
		#define MAX_NODE_ESTIMATION_COST 10000000.f
		// Estimator itself: 
		// Node, SqrDistance2node, stop/continue
		// Return: min - best, max - worse
		virtual	float	Estimate	(NodeCompressed* Node, float SqrDist, BOOL& bStop)=0;
	};

	const int	hashSize	= 32;

	class	Path
	{
	public:
		vector<u32>	Nodes;
	};
};

class CAIMapShortestPathNode : public CTemplateNode {
public:
	IC float	ffEvaluate(u32 dwStartNode, u32 dwFinishNode);
	IC u32		dwfGetNodeEdgeCount(u32 dwNode);
	IC u32		dwfGetNodeNeighbour(u32 dwNode, u32 dwNeighbourIndex);
	IC bool		bfCheckIfAccessible(u32 dwNode);
};

class CAIMapLCDPathNode : public CTemplateNode {
private:
	float	
		m_fCriteriaLightWeight,
		m_fCriteriaCoverWeight,
		m_fCriteriaDistanceWeight;

public:
	CAIMapLCDPathNode()
	{
		m_fCriteriaLightWeight = 5.f;
		m_fCriteriaCoverWeight = 10.f;
		m_fCriteriaDistanceWeight = 40.f;
	}
	IC float	ffEvaluate(u32 dwStartNode, u32 dwFinishNode);
	IC u32		dwfGetNodeEdgeCount(u32 dwNode);
	IC u32		dwfGetNodeNeighbour(u32 dwNode, u32 dwNeighbourIndex);
	IC bool		bfCheckIfAccessible(u32 dwNode);
};

class CAIMapEnemyPathNode : public CTemplateNode {
private:
	float	
		m_fCriteriaEnemyViewWeight,
		m_fCriteriaLightWeight,
		m_fCriteriaCoverWeight,
		m_fCriteriaDistanceWeight,
		m_fOptimalEnemyDistance;
public:
	CAIMapEnemyPathNode()
	{
		m_fCriteriaLightWeight = 5.f;
		m_fCriteriaCoverWeight = 10.f;
		m_fCriteriaDistanceWeight = 40.f;
		m_fCriteriaEnemyViewWeight = 100.f;
		m_fOptimalEnemyDistance = 30.f;
	}
	IC float	ffEvaluate(u32 dwStartNode, u32 dwFinishNode);
	IC u32		dwfGetNodeEdgeCount(u32 dwNode);
	IC u32		dwfGetNodeNeighbour(u32 dwNode, u32 dwNeighbourIndex);
	IC bool		bfCheckIfAccessible(u32 dwNode);
};

class CAIGraphShortestPathNode : public CTemplateNode {
public:
	IC float	ffEvaluate(u32 dwStartNode, u32 dwFinishNode);
	IC u32		dwfGetNodeEdgeCount(u32 dwNode);
	IC u32		dwfGetNodeNeighbour(u32 dwNode, u32 dwNeighbourIndex);
	IC bool		bfCheckIfAccessible(u32 dwNode);
};

class CAI_Space	: public pureDeviceCreate, pureDeviceDestroy
{
private:
	// Initial data
	CStream*						vfs;			// virtual file
	hdrNODES						m_header;		// m_header
	BYTE*							m_nodes;		// virtual nodes DATA array
	NodeCompressed**				m_nodes_ptr;	// pointers to node's data

	// Debug
	Shader*							sh_debug;

	CStream*						m_tpGraphVFS;			// virtual file
	AI::SGraphHeader				m_tGraphHeader;			// graph header
public:
	// Query
	vector<bool>					q_mark_bit;		// temporal usage mark for queries
	vector<bool>					q_mark_bit_x;		// temporal usage mark for queries
	vector<BYTE>					q_mark;			// temporal usage mark for queries
	vector<u32>						q_stack;
	
	CAI_Space		();
	~CAI_Space		();

	void			Load			(LPCSTR name);
	void			Render			();

	const hdrNODES	GetHeader		(){return m_header;};
	void			q_Range(u32 StartNode, const Fvector& BasePos, float Range, AI::NodeEstimator& Estimator, float &fOldCost, u32 dwTimeDifference);
	void			q_Range			(u32 StartNode, const Fvector& Pos,	float Range,	AI::NodeEstimator& Estimator, float &fOldCost);
	void			q_Range_Bit		(u32 StartNode, const Fvector& Pos,	float Range,	AI::NodeEstimator& Estimator, float &fOldCost);
	void			q_Range_Bit		(u32 StartNode, const Fvector& BasePos, float Range, NodePosition* QueryPosition, u32 &BestNode, float &BestCost);
	void			q_Range_Bit_X	(u32 StartNode, const Fvector& BasePos, float Range, NodePosition* QueryPosition, u32 &BestNode, float &BestCost);
	//void			q_Range_Bit		(u32 StartNode, const Fvector& BasePos, float Range, u32 &BestNode, float &BestCost);
	u32				q_Node			(u32 PrevNode,  const Fvector& Pos, bool bShortSearch = false);

	AI::SGraphVertex						*m_tpaGraph;			// graph
	SNode									*m_tpHeap;
	SIndexNode								*m_tpIndexes;
	float									m_fSize,m_fYSize,m_fSize2,m_fYSize2;
	CAStarSearch<CAIMapShortestPathNode>	*m_tpMapPath;
	CAStarSearch<CAIMapLCDPathNode>			*m_tpLCDPath;
	CAStarSearch<CAIMapEnemyPathNode>		*m_tpEnemyPath;
	CAStarSearch<CAIGraphShortestPathNode>	*m_tpGraphPath;
//	// yet another A* search
	#define DEFAULT_LIGHT_WEIGHT		  5.f 
	#define DEFAULT_COVER_WEIGHT		 10.f 
	#define DEFAULT_DISTANCE_WEIGHT		 40.f
	#define DEFAULT_ENEMY_VIEW_WEIGHT	100.f
	float			vfFindTheXestPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, float fLightWeight = DEFAULT_LIGHT_WEIGHT, float fCoverWeight = DEFAULT_COVER_WEIGHT, float fDistanceWeight = DEFAULT_DISTANCE_WEIGHT);
	float			vfFindTheXestPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, NodeCompressed& tEnemyNode, float fOptimalEnemyDistance, float fLightWeight = DEFAULT_LIGHT_WEIGHT, float fCoverWeight = DEFAULT_COVER_WEIGHT, float fDistanceWeight = DEFAULT_DISTANCE_WEIGHT, float fEnemyViewWeight = DEFAULT_ENEMY_VIEW_WEIGHT);
	float			vfFindTheXestPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, Fvector tEnemyPosition, float fOptimalEnemyDistance, float fLightWeight, float fCoverWeight, float fDistanceWeight, float fEnemyViewWeight);
	void			vfLoadSearch();
	void			vfUnloadSearch();
	// 

	// yet another best node search
	//void			vfFindTheBestNode(u32 StartNode, const Fvector& Pos,	float Range,	AI::NodeEstimator& Estimator);
	// 

	int				q_LoadSearch	(const Fvector& Pos);	// <0 - failure

	// Helper functions
	IC	const hdrNODES&		Header()		{ return m_header; }
	IC	NodeCompressed*		Node(u32 ID)	{ return vfs?m_nodes_ptr[ID]:NULL; }

	IC	void		UnpackPosition	(Fvector& Pdest, const NodePosition& Psrc)
	{
		Pdest.x = float(Psrc.x)*m_header.size;
		Pdest.y = (float(Psrc.y)/65535)*m_header.size_y + m_header.aabb.min.y;
		Pdest.z = float(Psrc.z)*m_header.size;
	}
	IC	u32		UnpackLink		(NodeLink& L)
	{	return (*LPDWORD(&L))&0x00ffffff;	}

	IC	void		PackPosition	(NodePosition& Pdest, const Fvector& Psrc)
	{
		float sp = 1/m_header.size;
		int px	= iFloor(Psrc.x*sp+EPS_L + .5f);
		int py	= iFloor(65535.f*(Psrc.y-m_header.aabb.min.y)/(m_header.size_y)+EPS_L);
		int pz	= iFloor(Psrc.z*sp+EPS_L + .5f);
		/**/
		clamp	(px,-32767,32767);	Pdest.x = s16	(px);
		clamp	(py,0,     65535);	Pdest.y = u16	(py);
		clamp	(pz,-32767,32767);	Pdest.z = s16	(pz);
	}

	// REF-counting
	IC  BYTE		ref_add			(u32 ID)	
	{
		if (vfs && ID!=u32(-1))	return q_mark[ID] += BYTE(1);	
		else						return BYTE(0);
	}
	IC	BYTE		ref_dec			(u32 ID)
	{
		if (vfs && ID!=u32(-1))	return q_mark[ID] -= BYTE(1);	
		else						return BYTE(0);
	}

	// Utilities
	float			u_SqrDistance2Node	(const Fvector& P, const NodeCompressed* Node);
	IC BOOL			u_InsideNode		(const NodeCompressed& N, const NodePosition& P)
	{
		return 	((P.x>=N.p0.x)&&(P.x<=N.p1.x))&&	// X inside
				((P.z>=N.p0.z)&&(P.z<=N.p1.z));		// Z inside
	}
	
	IC bool bfInsideNode(NodeCompressed *tpNode, Fvector &tCurrentPosition)
	{
		Fvector tP0, tP1;
		float fHalfSubNodeSize = m_header.size*.5f;
		UnpackPosition(tP0,tpNode->p0);
		UnpackPosition(tP1,tpNode->p1);
		return(
			(tCurrentPosition.x >= tP0.x - fHalfSubNodeSize - EPS) &&
			(tCurrentPosition.z >= tP0.z - fHalfSubNodeSize - EPS) &&
			(tCurrentPosition.x <= tP1.x + fHalfSubNodeSize + EPS) &&
			(tCurrentPosition.z <= tP1.z + fHalfSubNodeSize + EPS)
		);
	}

	IC float ffGetY(NodeCompressed &tNode, float X, float Z)
	{
		Fvector	DUP, vNorm, v, v1, P0;
		DUP.set(0,1,0);
		pvDecompress(vNorm,tNode.plane);
		Fplane PL; 
		UnpackPosition(P0,tNode.p0);
		PL.build(P0,vNorm);
		v.set(X,P0.y,Z);	
		PL.intersectRayPoint(v,DUP,v1);	
		return(v1.y);
	}

	IC	bool bfTooSmallAngle(float fAngle0, float fAngle1, float fDelta)
	{
		return(_abs(fAngle0 - fAngle1) < fDelta) || ((_abs(_abs(fAngle0 - fAngle1) - PI_MUL_2) < fDelta));
	}

	Fvector	tfGetNodeCenter(u32 dwNodeID);
	Fvector	tfGetNodeCenter(NodeCompressed *tpNode);
	
	float	ffGetDistanceBetweenNodeCenters(u32 dwNodeID0, u32 dwNodeID1);
	float	ffGetDistanceBetweenNodeCenters(NodeCompressed *tpNode0, u32 dwNodeID1);
	float	ffGetDistanceBetweenNodeCenters(u32 dwNodeID0, NodeCompressed *tpNode1);
	float	ffGetDistanceBetweenNodeCenters(NodeCompressed *tpNode0, NodeCompressed *tpNode1);
	void	vfCreateFastRealisticPath(vector<Fvector> &tpaPoints, u32 dwStartNode, vector<Fvector> &tpaDeviations, vector<Fvector> &tpaPath, vector<u32> &dwaNodes, bool bLooped, bool bUseDeviations = false, float fRoundedDistanceMin = 2.0f, float fRoundedDistanceMax = 2.0f, float fRadiusMin = 3.0f, float fRadiusMax = 3.0f, float fSuitableAngle = PI_DIV_8*.375f, float fSegmentSizeMin = .35f, float fSegmentSizeMax = 1.4f);
	float	ffMarkNodesInDirection(u32 dwStartNode, Fvector tStartPosition, Fvector tDirection, vector<bool> &tpaMarks, float fDistance, vector<u32> &tpaStack);
	bool	bfCheckNodeInDirection(u32 dwStartNode, Fvector tStartPosition, u32 dwFinishNode);
	u32		dwfCheckPositionInDirection(u32 dwStartNode, Fvector tStartPosition, Fvector tFinishPosition);
	//void	vfCreate2DMap(char *caFile0, char *caFile1, char *caFile2);

	// Device dependance
	virtual void	OnDeviceCreate	();
	virtual void	OnDeviceDestroy	();
};

#endif // !defined(AFX_AI_SPACE_H__58DA6D1C_2A38_4242_8327_A4EDF2D8EC0C__INCLUDED_)